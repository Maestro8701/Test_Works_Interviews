//
// Created by never on 24.01.2025.
//

#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.0.5.4"

// Display
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DISPLAY_TIMEOUT 5000

// LoRa
#define LORA_CS 18
#define LORA_DIO0 26
#define LORA_RST 23
#define LORA_BUSY -1

// Messenger
#define SEND_MESSAGES_DELAY 100
#define ACK_TIMEOUT_MS 40000
#define TTL 15
#define BROADCAST_ADDRESS 255
#define HELLO_INTERVAL 600000
#define HEARTBEAT_INTERVAL 150000
#define RESEND_MESSAGES_INTERVAL 30000
#define MAX_MESSAGE_TEXT_SIZE 64

// Contacts
#define MAX_CONTACTS 20

#endif //CONFIG_H
