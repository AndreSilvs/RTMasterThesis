#pragma once

#include <chrono>

class DebugTimer {
public:
    DebugTimer();
    ~DebugTimer();

    // Start counting => create timestamp
    void start();
    // Stop counting => current timestamp - start timestamp
    long long stop();

    // Returns time elapsed in miliseconds
    long long getTimeElapsed() const;

private:
    std::chrono::steady_clock::time_point _start;
    long long _timeElapsed;
};
