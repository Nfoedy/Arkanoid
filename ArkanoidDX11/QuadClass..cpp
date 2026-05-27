#include "QuadClass.h"


QuadClass::QuadClass()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    m_vertexCount = 0;
    m_indexCount = 0;
}


QuadClass::~QuadClass()
{
}


bool QuadClass::Initialize(ID3D11Device* device)
{
    return InitializeBuffers(device);
}


void QuadClass::Shutdown()
{
    ShutdownBuffers();
}


void QuadClass::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);
}


int QuadClass::GetIndexCount() const
{
    return m_indexCount;
}


bool QuadClass::InitializeBuffers(ID3D11Device* device)
{
    // Un rettangolo è composto da 4 vertici
    m_vertexCount = 4;

    // Un rettangolo viene disegnato con 2 triangoli
    // Ogni triangolo ha 3 indici
    // Quindi 2 triangoli = 6 indici
    m_indexCount = 6;

    VertexType* vertices = new VertexType[m_vertexCount];
    unsigned long* indices = new unsigned long[m_indexCount];

    if (!vertices || !indices)
    {
        return false;
    }

    /*
        Coordinate in clip space:

        x = -1 sinistra
        x =  1 destra

        y = -1 basso
        y =  1 alto

        Per ora creiamo un rettangolo al centro dello schermo.
    */

    // Alto-sinistra
    vertices[0].position = DirectX::XMFLOAT3(-0.5f, 0.3f, 0.0f);
    vertices[0].color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    // Alto-destra
    vertices[1].position = DirectX::XMFLOAT3(0.5f, 0.3f, 0.0f);
    vertices[1].color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // Basso-destra
    vertices[2].position = DirectX::XMFLOAT3(0.5f, -0.3f, 0.0f);
    vertices[2].color = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    // Basso-sinistra
    vertices[3].position = DirectX::XMFLOAT3(-0.5f, -0.3f, 0.0f);
    vertices[3].color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);


    /*
        Indici del rettangolo.

        I vertici sono:

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
        Creazione Vertex Buffer
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
        Creazione Index Buffer
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


void QuadClass::ShutdownBuffers()
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


void QuadClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // Mando il vertex buffer alla Input Assembler stage
    deviceContext->IASetVertexBuffers(
        0,
        1,
        &m_vertexBuffer,
        &stride,
        &offset
    );

    // Mando l'index buffer alla pipeline
    deviceContext->IASetIndexBuffer(
        m_indexBuffer,
        DXGI_FORMAT_R32_UINT,
        0
    );

    // Diciamo a DirectX che vogliamo disegnare triangoli
    deviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
}