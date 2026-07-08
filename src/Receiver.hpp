#pragma once

#include <Arduino.h>
#include <array>

#include "Config.hpp"
#include "Utility.hpp"
#include "SheetMusic.hpp"
#include "hack/Receiver.hpp"

namespace receiver_app
{
    inline hack::time_t beat_timing_us = 0;
    inline hack::time_t beat_period_us = 0;
    inline int beat_count = 0;

    inline hack::size_t note_index = 0;

    inline void onPacketReceived(const std::array<uint8_t, app::frame_size> &data)
    {
        if (!hack::readBit(data[0], app::machine_id_bit))
            return;

        beat_timing_us = micros();
        beat_period_us = app::period(data[1]);
        ++beat_count;
    }

    inline void emitNoteCsv(const NoteEvent &note)
    {
        Serial.print(note.amplitude, 0);
        Serial.print(",");
        Serial.print(note.pitch, 0);
        Serial.print(",");
        Serial.println(note.durationBeat * static_cast<float>(beat_period_us), 0);
    }

    inline hack::Receiver<app::receiver_pin, app::frame_size> receiver;

    inline void setup()
    {
        Serial.begin(app::serial_baud);
        receiver.begin(onPacketReceived);
    }

    inline void loop()
    {
        receiver.update();

        while (note_index < sheet_music.size())
        {
            const NoteEvent &note = sheet_music[note_index];

            const double beatOffset = note.startBeat - static_cast<double>(beat_count);
            if (1.0 < beatOffset)
                return;

            if (beatOffset < 0.0)
            {
                ++note_index;
                continue;
            }

            if (beat_period_us == 0)
                return;

            const double beatProgress = static_cast<double>(micros() - beat_timing_us) / static_cast<double>(beat_period_us);
            if (beatOffset <= beatProgress)
            {
                emitNoteCsv(note);
                ++note_index;
                continue;
            }
            return;
        }
    }
}
