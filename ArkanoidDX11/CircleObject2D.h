#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class CircleObject2D
{
private:
    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

public:
    CircleObject2D();
    ~CircleObject2D();

    bool Initialize(
        ID3D11Device* device,
        float x,
        float y,
        float radius,
        float r,
        float g,
        float b,
        int segments = 32
    );

    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    void SetPosition(float x, float y);
    void SetRadius(float radius);
    void SetColor(float r, float g, float b);

    float GetX() const;
    float GetY() const;
    float GetRadius() const;

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

    float m_x;
    float m_y;

    float m_radius;

    float m_r;
    float m_g;
    float m_b;

    int m_segments;
};