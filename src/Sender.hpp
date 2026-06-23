#pragma once

#include <Arduino.h>
#include <cstdint>
#include "Clock.hpp"
#include "Config.hpp"
#include "RingContainer.hpp"

namespace hack {
    template<pin_size_t PIN>
    class Sender
    {
        enum class Mode {
            Idle,
            Preamble,
            Start,
            Payload
        };

        RingContainer<uint8_t, 256> _buffer;
        Clock<Config::half_bit_us> _clock;

        Mode _mode;
        bool _halfPhase;
        size_t _index;

    public:
        Sender() :
            _buffer(),
            _clock(),
            _mode(Mode::Idle),
            _halfPhase(false),
            _index(0)
        {
            pinMode(PIN, OUTPUT);
            digitalWrite(PIN, LOW);
        }

        void update(time_us_t now = micros()) {
            if (!_clock.update(now)) {
                return;
            }

            switch (_mode) {
            case Mode::Idle:
                digitalWrite(PIN, LOW);
                return;

            case Mode::Preamble:
                if (write(Config::preamble)) {
                    _mode = Mode::Start;
                }
                return;

            case Mode::Start:
                if (write(Config::start_pattern)) {
                    _mode = Mode::Payload;
                }
                return;

            case Mode::Payload:
                if (_buffer.empty()) {
                    _mode = Mode::Idle;
                    digitalWrite(PIN, LOW);
                    return;
                }

                if (write(_buffer.front())) {
                    _buffer.pop_front();
                }
            }
        }

        bool operator()(uint8_t x) {
            if (_buffer.full()) {
                return false;
            }

            if (_mode == Mode::Idle) {
                _mode = Mode::Preamble;
            }
            _buffer.push_back(x);
            return true;
        }
    private:
        template<typename T>
        bool write(T x) {
            bool data = readBit(x, _index);
            bool bit = data ^ _halfPhase;
            digitalWrite(PIN, !bit ? LOW : HIGH);

            _halfPhase = !_halfPhase;
            if (_halfPhase) {
                return false;
            }

            if (++_index == bit_size(x)) {
                _index = 0;
                return true;
            }
            return false;
        }
    };
}
