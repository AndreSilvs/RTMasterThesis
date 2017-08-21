#pragma once

class Timer {
public:
    Timer();
    Timer( float time );

    void start();
    void start( float time );

    bool update( float time );

    bool hasStopped();

    float getTargetTime();
    float getElapsedTime();
    float getRemainingTime();

    float getPercentComplete();
    float getPercentRemaining();

    float getTimeScale();
    void setTimeScale( float scale = 1.0f );

private:
    float _targetTime = 1.0f;
    float _elapsedTime = 0.0f;
    float _timeScale = 1.0f;
};
