#pragma once

#include <climits>

using time_us_t = unsigned long;

template<typename T>
static constexpr size_t bit_size(T) {
    return sizeof(T) * CHAR_BIT;
}

template<typename T>
bool readBit(T x, size_t index) {
    return bitRead(x, bit_size(x) - 1 - index);
    //return bitRead(x, index);
}

// template<typename T>
// void writeBit(T& x, size_t index, bool value) {
//     bitWrite(x, index, value);
// }