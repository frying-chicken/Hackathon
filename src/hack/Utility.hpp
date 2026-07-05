#pragma once

#include <Arduino.h>
#include <climits>

#include "Types.hpp"

namespace hack {
    template<typename T>
    constexpr size_t bit_size() { return sizeof(T) * CHAR_BIT; }

    template<typename T>
    constexpr size_t bit_size(const T&) { return bit_size<T>(); }

    template<typename T>
    constexpr bool readBit(const T x, size_t index) {
        return bitRead(x, index);
    }
    template<typename T>
    constexpr void writeBit(T& x, size_t index, bool value) {
        bitWrite(x, index, value);
    }

    template<typename T>
    constexpr void shiftInBit(T& data, bool bit) {
        data >>= 1;
        writeBit(data, bit_size<T>() - 1, bit);
    }
}
