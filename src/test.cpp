#include <Arduino.h>
#include <cstdint>

#include "Config.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

#include "Clock.hpp"

void onReceive(uint8_t x) {
    Serial.println(x);
}

Sender<2> sender;
Receiver<A0> receiver(onReceive);

Clock<500'000> clock;

uint8_t next_payload = 1;
time_t last_send = 0;

void setup() {
    Serial.begin(250000);

    Serial.println("Test");

    for(int i=0;i<100;++i){
        sender(i);
    }
}

void loop() {
    time_t n = micros();

    if(clock.update(n)){
        sender(1);
    }

    sender.update(n);
    receiver.update(n);
}
