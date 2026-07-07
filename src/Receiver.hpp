#pragma once

#include <Arduino.h>
#include <array>

#include "Config.hpp"
#include "SheetMusic.hpp"
#include "hack/Receiver.hpp"

namespace receiver_app {
    constexpr int kSheetRows = static_cast<int>(sheetMusic.size());

    int noteIndex = 0;
    int rowIndex = 0;

    int beatIndex = 0;
    hack::time_t beatTiming = 0;
    hack::time_t periodUs = 0;

    void onPacket(const std::array<uint8_t, FrameSize> &data)
    {
        if (!hack::readBit(data[0], ReceiverIdBit)) {
            return;
        }
        if (data[1] == 0) {
            return;
        }

        ++beatIndex;
        beatTiming = micros();
        periodUs = 60 * 1'000'000 / data[1];
    }

    hack::Receiver<ReceiverPin, FrameSize> receiver(onPacket);

    void setup()
    {
        Serial.begin(SerialBaud);
        receiver.begin();
    }

    void loop()
    {
        receiver.update();

        if (rowIndex >= kSheetRows)
        {
            return;
        }

        if (noteIndex >= kCols)
        {
            noteIndex = 0;
            ++rowIndex;
            return;
        }

        const NoteEvent note = sheetMusic[rowIndex][noteIndex];

        if (note.isRest())
        {
            noteIndex = 0;
            ++rowIndex;
            return;
        }

        if (periodUs == 0)
        {
            return;
        }

        const double beatOffset = static_cast<double>(note.startBeat) - static_cast<double>(beatIndex);
        if (beatOffset < 0.0 || 1.0 < beatOffset)
        {
            return;
        }

        const double beatProgress = static_cast<double>(micros() - beatTiming) / static_cast<double>(periodUs);
        if (beatOffset < beatProgress)
        {
            Serial.print(note.amplitude, 0);
            Serial.print(",");
            Serial.print(note.pitch, 0);
            Serial.print(",");
            Serial.println(note.durationBeat * static_cast<float>(periodUs), 0);

            ++noteIndex;
        }
    }
}
