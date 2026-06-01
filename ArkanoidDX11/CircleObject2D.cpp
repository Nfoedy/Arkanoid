#include "CircleObject2D.h"

#include <cmath>
#include <cstring>


/*
    CircleObject2D gestisce un cerchio 2D disegnato con DirectX

    La classe si occupa di:
    - creare vertex buffer e index buffer
    - costruire un cerchio tramite triangoli
    - aggiornare posizione, raggio e colore
    - fornire i bordi per le collisioni
*/


// Inizializza i valori base del cerchio
CircleObject2D::CircleObject2D()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    m_vertexCount = 0;
    m_indexCount = 0;

    m_x = 0.0f;
    m_y = 0.0f;

    m_radius = 0.0f;

    m_r = 1.0f;
    m_g = 1.0f;
    m_b = 1.0f;

    m_segments = 32;
}


// Distruttore della classe cerchio
CircleObject2D::~CircleObject2D()
{
}


// Inizializza il cerchio con posizione, raggio, colore e segmenti
bool CircleObject2D::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float radius,
    float r,
    float g,
    float b,
    int segments
)
{
    m_x = x;
    m_y = y;

    m_radius = radius;

    m_r = r;
    m_g = g;
    m_b = b;

    m_segments = segments;

    return InitializeBuffers(device);
}


// Rilascia le risorse del cerchio
void CircleObject2D::Shutdown()
{
    ShutdownBuffers();
}


// Aggiorna e manda il cerchio alla pipeline grafica
void CircleObject2D::Render(ID3D11DeviceContext* deviceContext)
{
    UpdateBuffers(deviceContext);
    RenderBuffers(deviceContext);
}


// Imposta la posizione del cerchio
void CircleObject2D::SetPosition(float x, float y)
{
    m_x = x;
    m_y = y;
}


// Imposta il raggio del cerchio
void CircleObject2D::SetRadius(float radius)
{
    m_radius = radius;
}


// Imposta il colore del cerchio
void CircleObject2D::SetColor(float r, float g, float b)
{
    m_r = r;
    m_g = g;
    m_b = b;
}


// Restituisce la posizione X del cerchio
float CircleObject2D::GetX() const
{
    return m_x;
}


// Restituisce la posizione Y del cerchio
float CircleObject2D::GetY() const
{
    return m_y;
}


// Restituisce il raggio del cerchio
float CircleObject2D::GetRadius() const
{
    return m_radius;
}


// Restituisce il bordo sinistro del cerchio
float CircleObject2D::GetLeft() const
{
    return m_x - m_radius;
}


// Restituisce il bordo destro del cerchio
float CircleObject2D::GetRight() const
{
    return m_x + m_radius;
}


// Restituisce il bordo superiore del cerchio
float CircleObject2D::GetTop() const
{
    return m_y + m_radius;
}


// Restituisce il bordo inferiore del cerchio
float CircleObject2D::GetBottom() const
{
    return m_y - m_radius;
}


// Restituisce il numero di indici da disegnare
int CircleObject2D::GetIndexCount() const
{
    return m_indexCount;
}


// Crea i buffer necessari per disegnare il cerchio
bool CircleObject2D::InitializeBuffers(ID3D11Device* device)
{
    m_vertexCount = m_segments + 2;
    m_indexCount = m_segments * 3;

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

    int index = 0;

    for (int i = 0; i < m_segments; i++)
    {
        indices[index++] = 0;
        indices[index++] = i + 1;
        indices[index++] = i + 2;
    }

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
void CircleObject2D::ShutdownBuffers()
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


// Aggiorna i vertici del cerchio
bool CircleObject2D::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    constexpr float PI = 3.14159265359f;

    VertexType* vertices = new VertexType[m_vertexCount];

    if (!vertices)
    {
        return false;
    }

    vertices[0].position = DirectX::XMFLOAT3(m_x, m_y, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    for (int i = 0; i <= m_segments; i++)
    {
        float angle = -2.0f * PI * static_cast<float>(i) / static_cast<float>(m_segments);

        float x = m_x + std::cos(angle) * m_radius;
        float y = m_y + std::sin(angle) * m_radius;

        vertices[i + 1].position = DirectX::XMFLOAT3(x, y, 0.0f);
        vertices[i + 1].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);
    }

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
        delete[] vertices;
        return false;
    }

    VertexType* verticesPtr = static_cast<VertexType*>(mappedResource.pData);

    std::memcpy(verticesPtr, vertices, sizeof(VertexType) * m_vertexCount);

    deviceContext->Unmap(m_vertexBuffer, 0);

    delete[] vertices;
    vertices = nullptr;

    return true;
}


// Imposta i buffer nella pipeline grafica
void CircleObject2D::RenderBuffers(ID3D11DeviceContext* deviceContext)
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