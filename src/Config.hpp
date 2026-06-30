#pragma once

#include <cstdint>
#include "Utility.hpp"

namespace hack::Config {
    constexpr time_t half_bit_us = 1000;
    constexpr time_t bit_us = half_bit_us * 2;
    constexpr time_t baseline_us = bit_us * 16;
    constexpr time_t margin_us = 100;

    constexpr uint32_t calibration = 0x5555'5555;
    constexpr uint32_t preamble = 0x0000'0000;
    constexpr uint8_t start = 0b1100'0101;

    constexpr size_t buffer_size = 1024 * 4;
    constexpr size_t read_preamble_size = 32;
}
