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
        void begin(void (*callback)(const std::array<uint8_t, Capacity> &))
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
            std::optional<bool> bit = receive(now);
            if (!bit)
                return;

            if (*bit == true)
            {
                resetState();
                return;
            }

            if (++_bitIndex == Config::preamble_required)
            {
                _mode = Mode::Start;
                _bitIndex = 0;
            }
        }

        void updateStart(time_t now)
        {
            std::optional<bool> bit = receive(now);
            if (!bit)
                return;

            shiftInBit(_data, *bit);

            if (_data == Config::start)
            {
                _mode = Mode::Payload;
                _data = 0;
                _bitIndex = 0;
            }
        }

        void updatePayload(time_t now)
        {
            std::optional<bool> bit = receive(now);
            if (!bit)
                return;

            writeBit(_data, _bitIndex, *bit);

            if (++_bitIndex == bit_size(_data))
                pushPacket();
        }

        bool isBefore(time_t now) const { return now - _lastTime < Config::bit_us - Config::margin_us; }
        bool isAfter(time_t now) const { return Config::bit_us + Config::margin_us < now - _lastTime; }

        static bool level(int x, int base) { return base < x; }

        std::optional<bool> decode(time_t sample_timing, bool useBaseline) const
        {
            if (sample_timing < Config::half_bit_us + Config::window_us)
                return std::nullopt;

            const time_t middle = sample_timing - Config::half_bit_us;

            std::optional<uint32_t> first = _prefixSumWindow.average(middle - Config::window_us, middle);
            std::optional<uint32_t> second = _prefixSumWindow.average(middle, middle + Config::window_us);

            if (!first || !second)
                return std::nullopt;

            if (useBaseline)
            {
                if (sample_timing < Config::bit_us + Config::threshold_window_us)
                    return std::nullopt;

                const time_t begin = sample_timing - Config::bit_us;

                std::optional<uint32_t> base = _prefixSumWindow.average(begin - Config::threshold_window_us, begin);

                if (!base)
                    return std::nullopt;

                bool bit = level(*first, *base);
                if (bit == level(*second, *base))
                    return std::nullopt;

                return bit;
            }

            return *first < *second;
        }

        std::optional<bool> receive(time_t now)
        {
            bool useBaseline = true;
            time_t sampleUs = now;

            if (_lastTime != 0)
            {
                if (isBefore(now))
                    return std::nullopt;

                if (isAfter(now))
                {
                    //sampleUs = _lastTime + Config::bit_us;
                    useBaseline = false;
                }
            }

            std::optional<bool> bit = decode(sampleUs,true);// useBaseline);
            if (!bit)
            {
                if (!useBaseline)
                    resetState();
                return std::nullopt;
            }

            _lastTime = sampleUs;
            return *bit;
        }

        void pushPacket()
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

        void resetState()
        {
            _mode = Mode::Idle;
            _data = 0;
            _bitIndex = 0;
            _packetSize = 0;
            _lastTime = 0;
        }
    };
}
