#pragma once

#include <Arduino.h>

#include "RingContainer.hpp"
#include "clock.hpp"
#include "LinkConfig.hpp"

template<pin_size_t PIN>
class Receiver
{
    enum class Mode {
        Idle,
        Payload
    };

public:
    using Callback = void (*)(byte);

    Clock<LinkConfig::SampleMicros> clock;

    RingContainer<byte, 256> _buffer;

private:
    Callback _callback;

    Mode _mode = Mode::Idle;

    byte data = 0;
    size_t index = 0;
public:
    Receiver(Callback callback)
        : _callback(callback)
    {
    }

    void update(unsigned long now = micros()) {
        if (!clock.update(now)) {
            return;
        }

        int value = analogRead(PIN);
        int baseline = _baseline.average();
        _baseline.update(value);

        _buffer.push_front(value);

        if (_mode == Mode::Idle) {
            if (_buffer.size() < 3 * 8) {
                return;
            }

            int s = 0;
            for (size_t i = 0; i < 3 * 4; ++i) {
                s += _buffer[i];
            }

            bool high = baseline < s / 3 * 4;

            if (high) {
                _mode = Mode::Payload;
                _buffer.reset();
            }

            return;
        }

        if (_mode == Mode::Payload) {
            if (_buffer.size() < 6) {
                return;
            }

            int s0 = 0;
            for (size_t i = 0; i < 3; ++i) {
                s0 += _buffer[i];
            }
            int s1 = 0;
            for (size_t i = 3; i < 6; ++i) {
                s1 += _buffer[i];
            }

            bool h0 = baseline < s0 / 3;
            bool h1 = baseline < s1 / 3;

            if (h0 != h1) {
                bitWrite(data, 7 - index, h1);
                if (++index == 8) {
                    index = 0;

                    _callback(data);
                    data = 0;
                    _buffer.reset();
                }
            }
        }
    }
};
