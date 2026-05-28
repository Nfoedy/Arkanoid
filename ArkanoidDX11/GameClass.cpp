#include "GameClass.h"


GameClass::GameClass()
{
    m_screenWidth = 0;
    m_screenHeight = 0;

    m_D3D = nullptr;
    m_Input = nullptr;
    m_ColorShader = nullptr;

    m_Paddle = nullptr;
    m_Ball = nullptr;

    m_GameState = GameState::Playing;
}


GameClass::~GameClass()
{
}


bool GameClass::Initialize(HWND hwnd, int width, int height)
{
    m_screenWidth = width;
    m_screenHeight = height;

    /*
        Inizializzazione Input
    */

    m_Input = new InputClass();

    if (!m_Input)
    {
        return false;
    }

    m_Input->Initialize();


    /*
        Inizializzazione DirectX
    */

    m_D3D = new D3DClass();

    if (!m_D3D)
    {
        Shutdown();
        return false;
    }

    if (!m_D3D->Initialize(hwnd, width, height))
    {
        MessageBox(nullptr, L"Errore inizializzazione DirectX!", L"Errore", MB_OK);
        Shutdown();
        return false;
    }


    /*
        Inizializzazione Color Shader
    */

    m_ColorShader = new ColorShaderClass();

    if (!m_ColorShader)
    {
        Shutdown();
        return false;
    }

    if (!m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd))
    {
        MessageBox(nullptr, L"Errore inizializzazione Color Shader!", L"Errore", MB_OK);
        Shutdown();
        return false;
    }


    /*
        Inizializzazione Paddle
    */

    m_Paddle = new PaddleClass();

    if (!m_Paddle)
    {
        Shutdown();
        return false;
    }

    if (!m_Paddle->Initialize(
        m_D3D->GetDevice(),
        0.0f,
        -0.8f,
        0.35f,
        0.08f
    ))
    {
        MessageBox(nullptr, L"Errore inizializzazione Paddle!", L"Errore", MB_OK);
        Shutdown();
        return false;
    }


    /*
        Inizializzazione Ball
    */

    m_Ball = new BallClass();

    if (!m_Ball)
    {
        Shutdown();
        return false;
    }

    if (!m_Ball->Initialize(
        m_D3D->GetDevice(),
        0.0f,
        -0.2f,
        0.06f
    ))
    {
        MessageBox(nullptr, L"Errore inizializzazione Ball!", L"Errore", MB_OK);
        Shutdown();
        return false;
    }


    /*
        Inizializzazione Bricks
    */

    if (!InitializeBricks())
    {
        MessageBox(nullptr, L"Errore inizializzazione Bricks!", L"Errore", MB_OK);
        Shutdown();
        return false;
    }

    m_GameState = GameState::Playing;

    return true;
}


void GameClass::Shutdown()
{
    /*
        Shutdown Bricks
    */

    ShutdownBricks();


    /*
        Shutdown Ball
    */

    if (m_Ball)
    {
        m_Ball->Shutdown();
        delete m_Ball;
        m_Ball = nullptr;
    }


    /*
        Shutdown Paddle
    */

    if (m_Paddle)
    {
        m_Paddle->Shutdown();
        delete m_Paddle;
        m_Paddle = nullptr;
    }


    /*
        Shutdown Color Shader
    */

    if (m_ColorShader)
    {
        m_ColorShader->Shutdown();
        delete m_ColorShader;
        m_ColorShader = nullptr;
    }


    /*
        Shutdown DirectX
    */

    if (m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = nullptr;
    }


    /*
        Shutdown Input
    */

    if (m_Input)
    {
        delete m_Input;
        m_Input = nullptr;
    }
}


