#pragma once

#include <d3d11.h>

#include "RectObject2D.h"


class BrickClass
{
public:
    BrickClass();
    ~BrickClass();

    // Inizializza un singolo brick con posizione, dimensione e colore.
    bool Initialize(
        ID3D11Device* device,
        float x,
        float y,
        float width,
        float height,
        float r,
        float g,
        float b
    );

    // Rilascia le risorse DirectX del brick.
    void Shutdown();

    // Manda vertex buffer e index buffer alla pipeline.
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount() const;

    bool IsActive() const;
    void SetActive(bool active);

    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;


private:
    RectObject2D m_Rect;

    bool m_isActive;
};