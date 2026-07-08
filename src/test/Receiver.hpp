#pragma once

#include <Arduino.h>
#include <array>

#include "hack/Receiver.hpp"

namespace receiver_test
{
    inline void onPacketReceived(const std::array<uint8_t, 256> &data)
    {
        for (int i = 0; i < 256; ++i)
        {
            Serial.println(data[i]);
        }
    }

    inline hack::Receiver<A0, 256> receiver;

    inline void setup()
    {
        Serial.begin(250000);
        receiver.begin(onPacketReceived);
    }

    inline void loop()
    {
        receiver.update();
    }
}
