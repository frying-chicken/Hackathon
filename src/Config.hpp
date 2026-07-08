#pragma once

#include <Arduino.h>
#include <cstdint>

#include "hack/Types.hpp"

namespace app
{
    constexpr unsigned long serial_baud = 250000;
    constexpr hack::size_t frame_size = 2;

    constexpr pin_size_t receiver_pin = A0;
    constexpr pin_size_t sender_pin = 2;

    constexpr uint8_t machine_id_bit = 4;
}
