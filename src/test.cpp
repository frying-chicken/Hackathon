#include <Arduino.h>
#include <cstdint>

#include "Config.hpp"
#include "Sender.hpp"

Sender<Config::sender_pin> sender;
//Receiver<A0> receiver([](uint8_t x) {Serial.println(x);});

void setup() {  
    Serial.begin(250000);
    Serial.println(".");

    sender(0);
    for (size_t i = 0; i < Config::startup_payload_count; ++i) {
        sender(0);
    }

    RingContainer<uint8_t, Config::debug_ring_capacity> a;
    a.push_front(1);
    a.push_front(2);
    a.push_front(3);

    Serial.println(a.front());

}

void loop() {
    Config::Time n = micros();
    sender.update(n);
//    receiver.update(n);
}
