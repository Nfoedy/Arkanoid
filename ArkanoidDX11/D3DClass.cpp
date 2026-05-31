#include "D3DClass.h"

// Dico al linker di collegare le librerie DirectX necessarie
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")


D3DClass::D3DClass()
{
    // Inizializzo per creare le risorse
    m_device = nullptr;
    m_deviceContext = nullptr;
    m_swapChain = nullptr;
    m_renderTargetView = nullptr;
}


D3DClass::~D3DClass()
{
}


bool D3DClass::Initialize(HWND hwnd, int width, int height)
{
    HRESULT result;


    // 1. Descrizione della Swap Chain

    // La swap chain gestisce i buffer usati per mostrare i frame
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

    // Dimensione del back buffer
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;

    // Usiamo BGRA perché Direct2D richiede il supporto BGRA per disegnare sul back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    // Refresh rate, per ora 60 Hz
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

    // Antialiasing disattivato
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Il buffer verrà usato come render target, cioè come destinazione del rendering
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Numero di back buffer
    swapChainDesc.BufferCount = 1;

    // Finestra su cui DirectX presenterà il risultato
    swapChainDesc.OutputWindow = hwnd;

    // TRUE = modalità finestra
    // FALSE = fullscreen
    swapChainDesc.Windowed = TRUE;

    // Modalità di swap
    // DISCARD significa che dopo Present il contenuto del back buffer può essere scartato
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Necessario per permettere a Direct2D di disegnare sopra il back buffer DirectX.
    unsigned int createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;


    // 2. Creazione Device, DeviceContext e SwapChain

    result = D3D11CreateDeviceAndSwapChain(
        nullptr,                    // GPU di default
        D3D_DRIVER_TYPE_HARDWARE,   // usa la GPU hardware
        nullptr,                    // software rasterizer non usato
        createDeviceFlags,                          // flag, per ora nessuno
        nullptr,                    // feature levels automatici
        0,                          // numero feature levels
        D3D11_SDK_VERSION,          // versione SDK
        &swapChainDesc,             // descrizione swap chain
        &m_swapChain,               // output: swap chain
        &m_device,                  // output: device
        nullptr,                    // feature level scelto, non ci serve ora
        &m_deviceContext            // output: device context
    );

    if (FAILED(result))
    {
        return false;
    }

    // 3. Recupero del Back Buffer

    // Il back buffer è una texture 2D interna alla swap chain
    ID3D11Texture2D* backBuffer = nullptr;

    result = m_swapChain->GetBuffer(
        0,                              // primo buffer
        __uuidof(ID3D11Texture2D),      // voglo una texture 2D
        reinterpret_cast<void**>(&backBuffer)
    );

    if (FAILED(result))
    {
        Shutdown();
        return false;
    }


    // 4. Creazione RenderTargetView

    // Creiamo una view sul back buffer
    // DirectX non disegna direttamente sulla texture, ma su una RenderTargetView
    result = m_device->CreateRenderTargetView(
        backBuffer,
        nullptr,
        &m_renderTargetView
    );

    // Ora che abbiamo la RenderTargetView, possiamo rilasciare il riferimento diretto al back buffer
    backBuffer->Release();
    backBuffer = nullptr;

    if (FAILED(result))
    {
        Shutdown();
        return false;
    }


    // 5. Impostazione Render Target

    // Diciamo alla pipeline che il render target attivo è la nostra RenderTargetView
    m_deviceContext->OMSetRenderTargets(
        1,
        &m_renderTargetView,
        nullptr
    );


    // 6. Impostazione Viewport

    // Il viewport definisce l'area della finestra dove DirectX disegna
    D3D11_VIEWPORT viewport = {};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_deviceContext->RSSetViewports(1, &viewport);

    return true;
}


void D3DClass::Shutdown()
{
    // Se in futuro fosse attivo il fullscreen, lo disattiviamo prima di rilasciare la swap chain.
    if (m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, nullptr);
    }

    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    if (m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }

    if (m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }

    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }
}


void D3DClass::BeginScene(float r, float g, float b, float a)
{
    // Colore con cui puliamo lo schermo
    float clearColor[4] = { r, g, b, a };

    // Pulisce il render target con il colore scelto
    m_deviceContext->ClearRenderTargetView(
        m_renderTargetView,
        clearColor
    );
}


void D3DClass::EndScene()
{
    // Presenta il back buffer sullo schermo
    // 1 = VSync attivo
    // 0 = nessun flag speciale
    m_swapChain->Present(1, 0);
}


ID3D11Device* D3DClass::GetDevice()
{
    return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return m_deviceContext;
}

IDXGISwapChain* D3DClass::GetSwapChain()
{
    return m_swapChain;
}