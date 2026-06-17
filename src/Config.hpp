#pragma once

#include <cstddef>
#include <cstdint>

#include "Utility.h"

namespace hack::Config {
    constexpr time_us_t half_bit_us = 400;
    constexpr time_us_t bit_us = half_bit_us * 2;

    constexpr uint64_t preamble = 0;
    constexpr uint8_t start_pattern = 0b1011'1011;

    constexpr time_us_t baseline_us = bit_us * 4;
}
