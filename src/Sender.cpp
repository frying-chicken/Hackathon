// #define BAUD 250000 // ボーレート（速め）

// #include <Arduino.h>

// #include "hack/Sender.hpp"
// #include "hack/PrefixSumWindow.hpp"

// hack::Sender<2, 256> sender;
// hack::PrefixSumWindow<hack::time_t, uint32_t, 256> prefixSumWindow;

// hack::time_t _t;
// int bpm = 100;
// hack::time_t period_us = 60 * 1'000'000 / bpm;

// int beat = 0;

// void setup() {
//     sender.begin();

//     Serial.begin(BAUD);
//     Serial.println("Start");
//     _t = micros();

//     delay(100);
// }

// void loop() {
//     sender.update();

//     const int value = analogRead(A0);
//     prefixSumWindow.push(micros(), value);

//     if (_t + period_us < micros()) {
//         uint8_t v = 0;
//         for (int i = 0;i < 8;++i) {
//             hack::writeBit(v, i, i < (beat++ / 8));
//         }
//         Serial.println(v);
//         sender(v);
//         sender(bpm);

//         std::optional<int> x = prefixSumWindow.average(0, micros());
//         if (x) { bpm = *x / 4; }
//         else { bpm = 120; }

//         bpm = 120;

//         period_us = 60 * 1'000'000 / bpm;

//         _t = micros();
//     }
// }

