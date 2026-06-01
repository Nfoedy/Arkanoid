#pragma once

#include <d3d11.h>

#include "RectObject2D.h"


class BrickClass
{
public:
    BrickClass();
    ~BrickClass();

    // Inizializza il brick con posizione, dim e colore
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

    // Rilascia le risorse del brick
    void Shutdown();

    // Manda il brick alla pipeline grafica
    void Render(ID3D11DeviceContext* deviceContext);

    // Restituisce il numero di indici da disegnare
    int GetIndexCount() const;

    // Controlla se il brick è ancora attivo
    bool IsActive() const;

    // Imposta lo stato attivo del brick
    void SetActive(bool active);

    // Restituisce il bordo sinistro del brick
    float GetLeft() const;

    // Restituisce il bordo destro del brick
    float GetRight() const;

    // Restituisce il bordo superiore del brick
    float GetTop() const;

    // Restituisce il bordo inferiore del brick
    float GetBottom() const;

    // Restituisce la posizione X del brick
    float GetX() const;

    // Restituisce la posizione Y del brick
    float GetY() const;

private:
    RectObject2D m_Rect;

    bool m_isActive;
};