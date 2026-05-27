#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class PaddleClass
{
private:
    // Struttura di un vertice, deve combaciare con l'Input Layout di ColorShaderClass
    struct VertexType
    {
        DirectX::XMFLOAT3 position; // POSITION nello shader
        DirectX::XMFLOAT4 color;    // COLOR nello shader
    };

public:
    PaddleClass();
    ~PaddleClass();

    // Inizializza il paddle con posizione e dimensione.
    bool Initialize(
        ID3D11Device* device,
        float x,
        float y,
        float width,
        float height
    );


    void Shutdown();        // Rilascia vertex buffer e index buffer
    void Render(ID3D11DeviceContext* deviceContext);       // Aggiorna il vertex buffer e manda i buffer alla pipeline

    // Movimento del paddle
    void MoveLeft();
    void MoveRight();

    // Ritorna il numero di indici da disegnare
    int GetIndexCount() const;

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();

    // Aggiorna i vertici in base alla posizione attuale del paddle
    bool UpdateBuffers(ID3D11DeviceContext* deviceContext);

    // Manda vertex buffer e index buffer alla pipeline
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

    // Impedisce al paddle di uscire dallo schermo
    void ClampToScreen();

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;

    // Posizione centrale del paddle in clip space
    float m_x;
    float m_y;

    // Dimensioni del paddle in clip space
    float m_width;
    float m_height;

    // Velocità per frame, no delta time per ora
    float m_speed;
};