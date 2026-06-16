#pragma once

#include <cstddef>
#include <cstdint>

#include "Utility.h"

namespace Config {
    constexpr time_us_t half_bit_us = 450UL;
    constexpr time_us_t bit_us = half_bit_us * 2;

    //constexpr size_t startup_payload_count = 100;
    //constexpr size_t debug_ring_capacity = 10;
    //constexpr time_t receiver_sample_micros = 50UL;
    //constexpr int receiver_high_margin = 24;

    constexpr uint64_t preamble = 0x5555'5555'5555'5555;
    constexpr uint16_t start_pattern = 0b0100'0011'1100'0111;

    // constexpr size_t baseline_half_bits = 16;
    // constexpr size_t start_detect_half_bits = 16;

    // constexpr time_t half_bits_to_micros(size_t half_bits) {
    //     return half_bit_micros * half_bits;
    // }

    // constexpr time_t baseline_micros = half_bits_to_micros(baseline_half_bits);
    // constexpr time_t start_detect_micros = half_bits_to_micros(start_detect_half_bits);

    // constexpr time_t receiver_history_micros = baseline_micros + start_detect_micros;
    // constexpr size_t receiver_min_samples =
    //     static_cast<size_t>((receiver_history_micros + receiver_sample_micros - 1UL) / receiver_sample_micros);
    // constexpr size_t receiver_sample_capacity = receiver_min_samples + 16;
}
