#include <Arduino.h>

#include "Config.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

#include "Clock.hpp"

void onReceive(uint8_t x) {
    Serial.println(x);
}

hack::Receiver<A0,1> receiver(onReceive);

void setup() {
    Serial.begin(250000);
}

int c = 0;
time_t x =0;

void loop() {
    time_t s = micros();
    receiver.update();
    x+=micros()-s;
    if(++c == 1'000'000){
        Serial.print(x/1'000'000);
    }
}
