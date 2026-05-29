#pragma once

#include <windows.h>
#include <vector>

#include "D3DClass.h"
#include "InputClass.h"
#include "ColorShaderClass.h"
#include "PaddleClass.h"
#include "BallClass.h"
#include "BrickClass.h"
#include "TimerClass.h"


enum class GameState
{
    MainMenu,
    Ready,
    Playing,
    Paused,
    Win,
    Lose
};

class GameClass
{
public:
    GameClass();
    ~GameClass();

    // Inizializza tutti i sistemi del gioco.
    bool Initialize(HWND hwnd, int width, int height);

    // Rilascia tutte le risorse.
    void Shutdown();

    // Frame principale: input, update, collisioni, render.
    bool Frame();

    // Funzioni chiamate dal WndProc.
    void KeyDown(unsigned int key);
    void KeyUp(unsigned int key);



private:
    void HandleInput(float deltaTime);
    void Update(float deltaTime);
    void Render();

    bool InitializeBricks();
    void ShutdownBricks();

    bool CheckAABBCollision(
        float leftA,
        float rightA,
        float topA,
        float bottomA,
        float leftB,
        float rightB,
        float topB,
        float bottomB
    ) const;

    void CheckPaddleBallCollision();
    void CheckBallBrickCollision();

    bool AreAllBricksDestroyed() const;
    void CheckGameState();
    void ResetGame();

    void StartNewGame();
    void ResetRound();
    void HandleBallLost();
    void UpdateWindowTitle();

    void PositionBallOnPaddle();
    void LaunchBall();



private:
    int m_screenWidth;
    int m_screenHeight;

    HWND m_hwnd;

    int m_score;
    int m_lives;

    bool m_pWasDown;

    D3DClass* m_D3D;
    InputClass* m_Input;
    ColorShaderClass* m_ColorShader;

    PaddleClass* m_Paddle;
    BallClass* m_Ball;

    TimerClass* m_Timer;

    std::vector<BrickClass*> m_Bricks;

    GameState m_GameState;
};