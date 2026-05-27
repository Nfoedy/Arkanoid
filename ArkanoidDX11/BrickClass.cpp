#include "BrickClass.h"


BrickClass::BrickClass()
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

    m_isActive = true;
}


BrickClass::~BrickClass()
{
}


bool BrickClass::Initialize(
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

    m_isActive = true;

    return InitializeBuffers(device);
}


void BrickClass::Shutdown()
{
    ShutdownBuffers();
}


void BrickClass::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);
}


int BrickClass::GetIndexCount() const
{
    return m_indexCount;
}


bool BrickClass::IsActive() const
{
    return m_isActive;
}


void BrickClass::SetActive(bool active)
{
    m_isActive = active;
}


float BrickClass::GetLeft() const
{
    return m_x - (m_width * 0.5f);
}


float BrickClass::GetRight() const
{
    return m_x + (m_width * 0.5f);
}


float BrickClass::GetTop() const
{
    return m_y + (m_height * 0.5f);
}


float BrickClass::GetBottom() const
{
    return m_y - (m_height * 0.5f);
}


bool BrickClass::InitializeBuffers(ID3D11Device* device)
{
    m_vertexCount = 4;
    m_indexCount = 6;

    VertexType* vertices = new VertexType[m_vertexCount];
    unsigned long* indices = new unsigned long[m_indexCount];

    if (!vertices || !indices)
    {
        return false;
    }

    /*
        Calcolo dei lati del brick partendo dal centro.

        m_x / m_y = centro
        m_width / m_height = dimensione totale
    */

    float left = GetLeft();
    float right = GetRight();
    float top = GetTop();
    float bottom = GetBottom();

    /*
        Vertici del rettangolo:

        0 -------- 1
        |        / |
        |      /   |
        |    /     |
        3 -------- 2
    */

    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    vertices[1].position = DirectX::XMFLOAT3(right, top, 0.0f);
    vertices[1].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    vertices[2].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
    vertices[2].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    vertices[3].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
    vertices[3].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);


    /*
        Due triangoli:

        Primo triangolo:  0, 1, 2
        Secondo triangolo: 0, 2, 3
    */

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;


    /*
        Vertex buffer statico.

        I brick per ora non si muovono,
        quindi non serve D3D11_USAGE_DYNAMIC.
    */

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
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
        Index buffer statico.
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


void BrickClass::ShutdownBuffers()
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


void BrickClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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