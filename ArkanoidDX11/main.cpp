#define WIN32_LEAN_AND_MEAN

#include <windows.h> // Libreria per le API Win32

#include "D3DClass.h"


// Puntatore globale temporaneo alla classe DirectX
D3DClass* g_D3D = nullptr;



// Funzione che Windows chiama ogni volta che succede qualcosa alla finestra
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // hwnd = handle della finestra che ha ricevuto il messaggio
    // msg = tipo di messaggio ricevuto da Windows
    // wParam = parametro aggiuntivo al messaggio 
    // lParma = altro parametro aggiuntivo al messaggio

    switch (msg)
    {

        // Alla distruzione della finestra
    case WM_DESTROY:
        PostQuitMessage(0);     // Dice di uscre dal message loop
        return 0;

        // Quando viene premuto un tasto della tastiera
    case WM_KEYDOWN:
        // Se viene premuto ESC
        if (wParam == VK_ESCAPE)
        {
            // Chiudo il programma
            PostQuitMessage(0);
        }
        return 0;
    }

    // Tutti i messaggi che non gestisco vengono passati alla gestione di default di Windows
    return DefWindowProc(hwnd, msg, wParam, lParam);
}



// Funzione chiamata ogni frame
void Render()
{
    g_D3D->BeginScene(0.1f, 0.1f, 0.4f, 1.0f);

    // In futuro qui disegneremo:
    // paddle, ball, bricks, ecc.

    g_D3D->EndScene();
}




// è il main(), ma per programmi Win32 con finestra
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    // hInstance = Handle dell'istanza corrente del sistema
    // hPrevInstance = Non usato nelle app moderne
    // pCmdLine = Argomenti da command line
    // nCmdShow = Come mostrare la finestra

    // dim
    const int WIDTH = 800;
    const int HEIGHT = 600;

    // Titilo della finestra
    const wchar_t* CLASS_NAME = L"ArkanoidDX11WindowClass";


    /*
        1. Registrazione della classe finestra
    */

    // Contiene le info per dire a Windows che finestra voglio creare
    WNDCLASSEX wc = {};

    // Dimensione della struttura.
    wc.cbSize = sizeof(WNDCLASSEX);

    // Stile della finestra.
    wc.style = CS_HREDRAW | CS_VREDRAW;

    // Funzione che gestisce i messaggi della finestra.
    wc.lpfnWndProc = WndProc;

    // Istanza dell'applicazione.
    wc.hInstance = hInstance;

    // Cursore standard della freccia.
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // Nome della classe finestra.
    wc.lpszClassName = CLASS_NAME;

    // Registriamo la classe finestra presso Windows.
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
        0,                                      // stile esteso, per ora nessuno
        CLASS_NAME,                             // classe finestra registrata prima
        L"Arkanoid DX11",                       // titolo visibile della finestra
        WS_OVERLAPPEDWINDOW,                    // finestra standard con bordi e titolo
        CW_USEDEFAULT,                          // posizione X automatica
        CW_USEDEFAULT,                          // posizione Y automatica
        windowRect.right - windowRect.left,     // larghezza totale finestra
        windowRect.bottom - windowRect.top,     // altezza totale finestra
        nullptr,                                // finestra padre, non serve
        nullptr,                                // menu, non serve
        hInstance,                              // istanza applicazione
        nullptr                                 // dati extra, non servono
    );

    // Se hwnd è nullptr, la finestra non è stata creata correttamente.
    if (!hwnd)
    {
        MessageBox(nullptr, L"Errore registrazione finestra!", L"Errore", MB_OK);

        return -1;
    }


    /*
        4. Mostrare la finestra
    */

    // Mostra la finestra a schermo.
    ShowWindow(hwnd, nCmdShow);

    // Forza un primo aggiornamento della finestra.
    UpdateWindow(hwnd);


    /*
        Inizializzazione DirectX
    */

    g_D3D = new D3DClass();

    if (!g_D3D)
    {
        return -1;
    }

    if (!g_D3D->Initialize(hwnd, WIDTH, HEIGHT))
    {
        MessageBox(nullptr, L"Errore inizializzazione DirectX!", L"Errore", MB_OK);

        g_D3D->Shutdown();
        delete g_D3D;
        g_D3D = nullptr;

        return -1;
    }


    /*
        5. Message loop
    */

    // MSG contiene i messaggi che Windows invia alla nostra applicazione.
    MSG msg = {};

    // Il programma continua finché non riceve WM_QUIT.
    while (msg.message != WM_QUIT)
    {

        // PM_REMOVE significa: se trova un messaggio, rimuovilo dalla coda.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Traduce alcuni messaggi da tastiera.
            TranslateMessage(&msg);

            // Manda il messaggio alla WndProc.
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }


    /*
        Shutdown DirectX
    */

    if (g_D3D)
    {
        g_D3D->Shutdown();
        delete g_D3D;
        g_D3D = nullptr;
    }


    // Quando usco dal loop, ritorno il codice di uscita.
    return static_cast<int>(msg.wParam);


}