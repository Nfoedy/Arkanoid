#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>    // header principale di DirectX11



class D3DClass
{
public:
    D3DClass();
    ~D3DClass();

    // Inizializza DirectX 11: device, device context, swap chain, render target e viewport
    bool Initialize(HWND hwnd, int width, int height);

    // Rilascia tutte le risorse DirectX create
    void Shutdown();

    // Inizio frame: pulisce lo schermo con un colore
    void BeginScene(float r, float g, float b, float a);

    // Fine frame: mostra il back buffer sullo schermo
    void EndScene();

private:
    // Device = oggetto principale per creare risorse DirectX
    ID3D11Device* m_device;

    // DeviceContext = oggetto usato per dare comandi alla GPU
    ID3D11DeviceContext* m_deviceContext;

    // SwapChain = gestisce il back buffer e il front buffer
    // Serve per presentare il frame a schermo
    IDXGISwapChain* m_swapChain;

    // RenderTargetView = "vista" sul back buffer
    // DirectX disegna su questa view
    ID3D11RenderTargetView* m_renderTargetView;
};