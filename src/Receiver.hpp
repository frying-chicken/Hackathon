#pragma once

#include <Arduino.h>
#include <climits>
#include <cstdint>
#include <type_traits>

#include "PrefixSumWindow.hpp"
#include "Config.hpp"
#include "Utility.hpp"

namespace hack {
    template<pin_size_t PIN, typename T>
    class Receiver
    {
        enum class Mode {
            Idle,
            Start,
            Payload
        };

    public:
        using Callback = void (*)(T);

    private:
        Callback _callback;

        T _data = 0;
        size_t _index = 0;

        Mode _mode = Mode::Idle;

        PrefixSumWindow<time_t, std::make_unsigned_t<T>, 1024 * 2> _prefixSumWindow;

        time_t _lastTime = 0;

        bool _level = true;
        time_t _bit_us = 0;

    public:
        Receiver(Callback callback)
            : _callback(callback)
        {
            if (_callback == nullptr) {
                _callback = [](T) {};
            }
        }

        void update(time_t now = micros()) {
            const int sample = analogRead(PIN);
            _prefixSumWindow.push(now, sample);

            int baseline = _prefixSumWindow.average(now - Config::baseline_us, now);

            switch (_mode) {
            case Mode::Idle: {
                bool level = baseline < sample;

                if (level == _level) {
                    return;
                }
                _level = level;

                if (level == true) {
                    return;
                }

                if (_lastTime != 0) {
                    _bit_us += now - _lastTime;
                }
                _lastTime = now;

                if (++_index == 16 + 1) {
                    _index = 0;
                    _bit_us /= 16;

                    _mode = Mode::Start;
                }
                return;
            }
            case Mode::Start: {
                if(read(now, baseline)){
                    //Serial.println(_data);
                }
                if (_data == Config::start_pattern) {
                    _index = 0;
                    _mode = Mode::Payload;
                }
                return;
            }
            case Mode::Payload: {
                if (read(now, baseline)) {
                    _callback(_data);
                }
                return;
            }
            }
        }

    private:
        bool read(time_t now, int baseline) {
            if (now < _lastTime + _bit_us - 100) {
                return false;
            }

            if (_lastTime + _bit_us + 100 < now) {
                changeIdle();
                _index = 0;
                return false;
            }

            bool first = baseline < _prefixSumWindow.average(now - _bit_us + 200, now - _bit_us / 2);
            bool second = baseline < _prefixSumWindow.average(now - _bit_us / 2, now - 200);

            if (first == second) {
                return false;
            }

            _data = (_data << 1) | (first ? 0 : 1);
            _lastTime += _bit_us;

            if (++_index == bit_size(_data)) {
                _index = 0;
                return true;
            }
            return false;
        }

        void changeIdle() {
            _mode = Mode::Idle;
            _data = 0;
            _index = 0;
            _lastTime = 0;
            _bit_us = 0;
        }
    };
}
