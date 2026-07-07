#include <Arduino.h>

#include "Receiver.hpp"
#include "Sender.hpp"

enum class Mode
{
    Sender,
    Receiver
};

constexpr Mode mode = Mode::Sender;

void setup()
{
    if constexpr (mode == Mode::Sender)
    {
        sender_app::setup();
    }
    if constexpr (mode == Mode::Receiver)
    {
        receiver_app::setup();
    }
}
void loop()
{
    if constexpr (mode == Mode::Sender)
    {
        sender_app::loop();
    }
    if constexpr (mode == Mode::Receiver)
    {
        receiver_app::loop();
    }
}
