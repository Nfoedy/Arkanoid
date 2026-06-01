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


// Stati principali del gioco.
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

    bool Initialize(HWND hwnd, int width, int height);
    void Shutdown();

    bool Frame();

    void KeyDown(unsigned int key);
    void KeyUp(unsigned int key);

private:
    void HandleInput(float deltaTime);
    void Update(float deltaTime);
    void Render();

    bool InitializeBricks();
    void ShutdownBricks();

    void StartNewGame();
    void ResetGame();
    void ResetRound();

    void PositionBallOnPaddle();
    void LaunchBall();

    void CheckGameState();
    void HandleBallLost();
    bool AreAllBricksDestroyed() const;

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

    void CheckPaddleBallCollision(BallClass* ball);
    void CheckBallBrickCollision(BallClass* ball);
    void CheckBallsOutOfBounds();

    void ShutdownPowerUps();
    void UpdatePowerUps(float deltaTime);
    void RenderPowerUps();

    void TrySpawnPowerUp(float x, float y);
    void CheckPaddlePowerUpCollision();
    PowerUpType GetRandomPowerUpType() const;

    void ApplyPowerUp(PowerUpType type);
    void UpdateActiveEffects(float deltaTime);
    void ResetActiveEffects();

    void SpawnSecondBall();
    void ShutdownSecondBall();

    void RenderTextUI();
    void UpdateWindowTitle();

    bool HandleMenuInput();
    bool IsMenuState() const;

private:
    int m_screenWidth;
    int m_screenHeight;

    HWND m_hwnd;

    D3DClass* m_D3D;
    InputClass* m_Input;
    TimerClass* m_Timer;
    ColorShaderClass* m_ColorShader;
    TextRendererClass* m_TextRenderer;

    PaddleClass* m_Paddle;
    BallClass* m_Ball;
    BallClass* m_SecondBall;

    std::vector<BrickClass*> m_Bricks;
    std::vector<PowerUpClass*> m_PowerUps;

    GameState m_GameState;

    int m_score;
    int m_lives;

    bool m_pWasDown;

    int m_menuSelectedIndex;
    bool m_upWasDown;
    bool m_downWasDown;
    bool m_enterWasDown;

    bool m_isPaddleSizeEffectActive;
    float m_paddleSizeEffectTimer;

    bool m_isBallSpeedEffectActive;
    float m_ballSpeedEffectTimer;
};