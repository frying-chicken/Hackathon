#pragma once

#include <Arduino.h>
#include <climits>
#include <cstdint>
#include <stdexcept>

#include "PrefixSumWindow.hpp"
#include "Config.hpp"
#include "Utility.h"

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

    PrefixSumWindow<time_us_t, int, 1024> _prefixSumWindow;

    time_us_t _lastTime = 0;

public:
    Receiver(Callback callback)
        : _callback(callback)
    {
        if (_callback == nullptr) {
            _callback = [](uint8_t) {};
        }
    }

    void update(time_us_t now = micros()) {
        const int sample = analogRead(PIN);
        _prefixSumWindow.push(now, sample);

        switch (_mode) {
        case Mode::Idle: {
            int baseline = average(now - 450 * 16, 450 * 8);

            uint16_t x = 0;
            for (size_t i = 0;i < bit_size(Config::start_pattern);++i) {
                bool y = read(now - i * Config::half_bit_us, baseline);
                writeBit(x, i, y);
            }
            Serial.println(x);
            if (x == Config::start_pattern) {
                _mode = Mode::Payload;
                _lastTime = now;
            }
            return;
        }
        case Mode::Payload: {
            Serial.println("debug");
            if (now < _lastTime + Config::bit_us - 150) {
                return;
            }

            if (_lastTime + Config::bit_us + 150 < now) {
                _mode = Mode::Idle;
                _index = 0;
                return;
            }

            int baseline = average(now - Config::bit_us, 450 * 4);

            bool first = read(now - Config::half_bit_us, baseline);
            bool second = read(now, baseline);

            if (first == second) {
                return;
            }

            writeBit(_data, _index, first);

            if (++_index == bit_size<uint8_t>()) {
                _callback(_data);
                _lastTime = now;
                _index = 0;
            }
        }
        }
    }

private:
    int average(time_us_t end, time_us_t duration) {
        return _prefixSumWindow.average(end - duration, end);
    }

    bool read(time_us_t end, int baseline) {
        return baseline < average(end, Config::half_bit_us);
    }
};
