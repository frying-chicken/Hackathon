#pragma once

#include "hack/Types.hpp"

namespace app
{
    template <typename T>
    constexpr hack::time_t period(T bpm)
    {
        return bpm == 0 ? 0 : 60UL * 1'000'000UL / static_cast<hack::time_t>(bpm);
    }
}
