#pragma once

#include <Arduino.h>
#include <cstdint>

#include "hack/Types.hpp"

constexpr unsigned long SerialBaud = 250000;
constexpr hack::size_t FrameSize = 256;

constexpr uint8_t ReceiverPin = static_cast<uint8_t>(A0);
constexpr uint8_t SenderPin = 2;

constexpr uint8_t ReceiverIdBit = 0;