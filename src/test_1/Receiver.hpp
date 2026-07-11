#pragma once

#include <Arduino.h>
#include <array>

#include "hack/Receiver.hpp"

namespace receiver_test_1
{
    inline void onPacketReceived(const std::array<uint8_t, 256> &data)
    {
        for (size_t i = 0; i < data.size(); ++i)
            if (data[i] != i)
            {
                Serial.print("fail");
                Serial.print(i);
                Serial.print(" : ");
                Serial.println(data[i]);
                digitalWrite(LED_BUILTIN, HIGH);
                return;
            }
        Serial.println("success");
    }

    inline hack::Receiver<A0, 256> receiver;

    inline void setup()
    {
        Serial.begin(250000);
        Serial.println("Receiver Test");

        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);

        receiver.begin(onPacketReceived);
    }

    inline void loop()
    {
        receiver.update();
    }
}
