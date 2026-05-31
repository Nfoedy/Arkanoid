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
#include "PowerUpClass.h"
#include "TextRendererClass.h"


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

    void ShutdownPowerUps();
    void UpdatePowerUps(float deltaTime);
    void RenderPowerUps();

    void TrySpawnPowerUp(float x, float y);
    void CheckPaddlePowerUpCollision();

    PowerUpType GetRandomPowerUpType() const;


    void ApplyPowerUp(PowerUpType type);
    void UpdateActiveEffects(float deltaTime);
    void ResetActiveEffects();

    void RenderTextUI();

    bool HandleMainMenuInput();


private:
    int m_screenWidth;
    int m_screenHeight;

    HWND m_hwnd;

    int m_score;
    int m_lives;

    bool m_pWasDown;

    bool m_isPaddleSizeEffectActive;
    float m_paddleSizeEffectTimer;

    bool m_isBallSpeedEffectActive;
    float m_ballSpeedEffectTimer;

    int m_menuSelectedIndex;

    bool m_upWasDown;
    bool m_downWasDown;
    bool m_enterWasDown;

    D3DClass* m_D3D;
    InputClass* m_Input;
    ColorShaderClass* m_ColorShader;

    PaddleClass* m_Paddle;
    BallClass* m_Ball;

    TimerClass* m_Timer;

    TextRendererClass* m_TextRenderer;

    std::vector<BrickClass*> m_Bricks;

    GameState m_GameState;

    std::vector<PowerUpClass*> m_PowerUps;
};