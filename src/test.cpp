#include <Arduino.h>

#include "Sender.hpp"
#include "Receiver.hpp"

Sender<2> sender;
Receiver<A0> receiver([](byte x) {Serial.println(x);});

void setup() {  
    Serial.begin(250000);
    Serial.println(".");

    sender(0);
    for (int i = 0;i < 100;++i) {
        sender(0);
    }

    RingContainer<byte,10> a;
    a.push_front(1);
    a.push_front(2);
    a.push_front(3);

    Serial.println(a.front());

}

void loop() {
    unsigned long n = micros();
    sender.update(n);
    receiver.update(n);
}