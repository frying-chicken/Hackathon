#pragma once

#include <Arduino.h>
#include <array>

#include "hack/Config.hpp"
#include "hack/Sender.hpp"

namespace sender_test_0
{
    constexpr size_t num_runs = 100;

    inline hack::Sender<2, 256> sender;
    inline std::array<uint8_t, 256> frame = {};

    inline std::array<hack::time_t, 2> times = {0, 0};
    inline hack::time_t total_diff = 0;
    inline size_t run_count = 0;
    inline bool waiting = false;

    inline hack::Clock<1'000'000> clock;

    inline void setup()
    {
        Serial.begin(250000);
        Serial.println("Sender Test");

        sender.begin();

        pinMode(4, INPUT);
        pinMode(5, INPUT);

        for (size_t i = 0; i < frame.size(); ++i)
            frame[i] = i;

        clock.reset();
    }

    inline void loop()
    {
        sender.update();

        if (waiting)
        {
            if (digitalRead(4) == HIGH && times[0] == 0)
                times[0] = micros();
            if (digitalRead(5) == HIGH && times[1] == 0)
                times[1] = micros();

            if (times[0] != 0 && times[1] != 0)
            {
                hack::time_t diff = times[0] < times[1]
                    ? times[1] - times[0]
                    : times[0] - times[1];
                total_diff += diff;

                if (++run_count == num_runs)
                {
                    Serial.println(total_diff / num_runs);
                    total_diff = 0;
                    run_count = 0;
                }

                times = {0, 0};
                waiting = false;
            }
        }
        if (!waiting && clock.update())
        {
            sender(frame);
            waiting = true;
        }
    }
}
