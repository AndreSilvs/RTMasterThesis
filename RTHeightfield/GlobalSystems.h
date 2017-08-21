#pragma once

#include "InputData.h"
#include "TextureManager.h"
#include "Font.h"
#include "TimeManager.h"
#include "WindowManager.h"
#include "DynamicFloatMap.h"
#include "ModelManager.h"
#include "RNG.h"

namespace globalSystem {
    extern WindowManagerGL window;
    extern TimeData time;
    extern MouseData mouse;
    extern KeyPressData keys;
    extern TextureManager textures;
    extern ModelManager models;
    extern DynamicFloatMap runtimeData;
    extern RandomNumberGenerator rng;

    extern Font gameFont;
    extern Font gameFontLarge;
    extern Font gameFontHuge;
}