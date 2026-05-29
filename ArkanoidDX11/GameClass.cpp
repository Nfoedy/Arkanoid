#include "GameClass.h"
#include "GameConfig.h"


GameClass::GameClass()
{
    m_screenWidth = 0;
    m_screenHeight = 0;

    m_D3D = nullptr;
    m_Input = nullptr;
    m_ColorShader = nullptr;

    m_Paddle = nullptr;
    m_Ball = nullptr;

    m_Timer = nullptr;

    m_hwnd = nullptr;

    m_score = 0;
    m_lives = GameConfig::InitialLives;

    m_pWasDown = false;

    m_GameState = GameState::MainMenu;
}


GameClass::~GameClass()
{
}


bool GameClass::Initialize(HWND hwnd, int width, int height)
{
    m_screenWidth = width;
    m_screenHeight = height;

    m_hwnd = hwnd;

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
        Inizializzazione Timer
    */

    m_Timer = new TimerClass();

    if (!m_Timer)
    {
        Shutdown();
        return false;
    }

    m_Timer->Initialize();


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
        GameConfig::PaddleStartX,
        GameConfig::PaddleStartY,
        GameConfig::PaddleWidth,
        GameConfig::PaddleHeight
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
        GameConfig::PaddleStartX,
        GameConfig::PaddleStartY,
        GameConfig::BallSize
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

    m_score = 0;
    m_lives = GameConfig::InitialLives;

    m_GameState = GameState::MainMenu;

    PositionBallOnPaddle();

    UpdateWindowTitle();

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


    /*
        Shutdown Timer
    */

    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = nullptr;
    }
}


