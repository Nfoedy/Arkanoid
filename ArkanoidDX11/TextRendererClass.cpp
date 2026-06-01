#include "TextRendererClass.h"

#include <cwchar>


#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")


/*
    TextRendererClass gestisce il rendering del testo con DirectWrite e Direct2D

    La classe si occupa di:
    - creare le risorse Direct2D e DirectWrite
    - disegnare titoli, menu e testi piccoli
    - disegnare HUD con score e vite
    - scrivere direttamente sopra il back buffer DirectX
*/


// Inizializza i puntatori Direct2D e DirectWrite
TextRendererClass::TextRendererClass()
{
    m_d2dFactory = nullptr;
    m_renderTarget = nullptr;

    m_dwriteFactory = nullptr;

    m_titleFormat = nullptr;
    m_menuFormat = nullptr;
    m_smallFormat = nullptr;
    m_hudLeftFormat = nullptr;
    m_hudRightFormat = nullptr;

    m_whiteBrush = nullptr;
    m_yellowBrush = nullptr;
    m_grayBrush = nullptr;
}


// Distruttore della classe text renderer
TextRendererClass::~TextRendererClass()
{
}


// Inizializza Direct2D e DirectWrite sulla swap chain
bool TextRendererClass::Initialize(IDXGISwapChain* swapChain)
{
    if (!swapChain)
    {
        return false;
    }

    HRESULT result;

    result = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &m_d2dFactory
    );

    if (FAILED(result))
    {
        return false;
    }

    result = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_dwriteFactory)
    );

    if (FAILED(result))
    {
        return false;
    }

    IDXGISurface* dxgiBackBuffer = nullptr;

    result = swapChain->GetBuffer(
        0,
        __uuidof(IDXGISurface),
        reinterpret_cast<void**>(&dxgiBackBuffer)
    );

    if (FAILED(result))
    {
        return false;
    }

    D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(
                DXGI_FORMAT_UNKNOWN,
                D2D1_ALPHA_MODE_PREMULTIPLIED
            )
        );

    result = m_d2dFactory->CreateDxgiSurfaceRenderTarget(
        dxgiBackBuffer,
        &renderTargetProperties,
        &m_renderTarget
    );

    dxgiBackBuffer->Release();
    dxgiBackBuffer = nullptr;

    if (FAILED(result))
    {
        return false;
    }

    result = m_dwriteFactory->CreateTextFormat(
        L"Bahnschrift",
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        56.0f,
        L"en-us",
        &m_titleFormat
    );

    if (FAILED(result))
    {
        return false;
    }

    result = m_dwriteFactory->CreateTextFormat(
        L"Bahnschrift",
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        34.0f,
        L"en-us",
        &m_menuFormat
    );

    if (FAILED(result))
    {
        return false;
    }

    result = m_dwriteFactory->CreateTextFormat(
        L"Bahnschrift",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        22.0f,
        L"en-us",
        &m_smallFormat
    );

    if (FAILED(result))
    {
        return false;
    }

    result = m_dwriteFactory->CreateTextFormat(
        L"Bahnschrift",
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        22.0f,
        L"en-us",
        &m_hudLeftFormat
    );

    if (FAILED(result))
    {
        return false;
    }

    result = m_dwriteFactory->CreateTextFormat(
        L"Bahnschrift",
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        22.0f,
        L"en-us",
        &m_hudRightFormat
    );

    if (FAILED(result))
    {
        return false;
    }

    m_titleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_titleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_menuFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_menuFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_smallFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_smallFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_hudLeftFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    m_hudLeftFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_hudRightFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    m_hudRightFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    result = m_renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f),
        &m_whiteBrush
    );

    if (FAILED(result))
    {
        return false;
    }

    result = m_renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(1.0f, 0.85f, 0.15f, 1.0f),
        &m_yellowBrush
    );

    if (FAILED(result))
    {
        return false;
    }

    result = m_renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0.65f, 0.65f, 0.75f, 1.0f),
        &m_grayBrush
    );

    if (FAILED(result))
    {
        return false;
    }

    return true;
}


