// #include <Arduino.h>

// #include "Sender.hpp"

// #include "Clock.hpp"

// uint8_t value = 0;

// hack::Sender<2, uint8_t> sender;

// hack::Clock<5'000'000> clock;

// void setup() {
//     Serial.begin(250000);

//     clock.update();
//     Serial.println("Test");
// }

// void loop() {
//     if (clock.update()) {
//         const uint8_t inverse = static_cast<uint8_t>(UINT8_MAX - value);
//         sender(value);
//         Serial.print("in:");
//         Serial.println(value);
//         ++value;
//     }

//     sender.update();
// }
