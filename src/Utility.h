#pragma once

#include <Arduino.h>
#include <climits>
#include <cstddef>

namespace hack {
    using time_us_t = unsigned long;

    template<typename T>
    constexpr size_t bit_size(const T&) {
        return sizeof(T) * CHAR_BIT;
    }

    template<typename T>
    constexpr bool readBit(T x, size_t index) {
        return bitRead(x, bit_size(x) - 1 - index);
    }
}
