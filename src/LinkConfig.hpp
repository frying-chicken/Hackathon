#pragma once

#include <stddef.h>

namespace LinkConfig {
    constexpr unsigned long HalfBitMicros = 450;
    constexpr size_t SamplesPerHalfBit = 3;
    constexpr unsigned long SampleMicros = HalfBitMicros / SamplesPerHalfBit;

    constexpr size_t StartLowBits = 4;
    constexpr size_t StartPatternBits = StartLowBits * 2;

    constexpr size_t BaselineSamples = StartPatternBits * SamplesPerHalfBit;
    constexpr size_t SampleAverageSamples = SamplesPerHalfBit;

    constexpr size_t PayloadBits = 8;
    constexpr size_t PayloadSamplesPerBit = 2 * SamplesPerHalfBit;
}
