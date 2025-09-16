//
// Created by never on 20.05.2025.
//

#ifndef DISPLAYSERVICE_H
#define DISPLAYSERVICE_H
#include <string>

#include "Adafruit_SSD1306.h"
#include "config.h"


class DisplayService {
public:
    explicit DisplayService(Adafruit_SSD1306& display) : _display(display) {
        _offTimer = xTimerCreate(
            "DisplayOffTimer",
            pdMS_TO_TICKS(DISPLAY_TIMEOUT),
            pdFALSE,
            this,
            DisplayService::timerCallback);
    }

    void init();
    void enableDisplay();
    void disableDisplay();
    void clear();
    void printLine(const std::string& text);
    void printLine(String& text);
    void clearAndPrintLine(const std::string& text);
    void clearAndPrintLine(String& text);
private:
    bool _displayEnabled = false;
    Adafruit_SSD1306& _display;

    TimerHandle_t _offTimer = nullptr;

    static void timerCallback(TimerHandle_t xTimer);
    void startTimer();
};



#endif //DISPLAYSERVICE_H
