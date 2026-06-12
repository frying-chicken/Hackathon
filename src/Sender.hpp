#pragma once

#include <Arduino.h>
#include "RingContainer.hpp"
#include "clock.hpp"
#include "LinkConfig.hpp"

template<pin_size_t PIN>
class Sender
{
    enum class Mode {
        Idle,
        Start,
        Payload
    };

    RingContainer<byte, 256> _buffer;
    Clock<LinkConfig::HalfBitMicros> _clock;

    Mode _mode;
    bool _halfPhase;
    int _index;

public:
    Sender() :
        _buffer(),
        _clock(),
        _mode(Mode::Idle),
        _halfPhase(false),
        _index(0)
    {
        pinMode(PIN, OUTPUT);
    }

    void update(unsigned long now = micros()) {
        if (!_clock.update(now)) {
            return;
        }

        if (_mode == Mode::Idle) {
            digitalWrite(PIN, LOW);
            return;
        }

        if (_mode == Mode::Start) {
            digitalWrite(PIN, _index < static_cast<int>(LinkConfig::StartLowBits) ? LOW : HIGH);

            if (++_index == static_cast<int>(LinkConfig::StartPatternBits)) {
                _index = 0;
                _mode = Mode::Payload;
            }
            return;
        }

        if (_mode == Mode::Payload) {
            if (_buffer.empty()) {
                _mode = Mode::Idle;
                return;
            }

            bool data = bitRead(_buffer.front(), 7 - _index);
            bool bit = data ^ _halfPhase;
            digitalWrite(PIN, bit ? LOW : HIGH);

            _halfPhase = !_halfPhase;

            if (_halfPhase) {
                return;
            }

            if (++_index == 8) {
                _index = 0;
                _buffer.pop_front();
            }
        }
    }

    void operator()(byte x) {
        if (_mode == Mode::Idle) {
            _mode = Mode::Start;
        }

        _buffer.push_back(x);
    }
};
