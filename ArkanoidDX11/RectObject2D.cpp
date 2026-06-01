#include "RectObject2D.h"

#include <cstring>


/*
    RectObject2D gestisce un rettangolo 2D disegnato con DirectX

    La classe si occupa di:
    - creare vertex buffer e index buffer
    - aggiornare posizione, dimensione e colore
    - mandare i buffer alla pipeline grafica
    - fornire i bordi per le collisioni AABB
*/


// Inizializza i valori base del rettangolo
RectObject2D::RectObject2D()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    m_vertexCount = 0;
    m_indexCount = 0;

    m_x = 0.0f;
    m_y = 0.0f;

    m_width = 0.0f;
    m_height = 0.0f;

    m_r = 1.0f;
    m_g = 1.0f;
    m_b = 1.0f;
}


// Distruttore della classe rettangolo
RectObject2D::~RectObject2D()
{
}


// Inizializza il rettangolo con posizione, dimensione e colore
bool RectObject2D::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float width,
    float height,
    float r,
    float g,
    float b
)
{
    m_x = x;
    m_y = y;

    m_width = width;
    m_height = height;

    m_r = r;
    m_g = g;
    m_b = b;

    return InitializeBuffers(device);
}


// Rilascia le risorse del rettangolo
void RectObject2D::Shutdown()
{
    ShutdownBuffers();
}


// Aggiorna e manda il rettangolo alla pipeline grafica
void RectObject2D::Render(ID3D11DeviceContext* deviceContext)
{
    UpdateBuffers(deviceContext);
    RenderBuffers(deviceContext);
}


// Imposta la posizione del rettangolo
void RectObject2D::SetPosition(float x, float y)
{
    m_x = x;
    m_y = y;
}


// Imposta la dimensione del rettangolo
void RectObject2D::SetSize(float width, float height)
{
    m_width = width;
    m_height = height;
}


// Imposta il colore del rettangolo
void RectObject2D::SetColor(float r, float g, float b)
{
    m_r = r;
    m_g = g;
    m_b = b;
}


// Restituisce la posizione X del rettangolo
float RectObject2D::GetX() const
{
    return m_x;
}


// Restituisce la posizione Y del rettangolo
float RectObject2D::GetY() const
{
    return m_y;
}


// Restituisce la larghezza del rettangolo
float RectObject2D::GetWidth() const
{
    return m_width;
}


// Restituisce l'altezza del rettangolo
float RectObject2D::GetHeight() const
{
    return m_height;
}


// Restituisce il bordo sinistro del rettangolo
float RectObject2D::GetLeft() const
{
    return m_x - (m_width * 0.5f);
}


// Restituisce il bordo destro del rettangolo
float RectObject2D::GetRight() const
{
    return m_x + (m_width * 0.5f);
}


// Restituisce il bordo superiore del rettangolo
float RectObject2D::GetTop() const
{
    return m_y + (m_height * 0.5f);
}


// Restituisce il bordo inferiore del rettangolo
float RectObject2D::GetBottom() const
{
    return m_y - (m_height * 0.5f);
}


// Restituisce il numero di indici da disegnare
int RectObject2D::GetIndexCount() const
{
    return m_indexCount;
}


// Crea i buffer necessari per disegnare il rettangolo
bool RectObject2D::InitializeBuffers(ID3D11Device* device)
{
    m_vertexCount = 4;
    m_indexCount = 6;

    VertexType* vertices = new VertexType[m_vertexCount];
    unsigned long* indices = new unsigned long[m_indexCount];

    if (!vertices || !indices)
    {
        return false;
    }

    for (int i = 0; i < m_vertexCount; i++)
    {
        vertices[i].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices[i].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);
    }

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};

    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(
        &vertexBufferDesc,
        &vertexData,
        &m_vertexBuffer
    );

    if (FAILED(result))
    {
        delete[] vertices;
        delete[] indices;

        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc = {};

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};

    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(
        &indexBufferDesc,
        &indexData,
        &m_indexBuffer
    );

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    if (FAILED(result))
    {
        return false;
    }

    return true;
}


// Rilascia vertex buffer e index buffer
void RectObject2D::ShutdownBuffers()
{
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
}


// Aggiorna i vertici del rettangolo
bool RectObject2D::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    float left = GetLeft();
    float right = GetRight();
    float top = GetTop();
    float bottom = GetBottom();

    VertexType vertices[4];

    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    vertices[1].position = DirectX::XMFLOAT3(right, top, 0.0f);
    vertices[1].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    vertices[2].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
    vertices[2].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    vertices[3].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
    vertices[3].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    HRESULT result = deviceContext->Map(
        m_vertexBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedResource
    );

    if (FAILED(result))
    {
        return false;
    }

    VertexType* verticesPtr = static_cast<VertexType*>(mappedResource.pData);

    std::memcpy(verticesPtr, vertices, sizeof(VertexType) * m_vertexCount);

    deviceContext->Unmap(m_vertexBuffer, 0);

    return true;
}


// Imposta i buffer nella pipeline grafica
void RectObject2D::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(
        0,
        1,
        &m_vertexBuffer,
        &stride,
        &offset
    );

    deviceContext->IASetIndexBuffer(
        m_indexBuffer,
        DXGI_FORMAT_R32_UINT,
        0
    );

    deviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
}