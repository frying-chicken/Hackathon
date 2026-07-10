#pragma once

#include <Arduino.h>

#include "Types.hpp"

namespace hack
{
    class Timer
    {
        time_t _last = 0;

    public:
        void reset() { _last = micros(); }

        bool update(time_t interval)
        {
            if (interval == 0)
                return false;

            const time_t now = micros();
            if (now - _last < interval)
                return false;

            _last += interval;
            if (interval <= now - _last)
            {
                _last = now;
            }
            return true;
        }
    };
}
