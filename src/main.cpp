#include <Arduino.h>

#include "test/Receiver.hpp"
#include "test/Sender.hpp"

enum class Mode
{
    Sender,
    Receiver
};

constexpr Mode mode = Mode::Sender;
//constexpr Mode mode = Mode::Receiver;

void setup()
{
    if constexpr (mode == Mode::Sender)
        sender_test::setup();
    if constexpr (mode == Mode::Receiver)
        receiver_test::setup();
}
void loop()
{
    if constexpr (mode == Mode::Sender)
        sender_test::loop();
    if constexpr (mode == Mode::Receiver)
        receiver_test::loop();
}
