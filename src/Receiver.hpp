#pragma once

#include <Arduino.h>
#include <climits>
#include <cstdint>

#include "PrefixSumWindow.hpp"
#include "Config.hpp"

template<pin_size_t PIN>
class Receiver
{
    enum class Mode {
        Idle,
        Payload
    };

public:
    using Callback = void (*)(uint8_t);

private:
    Callback _callback;

    Mode _mode = Mode::Idle;

    uint8_t _data = 0;
    size_t _index = 0;

    PrefixSumWindow<Config::Time, int, Config::receiver_sample_capacity> _prefixSumWindow;

    Config::Time _lastTime = 0;

public:
    Receiver(Callback callback)
        : _callback(callback)
    {
    }

    void update(Config::Time now = micros()) {
        _prefixSumWindow.push(now, analogRead(PIN));

        switch (_mode) {
        case Mode::Idle:
            if (_lastTime + Config::start_detect_micros < now && detectStartPattern(now, baseline)) {
                _mode = Mode::Payload;
                _lastTime = now;
            }
            return;

        case Mode::Payload:
            if (_lastTime + Config::bit_micros < now) {
                bool x = isHigh(now - Config::bit_micros, now - Config::half_bit_micros, baseline);
                bool y = isHigh(now - Config::half_bit_micros, now, baseline);

                if (x == y) {
                    _mode = Mode::Idle;
                    _index = 0;
                    return;
                }

                write(_data, _index, x);
                _lastTime = now;

                if (++_index == bit_size<uint8_t>()) {
                    _callback(_data);
                    _index = 0;
                }
            }
        }
    }

private:
    bool detectStartPattern(Config::Time now, int baseline) const {
        bool a = isHigh(
            now - Config::half_bits_to_micros(Config::start_detect_half_bits),
            now - Config::half_bits_to_micros(Config::start_first_low_end_half_bits),
            baseline);
        bool b = isHigh(
            now - Config::half_bits_to_micros(Config::start_first_low_end_half_bits),
            now - Config::half_bits_to_micros(Config::start_first_high_end_half_bits),
            baseline);
        bool c = isHigh(
            now - Config::half_bits_to_micros(Config::start_first_high_end_half_bits),
            now - Config::half_bits_to_micros(Config::start_second_low_end_half_bits),
            baseline);
        bool d = isHigh(
            now - Config::half_bits_to_micros(Config::start_second_low_end_half_bits),
            now,
            baseline);

        return !a && b && !c && d;
    }

    bool isHigh(Config::Time begin, Config::Time end, int baseline) const {
        return baseline < _prefixSumWindow.average(begin, end);
    }

    template<typename T>
    void write(T& x, size_t index, bool y) {
        bitWrite(x, bit_size<T>() - 1 - index, y);
    }
    template<typename T>
    static constexpr size_t bit_size() {
        return sizeof(T) * CHAR_BIT;
    }
};
