#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class QuadClass
{
private:
    // Struttura di un singolo vertice, deve combaciare con l'Input Layout di ColorShaderClass
    struct VertexType
    {
        DirectX::XMFLOAT3 position; // POSITION nello shader
        DirectX::XMFLOAT4 color;    // COLOR nello shader
    };

public:
    QuadClass();
    ~QuadClass();


    bool Initialize(ID3D11Device* device);      // Crea vertex buffer e index buffer
    void Shutdown();    // Rilascia le risorse create

    void Render(ID3D11DeviceContext* deviceContext);    // Manda i buffer alla pipeline grafica

    int GetIndexCount() const;      // Ritorna il numero di indici da disegnare

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;
};