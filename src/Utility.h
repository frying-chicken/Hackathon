#pragma once

#include <Arduino.h>
#include <climits>
#include <cstddef>
#include <type_traits>

namespace hack {
    using time_us_t = unsigned long;
    using signed_time_us_t = std::make_signed_t<time_us_t>;

    template<typename T>
    constexpr size_t bit_size(const T&) {
        return sizeof(T) * CHAR_BIT;
    }

    template<typename T>
    constexpr bool readBit(T x, size_t index) {
        return bitRead(x, bit_size(x) - 1 - index);
    }

    constexpr time_us_t elapsedSince(time_us_t now, time_us_t previous) {
        return now - previous;
    }

    constexpr bool deadlineReached(time_us_t now, time_us_t deadline) {
        return static_cast<signed_time_us_t>(now - deadline) >= 0;
    }
}
