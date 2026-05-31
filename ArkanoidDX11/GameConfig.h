#pragma once


namespace GameConfig
{
    // ============================================================
    // Window
    // ============================================================

    constexpr int ScreenWidth = 800;
    constexpr int ScreenHeight = 600;


    // ============================================================
    // Paddle
    // ============================================================

    constexpr float PaddleStartX = 0.0f;
    constexpr float PaddleStartY = -0.8f;

    constexpr float PaddleWidth = 0.35f;
    constexpr float PaddleHeight = 0.08f;

    constexpr float PaddleSpeed = 1.25f;


    // ============================================================
    // Ball
    // ============================================================

    constexpr float BallSize = 0.06f;

    constexpr float BallInitialVelocityX = 0.60f;
    constexpr float BallInitialVelocityY = 0.78f;

    constexpr float BallMaxHorizontalSpeed = 0.78f;
    constexpr float BallMinHorizontalSpeed = 0.22f;



    // ============================================================
    // Bricks
    // ============================================================

    constexpr int BrickRows = 4;
    constexpr int BrickColumns = 8;

    constexpr float BrickWidth = 0.20f;
    constexpr float BrickHeight = 0.08f;

    constexpr float BrickSpacingX = 0.03f;
    constexpr float BrickSpacingY = 0.03f;

    constexpr float BrickStartX = -0.805f;
    constexpr float BrickStartY = 0.75f;


    // ============================================================
    // Gameplay
    // ============================================================

    constexpr int InitialLives = 3;
    constexpr int BrickScore = 100;


    // ============================================================
    // PowerUps / Malus
    // ============================================================

    constexpr float PowerUpDropChance = 0.20f;
    constexpr float PowerUpFallSpeed = 0.6f;
    constexpr float PowerUpSize = 0.06f;
    constexpr float PowerUpDuration = 6.0f;

    constexpr float PaddleGrowWidth = 0.55f;
    constexpr float PaddleShrinkWidth = 0.24f;

    constexpr float BallSpeedMultiplier = 1.5f;
}