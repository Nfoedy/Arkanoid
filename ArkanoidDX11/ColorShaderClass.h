#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>


class ColorShaderClass
{
public:
    ColorShaderClass();
    ~ColorShaderClass();


    bool Initialize(ID3D11Device* device, HWND hwnd);       // Inizializza vertex shader, pixel shader e input layout
    void Shutdown();        // Rilascia tutte le risorse create da questa classe

    void RenderShader(ID3D11DeviceContext* deviceContext);      // Attiva lo shader prima di disegnare

private:
    // Compila gli shader da file e crea gli oggetti DirectX
    bool InitializeShader(
        ID3D11Device* device,
        HWND hwnd,
        const wchar_t* vsFilename,
        const wchar_t* psFilename
    );

    // Rilascia vertex shader, pixel shader e input layout
    void ShutdownShader();

private:
    // Vertex Shader compilato e creato sulla GPU
    ID3D11VertexShader* m_vertexShader;

    // Pixel Shader compilato e creato sulla GPU
    ID3D11PixelShader* m_pixelShader;

    // Input Layout.
    // Dice a DirectX come leggere i dati dei vertici:
    // POSITION = float3
    // COLOR    = float4
    ID3D11InputLayout* m_layout;
};