// Rilascia tutte le risorse Direct2D e DirectWrite
void TextRendererClass::Shutdown()
{
    if (m_grayBrush)
    {
        m_grayBrush->Release();
        m_grayBrush = nullptr;
    }

    if (m_yellowBrush)
    {
        m_yellowBrush->Release();
        m_yellowBrush = nullptr;
    }

    if (m_whiteBrush)
    {
        m_whiteBrush->Release();
        m_whiteBrush = nullptr;
    }

    if (m_hudRightFormat)
    {
        m_hudRightFormat->Release();
        m_hudRightFormat = nullptr;
    }

    if (m_hudLeftFormat)
    {
        m_hudLeftFormat->Release();
        m_hudLeftFormat = nullptr;
    }

    if (m_smallFormat)
    {
        m_smallFormat->Release();
        m_smallFormat = nullptr;
    }

    if (m_menuFormat)
    {
        m_menuFormat->Release();
        m_menuFormat = nullptr;
    }

    if (m_titleFormat)
    {
        m_titleFormat->Release();
        m_titleFormat = nullptr;
    }

    if (m_renderTarget)
    {
        m_renderTarget->Release();
        m_renderTarget = nullptr;
    }

    if (m_dwriteFactory)
    {
        m_dwriteFactory->Release();
        m_dwriteFactory = nullptr;
    }

    if (m_d2dFactory)
    {
        m_d2dFactory->Release();
        m_d2dFactory = nullptr;
    }
}


// Inizia il rendering del testo
void TextRendererClass::BeginDraw()
{
    if (m_renderTarget)
    {
        m_renderTarget->BeginDraw();
    }
}


// Termina il rendering del testo
void TextRendererClass::EndDraw()
{
    if (m_renderTarget)
    {
        m_renderTarget->EndDraw();
    }
}


// Disegna un titolo centrato
void TextRendererClass::DrawTitle(const wchar_t* text, float y)
{
    if (!m_renderTarget)
    {
        return;
    }

    D2D1_SIZE_F size = m_renderTarget->GetSize();

    DrawTextLine(
        text,
        0.0f,
        y,
        size.width,
        80.0f,
        m_titleFormat,
        m_whiteBrush
    );
}


// Disegna una voce di menu
void TextRendererClass::DrawMenuItem(const wchar_t* text, float y, bool selected)
{
    if (!m_renderTarget)
    {
        return;
    }

    D2D1_SIZE_F size = m_renderTarget->GetSize();

    ID2D1Brush* brush = selected ? m_yellowBrush : m_grayBrush;

    DrawTextLine(
        text,
        0.0f,
        y,
        size.width,
        50.0f,
        m_menuFormat,
        brush
    );
}


// Disegna un testo piccolo centrato
void TextRendererClass::DrawSmallText(const wchar_t* text, float y)
{
    if (!m_renderTarget)
    {
        return;
    }

    D2D1_SIZE_F size = m_renderTarget->GetSize();

    DrawTextLine(
        text,
        0.0f,
        y,
        size.width,
        35.0f,
        m_smallFormat,
        m_grayBrush
    );
}


// Disegna un testo HUD in basso a sinistra
void TextRendererClass::DrawBottomLeftText(const wchar_t* text)
{
    if (!m_renderTarget)
    {
        return;
    }

    D2D1_SIZE_F size = m_renderTarget->GetSize();

    DrawTextLine(
        text,
        20.0f,
        size.height - 45.0f,
        300.0f,
        35.0f,
        m_hudLeftFormat,
        m_whiteBrush
    );
}


// Disegna un testo HUD in basso a destra
void TextRendererClass::DrawBottomRightText(const wchar_t* text)
{
    if (!m_renderTarget)
    {
        return;
    }

    D2D1_SIZE_F size = m_renderTarget->GetSize();

    DrawTextLine(
        text,
        size.width - 320.0f,
        size.height - 45.0f,
        300.0f,
        35.0f,
        m_hudRightFormat,
        m_whiteBrush
    );
}


// Disegna una singola riga di testo
void TextRendererClass::DrawTextLine(
    const wchar_t* text,
    float x,
    float y,
    float width,
    float height,
    IDWriteTextFormat* format,
    ID2D1Brush* brush
)
{
    if (!m_renderTarget || !format || !brush || !text)
    {
        return;
    }

    D2D1_RECT_F textRect =
        D2D1::RectF(
            x,
            y,
            x + width,
            y + height
        );

    m_renderTarget->DrawTextW(
        text,
        static_cast<UINT32>(std::wcslen(text)),
        format,
        textRect,
        brush
    );
}