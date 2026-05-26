#include "clock.h"

void GameClock::reset() {
    startTime = std::chrono::steady_clock::now();
}

std::string GameClock::getTimeStr() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now- startTime); //line to extract duration of time since beginning
    int totalSec = duration.count();
    int hours = totalSec / 3600;
    int minutes = (totalSec % 3600) / 60;
    int sec = totalSec % 60;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << sec;
    return oss.str();
}