#include "BallClass.h"

#include <cstring>


BallClass::BallClass()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    m_vertexCount = 0;
    m_indexCount = 0;

    m_x = 0.0f;
    m_y = 0.0f;

    m_size = 0.0f;

    m_velocityX = 0.01f;
    m_velocityY = 0.012f;
}


BallClass::~BallClass()
{
}


bool BallClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float size
)
{
    m_x = x;
    m_y = y;
    m_size = size;

    return InitializeBuffers(device);
}


void BallClass::Shutdown()
{
    ShutdownBuffers();
}


void BallClass::Update()
{
    // Aggiorno la posizione della palla.
    m_x += m_velocityX;
    m_y += m_velocityY;

    // Controllo se ha toccato i bordi dello schermo.
    CheckWallCollision();
}


void BallClass::Render(ID3D11DeviceContext* deviceContext)
{
    UpdateBuffers(deviceContext);
    RenderBuffers(deviceContext);
}


int BallClass::GetIndexCount() const
{
    return m_indexCount;
}


bool BallClass::InitializeBuffers(ID3D11Device* device)
{
    m_vertexCount = 4;
    m_indexCount = 6;

    VertexType* vertices = new VertexType[m_vertexCount];
    unsigned long* indices = new unsigned long[m_indexCount];

    if (!vertices || !indices)
    {
        return false;
    }

    // Inizializzo i vertici a zero.
    // Verranno aggiornati ogni frame con UpdateBuffers().
    for (int i = 0; i < m_vertexCount; i++)
    {
        vertices[i].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices[i].color = DirectX::XMFLOAT4(1.0f, 0.9f, 0.2f, 1.0f);
    }

    /*
        Rettangolo formato da due triangoli:

        0 -------- 1
        |        / |
        |      /   |
        |    /     |
        3 -------- 2
    */

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;


    /*
        Vertex buffer dinamico:
        la palla si muove, quindi i vertici cambiano ogni frame.
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
        gli indici non cambiano mai.
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


void BallClass::ShutdownBuffers()
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


bool BallClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    float halfSize = m_size * 0.5f;

    float left = m_x - halfSize;
    float right = m_x + halfSize;
    float top = m_y + halfSize;
    float bottom = m_y - halfSize;

    VertexType vertices[4];

    // Alto-sinistra
    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(1.0f, 0.9f, 0.2f, 1.0f);

    // Alto-destra
    vertices[1].position = DirectX::XMFLOAT3(right, top, 0.0f);
    vertices[1].color = DirectX::XMFLOAT4(1.0f, 0.9f, 0.2f, 1.0f);

    // Basso-destra
    vertices[2].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
    vertices[2].color = DirectX::XMFLOAT4(1.0f, 0.9f, 0.2f, 1.0f);

    // Basso-sinistra
    vertices[3].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
    vertices[3].color = DirectX::XMFLOAT4(1.0f, 0.9f, 0.2f, 1.0f);


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

    memcpy(verticesPtr, vertices, sizeof(VertexType) * m_vertexCount);

    deviceContext->Unmap(m_vertexBuffer, 0);

    return true;
}


void BallClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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


void BallClass::CheckWallCollision()
{
    float halfSize = m_size * 0.5f;

    // Bordo sinistro.
    if (m_x - halfSize < -1.0f)
    {
        m_x = -1.0f + halfSize;
        m_velocityX *= -1.0f;
    }

    // Bordo destro.
    if (m_x + halfSize > 1.0f)
    {
        m_x = 1.0f - halfSize;
        m_velocityX *= -1.0f;
    }

    // Bordo alto.
    if (m_y + halfSize > 1.0f)
    {
        m_y = 1.0f - halfSize;
        m_velocityY *= -1.0f;
    }


}



float BallClass::GetLeft() const
{
    return m_x - (m_size * 0.5f);
}


float BallClass::GetRight() const
{
    return m_x + (m_size * 0.5f);
}


float BallClass::GetTop() const
{
    return m_y + (m_size * 0.5f);
}


float BallClass::GetBottom() const
{
    return m_y - (m_size * 0.5f);
}


bool BallClass::IsMovingDown() const
{
    return m_velocityY < 0.0f;
}


void BallClass::BounceFromPaddle(float paddleTop)
{
    // Spostiamo la palla appena sopra il paddle.
    // Questo evita che rimanga incastrata dentro il paddle
    // e continui a invertire direzione ogni frame.
    m_y = paddleTop + (m_size * 0.5f);

    // Se la palla stava andando verso il basso,
    // invertiamo la direzione verticale.
    if (m_velocityY < 0.0f)
    {
        m_velocityY *= -1.0f;
    }
}


void BallClass::BounceY()
{
    m_velocityY *= -1.0f;
}


void BallClass::Reset(float x, float y, float velocityX, float velocityY)
{
    m_x = x;
    m_y = y;

    m_velocityX = velocityX;
    m_velocityY = velocityY;
}


bool BallClass::IsBelowBottom() const
{
    return GetBottom() < -1.0f;
}