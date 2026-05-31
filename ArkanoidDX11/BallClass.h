#pragma once

#include <d3d11.h>

#include "CircleObject2D.h"


class BallClass
{
public:
    BallClass();
    ~BallClass();

    bool Initialize(
        ID3D11Device* device,
        float x,
        float y,
        float size
    );

    void Shutdown();

    // Aggiorna posizione e rimbalzi.
    void Update(float deltaTime);

    // Manda la palla alla pipeline.
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount() const;

    // Reset della palla a una posizione e velocità iniziale.
    void Reset(float x, float y, float velocityX, float velocityY);

    // Controlla se la palla è uscita dal basso dello schermo.
    bool IsBelowBottom() const;

    // Getter per collisioni.
    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;

    bool IsMovingDown() const;

    // Rimbalzo sul paddle.
    void BounceFromPaddle(float paddleTop, float hitFactor);

    // Rimbalzi semplici.
    void BounceY();
    void BounceX();

    // Imposta solo la posizione della palla.
    void SetPosition(float x, float y);

    // Imposta solo la velocità della palla.
    void SetVelocity(float velocityX, float velocityY);

    // Moltiplica la velocità attuale della palla. Utile per malus tipo palla più veloce.
    void MultiplyVelocity(float factor);

    // Imposta un moltiplicatore di velocità.
    // 1.0f = velocità normale.
    // > 1.0f = palla più veloce.
    void SetSpeedMultiplier(float multiplier);

private:
    void CheckWallCollision();

private:

    CircleObject2D m_Circle;

    float m_size;

    float m_velocityX;
    float m_velocityY;

    float m_speedMultiplier;
};