#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "Utils.h"
#include <DebugLog.h>

#include "core/Karma.h"


void setup() {
    Serial.begin(115200);
    Wire.begin(OLED_SDA, OLED_SCL);

    static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
    static SX1276 lora = new Module(LORA_CS, LORA_DIO0, LORA_RST, LORA_BUSY);
    static Karma karma(lora, display);

    karma.init();
}

void loop() {
}
