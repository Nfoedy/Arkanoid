#pragma once

#include <d3d11.h>

#include "RectObject2D.h"


enum class PowerUpType
{
    PaddleGrow,
    PaddleShrink,
    BallSpeedUp
};


class PowerUpClass
{
public:
    PowerUpClass();
    ~PowerUpClass();

    bool Initialize(
        ID3D11Device* device,
        float x,
        float y,
        PowerUpType type
    );

    void Shutdown();

    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount() const;

    bool IsActive() const;
    void SetActive(bool active);

    PowerUpType GetType() const;

    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;

private:
    RectObject2D m_Rect;

    PowerUpType m_type;

    bool m_isActive;

    float m_fallSpeed;
};