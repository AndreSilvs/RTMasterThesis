#include "InputData.h"

bool KeyPressData::wasKeyPressed( int key ) {
    return _keysPressed.count( key ) > 0;
}
bool KeyPressData::wasKeyReleased( int key ) {
    return _keysReleased.count( key ) > 0;
}
void KeyPressData::pressKey( int key ) {
    _keysPressed.insert( key );
}
void KeyPressData::releaseKey( int key ) {
    _keysReleased.insert( key );
}
void KeyPressData::clear() {
    _keysPressed.clear();
    _keysReleased.clear();
}