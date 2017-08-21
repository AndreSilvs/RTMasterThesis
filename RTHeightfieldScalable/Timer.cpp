#include "Timer.h"

Timer::Timer() {}
Timer::Timer( float time ) :
    _targetTime( time )
{
}

void Timer::start() {
    _elapsedTime = 0.0f;
}
void Timer::start( float time ) {
    _targetTime = time;
    _elapsedTime = 0.0f;
}

bool Timer::update( float time ) {
    return ( _elapsedTime += ( time * _timeScale ) ) >= _targetTime;
}

bool Timer::hasStopped() {
    return _elapsedTime >= _targetTime;
}

float Timer::getTargetTime() {
    return _targetTime;
}
float Timer::getElapsedTime() {
    return _elapsedTime;
}
float Timer::getRemainingTime() {
    return _targetTime - _elapsedTime;
}
float Timer::getPercentComplete() {
    return getElapsedTime() / _targetTime;
}
float Timer::getPercentRemaining() {
    return getRemainingTime() / _targetTime;
}

float Timer::getTimeScale() {
    return _timeScale;
}
void Timer::setTimeScale( float scale ) {
    _timeScale = scale;
}