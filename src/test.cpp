#include <Arduino.h>

#include "Config.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

#include "Clock.hpp"

uint32_t value = 0;

void onReceive(uint8_t x) {
    Serial.println(x);
}

hack::Receiver<A0> receiver(onReceive);
hack::Sender<2, 256> sender;

hack::Clock<5'000'000> clock;

void setup() {
    Serial.begin(250000);
    Serial.println("Test ver1");

    clock.update();
}

time_t x = 0;

void loop() {
    if (clock.update()) {
        sender(value);
        Serial.print("in:");
        Serial.println(value++);
    }

    sender.update();
    receiver.update();
}
