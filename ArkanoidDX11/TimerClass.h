#pragma once

#include <chrono>


class TimerClass
{
public:
    TimerClass();
    ~TimerClass();

    // Inizializza il timer.
    void Initialize();

    // Aggiorna il delta time.
    void Frame();

    // Ritorna il tempo passato tra un frame e l'altro, in secondi.
    float GetDeltaTime() const;

private:
    std::chrono::high_resolution_clock::time_point m_lastTime;
    float m_deltaTime;
};