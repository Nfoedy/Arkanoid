#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>    // header principale di DirectX11



class D3DClass
{
public:
    D3DClass();
    ~D3DClass();

    
    bool Initialize(HWND hwnd, int width, int height);  // Inizializza DirectX 11: device, device context, swap chain, render target e viewport
    void Shutdown();    // Rilascia tutte le risorse DirectX create

 
    void BeginScene(float r, float g, float b, float a);       // Inizio frame: pulisce lo schermo con un colore
    void EndScene();        // Fine frame: mostra il back buffer sullo schermo

    // Ritorna il device DirectX, serve alle altre classi per creare risorse GPU
    ID3D11Device* GetDevice();

    // Ritorna il device context, serve per mandare comandi alla GPU
    ID3D11DeviceContext* GetDeviceContext();

    IDXGISwapChain* GetSwapChain();


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