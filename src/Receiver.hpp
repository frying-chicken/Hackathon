#pragma once

#include <Arduino.h>

#include "AnalogWindow.hpp"
#include "LinkConfig.hpp"

template<pin_size_t PIN>
class Receiver
{
    enum class Mode {
        Idle,
        Payload
    };

public:
    using Callback = void (*)(byte);

private:
    Callback _callback;

    Mode _mode = Mode::Idle;

    byte _data = 0;
    size_t _index = 0;

    AnalogSampler<PIN, 256> _analog;

public:
    Receiver(Callback callback)
        : _callback(callback)
    {
    }

    void update(unsigned long now = micros()) {
        _analog.update(now);
    }

    const AnalogWindow<256>& analog() const { return _analog.window(); }
};
