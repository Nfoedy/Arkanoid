#include "ColorShaderClass.h"

#pragma comment(lib, "d3dcompiler.lib")     // Libreria necessaria per compilare gli shader HLSL da file.


ColorShaderClass::ColorShaderClass()
{
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_layout = nullptr;
}


ColorShaderClass::~ColorShaderClass()
{
}


bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // Compiliamo e inizializziamo gli shader color.vs e color.ps
    return InitializeShader(device, hwnd, L"color.vs", L"color.ps");
}


void ColorShaderClass::Shutdown()
{
    ShutdownShader();
}


bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename)
{
    HRESULT result;

    // Buffer che conterranno il bytecode compilato degli shader
    ID3DBlob* vertexShaderBuffer = nullptr;
    ID3DBlob* pixelShaderBuffer = nullptr;

    // Buffer per eventuali messaggi di errore durante la compilazione
    ID3DBlob* errorMessage = nullptr;


    /*
        1. Compilazione Vertex Shader
    */

    result = D3DCompileFromFile(
        vsFilename,                     // nome file shader
        nullptr,                        // macro, non usate
        nullptr,                        // include handler, non usato
        "ColorVertexShader",            // nome funzione nello shader
        "vs_5_0",                       // profilo Vertex Shader DirectX 11
        D3D10_SHADER_ENABLE_STRICTNESS, // compilazione più rigida
        0,
        &vertexShaderBuffer,            // output bytecode shader
        &errorMessage                   // output eventuali errori
    );

    if (FAILED(result))
    {
        if (errorMessage)
        {
            MessageBoxA(hwnd, static_cast<char*>(errorMessage->GetBufferPointer()), "Errore Vertex Shader", MB_OK);
            errorMessage->Release();
            errorMessage = nullptr;
        }
        else
        {
            MessageBox(hwnd, vsFilename, L"File vertex shader non trovato", MB_OK);
        }

        return false;
    }


    /*
        2. Compilazione Pixel Shader
    */

    result = D3DCompileFromFile(
        psFilename,                     // nome file shader
        nullptr,                        // macro, non usate
        nullptr,                        // include handler, non usato
        "ColorPixelShader",             // nome funzione nello shader
        "ps_5_0",                       // profilo Pixel Shader DirectX 11
        D3D10_SHADER_ENABLE_STRICTNESS, // compilazione più rigida
        0,
        &pixelShaderBuffer,             // output bytecode shader
        &errorMessage                   // output eventuali errori
    );

    if (FAILED(result))
    {
        if (errorMessage)
        {
            MessageBoxA( hwnd, static_cast<char*>(errorMessage->GetBufferPointer()), "Errore Pixel Shader", MB_OK);
            errorMessage->Release();
            errorMessage = nullptr;
        }
        else
        {
            MessageBox( hwnd, psFilename, L"File pixel shader non trovato", MB_OK);
        }

        if (vertexShaderBuffer)
        {
            vertexShaderBuffer->Release();
            vertexShaderBuffer = nullptr;
        }

        return false;
    }


    /*
        3. Creazione Vertex Shader
    */

    result = device->CreateVertexShader(
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        nullptr,
        &m_vertexShader
    );

    if (FAILED(result))
    {
        vertexShaderBuffer->Release();
        pixelShaderBuffer->Release();

        return false;
    }


    /*
        4. Creazione Pixel Shader
    */

    result = device->CreatePixelShader(
        pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(),
        nullptr,
        &m_pixelShader
    );

    if (FAILED(result))
    {
        vertexShaderBuffer->Release();
        pixelShaderBuffer->Release();

        return false;
    }


    /*
        5. Creazione Input Layout
    */

    // L'input layout deve descrivere esattamente la struttura dei vertici.
    // Nel nostro caso:
    // POSITION = XMFLOAT3 / float3
    // COLOR    = XMFLOAT4 / float4

    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

    // POSITION
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    // COLOR
    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(
        polygonLayout,
        numElements,
        vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(),
        &m_layout
    );

    // Dopo aver creato gli shader e l'input layout, i buffer compilati non servono più
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    if (FAILED(result))
    {
        return false;
    }

    return true;
}


void ColorShaderClass::ShutdownShader()
{
    if (m_layout)
    {
        m_layout->Release();
        m_layout = nullptr;
    }

    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }

    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }
}


void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
    // Impostiamo l'input layout, così DirectX sa come interpretare i dati dei vertici.
    deviceContext->IASetInputLayout(m_layout);

    // Attiviamo il Vertex Shader.
    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);

    // Attiviamo il Pixel Shader.
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
}