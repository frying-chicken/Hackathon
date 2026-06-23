#pragma once

#include <Arduino.h>
#include "Utility.h"

namespace hack {
    template<time_us_t Interval>
    class Clock
    {
        static_assert(0 < Interval);
        time_us_t _deadline;

    public:
        Clock(time_us_t now = micros())
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
}
