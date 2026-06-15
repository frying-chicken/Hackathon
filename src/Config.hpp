#pragma once

#include <cstddef>
#include <cstdint>

namespace Config {
    using Time = unsigned long;

    constexpr Time half_bit_us = 450UL;
    constexpr Time bit_us = half_bit_us * 2UL;

    constexpr size_t startup_payload_count = 100;
    constexpr size_t debug_ring_capacity = 10;

    constexpr uint32_t preamble = 0x5555'5555;
    constexpr uint16_t start_pattern = 0b0100'0011'1100'0111;

    constexpr size_t baseline_half_bits = 16;
    constexpr size_t start_detect_half_bits = 14;
    constexpr size_t start_first_low_end_half_bits = 10;
    constexpr size_t start_first_high_end_half_bits = 6;
    constexpr size_t start_second_low_end_half_bits = 3;

    constexpr Time half_bits_to_micros(size_t half_bits) {
        return half_bit_micros * half_bits;
    }

    constexpr Time baseline_micros = half_bits_to_micros(baseline_half_bits);
    constexpr Time start_detect_micros = half_bits_to_micros(start_detect_half_bits);
}
