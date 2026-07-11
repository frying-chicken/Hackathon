#pragma once

#include <Arduino.h>
#include <cstdint>

#include "hack/Types.hpp"

namespace app
{
    // Serial モニタのボーレート
    constexpr unsigned long serial_baud = 250000;
    // 送受信 payload のバイト数 (byte0: 宛先ビット群, byte1: bpm)
    constexpr hack::size_t frame_size = 2;

    // Receiver のアナログ入力ピン (Sender 側では bpm 自動調整用の入力としても使用)
    constexpr pin_size_t receiver_pin = A0;
    // Sender のデジタル出力ピン
    constexpr pin_size_t sender_pin = 2;

    // payload byte0 内で、この Receiver 宛てかどうかを示すビット位置 (マスクではなくビット番号)
    constexpr uint8_t machine_id_bit = 0;
}
