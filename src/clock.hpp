#pragma once

#include <Arduino.h>

template<unsigned long Interval>
class Clock
{
    unsigned long _deadline;

public:
    Clock(unsigned long now = micros())
        : _deadline(now + Interval) {
    }

    bool update(unsigned long now = micros()) {
        if (now < _deadline) {
            return false;
        }

        if (now < _deadline + Interval) {
            _deadline = _deadline + Interval;
        }
        else {
            _deadline = now + Interval;
        }
        return true;
    }
};