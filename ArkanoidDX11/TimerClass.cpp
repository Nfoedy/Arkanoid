#include "TimerClass.h"


TimerClass::TimerClass()
{
    m_deltaTime = 0.0f;
}


TimerClass::~TimerClass()
{
}


void TimerClass::Initialize()
{
    m_lastTime = std::chrono::high_resolution_clock::now();
    m_deltaTime = 0.0f;
}


void TimerClass::Frame()
{
    auto currentTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> elapsedTime = currentTime - m_lastTime;

    m_deltaTime = elapsedTime.count();

    m_lastTime = currentTime;
}


float TimerClass::GetDeltaTime() const
{
    return m_deltaTime;
}