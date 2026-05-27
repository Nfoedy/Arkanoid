#include "PaddleClass.h"


PaddleClass::PaddleClass()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    m_vertexCount = 0;
    m_indexCount = 0;

    m_x = 0.0f;
    m_y = 0.0f;

    m_width = 0.0f;
    m_height = 0.0f;

    m_speed = 0.02f;
}


PaddleClass::~PaddleClass()
{
}


bool PaddleClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float width,
    float height
)
{
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;

    return InitializeBuffers(device);
}


void PaddleClass::Shutdown()
{
    ShutdownBuffers();
}


void PaddleClass::Render(ID3D11DeviceContext* deviceContext)
{
    // Prima aggiorniamo il vertex buffer con la posizione attuale
    UpdateBuffers(deviceContext);

    // Poi mandiamo i buffer alla pipeline
    RenderBuffers(deviceContext);
}


void PaddleClass::MoveLeft()
{
    m_x -= m_speed;
    ClampToScreen();
}


void PaddleClass::MoveRight()
{
    m_x += m_speed;
    ClampToScreen();
}


int PaddleClass::GetIndexCount() const
{
    return m_indexCount;
}


bool PaddleClass::InitializeBuffers(ID3D11Device* device)
{
    // Il paddle è un rettangolo: 4 vertici, 6 indici.
    m_vertexCount = 4;
    m_indexCount = 6;

    VertexType* vertices = new VertexType[m_vertexCount];
    unsigned long* indices = new unsigned long[m_indexCount];

    if (!vertices || !indices)
    {
        return false;
    }

    /*
        Per ora inizializziamo i vertici a zero.
        Subito dopo li aggiorneremo con UpdateBuffers().
        Il vertex buffer sarà dinamico, perché il paddle si muove.
    */

    for (int i = 0; i < m_vertexCount; i++)
    {
        vertices[i].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices[i].color = DirectX::XMFLOAT4(0.9f, 0.9f, 1.0f, 1.0f);
    }

    /*
        Indici del rettangolo:

        0 -------- 1
        |        / |
        |      /   |
        |    /     |
        3 -------- 2

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
        Creazione Vertex Buffer dinamico.

        D3D11_USAGE_DYNAMIC:
        il buffer può essere aggiornato dalla CPU.

        D3D11_CPU_ACCESS_WRITE:
        permettiamo alla CPU di scrivere nel buffer.
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
        Creazione Index Buffer statico.

        Gli indici non cambiano mai:
        il rettangolo sarà sempre formato dagli stessi due triangoli.
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


void PaddleClass::ShutdownBuffers()
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


bool PaddleClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    /*
        Calcolo dei quattro lati del paddle.

        m_x e m_y rappresentano il centro del paddle.
        width e height sono la dimensione totale.
    */

    float left = m_x - (m_width * 0.5f);
    float right = m_x + (m_width * 0.5f);
    float top = m_y + (m_height * 0.5f);
    float bottom = m_y - (m_height * 0.5f);

    VertexType vertices[4];

    // Alto-sinistra
    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(0.9f, 0.9f, 1.0f, 1.0f);

    // Alto-destra
    vertices[1].position = DirectX::XMFLOAT3(right, top, 0.0f);
    vertices[1].color = DirectX::XMFLOAT4(0.9f, 0.9f, 1.0f, 1.0f);

    // Basso-destra
    vertices[2].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
    vertices[2].color = DirectX::XMFLOAT4(0.9f, 0.9f, 1.0f, 1.0f);

    // Basso-sinistra
    vertices[3].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
    vertices[3].color = DirectX::XMFLOAT4(0.9f, 0.9f, 1.0f, 1.0f);


    /*
        Per aggiornare un buffer dinamico:
        1. Map()
        2. memcpy()
        3. Unmap()
    */

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


void PaddleClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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


void PaddleClass::ClampToScreen()
{
    float halfWidth = m_width * 0.5f;

    if (m_x - halfWidth < -1.0f)
    {
        m_x = -1.0f + halfWidth;
    }

    if (m_x + halfWidth > 1.0f)
    {
        m_x = 1.0f - halfWidth;
    }
}


float PaddleClass::GetLeft() const
{
    return m_x - (m_width * 0.5f);
}


float PaddleClass::GetRight() const
{
    return m_x + (m_width * 0.5f);
}


float PaddleClass::GetTop() const
{
    return m_y + (m_height * 0.5f);
}


float PaddleClass::GetBottom() const
{
    return m_y - (m_height * 0.5f);
}