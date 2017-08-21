#pragma once

#include <set>

#include "Vector.h"

// Mouse button states: CLICK, HOLD, RELEASED
enum MouseButtonState { BUTTON_CLICKED, BUTTON_HELD, BUTTON_RELEASED };

struct MouseData {
    std::set<int> buttonState;
    Vector2D mouseCoords;
};

class KeyPressData {
public:

    bool wasKeyPressed( int key );
    bool wasKeyReleased( int key );
    void pressKey( int key );
    void releaseKey( int key );
    void clear();

private:
    std::set<int> _keysPressed;
    //std::set<int> _keysDown;
    std::set<int> _keysReleased;
};