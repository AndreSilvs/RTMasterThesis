#include "GlobalSystems.h"

namespace globalSystem {
    WindowManagerGL window;
    TimeData time;
    MouseData mouse;
    KeyPressData keys;
    TextureManager textures;
    ModelManager models;
    DynamicFloatMap runtimeData;

    RandomNumberGenerator rng;

    Font gameFont;
    Font gameFontLarge;
    Font gameFontHuge;
}