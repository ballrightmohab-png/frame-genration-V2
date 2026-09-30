#include "LeviMod.h"
#include "FrameGenMod.hpp"

// Main dynamic plugin entry points delegating to FrameGenMod
extern "C" LEVI_API void LeviMod_OnLoad() {
    LeviMod::FrameGenMod::getInstance().load();
    LeviMod::FrameGenMod::getInstance().enable();
}

extern "C" LEVI_API void LeviMod_OnUnload() {
    LeviMod::FrameGenMod::getInstance().disable();
    LeviMod::FrameGenMod::getInstance().unload();
}
