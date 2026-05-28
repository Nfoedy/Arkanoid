#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class RectObject2D
{
private:
    // Struttura di un vertice.
    // Deve combaciare con l'Input Layout di ColorShaderClass.
    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

public:
    RectObject2D();
    ~RectObject2D();

    // Inizializza un rettangolo 2D con posizione, dimensione e colore.
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

    // Rilascia vertex buffer e index buffer.
    void Shutdown();

    // Aggiorna il vertex buffer e manda i buffer alla pipeline.
    void Render(ID3D11DeviceContext* deviceContext);

    // Modifica posizione, dimensione e colore.
    void SetPosition(float x, float y);
    void SetSize(float width, float height);
    void SetColor(float r, float g, float b);

    // Getter posizione/dimensione.
    float GetX() const;
    float GetY() const;
    float GetWidth() const;
    float GetHeight() const;

    // Getter per collisioni AABB.
    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;

    int GetIndexCount() const;

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();

    bool UpdateBuffers(ID3D11DeviceContext* deviceContext);
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;

    // Centro del rettangolo in clip space.
    float m_x;
    float m_y;

    // Dimensione del rettangolo.
    float m_width;
    float m_height;

    // Colore del rettangolo.
    float m_r;
    float m_g;
    float m_b;
};