bool GameClass::Frame()
{
    if (!m_Input)
    {
        return false;
    }

    // ESC chiude il gioco.
    if (m_Input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    // Restart con R se siamo in Win o Lose.
    if (m_Input->IsKeyDown('R') && m_GameState != GameState::Playing)
    {
        ResetGame();
    }

    if (m_GameState == GameState::Playing)
    {
        HandleInput();
        Update();

        CheckPaddleBallCollision();
        CheckBallBrickCollision();
        CheckGameState();
    }

    Render();

    return true;
}


void GameClass::KeyDown(unsigned int key)
{
    if (m_Input)
    {
        m_Input->KeyDown(key);
    }
}


void GameClass::KeyUp(unsigned int key)
{
    if (m_Input)
    {
        m_Input->KeyUp(key);
    }
}


void GameClass::HandleInput()
{
    if (!m_Input || !m_Paddle)
    {
        return;
    }

    if (m_Input->IsKeyDown(VK_LEFT) || m_Input->IsKeyDown('A'))
    {
        m_Paddle->MoveLeft();
    }

    if (m_Input->IsKeyDown(VK_RIGHT) || m_Input->IsKeyDown('D'))
    {
        m_Paddle->MoveRight();
    }
}


void GameClass::Update()
{
    if (m_Ball)
    {
        m_Ball->Update();
    }
}


void GameClass::Render()
{
    if (!m_D3D || !m_ColorShader)
    {
        return;
    }

    /*
        Colore dello sfondo in base allo stato del gioco.
    */

    if (m_GameState == GameState::Playing)
    {
        // Blu: gioco in corso.
        m_D3D->BeginScene(0.1f, 0.1f, 0.4f, 1.0f);
    }
    else if (m_GameState == GameState::Win)
    {
        // Verde: vittoria.
        m_D3D->BeginScene(0.1f, 0.35f, 0.1f, 1.0f);
    }
    else
    {
        // Rosso: sconfitta.
        m_D3D->BeginScene(0.35f, 0.1f, 0.1f, 1.0f);
    }


    /*
        Disegno Bricks
    */

    for (BrickClass* brick : m_Bricks)
    {
        if (brick && brick->IsActive())
        {
            brick->Render(m_D3D->GetDeviceContext());

            m_ColorShader->RenderShader(m_D3D->GetDeviceContext());

            m_D3D->GetDeviceContext()->DrawIndexed(
                brick->GetIndexCount(),
                0,
                0
            );
        }
    }


    /*
        Disegno Paddle
    */

    if (m_Paddle)
    {
        m_Paddle->Render(m_D3D->GetDeviceContext());

        m_ColorShader->RenderShader(m_D3D->GetDeviceContext());

        m_D3D->GetDeviceContext()->DrawIndexed(
            m_Paddle->GetIndexCount(),
            0,
            0
        );
    }


    /*
        Disegno Ball
    */

    if (m_Ball)
    {
        m_Ball->Render(m_D3D->GetDeviceContext());

        m_ColorShader->RenderShader(m_D3D->GetDeviceContext());

        m_D3D->GetDeviceContext()->DrawIndexed(
            m_Ball->GetIndexCount(),
            0,
            0
        );
    }

    m_D3D->EndScene();
}


bool GameClass::InitializeBricks()
{
    if (!m_D3D)
    {
        return false;
    }

    const int rows = 4;
    const int columns = 8;

    const float brickWidth = 0.20f;
    const float brickHeight = 0.08f;

    const float spacingX = 0.03f;
    const float spacingY = 0.03f;

    const float startX = -0.805f;
    const float startY = 0.75f;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            float x = startX + col * (brickWidth + spacingX);
            float y = startY - row * (brickHeight + spacingY);

            float r = 1.0f;
            float g = 0.3f + row * 0.15f;
            float b = 0.2f + col * 0.05f;

            BrickClass* brick = new BrickClass();

            if (!brick)
            {
                return false;
            }

            if (!brick->Initialize(
                m_D3D->GetDevice(),
                x,
                y,
                brickWidth,
                brickHeight,
                r,
                g,
                b
            ))
            {
                brick->Shutdown();
                delete brick;
                brick = nullptr;

                return false;
            }

            m_Bricks.push_back(brick);
        }
    }

    return true;
}


void GameClass::ShutdownBricks()
{
    for (BrickClass* brick : m_Bricks)
    {
        if (brick)
        {
            brick->Shutdown();
            delete brick;
        }
    }

    m_Bricks.clear();
}


