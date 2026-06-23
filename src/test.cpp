#include <Arduino.h>

#include "Config.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

#include "Clock.hpp"

void onReceive(uint8_t x) {
    Serial.println(x);
}

uint8_t value = 0;

hack::Sender<2> sender;
hack::Receiver<A0> receiver(onReceive);

hack::Clock<5'000'000> clock;

void setup() {
    Serial.begin(250000);

    clock.update();
    Serial.println("Test");
}

void loop() {
    if (clock.update()) {
        const uint8_t inverse = static_cast<uint8_t>(UINT8_MAX - value);
        sender(value);
        sender(inverse);
        Serial.print("in:");
        Serial.print(value);
        Serial.print(',');
        Serial.println(inverse);
        ++value;
    }

    sender.update();
    receiver.update();
}
