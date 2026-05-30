#pragma once

#include "System/time.hpp"

#include <chrono>


//Segmented Accumulator (Anchor Clock)
class Clock{
public:

using SteadyClock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<SteadyClock>;

    Clock();

    TimeD restart();
    TimeD elapsedTime() const;

    void pause();
    void unpause();
    void setTimeScale(float scale);

private:
    TimePoint m_lastsetTimeStamp;
    TimeD m_accumulatedTime;

    float m_timescale = 1.0f;
    bool isPaused = false;

    TimeD new_accumulated_time() const;
};