bool GameClass::CheckAABBCollision(
    float leftA,
    float rightA,
    float topA,
    float bottomA,
    float leftB,
    float rightB,
    float topB,
    float bottomB
) const
{
    if (rightA < leftB)
    {
        return false;
    }

    if (leftA > rightB)
    {
        return false;
    }

    if (topA < bottomB)
    {
        return false;
    }

    if (bottomA > topB)
    {
        return false;
    }

    return true;
}


void GameClass::CheckPaddleBallCollision()
{
    if (!m_Paddle || !m_Ball)
    {
        return;
    }

    bool isColliding = CheckAABBCollision(
        m_Paddle->GetLeft(),
        m_Paddle->GetRight(),
        m_Paddle->GetTop(),
        m_Paddle->GetBottom(),

        m_Ball->GetLeft(),
        m_Ball->GetRight(),
        m_Ball->GetTop(),
        m_Ball->GetBottom()
    );

    if (isColliding && m_Ball->IsMovingDown())
    {
        m_Ball->BounceFromPaddle(m_Paddle->GetTop());
    }
}


void GameClass::CheckBallBrickCollision()
{
    if (!m_Ball)
    {
        return;
    }

    for (BrickClass* brick : m_Bricks)
    {
        if (!brick || !brick->IsActive())
        {
            continue;
        }

        bool isColliding = CheckAABBCollision(
            m_Ball->GetLeft(),
            m_Ball->GetRight(),
            m_Ball->GetTop(),
            m_Ball->GetBottom(),

            brick->GetLeft(),
            brick->GetRight(),
            brick->GetTop(),
            brick->GetBottom()
        );

        if (isColliding)
        {
            /*
                Calcoliamo quanto la palla è entrata nel brick
                da ogni lato.

                L'idea è:
                - se la sovrapposizione minore è sull'asse X,
                  allora la collisione è laterale
                - se la sovrapposizione minore è sull'asse Y,
                  allora la collisione è verticale
            */

            float overlapFromLeft = m_Ball->GetRight() - brick->GetLeft();
            float overlapFromRight = brick->GetRight() - m_Ball->GetLeft();

            float overlapFromBottom = m_Ball->GetTop() - brick->GetBottom();
            float overlapFromTop = brick->GetTop() - m_Ball->GetBottom();

            float minOverlapX =
                overlapFromLeft < overlapFromRight
                ? overlapFromLeft
                : overlapFromRight;

            float minOverlapY =
                overlapFromBottom < overlapFromTop
                ? overlapFromBottom
                : overlapFromTop;

            /*
                Disattiviamo il brick colpito.
                Nel Render(), i brick inattivi non vengono disegnati.
            */

            brick->SetActive(false);

            /*
                Se la penetrazione minore è sull'asse X,
                significa che la palla ha colpito il brick lateralmente.

                Altrimenti consideriamo la collisione verticale.
            */

            if (minOverlapX < minOverlapY)
            {
                m_Ball->BounceX();
            }
            else
            {
                m_Ball->BounceY();
            }

            /*
                Esco dopo il primo brick colpito.
                Così evitiamo di distruggere più brick nello stesso frame.
            */

            break;
        }
    }
}


bool GameClass::AreAllBricksDestroyed() const
{
    for (BrickClass* brick : m_Bricks)
    {
        if (brick && brick->IsActive())
        {
            return false;
        }
    }

    return true;
}


void GameClass::CheckGameState()
{
    if (!m_Ball)
    {
        return;
    }

    if (m_Ball->IsBelowBottom())
    {
        m_GameState = GameState::Lose;
        return;
    }

    if (AreAllBricksDestroyed())
    {
        m_GameState = GameState::Win;
        return;
    }
}


void GameClass::ResetGame()
{
    m_GameState = GameState::Playing;

    if (m_Paddle)
    {
        m_Paddle->Reset(
            0.0f,
            -0.8f
        );
    }

    if (m_Ball)
    {
        m_Ball->Reset(
            0.0f,
            -0.2f,
            0.01f,
            0.012f
        );
    }

    ShutdownBricks();
    InitializeBricks();
}