#define WIN32_LEAN_AND_MEAN

#include <windows.h> // Libreria per le API Win32

#include "GameClass.h"
#include "GameConfig.h"


// Puntatore globale temporaneo alla classe principale del gioco
GameClass* g_Game = nullptr;


// Funzione che Windows chiama ogni volta che succede qualcosa alla finestra
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // hwnd = handle della finestra che ha ricevuto il messaggio
    // msg = tipo di messaggio ricevuto da Windows
    // wParam = parametro aggiuntivo al messaggio 
    // lParam = altro parametro aggiuntivo al messaggio

    switch (msg)
    {
        // Alla distruzione della finestra
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // Quando viene premuto un tasto della tastiera
    case WM_KEYDOWN:
        if (g_Game)
        {
            g_Game->KeyDown(static_cast<unsigned int>(wParam));
        }
        return 0;

        // Quando viene rilasciato un tasto della tastiera
    case WM_KEYUP:
        if (g_Game)
        {
            g_Game->KeyUp(static_cast<unsigned int>(wParam));
        }
        return 0;
    }

    // Tutti i messaggi che non gestisco vengono passati alla gestione di default di Windows
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


// è il main(), ma per programmi Win32 con finestra
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    // hInstance = Handle dell'istanza corrente del sistema
    // hPrevInstance = Non usato nelle app moderne
    // pCmdLine = Argomenti da command line
    // nCmdShow = Come mostrare la finestra

    // dim
    const int WIDTH = GameConfig::ScreenWidth;
    const int HEIGHT = GameConfig::ScreenHeight;

    // Titolo della finestra
    const wchar_t* CLASS_NAME = L"ArkanoidDX11WindowClass";


    /*
        1. Registrazione della classe finestra
    */

    WNDCLASSEX wc = {};

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"Errore registrazione finestra!", L"Errore", MB_OK);
        return -1;
    }


    /*
        2. Calcolo dimensione reale della finestra
    */

    RECT windowRect = { 0, 0, WIDTH, HEIGHT };

    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);


    /*
        3. Creazione della finestra
    */

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Arkanoid DX11",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd)
    {
        MessageBox(nullptr, L"Errore creazione finestra!", L"Errore", MB_OK);
        return -1;
    }


    /*
        4. Mostrare la finestra
    */

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    /*
        5. Inizializzazione GameClass
    */

    g_Game = new GameClass();

    if (!g_Game)
    {
        return -1;
    }

    if (!g_Game->Initialize(hwnd, WIDTH, HEIGHT))
    {
        MessageBox(nullptr, L"Errore inizializzazione GameClass!", L"Errore", MB_OK);

        g_Game->Shutdown();
        delete g_Game;
        g_Game = nullptr;

        return -1;
    }


    /*
        6. Message loop
    */

    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (g_Game)
            {
                if (!g_Game->Frame())
                {
                    PostQuitMessage(0);
                }
            }
        }
    }


    /*
        7. Shutdown GameClass
    */

    if (g_Game)
    {
        g_Game->Shutdown();
        delete g_Game;
        g_Game = nullptr;
    }


    return static_cast<int>(msg.wParam);
}