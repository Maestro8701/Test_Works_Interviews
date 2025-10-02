//
// Created by never on 17.05.2025.
//

#include "Karma.h"

#include <cstring>

#include "Utils.h"
#include "DebugLog.h"
#include "protocol/Frame.h"
#include "protocol/MessageCodec.h"
#include "protocol/PacketHeader.h"
#include <sstream>

Karma* Karma::instance = nullptr;

bool Karma::init() {
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    _localAddress = Utils::getLocalAddress();
    auto startState = _lora.begin(433.0, 250.0, 10, 8, 0x12, 20, 20, 0);

    if (startState != RADIOLIB_ERR_NONE) {
        LOG_INFO("RADIOLIB START ERROR");
        return false;
    }

    LOG_INFO("START ACK MONITOR TASK");
    startAckMonitorTask();

    LOG_INFO("START SEND MESSAGES TASK");
    startSendMessagesTask();

    LOG_INFO("START SERIAL READ TASK");
    startSerialTask();

    LOG_INFO("START HEARTBEAT TASK");
    startHeartbeatTask();

    LOG_INFO("START RADIO TASK");
    // RadioTask запускается строго последним
    startRadioTask();
    LOG_INFO("RADIO TASK STARTED ✅");

    LOG_INFO("HELLO SENDING STARTED");
    _helloService.start();

    _displayService.printLine("LoRa Chat MVP");
    _displayService.printLine("Local address: " + Utils::getLocalAddressString());
    _displayService.printLine("Version: " + String(VERSION));

    return true;
}

void Karma::radioTaskEntry(void *parameter) {
    static_cast<Karma*>(parameter)->radioTask();
}

void Karma::startRadioTask() {
     const BaseType_t taskCreated = xTaskCreate(
         radioTaskEntry,
         "RadioTask",
         8192,
         this,
         tskIDLE_PRIORITY + 10,
         &_radioTaskHandle
     );

    configASSERT(taskCreated == pdPASS);
}

void Karma::onEvent(const Event& event) {
    switch (event.type) {
        case EventType::MESSAGE_READY:
            onMessageReadyHandler(event);
            break;
        case EventType::ACK_RECEIVED:
            onAckReceivedHandler(event);
            break;
        case EventType::ACK_TIMEOUT:
            onAckTimeoutExpired(event);
            break;
        case EventType::FOR_RELAY:
            onMessageForRelayReceived(event);
        default:
            LOG_INFO("HANDLER NOT FOUND");
            break;
    }
}

[[noreturn]] void Karma::radioTask() {
    LOG_INFO("radioTask started");

    _radioTaskHandle = xTaskGetCurrentTaskHandle();
    LOG_INFO("got task handle");

    _lora.setDio0Action(onDio0RiseStatic, RISING);
    LOG_INFO("set Dio0 action");

    _lora.startReceive();

    while (true) {
        // Ждём IRQ
        LOG_INFO("ЖДЁМ IRQ");
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (!handleIncomingPacket()) {
            LOG_INFO("HANDLE INCOMING PACKET ВЕРНУЛ FALSE");
            _lora.startReceive();
        }
    }
}


