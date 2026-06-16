#pragma once

#include <climits>

using time_us_t = unsigned long;

template<typename T>
static constexpr size_t bit_size() {
    return sizeof(T) * CHAR_BIT;
}
template<typename T>
static constexpr size_t bit_size(T) {
    return bit_size<T>();
}

template<typename T>
bool readBit(T x, size_t index) {
    return bitRead(x, bit_size<T>() - 1 - index);
}

template<typename T>
void writeBit(T& x, size_t index, bool value) {
    bitWrite(x, bit_size<T>() - 1 - index, value);
}