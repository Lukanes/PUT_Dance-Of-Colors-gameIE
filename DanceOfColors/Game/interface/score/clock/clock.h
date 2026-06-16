#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

class GameClock {
    private:
        std::chrono::steady_clock::time_point startTime;

    public:
        GameClock() : startTime(std::chrono::steady_clock::now()) {}
        void reset(); //reset timer to 00:00
        std::string getTimeStr() const; //getTime as HH:MM:SS
};
