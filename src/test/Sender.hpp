#pragma once

#include <Arduino.h>
#include <array>

#include "hack/Clock.hpp"
#include "hack/PrefixSumWindow.hpp"
#include "hack/Sender.hpp"

namespace sender_test
{
    inline hack::Sender<2, 256> sender;
    std::array<uint8_t, 256> frame = {};

    inline hack::Clock<1'000'000> beat_timer;

    inline void setup()
    {
        sender.begin();

        Serial.begin(250000);
        Serial.println("Start");

        for (int i = 0; i < 256; ++i)
            frame[i] = i;

        beat_timer.reset();
    }

    inline void loop()
    {
        sender.update();

        if (!beat_timer.update())
            return;

        sender(frame);
    }
}
