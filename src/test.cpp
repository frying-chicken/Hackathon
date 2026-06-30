#include <Arduino.h>

#include "Config.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

#include "Clock.hpp"

uint8_t value = 0;

void onReceive(uint8_t x) {
    Serial.println(x == value - 1);
}

hack::Receiver<A0> receiver(onReceive);
hack::Sender<2, uint8_t> sender;

hack::Clock<5'000'000> clock;

void setup() {
    Serial.begin(250000);
    Serial.println("Test");

    clock.update();
}

void loop() {
    if (clock.update()) {
        sender(value);
        Serial.print("in:");
        Serial.println(value++);
    }

    sender.update();
    receiver.update();
}
