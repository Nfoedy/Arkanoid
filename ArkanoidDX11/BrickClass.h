#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class BrickClass
{
private:

    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

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

    // Per ora i brick sono sempre attivi.
    // Più avanti useremo questo per distruggerli.
    bool IsActive() const;
    void SetActive(bool active);

    // Getter per collisioni future.
    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;

    // Centro del brick.
    float m_x;
    float m_y;

    // Dimensioni del brick.
    float m_width;
    float m_height;

    // Colore del brick.
    float m_r;
    float m_g;
    float m_b;

    // Indica se il brick deve essere disegnato.
    bool m_isActive;
};