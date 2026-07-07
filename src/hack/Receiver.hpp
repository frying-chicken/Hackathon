#pragma once

#include <Arduino.h>
#include <cstdint>
#include <optional>

#include "Config.hpp"
#include "PrefixSumWindow.hpp"
#include "RingContainer.hpp"
#include "Types.hpp"
#include "Utility.hpp"

namespace hack
{
    template <pin_size_t PIN, size_t Capacity>
    class Receiver
    {
        enum class Mode
        {
            Idle,
            Start,
            Payload,
        };

        RingContainer<uint8_t, Capacity> _buffer;
        PrefixSumWindow<time_t, uint32_t, Config::buffer_size> _prefixSumWindow;

        struct DecodedBit
        {
            bool value;
            time_t edge;
        };

        void (*_callback)(const RingContainer<uint8_t, Capacity> &);
        Mode _mode = Mode::Idle;
        uint8_t _data = 0;
        size_t _index = 0;
        time_t _lastTime = 0;

    public:
        Receiver(void (*callback)(const RingContainer<uint8_t, Capacity> &))
            : _callback(callback ? callback : [](const RingContainer<uint8_t, Capacity> &) {})
        {
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
            if (isBefore(now))
                return;

            std::optional<bool> x = decode(now);
            if (!x)
                return;

            if (*x == true)
            {
                resetState();
                return;
            }

            if (++_index == Config::preamble_required)
            {
                _mode = Mode::Start;
                _data = 0;
                _index = 0;
            }

            _lastTime = now;
            return;
        }
    case Mode::Start:
    {
        std::optional<bool> x = receive(now);
        if (!x)
            return;

        shiftInBit(_data, *x);

        if (_data == Config::start)
        {
            _mode = Mode::Payload;
            _data = 0;
            _index = 0;
        }
        return;
    }
    case Mode::Payload:
    {
        std::optional<bool> x = receive(now);
        if (!x)
            return;

        writeBit(_data, _index, *x);

        if (++_index == bit_size(_data))
        {
            pushPacket();
        }
        return;
    }

    private:
        bool isBefore(time_t now) const
        {
            return now - _lastTime < Config::bit_us - Config::margin_us;
        }
        bool isAfter(time_t now) const
        {
            return Config::bit_us + Config::margin_us < now - _lastTime;
        }

        static bool isHigh(int x, int base)
        {
            return base < x;
        }

        std::optional<bool> decode(time_t now) const
        {
            if (now < Config::bit_us + Config::threshold_window_us)
                return std::nullopt;
            const time_t begin = now - Config::bit_us;
            const time_t middle = now - Config::half_bit_us;
            const time_t end = now;

            std::optional<int> base = _prefixSumWindow.average(begin - Config::threshold_window_us, begin);
            std::optional<int> first = _prefixSumWindow.average(begin + Config::half_bit_us / 2, middle);
            std::optional<int> second = _prefixSumWindow.average(middle, end - Config::half_bit_us / 2);

            if (!base || !first || !second)
                return std::nullopt;

            bool x = isHigh(*first, *base);
            if (x == isHigh(*second, *base))
                return std::nullopt;

            return x;
        }

        std::optional<bool> receive(time_t now)
        {
            if (isBefore(now))
                return std::nullopt;
            if (isAfter(now))
            {
                finishPacket();
                return std::nullopt;
            }

            std::optional<bool> x = decode(now);
            if (!x)
                return std::nullopt;

            _lastTime = now;
            return *x;
        }

        void pushPacket()
        {
            if (_buffer.full())
            {
                flushPacket();
            }

            _buffer.push_back(_data);

            _data = 0;
            _index = 0;
        }

        void finishPacket()
        {
            if (!_buffer.empty())
            {
                flushPacket();
            }
            resetState();
        }

        void flushPacket()
        {
            _callback(_buffer);
            _buffer.reset();
        }

        void resetState()
        {
            _mode = Mode::Idle;
            _data = 0;
            _index = 0;
            _lastTime = 0;
        }
    };
}
