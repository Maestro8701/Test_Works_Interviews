//
// Created by never on 09.06.2025.
//

#ifndef HELLOSERVICE_H
#define HELLOSERVICE_H
#include <mutex>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

class HelloService {
public:
    HelloService() = default;

    bool init(uint32_t intervalMs, std::function<void()> onHello) {
        _intervalTicks = pdMS_TO_TICKS(intervalMs);
        _onHello = std::move(onHello);

        _timer = xTimerCreate(
            "HelloTimer",
            _intervalTicks,
            pdTRUE,
            this,
            timerCallbackStatic);

        return _timer != nullptr;
    }

    void start() const {
        if (_timer) {
            LOG_INFO("Hello таймер запущен");
            xTimerStart(_timer, 0);
        }
    }

    void shiftTimer(uint32_t offsetMs) const {
        if (!_timer) {
            LOG_INFO("Hello таймер не инициализирован");
            return;
        }

        const TickType_t now = xTaskGetTickCount();
        const TickType_t expiry = xTimerGetExpiryTime(_timer);
        const TickType_t remaining = expiry > now ? expiry - now : 0;

        uint32_t newRemaining = remaining + pdMS_TO_TICKS(offsetMs);
        if (newRemaining < 1) {
            newRemaining = 1;
        }

        xTimerChangePeriod(_timer, newRemaining, 0);

        LOG_INFO("Hello таймер сдвинут на " + String(offsetMs) + " мс");
    }
private:
    TimerHandle_t _timer = nullptr;
    TickType_t _intervalTicks;
    std::function<void()> _onHello;

    static void timerCallbackStatic(TimerHandle_t xTimer) {
        auto* self = static_cast<HelloService*>(pvTimerGetTimerID(xTimer));

        if (self && self->_onHello) {
            self->_onHello();
        }
    }
};

#endif //HELLOSERVICE_H
