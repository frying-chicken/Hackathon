#pragma once

#include <Arduino.h>
#include <array>

#include "hack/Receiver.hpp"

namespace receiver_test
{
    inline void onPacketReceived(const std::array<uint8_t, 256> &data)
    {
        digitalWrite(2, HIGH);
    }

    inline hack::Receiver<A0, 256> receiver;

    inline void setup()
    {
        pinMode(2, OUTPUT);
        digitalWrite(2, LOW);

        receiver.begin(onPacketReceived);
    }

    inline void loop()
    {
        receiver.update();
    }
}
