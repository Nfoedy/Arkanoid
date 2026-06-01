#pragma once

#include <d3d11.h>

#include "RectObject2D.h"


class PaddleClass
{
public:
    PaddleClass();
    ~PaddleClass();

    // Inizializza il paddle con posizione e dimensione
    bool Initialize(
        ID3D11Device* device,
        float x,
        float y,
        float width,
        float height
    );

    // Rilascia le risorse
    void Shutdown();

    // Manda il rettangolo alla pipeline
    void Render(ID3D11DeviceContext* deviceContext);

    // Movimento del paddle
    void MoveLeft(float deltaTime);
    void MoveRight(float deltaTime);

    // Riporta il paddle a una posizione iniziale
    void Reset(float x, float y);

    // Cambia temporaneamente la larghezza del paddle
    void SetWidth(float width);

    int GetIndexCount() const;

    // Getter per collisioni
    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;


private:
    // Impedisce al paddle di uscire dallo schermo
    void ClampToScreen();

private:
    RectObject2D m_Rect;

    // Velocità in unità clip-space al secondo
    float m_speed;
};