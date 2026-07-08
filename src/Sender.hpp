#pragma once

#include <Arduino.h>
#include <array>
#include <algorithm>
#include <optional>

#include "Config.hpp"
#include "Utility.hpp"
#include "hack/PrefixSumWindow.hpp"
#include "hack/Sender.hpp"

namespace sender_app
{
    inline hack::Sender<app::sender_pin, app::frame_size> sender;
    inline std::array<uint8_t, app::frame_size> frame = {};

    inline hack::PrefixSumWindow<hack::time_t, uint32_t, 256> bpm_window;

    inline uint8_t bpm = 100;
    inline uint8_t beat_count = 0;

    inline hack::Timer beat_timer;

    inline bool isBoot = false;

    inline void updateBpmFromAnalog()
    {
        const hack::time_t now = micros();
        std::optional<uint32_t> average = bpm_window.average(now - 1000, now);
        if (!average)
            return;

        bpm = std::clamp(*average / 4, static_cast<uint32_t>(10), static_cast<uint32_t>(200));
    }

    inline void setup()
    {
        sender.begin();

        Serial.begin(app::serial_baud);
        Serial.println("Start");
        beat_timer.reset();
    }

    inline void loop()
    {
        sender.update();

        const int value = analogRead(app::receiver_pin);
        const hack::time_t now = micros();
        bpm_window.push(now, value);

        const hack::time_t periodUs = app::period(bpm);
        if (!beat_timer.update(periodUs))
            return;

        if (!isBoot)
        {
            isBoot = true;
            frame[0] = 255;
            frame[1] = 0;
            sender(frame);
            return;
        }

        updateBpmFromAnalog();

        frame[0] = 0;
        for (hack::size_t i = 0; i < 8; ++i)
        {
            if (i < beat_count / 8)
            {
                hack::writeBit(frame[0], i, true);
            }
        }
        frame[1] = bpm;

        if (sender(frame))
            ++beat_count;
    }
}
