#include <Arduino.h>

#include "Config.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

#include "Clock.hpp"

void onReceive(uint8_t x) {
    Serial.println(x);
}

int x = 0;
hack::time_us_t y;

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
        sender(x);
        sender(255 - x);
        Serial.print("in:");
        Serial.print(x);
        Serial.println(255 - x);
        x++;
    }

    sender.update();
    receiver.update();
}
