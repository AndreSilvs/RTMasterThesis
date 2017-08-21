#include "DebugTimer.h"

DebugTimer::DebugTimer() {

}

DebugTimer::~DebugTimer() {

}

void DebugTimer::start() {
    _start = std::chrono::steady_clock::now();
}

long long DebugTimer::stop() {
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    return _timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>( end - _start ).count();
}

long long DebugTimer::getTimeElapsed() const {
    return _timeElapsed;
}