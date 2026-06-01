#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class RectObject2D
{
private:
    // Deve combaciare con l'input layout di ColorShaderClass.
    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

public:
    RectObject2D();
    ~RectObject2D();

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

    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    void SetPosition(float x, float y);
    void SetSize(float width, float height);
    void SetColor(float r, float g, float b);

    float GetX() const;
    float GetY() const;
    float GetWidth() const;
    float GetHeight() const;

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

    float m_width;
    float m_height;

    float m_r;
    float m_g;
    float m_b;
};