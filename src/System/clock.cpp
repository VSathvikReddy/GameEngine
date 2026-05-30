#include "System/clock.hpp"


Clock::Clock(){
    restart();
}

TimeD Clock::restart(){
    TimeD elapsed = elapsedTime(); 

    m_accumulatedTime = TimeD();
    m_lastsetTimeStamp = SteadyClock::now();
    m_timescale = 1.0f;
    isPaused = false;

    return elapsed;
}

void Clock::pause(){
    if(isPaused){
        return;
    }

    isPaused = true;
    m_accumulatedTime  = new_accumulated_time();
}

void Clock::unpause(){
    if(!isPaused){
        return;
    }
    isPaused = false;
    m_lastsetTimeStamp = SteadyClock::now();
}

void Clock::setTimeScale(float scale) {
    if(isPaused){
        m_timescale = scale;
        return;
    }
    m_accumulatedTime  = new_accumulated_time();
    m_lastsetTimeStamp = SteadyClock::now();

    m_timescale = scale;
}




TimeD Clock::elapsedTime() const{
    if(isPaused){
        return m_accumulatedTime;
    }else{
        return new_accumulated_time();
    }
}


TimeD Clock::new_accumulated_time() const{
    std::chrono::duration<double, std::milli> elapsed = SteadyClock::now() - m_lastsetTimeStamp;
    return m_accumulatedTime + TimeD::Milliseconds(elapsed.count()*m_timescale);
}