//
// Created by never on 20.05.2025.
//

#include "DisplayService.h"

#include "config.h"
#include "DebugLog.h"
#include "DebugLog/Types.h"

void DisplayService::init() {
    if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    _display.clearDisplay();
    _display.setTextSize(1, 1);
    _display.setTextColor(WHITE);
}

void DisplayService::enableDisplay() {
    if (!_displayEnabled) {
        _display.ssd1306_command(SSD1306_DISPLAYON);
        _displayEnabled = true;
    }
}

void DisplayService::disableDisplay() {
    if (_displayEnabled) {
        _display.ssd1306_command(SSD1306_DISPLAYOFF);
        _displayEnabled = false;
    }
}

void DisplayService::clear() {
    _display.clearDisplay();
    _display.setCursor(1, 1);
}

void DisplayService::printLine(const std::string& text) {
    enableDisplay();

    _display.println(text.c_str());
    _display.display();

    startTimer();
}

void DisplayService::printLine(String& text) {
    enableDisplay();

    _display.println(text);
    _display.display();

    startTimer();
}

void DisplayService::clearAndPrintLine(const std::string& text) {
    enableDisplay();

    clear();

    _display.println(text.c_str());
    _display.display();

    startTimer();
}

void DisplayService::clearAndPrintLine(String &text) {
    enableDisplay();
    clear();

    _display.println(text);
    _display.display();

    startTimer();
}

void DisplayService::timerCallback(TimerHandle_t xTimer) {
    auto* service = static_cast<DisplayService*>(pvTimerGetTimerID(xTimer));

    service->disableDisplay();
}

void DisplayService::startTimer() {
    if (xTimerIsTimerActive(_offTimer)) {
        xTimerStop(_offTimer, 0);
    }
    xTimerStart(_offTimer, 0);
}
