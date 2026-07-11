#pragma once

#include <Arduino.h>
#include <array>

#include "hack/Clock.hpp"
#include "hack/Sender.hpp"

namespace sender_test_1
{
    inline hack::Sender<2, 256> sender;
    inline std::array<uint8_t, 256> frame = {};

    inline hack::Clock<2'000'000> clock;

    inline void setup()
    {
        Serial.begin(250000);
        Serial.println("Sender Test");

        for (size_t i = 0; i < frame.size(); ++i)
            frame[i] = i;

        sender.begin();

        clock.reset();
    }

    inline void loop()
    {
        sender.update();

        if (clock.update())
        {
            sender(frame);
        }
    }
}
