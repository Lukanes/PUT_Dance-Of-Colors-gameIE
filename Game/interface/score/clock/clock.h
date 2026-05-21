#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <memory>

class GameClock {
    public: // first public to use TimePoints
        using ClockType = std::chrono::steady_clock;
        using TimePoint = ClockType::time_point;
        using Duration = std::chrono::duration<double>;
    private:
        TimePoint mStartTime;
        TimePoint mLastFrameTime;
    public: // second public for methods
        GameClock() : mStartTime(ClockType::now()), mLastFrameTime(mStartTime) {}
        double tick();
        double getTotalTime() const;
};

double GameClock::tick(){
    auto now = ClockType::now();
    Duration deltaTime = now - mLastFrameTime;
    mLastFrameTime = now;
    return deltaTime.count();
}

double GameClock::getTotalTime() const {
    return (ClockType::now() - mStartTime).count();
}