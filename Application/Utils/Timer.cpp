#include "Timer.h"

Timer::Timer(float targetTime)
    :targetTime(targetTime)
{
}

void Timer::Tick(float deltaTime)
{
    elapsedTime += deltaTime;

    if (elapsedTime < 0.0f)
    {
        Reset();
    }
}

void Timer::Reset()
{
    elapsedTime = 0.0f;
}

bool Timer::IsTimeout() const
{
    return elapsedTime >= targetTime;
}

void Timer::SetTargetTime(float newTargetTime)
{
    targetTime = newTargetTime;
}

float Timer::GetElapsedTime()
{
    return elapsedTime;
}
