#pragma once

#include <d3d11.h>

#include "CircleObject2D.h"


class BallClass
{
public:
    BallClass();
    ~BallClass();

    // Inizializza la palla con posizione e dimensione
    bool Initialize(
        ID3D11Device* device,
        float x,
        float y,
        float size
    );

    // Rilascia le risorse della palla
    void Shutdown();

    // Aggiorna posizione e collisioni con i bordi
    void Update(float deltaTime);

    // Manda la palla alla pipeline grafica
    void Render(ID3D11DeviceContext* deviceContext);

    // Restituisce il numero di indici da disegnare
    int GetIndexCount() const;

    // Resetta posizione e velocità della palla
    void Reset(float x, float y, float velocityX, float velocityY);

    // Controlla se la palla è uscita dal basso
    bool IsBelowBottom() const;

    // Restituisce il bordo sinistro della palla
    float GetLeft() const;

    // Restituisce il bordo destro della palla
    float GetRight() const;

    // Restituisce il bordo superiore della palla
    float GetTop() const;

    // Restituisce il bordo inferiore della palla
    float GetBottom() const;

    // Controlla se la palla si sta muovendo verso il basso
    bool IsMovingDown() const;

    // Gestisce il rimbalzo sul paddle
    void BounceFromPaddle(float paddleTop, float hitFactor);

    // Inverte la direzione verticale della palla
    void BounceY();

    // Inverte la direzione orizzontale della palla
    void BounceX();

    // Imposta la posizione della palla
    void SetPosition(float x, float y);

    // Imposta la velocità della palla
    void SetVelocity(float velocityX, float velocityY);

    // Imposta il moltiplicatore di velocità della palla
    void SetSpeedMultiplier(float multiplier);

    // Restituisce la posizione X della palla
    float GetX() const;

    // Restituisce la posizione Y della palla
    float GetY() const;

    // Restituisce la velocità orizzontale della palla
    float GetVelocityX() const;

    // Restituisce la velocità verticale della palla
    float GetVelocityY() const;

private:
    // Gestisce il rimbalzo sui bordi dello schermo
    void CheckWallCollision();

private:
    CircleObject2D m_Circle;

    float m_size;

    float m_velocityX;
    float m_velocityY;

    float m_speedMultiplier;
};