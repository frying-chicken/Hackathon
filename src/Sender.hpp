#pragma once

#include <Arduino.h>

#include "Clock.hpp"
#include "Config.hpp"
#include "RingContainer.hpp"
#include "Utility.hpp"

namespace hack {
    template<pin_size_t PIN, typename T = uint8_t>
    class Sender
    {
        enum class Mode {
            Idle,
            Preamble,
            Start,
            Payload
        };

        RingContainer<T, 256> _buffer;
        Clock<Config::half_bit_us> _clock;

        Mode _mode = Mode::Idle;
        bool _halfPhase = false;
        size_t _index = 0;

    public:
        Sender() {
            pinMode(PIN, OUTPUT);
            digitalWrite(PIN, LOW);
        }

        void update(time_t now = micros()) {
            if (!_clock.update(now)) {
                return;
            }

            switch (_mode) {
            case Mode::Idle: {
                digitalWrite(PIN, LOW);
                return;
            }
            case Mode::Preamble: {
                if (sendPreamble()) {
                    _mode = Mode::Start;
                }
                return;
            }
            case Mode::Start: {
                if (sendValue(Config::start_pattern)) {
                    _mode = Mode::Payload;
                }
                return;
            }
            case Mode::Payload: {
                if (!sendValue(_buffer.front())) {
                    return;
                }
                _buffer.pop_front();

                if (_buffer.empty()) {
                    _mode = Mode::Idle;
                    digitalWrite(PIN, LOW);
                }
            }
            }
        }

        bool operator()(T x) {
            if (_mode == Mode::Idle) {
                _mode = Mode::Preamble;
            }
            return _buffer.push_back(x);
        }

    private:
        bool sendPreamble() {
            return sendHalfBit(false, 64);
        }

        template<typename U>
        bool sendValue(U value) {
            return sendHalfBit(readBit(value, _index), bit_size(value));
        }

        bool sendHalfBit(bool data, size_t size) {
            bool bit = data ^ _halfPhase;
            digitalWrite(PIN, !bit ? LOW : HIGH);

            _halfPhase = !_halfPhase;
            if (_halfPhase) {
                return false;
            }

            if (++_index == size) {
                _index = 0;
                return true;
            }
            return false;
        }
    };
}
