#include <Arduino.h>
#include <cstdint>

#include "Config.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

void onReceive(uint8_t x) {
    Serial.println(x);
}

Sender<2> sender;
Receiver<A0> receiver(onReceive);

uint8_t next_payload = 1;
time_t last_send = 0;

void setup() {
    Serial.begin(250000);
    // const Config::Time serial_wait_limit = 3000000UL;
    // Config::Time wait_begin = micros();
    // while (!Serial && micros() - wait_begin < serial_wait_limit) {
    // }

    // sender.begin();
    Serial.println("Test");

    for(int i=0;i<100;++i){
        sender(i);
    }
}

void loop() {
    time_t n = micros();

    sender.update(n);
    receiver.update(n);
}
