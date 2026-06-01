#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <dxgi.h>


class TextRendererClass
{
public:
    TextRendererClass();
    ~TextRendererClass();

    bool Initialize(IDXGISwapChain* swapChain);
    void Shutdown();

    void BeginDraw();
    void EndDraw();

    void DrawTitle(const wchar_t* text, float y);
    void DrawMenuItem(const wchar_t* text, float y, bool selected);
    void DrawSmallText(const wchar_t* text, float y);

    void DrawBottomLeftText(const wchar_t* text);
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

    // Formati testo usati da menu e HUD
    IDWriteTextFormat* m_titleFormat;
    IDWriteTextFormat* m_menuFormat;
    IDWriteTextFormat* m_smallFormat;
    IDWriteTextFormat* m_hudLeftFormat;
    IDWriteTextFormat* m_hudRightFormat;

    // Brush usati per disegnare il testo
    ID2D1SolidColorBrush* m_whiteBrush;
    ID2D1SolidColorBrush* m_yellowBrush;
    ID2D1SolidColorBrush* m_grayBrush;
};