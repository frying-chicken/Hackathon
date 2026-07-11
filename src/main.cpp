#include <Arduino.h>

// ここでビルド対象を切り替える (プリプロセッサで未使用側の include を除外し、
// RAM/Flash 上に不要な Sender/Receiver インスタンスを持たないようにする)
#define HACK_MODE_APP 0
#define HACK_MODE_TEST_0 1
#define HACK_MODE_TEST_1 2

#define HACK_TARGET_SENDER 0
#define HACK_TARGET_RECEIVER 1

#define HACK_MODE HACK_MODE_TEST_0
#define HACK_TARGET HACK_TARGET_RECEIVER

#if HACK_MODE == HACK_MODE_TEST_0
    #if HACK_TARGET == HACK_TARGET_SENDER
        #include "test_0/Sender.hpp"
        namespace active = sender_test_0;
    #else
        #include "test_0/Receiver.hpp"
        namespace active = receiver_test_0;
    #endif
#elif HACK_MODE == HACK_MODE_TEST_1
    #if HACK_TARGET == HACK_TARGET_SENDER
        #include "test_1/Sender.hpp"
        namespace active = sender_test_1;
    #else
        #include "test_1/Receiver.hpp"
        namespace active = receiver_test_1;
    #endif
#elif HACK_MODE == HACK_MODE_APP
    #if HACK_TARGET == HACK_TARGET_SENDER
        #include "Sender.hpp"
        namespace active = sender;
    #else
        #include "Receiver.hpp"
        namespace active = receiver;
    #endif
#endif

void setup()
{
    active::setup();
}

void loop()
{
    active::loop();
}
