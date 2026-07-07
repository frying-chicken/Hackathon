#pragma once

#include <Arduino.h>
#include <array>
#include <optional>

#include "Config.hpp"
#include "hack/PrefixSumWindow.hpp"
#include "hack/Sender.hpp"

namespace sender_app {
    hack::Sender<SenderPin, FrameSize> sender;
    hack::PrefixSumWindow<hack::time_t, uint32_t, 256> prefixSumWindow;

    std::array<uint8_t, FrameSize> frame = {};

    hack::time_t lastBeatUs = 0;
    int bpm = 100;
    hack::time_t periodUs = 60 * 1'000'000 / bpm;
    int beat = 0;

    void setup() {
        sender.begin();

        Serial.begin(SerialBaud);
        Serial.println("Start");
        lastBeatUs = micros();

        delay(100);
    }

    void loop() {
        sender.update();

        const hack::time_t now = micros();
        const int value = analogRead(A0);
        prefixSumWindow.push(now, value);

        if (now - lastBeatUs < periodUs) {
            return;
        }

        frame.fill(0);
        hack::writeBit(frame[0], ReceiverIdBit, true);
        frame[1] = static_cast<uint8_t>(bpm);
        frame[2] = static_cast<uint8_t>(beat++);

        static_cast<void>(sender.operator()(frame));

        std::optional<uint32_t> x = prefixSumWindow.average(0, now);
        bpm = x ? static_cast<int>(*x / 4) : 120;
        bpm = 120;
        periodUs = 60 * 1'000'000 / bpm;
        lastBeatUs = now;
    }
}

