#pragma once

#include <Arduino.h>
#include <cstdint>

#include "Clock.hpp"
#include "Config.hpp"
#include "RingContainer.hpp"
#include "Utility.hpp"

namespace hack {
    template<pin_size_t PIN, size_t Capacity>
    class Sender
    {
        enum class Mode {
            Idle,
            Calibration,
            Preamble,
            Start,
            Payload
        };

        RingContainer<uint8_t, Capacity> _buffer;
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
            if (!_clock.update(now)) return;

            switch (_mode)
            {
            case Mode::Idle:
                digitalWrite(PIN, LOW);
                return;
            case Mode::Calibration:
                if (send(Config::calibration)) {
                    _mode = Mode::Preamble;
                }
                return;
            case Mode::Preamble:
                if (send(Config::preamble)) {
                    _mode = Mode::Start;
                }
                return;
            case Mode::Start:
                if (send(Config::start)) {
                    _mode = Mode::Payload;
                }
                return;
            case Mode::Payload:
                if (send(_buffer.front())) {
                    _buffer.pop_front();

                    if (_buffer.empty()) {
                        _mode = Mode::Idle;
                    }
                }
                return;
            }
        }

        bool operator()(uint8_t x) {
            if (!_buffer.push_back(x)) return false;

            if (_mode == Mode::Idle) {
                _mode = Mode::Calibration;
            }
            return true;
        }

    private:
        template<typename T>
        bool send(T data) {
            bool bit = readBit(data, _index) ^ _halfPhase;
            digitalWrite(PIN, !bit ? LOW : HIGH);

            _halfPhase = !_halfPhase;
            if (_halfPhase) return false;

            if (++_index == bit_size(data)) {
                _index = 0;
                return true;
            }
            return false;
        }
    };
}
