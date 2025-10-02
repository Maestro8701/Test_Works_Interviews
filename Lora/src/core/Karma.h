//
// Created by never on 17.05.2025.
//

#ifndef KARMA_H
#define KARMA_H
#include "AckRouteManager.h"
#include "Adafruit_SSD1306.h"
#include "DebugLog.h"
#include "Module.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeVector.h"
#include "display/DisplayService.h"
#include "events/EventBus.h"
#include "messages/MessageQueue.h"
#include "messages/MessageService2.h"
#include "modules/SX127x/SX1276.h"
#include "protocol/PacketProcessor.h"
#include "routing/RoutingService.h"
#include "services/HelloService.h"
#include "storage/PersistenceService.h"

class Karma final : public EventListener {
public:
    explicit Karma(SX1276& lora, Adafruit_SSD1306& display) :
       _display(display),
       _lora(lora),
        _displayService(_display),
       _messageService(_lora),
       _routingService(_sendAttemptManager),
       _packetProcessor(_eventBus)
    {
        instance = this;

        _helloService.init(HELLO_INTERVAL, [this]() {
            LOG_INFO("Отправка hello");
            Message hello = _messageService.createHelloMessage();
           _messageQueue.push(hello);
        });

        _displayService.init();

        _eventBus.init();
        _eventBus.subscribe(this);
        _eventBus.subscribe(&_packetProcessor);
    }

    bool init();

    MessageService2& messages() { return _messageService; }
    PersistenceService& persistence() { return _persistenceService; }

    void clearMessageQueue() { _messageQueue.clear(); }
    void clearSentMessages() { _sentMessages.clear(); }
    void clearReceivedMessages() { _receivedMessages.clear(); }

    void startRadioTask();

    void onEvent(const Event& event) override;
private:
    uint16_t _localAddress = {};
    static Karma* instance;
    SX1276& _lora;
    Adafruit_SSD1306& _display;
    MessageService2 _messageService;
    RoutingService _routingService;
    PersistenceService _persistenceService;
    PacketProcessor _packetProcessor;
    AckRouteManager _ackRouteManager;
    SendAttemptManager _sendAttemptManager;

    DisplayService _displayService;
    HelloService _helloService;

    EventBus _eventBus = {};

    ThreadSafeVector<Message> _sentMessages = {};
    ThreadSafeVector<uint32_t> _receivedMessages = {};
    MessageQueue _messageQueue = {};
    ThreadSafeQueue<Frame> _rawMessagesQueue = {};

    // --- RadioTask entry point & ISR
    TaskHandle_t _radioTaskHandle = nullptr;
    static void radioTaskEntry(void* parameter);
    void radioTask();
    void IRAM_ATTR onDio0Rise();
    static void IRAM_ATTR onDio0RiseStatic();

    // --- Tasks ---
    TaskHandle_t _sendMessagesTaskHandle = nullptr;
    static void sendMessagesTaskEntry(void* parameter);
    void sendMessagesTask();
    void startSendMessagesTask();

    TaskHandle_t _ackMonitorTaskHandle = nullptr;
    static void ackMonitorTaskEntry(void* parameter);
    void ackMonitorTask();
    void startAckMonitorTask();

    TaskHandle_t _readSerialTaskHandle = nullptr;
    static void readSerialTaskEntry(void* parameter);
    void readSerialTask();
    void startSerialTask();

    TaskHandle_t _heartbeatTaskHandle = nullptr;
    static void heartbeatTaskEntry(void* parameter);
    void heartbeatTask();
    void startHeartbeatTask();

    // event handlers
    void onMessageReadyHandler(const Event& event);
    void onAckReceivedHandler(const Event& event);
    void onMessageForRelayReceived(const Event& event);
    void onAckTimeoutExpired(const Event& event);

    // util functions
    bool isMessageAlreadyReceived(uint32_t messageId) const;
    void removeFromAckWaitingList(uint32_t messageId);
    bool isMessageForRelay(const PacketHeader& header);

    void ensureRadioIdle();
    void restartReceiveMode();

    bool handleIncomingPacket();

    void parseCommand(const std::string& input);
    void handleSendCommand(const std::vector<std::string>& args);
    void handleHelloCommand();

    bool isChannelBusy() const;
};

#endif //KARMA_H
