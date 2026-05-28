#include "RectObject2D.h"

#include <cstring>


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


RectObject2D::~RectObject2D()
{
}


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


void RectObject2D::Shutdown()
{
    ShutdownBuffers();
}


void RectObject2D::Render(ID3D11DeviceContext* deviceContext)
{
    // Aggiorniamo i vertici in base ai valori attuali.
    UpdateBuffers(deviceContext);

    // Mandiamo vertex buffer e index buffer alla pipeline.
    RenderBuffers(deviceContext);
}


void RectObject2D::SetPosition(float x, float y)
{
    m_x = x;
    m_y = y;
}


void RectObject2D::SetSize(float width, float height)
{
    m_width = width;
    m_height = height;
}


void RectObject2D::SetColor(float r, float g, float b)
{
    m_r = r;
    m_g = g;
    m_b = b;
}


float RectObject2D::GetX() const
{
    return m_x;
}


float RectObject2D::GetY() const
{
    return m_y;
}


float RectObject2D::GetWidth() const
{
    return m_width;
}


float RectObject2D::GetHeight() const
{
    return m_height;
}


float RectObject2D::GetLeft() const
{
    return m_x - (m_width * 0.5f);
}


float RectObject2D::GetRight() const
{
    return m_x + (m_width * 0.5f);
}


float RectObject2D::GetTop() const
{
    return m_y + (m_height * 0.5f);
}


float RectObject2D::GetBottom() const
{
    return m_y - (m_height * 0.5f);
}


int RectObject2D::GetIndexCount() const
{
    return m_indexCount;
}


bool RectObject2D::InitializeBuffers(ID3D11Device* device)
{
    /*
        Un rettangolo è formato da:
        - 4 vertici
        - 6 indici
        - 2 triangoli
    */

    m_vertexCount = 4;
    m_indexCount = 6;

    VertexType* vertices = new VertexType[m_vertexCount];
    unsigned long* indices = new unsigned long[m_indexCount];

    if (!vertices || !indices)
    {
        return false;
    }

    /*
        Inizializziamo i vertici a zero.
        La posizione reale viene aggiornata in UpdateBuffers().
    */

    for (int i = 0; i < m_vertexCount; i++)
    {
        vertices[i].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices[i].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);
    }

    /*
        Indici del rettangolo:

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
        Vertex buffer dinamico.

        Lo rendiamo dinamico perché paddle e ball si muoveranno.
        Per i brick non sarebbe strettamente necessario, ma per ora
        preferiamo avere una classe unica e semplice.
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
        Index buffer statico.

        Gli indici non cambiano mai.
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


bool RectObject2D::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    float left = GetLeft();
    float right = GetRight();
    float top = GetTop();
    float bottom = GetBottom();

    VertexType vertices[4];

    // Alto-sinistra
    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    // Alto-destra
    vertices[1].position = DirectX::XMFLOAT3(right, top, 0.0f);
    vertices[1].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    // Basso-destra
    vertices[2].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
    vertices[2].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);

    // Basso-sinistra
    vertices[3].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
    vertices[3].color = DirectX::XMFLOAT4(m_r, m_g, m_b, 1.0f);


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