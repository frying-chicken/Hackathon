#define BAUD 250000 // ボーレート（速め）
constexpr int ID = 0;

#include <Arduino.h>

#include "hack/Receiver.hpp"

namespace
{
    constexpr int kRows = 6;
    constexpr int kCols = 11;
    constexpr uint8_t kReceiverPin = static_cast<uint8_t>(A0);
}

int noteIndex = 0;
int rowIndex = 0;

int beatIndex = 0;
hack::time_t beatTiming = 0;
hack::time_t period_us = 0;

float melody[6][11] = {{440, 440, 493, 440, 440, 440, 000, 000, 000, 000, 000},
                       {440, 440, 440, 440, 392, 392, 440, 440, 392, 329, 000},
                       {440, 392, 440, 392, 440, 440, 392, 329, 000, 000, 000},
                       {440, 440, 440, 493, 440, 440, 440, 000, 000, 000, 000},
                       {440, 392, 392, 440, 392, 392, 440, 440, 329, 000, 000},
                       {440, 440, 440, 440, 440, 493, 440, 392, 440, 000, 000}};
float startTimes[6][11] = {{1.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                           {9.0f, 10.0f, 10.5f, 11.0f, 12.0f, 12.5f, 13.0f, 14.0f, 14.5f, 15.0f, 0.0f},
                           {17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 22.5f, 23.0f, 0.0f, 0.0f, 0.0f},
                           {25.0f, 26.0f, 27.0f, 28.0f, 29.0f, 30.0f, 31.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                           {33.0f, 34.0f, 34.5f, 35.0f, 36.0f, 36.5f, 37.0f, 38.0f, 39.0f, 0.0f, 0.0f},
                           {41.0f, 41.5f, 42.0f, 42.5f, 43.0f, 44.0f, 45.0f, 46.0f, 47.0f, 0.0f, 0.0f}};
float duration[6][11] = {{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                         {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
                         {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                         {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                         {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
                         {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 0.0f, 0.0f}};
float amplitudes[6][11] = {{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
                           {0.5, 0.5, 0.3, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0},
                           {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0},
                           {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0},
                           {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0},
                           {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0}};

void f(const std::array<uint8_t, 256> &data)
{
    if (hack::readBit(data[0], ID))
    {
        if (data[1] == 0)
            return;
        beatIndex++;
        beatTiming = micros();
        period_us = 60 * 1'000'000 / data[1];
    }
}

hack::Receiver<kReceiverPin, 256> receiver;

void setup()
{
    receiver.begin(f);
    Serial.begin(BAUD);
}

void loop()
{
    receiver.update();

    if (rowIndex >= kRows)
    {
        return; // 全部送信し終わったら何もしない
    }

    if (melody[rowIndex][noteIndex] == 0)
    {
        noteIndex = 0;
        rowIndex++;
        return;
    }

    if (period_us == 0)
    {
        return;
    }

    if (startTimes[rowIndex][noteIndex] - double(beatIndex) < 0)
    {
        return;
    }

    if (startTimes[rowIndex][noteIndex] - double(beatIndex) > 1)
    {
        return;
    }

    if (startTimes[rowIndex][noteIndex] - double(beatIndex) < double((micros() - beatTiming) / double(period_us)))
    {
        Serial.print(amplitudes[rowIndex][noteIndex], 0); // volume
        Serial.print(",");
        Serial.print(melody[rowIndex][noteIndex], 0); // pitch
        Serial.print(",");
        Serial.println(duration[rowIndex][noteIndex] * float(period_us), 0); // dur

        ++noteIndex;
    }
}
