#pragma once

#include <cstdint>
#include "Utility.hpp"

namespace hack::Config {
    constexpr time_t half_bit_us = 1000;
    constexpr time_t bit_us = half_bit_us * 2;

    constexpr size_t preamble_size = 64;
    constexpr uint16_t start_pattern = 0b1001'0011'0110'0011;

    constexpr time_t baseline_us = bit_us * 8;

    constexpr time_t margin = 100;
}
