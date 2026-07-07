#pragma once

#include <Arduino.h>
#include <cstdint>

#include "Clock.hpp"
#include "Config.hpp"
#include "RingContainer.hpp"
#include "Types.hpp"
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
        void begin() {
            pinMode(PIN, OUTPUT);
            digitalWrite(PIN, LOW);
            _clock.reset();
        }

        void update() {
            if (!_clock.update()) return;

            switch (_mode)
            {
            case Mode::Idle:
                updateIdle();
                return;
            case Mode::Calibration:
                updateCalibration();
                return;
            case Mode::Preamble:
                updatePreamble();
                return;
            case Mode::Start:
                updateStart();
                return;
            case Mode::Payload:
                updatePayload();
                return;
            }
        }

        bool operator()(uint8_t x) {
            if (_buffer.full()) return false;
            _buffer.push_back(x);

            if (_mode == Mode::Idle) {
                _mode = Mode::Calibration;
            }
            return true;
        }

    private:
        void updateIdle() {
            digitalWrite(PIN, LOW);
        }

        void updateCalibration() {
            if (send(Config::calibration)) {
                _mode = Mode::Preamble;
            }
        }

        void updatePreamble() {
            if (send(Config::preamble)) {
                _mode = Mode::Start;
            }
        }

        void updateStart() {
            if (send(Config::start)) {
                _mode = Mode::Payload;
            }
        }

        void updatePayload() {
            if (!send(_buffer.front())) return;

            _buffer.pop_front();
            if (_buffer.empty()) {
                _mode = Mode::Idle;
            }
        }

        template<typename T>
        bool send(T data) {
            const bool bit = readBit(data, _index) ^ _halfPhase;
            digitalWrite(PIN, bit ? HIGH : LOW);

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
