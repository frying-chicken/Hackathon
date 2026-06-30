#pragma once

#include <Arduino.h>
#include <cstdint>
#include <optional>

#include "Config.hpp"
#include "PrefixSumWindow.hpp"
#include "Utility.hpp"

namespace hack {
    template<pin_size_t PIN>
    class Receiver
    {
        enum class Mode {
            Idle,
            Start,
            Payload,
        };

        PrefixSumWindow<time_t, unsigned int, Config::buffer_size> _prefixSumWindow;

        void (*_callback)(uint8_t);
        Mode _mode = Mode::Idle;
        uint8_t _data = 0;
        size_t _index = 0;
        time_t _lastTime = 0;

    public:
        Receiver(void(*callback)(uint8_t))
            : _callback(callback ? callback : [](uint8_t) {}) {
        }

        void update(time_t now = micros()) {
            _prefixSumWindow.push(now, analogRead(PIN));

            switch (_mode) {
            case Mode::Idle: {
                if (isBefore(now)) return;

                std::optional<bool> x = decode(now);
                if (!x)return;

                if (*x == true) {
                    changeIdle();
                    return;
                }

                _lastTime = now;

                if (++_index == Config::read_preamble_size) {
                    _mode = Mode::Start;
                    _data = 0;
                    _index = 0;
                }
                return;
            }
            case Mode::Start: {
                std::optional<bool> x = receive(now);
                if (!x)return;

                _data = (_data << 1) | *x;

                if (_data == Config::start) {
                    _mode = Mode::Payload;
                    _data = 0;
                    _index = 0;
                }
                return;
            }
            case Mode::Payload: {
                std::optional<bool> x = receive(now);
                if (!x)return;

                writeBit(_data, _index, *x);
                if (++_index == bit_size(_data)) {
                    _callback(_data);
                    _data = 0;
                    _index = 0;
                }
                return;
            }
            }
        }

    private:
        bool isBefore(time_t now) const {
            return now < _lastTime + Config::bit_us - Config::margin_us;
        }

        bool isAfter(time_t now) const {
            return _lastTime + Config::bit_us + Config::margin_us < now;
        }

        static bool isLow(int x, int base) {
            return x < base;
        }

        std::optional<bool> decode(time_t now) const {
            const time_t begin = now - Config::bit_us;
            const time_t middle = now - Config::half_bit_us;
            const time_t end = now;

            std::optional<int> base = _prefixSumWindow.average(begin - Config::baseline_us, begin);
            std::optional<int> first = _prefixSumWindow.average(begin + Config::margin_us, middle);
            std::optional<int> second = _prefixSumWindow.average(middle, end - Config::margin_us);

            if (!base || !first || !second) return std::nullopt;

            bool x = isLow(*first, *base);
            if (x == isLow(*second, *base)) return std::nullopt;

            return x;
        }

        std::optional<bool> receive(time_t now) {
            if (isBefore(now)) return std::nullopt;
            if (isAfter(now)) {
                changeIdle();
                return std::nullopt;
            }

            std::optional<bool> x = decode(now);
            if (!x)return std::nullopt;

            _lastTime = now;
            return *x;
        }

        void changeIdle() {
            _mode = Mode::Idle;
            _data = 0;
            _index = 0;
            _lastTime = 0;
        }
    };
}
