#define BAUD 250000 // ボーレート（速め）
constexpr int ID = 0;

#include <Arduino.h>

#include "hack/Receiver.hpp"

int i = 0;
int j = 0;

int beatIndex = 0;
hack::time_t beatTiming = 0;
hack::time_t period_us = 0;

float melody[6][11] = { {440, 440, 493, 440, 440, 440, 000, 000, 000, 000, 000},
                      {440, 440, 440, 440, 392, 392, 440, 440, 392, 329, 000},
                      {440, 392, 440, 392, 440, 440, 392, 329, 000, 000, 000},
                      {440, 440, 440, 493, 440, 440, 440, 000, 000, 000, 000},
                      {440, 392, 392, 440, 392, 392, 440, 440, 329, 000, 000},
                      {440, 440, 440, 440, 440, 493, 440, 392, 440, 000, 000}
};
float startTimes[6][11] = { { 1.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f},
                         { 9.0f, 10.0f, 10.5f, 11.0f, 12.0f, 12.5f, 13.0f, 14.0f, 14.5f, 15.0f,  0.0f},
                         {17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 22.5f, 23.0f,  0.0f,  0.0f,  0.0f},
                         {25.0f, 26.0f, 27.0f, 28.0f, 29.0f, 30.0f, 31.0f,  0.0f,  0.0f,  0.0f,  0.0f},
                         {33.0f, 34.0f, 34.5f, 35.0f, 36.0f, 36.5f, 37.0f, 38.0f, 39.0f,  0.0f,  0.0f},
                         {41.0f, 41.5f, 42.0f, 42.5f, 43.0f, 44.0f, 45.0f, 46.0f, 47.0f,  0.0f,  0.0f}
};
float duration[6][11] = { {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                       {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
                       {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                       {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                       {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
                       {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 0.0f, 0.0f}
};
float amplitudes[6][11] = { {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
                         {0.5, 0.5, 0.3, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0},
                         {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0},
                         {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0},
                         {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0},
                         {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0}
};

void f(const hack::RingContainer<uint8_t, 256>& data) {
    if (hack::readBit(data[0], ID)) {
        beatIndex++;
        beatTiming = micros();
        period_us = 60 * 1'000'000 / data[1];
    }
}

hack::Receiver<A0, 256> receiver(f);

void setup() {
    Serial.begin(BAUD);
}

void loop() {
    receiver.update();

    if (j >= 6) {
        return; // 全部送信し終わったら何もしない
    }

    if (melody[j][i] == 0) {
        i = 0;
        j++;
        return; // 行を切り替えたこのフレームでは送信しない
    }

    if (startTimes[j][i] - beatIndex < (micros() - beatTiming) / period_us) {
        Serial.print(amplitudes[j][i], 2); // volume
        Serial.print(",");
        Serial.print(melody[j][i], 2);     // pitch
        Serial.print(",");
        Serial.println(duration[j][i] * period_us, 2); // dur

        ++i;
    }
}