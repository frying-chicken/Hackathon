#pragma once

#include <Arduino.h>
#include <array>
#include <cstdint>

#include "Clock.hpp"
#include "Config.hpp"
#include "Types.hpp"
#include "Utility.hpp"

namespace hack
{
    template <pin_size_t PIN, size_t Capacity>
    class Sender
    {
        enum class Mode
        {
            Idle,
            Calibration,
            Preamble,
            Start,
            Payload
        };

        std::array<uint8_t, Capacity> _buffer;
        Clock<Config::half_bit_us> _clock;

        Mode _mode = Mode::Idle;
        size_t _byteIndex = 0;
        bool _halfPhase = false;
        size_t _bitIndex = 0;

    public:
        void begin()
        {
            pinMode(PIN, OUTPUT);
            digitalWrite(PIN, LOW);
            _clock.reset();
        }

        void update()
        {
            if (!_clock.update())
                return;

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

        // Start sending one payload frame. Returns false while busy.
        bool operator()(const std::array<uint8_t, Capacity> &x)
        {
            if (_mode != Mode::Idle)
                return false;
            _mode = Mode::Calibration;
            _buffer = x;
            _byteIndex = 0;
            _bitIndex = 0;
            _halfPhase = false;
            return true;
        }

    private:
        void updateIdle() { digitalWrite(PIN, LOW); }

        void updateCalibration()
        {
            if (send(Config::calibration))
                _mode = Mode::Preamble;
        }

        void updatePreamble()
        {
            if (send(Config::preamble))
                _mode = Mode::Start;
        }

        void updateStart()
        {
            if (send(Config::start))
                _mode = Mode::Payload;
        }

        void updatePayload()
        {
            if (!send(_buffer[_byteIndex]))
                return;

            if (++_byteIndex == _buffer.size())
            {
                _mode = Mode::Idle;
                _byteIndex = 0;
                _bitIndex = 0;
                _halfPhase = false;
            }
        }

        template <typename T>
        bool send(T data)
        {
            const bool bit = readBit(data, _bitIndex) ^ _halfPhase;
            digitalWrite(PIN, bit ? HIGH : LOW);

            _halfPhase = !_halfPhase;
            if (_halfPhase)
                return false;

            if (++_bitIndex == bit_size(data))
            {
                _bitIndex = 0;
                return true;
            }
            return false;
        }
    };
}
