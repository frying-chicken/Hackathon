#pragma once

#include <cstddef>
#include <cstdint>

#include "Utility.h"

namespace hack::Config {
    constexpr time_us_t half_bit_us = 400;
    constexpr time_us_t bit_us = half_bit_us * 2;

    constexpr uint64_t preamble = 0;
    constexpr uint8_t start_pattern = 0b1011'1011;

    constexpr size_t sender_buffer_capacity = 256;
    constexpr size_t receiver_sample_capacity = 1024 * 2;

    constexpr time_us_t baseline_us = bit_us * 4;
    constexpr int level_hysteresis = 4;

    constexpr size_t preamble_period_count = 16;
    constexpr time_us_t min_preamble_bit_us = bit_us * 3 / 4;
    constexpr time_us_t max_preamble_bit_us = bit_us * 5 / 4;
    constexpr time_us_t sample_late_tolerance_us = half_bit_us / 2;
}
