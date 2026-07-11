#pragma once

#include <Arduino.h>
#include <array>
#include <cstdint>
#include <optional>

#include "Config.hpp"
#include "PrefixSumWindow.hpp"
#include "Types.hpp"
#include "Utility.hpp"

namespace hack
{
    template <pin_size_t PIN, size_t Capacity>
    class Receiver
    {
        static_assert(0 < Capacity);

        enum class Mode
        {
            Idle,
            Start,
            Payload,
        };

        std::array<uint8_t, Capacity> _buffer = {};
        PrefixSumWindow<time_t, uint32_t, Config::buffer_size> _prefixSumWindow;

        static void noCallback(const std::array<uint8_t, Capacity> &) {}

        void (*_callback)(const std::array<uint8_t, Capacity> &) = noCallback;
        Mode _mode = Mode::Idle;
        uint8_t _data = 0;
        size_t _bitIndex = 0;
        size_t _packetSize = 0;
        time_t _lastTime = 0;

    public:
        void begin(void (*callback)(const std::array<uint8_t, Capacity> &) = nullptr)
        {
            _callback = callback ? callback : noCallback;
            _prefixSumWindow.reset();
            resetState();
        }

        void update()
        {
            const int value = analogRead(PIN);
            const time_t now = micros();
            _prefixSumWindow.push(now, value);

            switch (_mode)
            {
            case Mode::Idle:
                updateIdle(now);
                return;
            case Mode::Start:
                updateStart(now);
                return;
            case Mode::Payload:
                updatePayload(now);
                return;
            }
        }

    private:
        void updateIdle(time_t now)
        {
            Bit bit = detectPreambleBit(now);
            if (!bit)
                return;

            if (*bit == true)
            {
                resetState();
                return;
            }

            if (++_bitIndex == Config::preamble_required)
            {
                enterMode(Mode::Start);
            }
        }

        void updateStart(time_t now)
        {
            Bit bit = receiveBit(now);
            if (!bit)
                return;

            shiftInBit(_data, *bit);

            if (_data == Config::start)
            {
                enterMode(Mode::Payload);
            }
        }

        void updatePayload(time_t now)
        {
            Bit bit = receiveBit(now);
            if (!bit)
                return;

            writeBit(_data, _bitIndex, *bit);

            if (++_bitIndex == bit_size(_data))
            {
                _buffer[_packetSize] = _data;

                _data = 0;
                _bitIndex = 0;

                if (++_packetSize == _buffer.size())
                {
                    _callback(_buffer);
                    resetState();
                }
            }
        }

        Bit decode(time_t sampleTiming)
        {
            std::optional<std::pair<uint32_t, uint32_t>> window = readWindow(sampleTiming);
            if (!window)
                return std::nullopt;

            std::optional<uint32_t> base = readBaseline(sampleTiming);
            if (!base)
                return std::nullopt;

            bool bit = level(window->first, *base);
            if (bit == level(window->second, *base))
                return std::nullopt;

            _lastTime = sampleTiming;

            return bit;
        }

        Bit detectPreambleBit(time_t now)
        {
            if (_lastTime != 0)
            {
                if (isBefore(now))
                    return std::nullopt;

                if (isAfter(now))
                {
                    resetState();
                    return std::nullopt;
                }
            }

            return decode(now);
        }

        Bit receiveBit(time_t now)
        {
            if (isBefore(now))
                return std::nullopt;

            if (isAfter(now))
            {
                auto window = readWindow(_lastTime + Config::bit_us);
                if (!window)
                {
                    resetState();
                    return std::nullopt;
                }
                _lastTime += Config::bit_us;
                return window->second < window->first;
            }

            return decode(now);
        }

        bool isBefore(time_t now) const { return now - _lastTime < Config::bit_us - Config::margin_us; }
        bool isAfter(time_t now) const { return Config::bit_us + Config::margin_us < now - _lastTime; }

        static bool level(int x, int base) { return base < x; }

        std::optional<std::pair<uint32_t, uint32_t>> readWindow(time_t sampleTiming) const
        {
            if (sampleTiming <= Config::half_bit_us)
                return std::nullopt;
            const time_t middle = sampleTiming - Config::half_bit_us;

            if (middle <= Config::window_us)
                return std::nullopt;
            std::optional<uint32_t> first = _prefixSumWindow.average(middle - Config::window_us, middle);
            std::optional<uint32_t> second = _prefixSumWindow.average(middle, middle + Config::window_us);
            if (!first || !second)
                return std::nullopt;

            return std::make_pair(*first, *second);
        }

        std::optional<uint32_t> readBaseline(time_t sampleTiming) const
        {
            if (sampleTiming <= Config::bit_us)
                return std::nullopt;
            const time_t begin = sampleTiming - Config::bit_us;

            if (begin <= Config::threshold_window_us)
                return std::nullopt;
            std::optional<uint32_t> base = _prefixSumWindow.average(begin - Config::threshold_window_us, begin);
            if (!base)
                return std::nullopt;

            return *base;
        }

        void enterMode(Mode mode)
        {
            _mode = mode;
            _data = 0;
            _bitIndex = 0;
            _packetSize = 0;
        }

        void resetState()
        {
            enterMode(Mode::Idle);
            _lastTime = 0;
        }
    };
}
