#pragma once

#include <Arduino.h>
#include "Utility.hpp"

namespace hack {
    template<time_t Interval>
    class Clock
    {
        static_assert(0 < Interval);
        time_t _deadline;

    public:
        Clock(time_t now = micros())
            : _deadline(now + Interval) {
        }

        bool update(time_t now = micros()) {
            if (now < _deadline) {
                return false;
            }

            time_t nextDeadline = _deadline + Interval;
            if (now < nextDeadline) {
                _deadline = nextDeadline;
            }
            else {
                _deadline = now + Interval;
            }
            return true;
        }
    };
}
