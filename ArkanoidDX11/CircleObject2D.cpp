#include "CircleObject2D.h"

#include <cmath>
#include <cstring>


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


CircleObject2D::~CircleObject2D()
{
}


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


void CircleObject2D::Shutdown()
{
    ShutdownBuffers();
}


void CircleObject2D::Render(ID3D11DeviceContext* deviceContext)
{
    UpdateBuffers(deviceContext);
    RenderBuffers(deviceContext);
}


void CircleObject2D::SetPosition(float x, float y)
{
    m_x = x;
    m_y = y;
}


void CircleObject2D::SetRadius(float radius)
{
    m_radius = radius;
}


void CircleObject2D::SetColor(float r, float g, float b)
{
    m_r = r;
    m_g = g;
    m_b = b;
}


float CircleObject2D::GetX() const
{
    return m_x;
}


float CircleObject2D::GetY() const
{
    return m_y;
}


float CircleObject2D::GetRadius() const
{
    return m_radius;
}


float CircleObject2D::GetLeft() const
{
    return m_x - m_radius;
}


float CircleObject2D::GetRight() const
{
    return m_x + m_radius;
}


float CircleObject2D::GetTop() const
{
    return m_y + m_radius;
}


float CircleObject2D::GetBottom() const
{
    return m_y - m_radius;
}


int CircleObject2D::GetIndexCount() const
{
    return m_indexCount;
}


bool CircleObject2D::InitializeBuffers(ID3D11Device* device)
{
    /*
        Cerchio disegnato come triangle fan.

        Vertici:
        - vertice 0 = centro
        - vertici successivi = bordo del cerchio

        Ogni triangolo:
        centro, punto bordo A, punto bordo B
    */

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

    /*
        Vertex buffer dinamico:
        la palla si muove, quindi aggiorniamo i vertici ogni frame.
    */

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


    /*
        Index buffer statico:
        gli indici non cambiano.
    */

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


bool CircleObject2D::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    constexpr float PI = 3.14159265359f;

    VertexType* vertices = new VertexType[m_vertexCount];

    if (!vertices)
    {
        return false;
    }

    /*
        Vertice centrale.
    */

    vertices[0].position = DirectX::XMFLOAT3(m_x, m_y, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);


    /*
        Vertici del bordo.

        Usiamo angolo negativo per mantenere un winding coerente
        con il resto dei rettangoli già disegnati.
    */

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

    memcpy(verticesPtr, vertices, sizeof(VertexType) * m_vertexCount);

    deviceContext->Unmap(m_vertexBuffer, 0);

    delete[] vertices;
    vertices = nullptr;

    return true;
}


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