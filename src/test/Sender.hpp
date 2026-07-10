#pragma once

#include <Arduino.h>
#include <array>
#include <algorithm>

#include "hack/Clock.hpp"
#include "hack/PrefixSumWindow.hpp"
#include "hack/Sender.hpp"

namespace sender_test
{
    inline hack::Sender<2, 256> sender;
    std::array<uint8_t, 256> frame = {};

    inline void setup()
    {
        Serial.begin(250000);

        sender.begin();

        pinMode(4, INPUT);
        pinMode(5, INPUT);
        pinMode(6, INPUT);
        pinMode(7, INPUT);
        pinMode(8, INPUT);

        for (size_t i = 0; i < frame.size(); ++i)
            frame[i] = i;

        sender(frame);
    }

    std::array<hack::time_t, 5> times;
    size_t index = 0;

    inline void loop()
    {
        sender.update();

        if (digitalRead(4) == HIGH && times[0] == 0)
        {
            times[0] = micros();
            index++;
        }
        if (digitalRead(5) == HIGH && times[1] == 0)
        {
            times[1] = micros();
            index++;
        }
        if (digitalRead(6) == HIGH && times[2] == 0)
        {
            times[2] = micros();
            index++;
        }
        if (digitalRead(7) == HIGH && times[3] == 0)
        {
            times[3] = micros();
            index++;
        }
        if (digitalRead(8) == HIGH && times[4] == 0)
        {
            times[4] = micros();
            index++;
        }

        if (index == 4)
        {
            hack::time_t max_time = std::max({times[0], times[1], times[2], times[3], times[4]});
            hack::time_t min_time = std::min({times[0], times[1], times[2], times[3], times[4]});

            Serial.println(max_time - min_time);
        }
    }
}
