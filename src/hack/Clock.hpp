#pragma once

#include <Arduino.h>

#include "Types.hpp"

namespace hack {
    template<time_t Interval>
    class Clock
    {
        static_assert(0 < Interval);
        time_t _last = 0;

    public:
        void reset() {
            _last = micros();
        }

        bool update() {
            const time_t now = micros();

            if (now - _last < Interval) return false;

            _last += Interval;
            if (Interval <= now - _last) {
                _last = now;
            }
            return true;
        }
    };
}
