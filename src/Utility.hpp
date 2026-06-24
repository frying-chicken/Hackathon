#pragma once

#include <Arduino.h>
#include <climits>
#include <cstddef>

namespace hack {
    using time_t = unsigned long;
    using size_t = std::size_t;

    template<typename T>
    constexpr size_t bit_size(const T&) {
        return sizeof(T) * CHAR_BIT;
    }

    template<typename T>
    constexpr bool readBit(const T& x, size_t index) {
        return bitRead(x, bit_size(x) - 1 - index);
    }
}
