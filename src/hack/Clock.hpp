#pragma once

#include "Timer.hpp"
#include "Types.hpp"

namespace hack
{
    template <time_t Interval>
    class Clock : public Timer
    {
        static_assert(0 < Interval);

    public:
        bool update()
        {
            return Timer::update(Interval);
        }
    };
}
