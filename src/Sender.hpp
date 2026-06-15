#pragma once

#include <Arduino.h>
#include <climits>
#include <cstdint>
#include "RingContainer.hpp"
#include "clock.hpp"
#include "Config.hpp"

template<pin_size_t PIN>
class Sender
{
    enum class Mode {
        Idle,
        Preamble,
        Start,
        Payload
    };

    RingContainer<uint8_t, 256> _buffer;
    Clock<Config::half_bit_us> _clock;

    Mode _mode;
    bool _halfPhase;
    size_t _index;

public:
    Sender() :
        _buffer(),
        _clock(),
        _mode(Mode::Idle),
        _halfPhase(false),
        _index(0)
    {
        pinMode(PIN, OUTPUT);
    }

    void update(Config::Time now = micros()) {
        if (!_clock.update(now)) {
            return;
        }

        switch (_mode) {
        case Mode::Idle:
            digitalWrite(PIN, LOW);
            return;

        case Mode::Preamble:
            writePattern(Config::preamble, Mode::Start);
            return;

        case Mode::Start:
            writePattern(Config::start_pattern, Mode::Payload);
            return;

        case Mode::Payload:
            if (_buffer.empty()) {
                _mode = Mode::Idle;
                digitalWrite(PIN, LOW);
                return;
            }

            write(_buffer.front(), _index, _halfPhase);

            _halfPhase = !_halfPhase;
            if (_halfPhase) {
                return;
            }
            if (++_index == bit_size<uint8_t>()) {
                _index = 0;
                _buffer.pop_front();
            }
        }
    }

    void operator()(uint8_t x) {
        if (_mode == Mode::Idle) {
            _mode = Mode::Preamble;
        }
        _buffer.push_back(x);
    }
private:
    template<typename T>
    void writePattern(T pattern, Mode nextMode) {
        write(pattern, _index);
        if (++_index == bit_size<T>()) {
            _index = 0;
            _mode = nextMode;
        }
    }

    template<typename T>
    void write(T x, size_t index, bool y = false) {
        bool data = bitRead(x, bit_size<T>() - 1 - index);
        bool bit = data ^ y;
        digitalWrite(PIN, !bit ? LOW : HIGH);
    }

    template<typename T>
    static constexpr size_t bit_size(T) {
        return sizeof(T) * CHAR_BIT;
    }
};