bool GameClass::Frame()
{
    if (!m_Input || !m_Timer)
    {
        return false;
    }

    m_Timer->Frame();

    float deltaTime = m_Timer->GetDeltaTime();

    // Evita salti enormi se il programma si blocca per un attimo.
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    // ESC chiude sempre il gioco.
    if (m_Input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }


    /*
        Main Menu

        Per ora il menu è controllato da tastiera:
        ENTER = Play
        ESC   = Exit
    */

    if (m_GameState == GameState::MainMenu)
    {
        if (m_Input->IsKeyDown(VK_RETURN))
        {
            StartNewGame();
        }

        Render();

        return true;
    }


    /*
        Gestione pausa con P.

        Funziona solo durante Playing e Paused.
        In Ready non mettiamo in pausa, perché la palla è già ferma.
    */

    bool pIsDown = m_Input->IsKeyDown('P');

    if (pIsDown && !m_pWasDown)
    {
        if (m_GameState == GameState::Playing)
        {
            m_GameState = GameState::Paused;
            UpdateWindowTitle();
        }
        else if (m_GameState == GameState::Paused)
        {
            m_GameState = GameState::Playing;
            UpdateWindowTitle();
        }
    }

    m_pWasDown = pIsDown;


    /*
        Win / Lose menu

        R = restart
        ESC = exit, già gestito sopra
    */

    if (m_Input->IsKeyDown('R') &&
        (m_GameState == GameState::Win || m_GameState == GameState::Lose))
    {
        ResetGame();
    }


    /*
        Stato Ready

        La palla resta sopra il paddle.
        Il player può muovere il paddle.
        SPACE lancia la palla.
    */

    if (m_GameState == GameState::Ready)
    {
        HandleInput(deltaTime);

        PositionBallOnPaddle();

        if (m_Input->IsKeyDown(VK_SPACE))
        {
            LaunchBall();
        }
    }
    else if (m_GameState == GameState::Playing)
    {
        HandleInput(deltaTime);

        Update(deltaTime);

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


void GameClass::HandleInput(float deltaTime)
{
    if (!m_Input || !m_Paddle)
    {
        return;
    }

    if (m_Input->IsKeyDown(VK_LEFT) || m_Input->IsKeyDown('A'))
    {
        m_Paddle->MoveLeft(deltaTime);
    }

    if (m_Input->IsKeyDown(VK_RIGHT) || m_Input->IsKeyDown('D'))
    {
        m_Paddle->MoveRight(deltaTime);
    }
}


void GameClass::Update(float deltaTime)
{
    if (m_Ball)
    {
        m_Ball->Update(deltaTime);
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

    if (m_GameState == GameState::MainMenu)
    {
        // Nero/blu scuro: main menu.
        m_D3D->BeginScene(0.02f, 0.02f, 0.08f, 1.0f);
    }
    else if (m_GameState == GameState::Ready)
    {
        // Blu più scuro: round pronto, palla ferma sopra il paddle.
        m_D3D->BeginScene(0.08f, 0.08f, 0.30f, 1.0f);
    }
    else if (m_GameState == GameState::Playing)
    {
        // Blu: gioco in corso.
        m_D3D->BeginScene(0.1f, 0.1f, 0.4f, 1.0f);
    }
    else if (m_GameState == GameState::Paused)
    {
        // Grigio/scuro: pausa.
        m_D3D->BeginScene(0.12f, 0.12f, 0.12f, 1.0f);
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
        Nel Main Menu non disegniamo ancora gli oggetti di gioco.
        Per ora il menu viene comunicato tramite titolo finestra.
    */

    if (m_GameState == GameState::MainMenu)
    {
        m_D3D->EndScene();
        return;
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

    const int rows = GameConfig::BrickRows;
    const int columns = GameConfig::BrickColumns;

    const float brickWidth = GameConfig::BrickWidth;
    const float brickHeight = GameConfig::BrickHeight;

    const float spacingX = GameConfig::BrickSpacingX;
    const float spacingY = GameConfig::BrickSpacingY;

    const float startX = GameConfig::BrickStartX;
    const float startY = GameConfig::BrickStartY;

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
        float paddleCenter =
            (m_Paddle->GetLeft() + m_Paddle->GetRight()) * 0.5f;

        float paddleHalfWidth =
            (m_Paddle->GetRight() - m_Paddle->GetLeft()) * 0.5f;

        float ballCenter =
            (m_Ball->GetLeft() + m_Ball->GetRight()) * 0.5f;

        float hitFactor =
            (ballCenter - paddleCenter) / paddleHalfWidth;

        m_Ball->BounceFromPaddle(
            m_Paddle->GetTop(),
            hitFactor
        );
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
                Score.
            */

            m_score += GameConfig::BrickScore;
            UpdateWindowTitle();

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
        HandleBallLost();
        return;
    }

    if (AreAllBricksDestroyed())
    {
        m_GameState = GameState::Win;
        UpdateWindowTitle();
        return;
    }
}


void GameClass::ResetGame()
{
    StartNewGame();
}


void GameClass::ResetRound()
{
    if (m_Paddle)
    {
        m_Paddle->Reset(
            GameConfig::PaddleStartX,
            GameConfig::PaddleStartY
        );
    }

    if (m_Ball)
    {
        m_Ball->SetVelocity(
            0.0f,
            0.0f
        );

        PositionBallOnPaddle();
    }
}


void GameClass::HandleBallLost()
{
    m_lives--;

    if (m_lives <= 0)
    {
        m_lives = 0;
        m_GameState = GameState::Lose;
        UpdateWindowTitle();
        return;
    }

    ResetRound();

    m_GameState = GameState::Ready;

    UpdateWindowTitle();
}


void GameClass::UpdateWindowTitle()
{
    if (!m_hwnd)
    {
        return;
    }

    wchar_t title[256];

    if (m_GameState == GameState::MainMenu)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | MAIN MENU | ENTER: Play | ESC: Exit"
        );
    }
    else if (m_GameState == GameState::Ready)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | Score: %d | Lives: %d | SPACE: Launch | P: Pause",
            m_score,
            m_lives
        );
    }
    else if (m_GameState == GameState::Playing)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | Score: %d | Lives: %d | P: Pause",
            m_score,
            m_lives
        );
    }
    else if (m_GameState == GameState::Paused)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | PAUSED | Score: %d | Lives: %d | P: Resume | ESC: Exit",
            m_score,
            m_lives
        );
    }
    else if (m_GameState == GameState::Win)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | YOU WIN | Score: %d | R: Restart | ESC: Exit",
            m_score
        );
    }
    else
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | GAME OVER | Score: %d | R: Restart | ESC: Exit",
            m_score
        );
    }

    SetWindowText(m_hwnd, title);
}


void GameClass::PositionBallOnPaddle()
{
    if (!m_Paddle || !m_Ball)
    {
        return;
    }

    /*
        Mettiamo la palla sopra il paddle.
    */

    const float ballHalfSize = GameConfig::BallSize * 0.5f;

    float paddleCenterX =
        (m_Paddle->GetLeft() + m_Paddle->GetRight()) * 0.5f;

    float ballY =
        m_Paddle->GetTop() + ballHalfSize;

    m_Ball->SetPosition(paddleCenterX, ballY);
}


void GameClass::LaunchBall()
{
    if (!m_Ball)
    {
        return;
    }

    /*
        Velocità iniziale della palla.
        Usiamo valori coerenti con il delta time.
    */

    m_Ball->SetVelocity(
        GameConfig::BallInitialVelocityX,
        GameConfig::BallInitialVelocityY
    );

    m_GameState = GameState::Playing;

    UpdateWindowTitle();
}


void GameClass::StartNewGame()
{
    m_score = 0;
    m_lives = GameConfig::InitialLives;

    m_pWasDown = false;

    ShutdownBricks();
    InitializeBricks();

    ResetRound();

    m_GameState = GameState::Ready;

    UpdateWindowTitle();
}