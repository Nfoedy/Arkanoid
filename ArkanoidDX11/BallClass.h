#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class BallClass
{
private:
    struct VertexType
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

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

    // Aggiorna posizione e rimbalzi
    void Update();

    // Aggiorna il vertex buffer e manda i buffer alla pipeline
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount() const;

    // Ritorna i lati della palla.
    // Per ora la palla è un quadratino.
    float GetLeft() const;
    float GetRight() const;
    float GetTop() const;
    float GetBottom() const;

    // Controlla se la palla sta andando verso il basso.
    bool IsMovingDown() const;

    // Fa rimbalzare la palla sul paddle.
    void BounceFromPaddle(float paddleTop);

    // Inverte la direzione verticale della palla la uso quando la palla colpisce un brick.
    void BounceY();

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();

    bool UpdateBuffers(ID3D11DeviceContext* deviceContext);
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

    void CheckWallCollision();

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;

    // Posizione centrale della palla in clip space
    float m_x;
    float m_y;

    // Dimensione della palla, per ora è un quadratino
    float m_size;

    // Velocità della palla
    float m_velocityX;
    float m_velocityY;
};