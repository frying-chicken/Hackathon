#pragma once

#include <Arduino.h>
#include "Utility.h"

template<time_t Interval>
class Clock
{
    time_us_t _deadline;

public:
    Clock(time_us_t now = 0)
        : _deadline(now + Interval) {
    }

    bool update(time_us_t now = micros()) {
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