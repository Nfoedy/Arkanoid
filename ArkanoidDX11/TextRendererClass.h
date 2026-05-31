#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <dxgi.h>


class TextRendererClass
{
public:
    TextRendererClass();
    ~TextRendererClass();

    // Inizializza Direct2D e DirectWrite usando la swap chain DirectX.
    bool Initialize(IDXGISwapChain* swapChain);

    // Rilascia tutte le risorse Direct2D/DirectWrite.
    void Shutdown();

    // Inizio e fine disegno testo.
    void BeginDraw();
    void EndDraw();

    // Disegno testi principali.
    void DrawTitle(const wchar_t* text, float y);
    void DrawMenuItem(const wchar_t* text, float y, bool selected);
    void DrawSmallText(const wchar_t* text, float y);


    // Disegna testo HUD in basso a sinistra.
    void DrawBottomLeftText(const wchar_t* text);

    // Disegna testo HUD in basso a destra.
    void DrawBottomRightText(const wchar_t* text);

private:
    void DrawTextLine(
        const wchar_t* text,
        float x,
        float y,
        float width,
        float height,
        IDWriteTextFormat* format,
        ID2D1Brush* brush
    );

private:
    ID2D1Factory* m_d2dFactory;
    ID2D1RenderTarget* m_renderTarget;

    IDWriteFactory* m_dwriteFactory;

    IDWriteTextFormat* m_titleFormat;
    IDWriteTextFormat* m_menuFormat;
    IDWriteTextFormat* m_smallFormat;

    ID2D1SolidColorBrush* m_whiteBrush;
    ID2D1SolidColorBrush* m_yellowBrush;
    ID2D1SolidColorBrush* m_grayBrush;

    IDWriteTextFormat* m_hudLeftFormat;
    IDWriteTextFormat* m_hudRightFormat;
};