// ------------ Send messages task ------------
void Karma::sendMessagesTask() {
    while (true) {

        if (_rawMessagesQueue.empty() && _messageQueue.empty()) {
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        LOG_INFO("SEND TASK INVOKED");

        _lora.clearDio0Action();
        ensureRadioIdle();

        if (isChannelBusy()) {
            restartReceiveMode();
            continue;
        }

        if (!_rawMessagesQueue.empty()) {
            Frame frame {};
            _rawMessagesQueue.pop(frame);

            const auto result = _messageService.sendFrame(frame);
            if (result != RADIOLIB_ERR_NONE) {
                LOG_INFO("ОШИБКА ОТПРАВКИ СООБЩЕНИЯ");
                LOG_INFO(result);
                continue;
            }

            restartReceiveMode();
        }

        ensureRadioIdle();

        if (isChannelBusy()) {
            restartReceiveMode();
            continue;
        }

        if (!_messageQueue.empty()) {
            Message message {};
            _messageQueue.pop(message);

            Serial.print("ТИП СООБЩЕНИЯ: ");
            Serial.println(message.typeToString());

            const auto result = _messageService.send(message);
             if (result != RADIOLIB_ERR_NONE) {
                 LOG_INFO("ОШИБКА ОТПРАВКИ СООБЩЕНИЯ");
                 LOG_INFO(result);
                 continue;
             }

             restartReceiveMode();
        }

        restartReceiveMode();

        vTaskDelay(pdMS_TO_TICKS(SEND_MESSAGES_DELAY));
    }
}

void Karma::startSendMessagesTask() {
    const BaseType_t taskCreated = xTaskCreate(
        sendMessagesTaskEntry,
        "SendMessagesTask",
        8192,
        this,
        tskIDLE_PRIORITY + 6,
        &_sendMessagesTaskHandle
    );

    configASSERT(taskCreated == pdPASS);
}

void Karma::sendMessagesTaskEntry(void* parameter) {
    static_cast<Karma*>(parameter)->sendMessagesTask();
}

// ------------ Ack monitor task ------------
void Karma::ackMonitorTask() {
    while (true) {
        auto snapshot = _sentMessages.snapshot();

        if (snapshot.empty()) {
            vTaskDelay(pdMS_TO_TICKS(RESEND_MESSAGES_INTERVAL));
            continue;
        }

        const auto now = millis();

        for (const auto& message : snapshot) {
            if (now - message.timestamp > ACK_TIMEOUT_MS) {
                Event ackTimeoutEvent = Event::make(
                    EventType::ACK_TIMEOUT,
                    std::make_shared<Message>(message)
                );

                _eventBus.emit(ackTimeoutEvent);

                break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(RESEND_MESSAGES_INTERVAL));
    }
}

void Karma::startAckMonitorTask() {
    const BaseType_t taskCreated = xTaskCreate(
        ackMonitorTaskEntry,
        "AckMonitorTask",
        4096,
        this,
        tskIDLE_PRIORITY + 4,
        &_ackMonitorTaskHandle
    );

    configASSERT(taskCreated == pdPASS);
}

void Karma::readSerialTaskEntry(void* parameter) {
    static_cast<Karma*>(parameter)->readSerialTask();
}

void Karma::readSerialTask() {
    while (true) {
        if (Serial.available()) {
            std::string input;
            while (Serial.available()) {
                char c = Serial.read();
                if (c == '\n') break;

                input += c;
            }

            parseCommand(input);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Karma::startSerialTask() {
    const BaseType_t taskCreated = xTaskCreate(
        readSerialTaskEntry,
        "ReadSerialTask",
        4096,
        this,
        tskIDLE_PRIORITY + 3,
        &_readSerialTaskHandle);

    configASSERT(taskCreated == pdPASS);
}

void Karma::heartbeatTaskEntry(void* parameter) {
    return static_cast<Karma*>(parameter)->heartbeatTask();
}

void Karma::heartbeatTask() {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_INTERVAL));

        size_t removedCount = _routingService.removeExpiredNeighbors();

        if (removedCount > 0) {
            LOG_INFO("Удалено соседей: " + String(removedCount));
        }

    }
}

void Karma::startHeartbeatTask() {
    const BaseType_t taskCreated = xTaskCreate(
        heartbeatTaskEntry,
        "HeartbeatTask",
        2048,
        this,
        tskIDLE_PRIORITY + 2,
        &_heartbeatTaskHandle
    );

    configASSERT(taskCreated == pdPASS);
}

void Karma::ackMonitorTaskEntry(void* parameter) {
    static_cast<Karma*>(parameter)->ackMonitorTask();
}

// ------------ IRQ handler ------------
void IRAM_ATTR Karma::onDio0Rise() {
    if (_radioTaskHandle == nullptr) {
        LOG_ERROR("ISR called but _radioTaskHandle is nullptr!");
        return;
    }

    LOG_INFO("DIO0 INVOKED");

    BaseType_t higherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(_radioTaskHandle, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

void IRAM_ATTR Karma::onDio0RiseStatic() {
    if (!instance) {
        LOG_INFO("INSTANCE IS NULL");
    }

        instance->onDio0Rise();
}

// ----------- event handlers -----------
void Karma::onMessageReadyHandler(const Event& event) {
    const auto message = event.get<Message>();

    _displayService.clearAndPrintLine(message->text);

    _receivedMessages.push_back(message->messageId);

    const Message ackMessage = _messageService.createAckForMessage(*message);
    _messageQueue.push(ackMessage);
}

void Karma::onAckReceivedHandler(const Event& event) {
    LOG_INFO("onAckReceivedHandler invoked");
    const auto message = event.get<Message>();

    removeFromAckWaitingList(message->messageId);
    _sendAttemptManager.markDelivered(message->messageId);

    // если мы отправили сообщение по широковещательному каналу, но получили ACK, то сохраняем маршрут
    if (message->nextHop == BROADCAST_ADDRESS &&
        message->destinationId != message->previousNode) {

        Route route {};
        route.fromNode = _localAddress;
        route.toNode = message->destinationId;
        route.nextHop = message->previousNode;

        _routingService.markSuccess(route.fromNode, route.toNode, route.nextHop);
    }
}

void Karma::onMessageForRelayReceived(const Event &event) {
    const auto frame = event.get<Frame>();

    LOG_INFO("MESSAGE FOR RELAY RECEIVED");

    frame->header.ttl--;

    uint16_t nextHop;

    switch (frame->header.messageType) {
        case MESSAGE_TYPE_ACK:
            nextHop = _ackRouteManager
                .popNodeIdForMessage(frame->header.messageId);

            frame->header.nextHop = nextHop;
            frame->header.previousNode = _localAddress;

            _rawMessagesQueue.push(*frame);
            break;

        case MESSAGE_TYPE_TEXT:
            nextHop = _routingService.getBestNextHop(
                frame->header
            );

            _ackRouteManager.registerRoute(
                frame->header.messageId,
                frame->header.previousNode
            );

            frame->header.previousNode = _localAddress;
            frame->header.nextHop = nextHop;

            _rawMessagesQueue.push(*frame);
            break;
        default:
            // TODO: логгер
            break;
    }
}

void Karma::onAckTimeoutExpired(const Event& event) {
    const auto message = event.get<Message>();

    _sendAttemptManager.incrementAttempt(message->messageId);

    const auto nextHop = _routingService.getBestNextHop(*message);


    message->nextHop = nextHop;

    LOG_INFO("Перепосылка пакета: " + String(message->messageId));

    _messageQueue.push(*message);
}


// ----------- utils -----------
bool Karma::isMessageAlreadyReceived(const uint32_t messageId) const {
    return std::find(_receivedMessages.begin(), _receivedMessages.end(), messageId) != _receivedMessages.end();
}

void Karma::removeFromAckWaitingList(uint32_t messageId) {
    _sentMessages.erase_if(
        [messageId](const Message& message)
        { return message.messageId == messageId; });
}

bool Karma::isMessageForRelay(const PacketHeader& header) {
    if (header.destinationId != _localAddress && header.nextHop == _localAddress) {
        return true;
    }

    return false;
}

void Karma::ensureRadioIdle() {
    const uint16_t irqFlags = _lora.getIRQFlags();
    if (irqFlags != RADIOLIB_SX127X_MASK_IRQ_FLAG_RX_DONE) {
        _lora.standby();
    }
}

void Karma::restartReceiveMode() {
    _lora.setDio0Action(onDio0RiseStatic, RISING);
    _lora.startReceive();
}

bool Karma::handleIncomingPacket() {
    const size_t packetLen = _lora.getPacketLength();

    Serial.print("ДЛИНА ПАКЕТА: ");
    Serial.println(packetLen);

    if (packetLen < sizeof(PacketHeader) || packetLen > MAX_PACKET_SIZE) {
        LOG_INFO("НЕКОРРЕКТНЫЙ РАЗМЕР ПАКЕТА: ");
        Serial.print(packetLen);
        return false;
    }

    uint8_t buffer[MAX_PACKET_SIZE];
    if (_lora.readData(buffer, packetLen) != RADIOLIB_ERR_NONE) {
        LOG_INFO("ОШИБКА ЧТЕНИЯ ПОЛНОГО ПАКЕТА");
        return false;
    }

    PacketHeader header{};
    std::memcpy(&header, buffer, sizeof(PacketHeader));

    // Проверка FROM_ID
    if (header.fromId == _localAddress) {
        LOG_INFO("СООБЩЕНИЕ ОТ СЕБЯ — ИГНОРИРУЕТСЯ");
        return false;
    }

    Serial.print("FROM ID: ");
    Serial.println(header.fromId);

    const float rssi = _lora.getRSSI();
    const float snr = _lora.getSNR();

    _routingService.analyzePacket(header.fromId, rssi, snr);
    _helloService.shiftTimer(HELLO_INTERVAL / 10);

    if (header.messageType == MESSAGE_TYPE_HELLO) {
        LOG_INFO("ПОЛУЧЕН HELLO ОТ: " + Utils::intToHexString(header.fromId));
        return true;
    }

    if (isMessageAlreadyReceived(header.messageId)) {
        auto msg = _messageService.createAckForMessage(header);
        LOG_INFO("СООБЩЕНИЕ УЖЕ ПОЛУЧЕНО. ПЕРЕПОСЫЛКА ACK...");
        _messageQueue.push(msg);
        return false;
    }

    auto frame = std::make_shared<Frame>();
    frame->header = header;
    frame->rssi = rssi;
    frame->snr = snr;

    const size_t payloadLen = packetLen - sizeof(PacketHeader);
    frame->length = payloadLen;

    if (payloadLen > 0) {
        std::memcpy(frame->payload, buffer + sizeof(PacketHeader), payloadLen);
    }

    if (header.destinationId != _localAddress && header.nextHop != _localAddress) {
        if (header.nextHop == BROADCAST_ADDRESS) {
            const Event msgForRelayEvent = Event::make(EventType::FOR_RELAY, frame);
            _eventBus.emit(msgForRelayEvent);
            return true;
        }
        LOG_INFO("ПАКЕТ АДРЕСОВАН НЕ НАМ");
        return false;
    }

    if (isMessageForRelay(header)) {
        const Event msgForRelayEvent = Event::make(EventType::FOR_RELAY, frame);
        _eventBus.emit(msgForRelayEvent);
        return true;
    }

    Event rawPacketEvent = Event::make(EventType::RAW_PACKET, frame);
    _eventBus.emit(rawPacketEvent);

    return true;
}


void Karma::parseCommand(const std::string& input) {
    std::istringstream stream(input);
    std::vector<std::string> tokens;
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        Serial.println("Command is empty");
        return;
    }

    if (tokens[0] == "send") {
        tokens.erase(tokens.begin());
        handleSendCommand(tokens);
    } else if (tokens[0] == "hello") {
        handleHelloCommand();
    } else if (tokens[0] == "restart") {
        esp_restart();
    } else {
        Serial.println("Unknown command");
    }
}

void Karma::handleSendCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        Serial.println("Usage: send <destinationId> <message text>");
        return;
    }

    uint16_t destinationId = Utils::getLocalAddressFromString(args[0]);

    std::string messageText;
    for (size_t i = 1; i < args.size(); ++i) {
        messageText += args[i];
        if (i < args.size() -1) {
            messageText += " ";
        }
    }

    Message msg;
    msg.fromId = _localAddress;
    msg.destinationId = destinationId;
    msg.previousNode = _localAddress;
    msg.messageType = MESSAGE_TYPE_TEXT;
    msg.messageId = esp_random();
    msg.ttl = TTL;
    msg.timestamp = millis();
    msg.text = messageText;

    _sendAttemptManager.registerMessage(msg.messageId);
    msg.nextHop = _routingService.getBestNextHop(msg);

    _messageQueue.push(msg);
    _sentMessages.push_back(msg);

    Serial.println("Message scheduled for sending");
}

void Karma::handleHelloCommand() {
    LOG_INFO("Отправляем hello...");
    auto msg = _messageService.createHelloMessage();
    _messageQueue.push(msg);
}

bool Karma::isChannelBusy() const {
    const auto scanResult = _lora.scanChannel();

    if (scanResult == RADIOLIB_PREAMBLE_DETECTED) {
        LOG_INFO("PREAMBLE DETECTED");
        return true;
    }

    return false;
}
