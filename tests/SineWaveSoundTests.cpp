#include <catch2/catch_test_macros.hpp>
#include "../src/PluginProcessor.h"

TEST_CASE ("SineWaveSound applies to all notes and channels", "[sound]")
{
    SineWaveSound sound;

    SECTION ("appliesToNote returns true for all valid MIDI notes")
    {
        REQUIRE (sound.appliesToNote (0) == true);
        REQUIRE (sound.appliesToNote (60) == true);
        REQUIRE (sound.appliesToNote (127) == true);
    }

    SECTION ("appliesToChannel returns true for all MIDI channels")
    {
        REQUIRE (sound.appliesToChannel (1) == true);
        REQUIRE (sound.appliesToChannel (10) == true);
        REQUIRE (sound.appliesToChannel (16) == true);
    }
}
