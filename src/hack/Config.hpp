#pragma once

#include <cstdint>

#include "Types.hpp"
#include "Utility.hpp"

namespace hack::Config {
    constexpr time_t half_bit_us = 250;
    constexpr time_t margin_us = 80;

    constexpr time_t bit_us = half_bit_us * 2;

    constexpr uint8_t calibration = 0x55;
    constexpr uint32_t preamble = 0x0000'0000;
    constexpr uint8_t start = 0b1100'0101;

    constexpr time_t threshold_window_us = bit_us * 8;
    constexpr size_t preamble_required = bit_size(preamble) / 2;

    constexpr size_t buffer_size = 2048;
}
