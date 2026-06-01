#include <cstdlib>
#include <ctime>

#include "GameClass.h"
#include "GameConfig.h"


/*
    GameClass gestisce il flusso principale del gioco

    La classe si occupa di:
    - inizializzare e rilasciare i sistemi principali
    - gestire stati di gioco, menu e input
    - aggiornare paddle, palle, brick e powerup
    - controllare collisioni, score, vite, win e lose
    - coordinare rendering DirectX e testo DirectWrite
*/


// Inizializza i valori base del gioco
GameClass::GameClass()
{
    m_screenWidth = 0;
    m_screenHeight = 0;

    m_D3D = nullptr;
    m_Input = nullptr;
    m_Timer = nullptr;
    m_ColorShader = nullptr;
    m_TextRenderer = nullptr;

    m_Paddle = nullptr;
    m_Ball = nullptr;
    m_SecondBall = nullptr;

    m_hwnd = nullptr;

    m_score = 0;
    m_lives = GameConfig::InitialLives;

    m_pWasDown = false;

    m_isPaddleSizeEffectActive = false;
    m_paddleSizeEffectTimer = 0.0f;

    m_isBallSpeedEffectActive = false;
    m_ballSpeedEffectTimer = 0.0f;

    m_menuSelectedIndex = 0;

    m_upWasDown = false;
    m_downWasDown = false;
    m_enterWasDown = false;

    m_GameState = GameState::MainMenu;
}


// Distruttore della classe gioco
GameClass::~GameClass()
{
}


// Inizializza sistemi, oggetti di gioco e stato iniziale
bool GameClass::Initialize(HWND hwnd, int width, int height)
{
    m_screenWidth = width;
    m_screenHeight = height;

    m_hwnd = hwnd;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Inizializza il sistema di input
    m_Input = new InputClass();

    if (!m_Input)
    {
        return false;
    }

    m_Input->Initialize();

    // Inizializza il timer del gioco
    m_Timer = new TimerClass();

    if (!m_Timer)
    {
        Shutdown();
        return false;
    }

    m_Timer->Initialize();

    // Inizializza DirectX
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

    // Inizializza il renderer del testo
    m_TextRenderer = new TextRendererClass();

    if (!m_TextRenderer)
    {
        Shutdown();
        return false;
    }

    if (!m_TextRenderer->Initialize(m_D3D->GetSwapChain()))
    {
        MessageBox(nullptr, L"Errore inizializzazione TextRendererClass!", L"Errore", MB_OK);
        Shutdown();
        return false;
    }

    // Inizializza lo shader colorato
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

    // Inizializza il paddle
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

    // Inizializza la palla principale
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

    // Inizializza la griglia di brick
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


// Rilascia tutte le risorse del gioco
void GameClass::Shutdown()
{
    ShutdownPowerUps();     // Shutdown PowerUps

    ShutdownBricks();       // Shutdown Bricks

    ShutdownSecondBall();   // Shutdown Second Ball

    // Shutdown Ball
    if (m_Ball)
    {
        m_Ball->Shutdown();
        delete m_Ball;
        m_Ball = nullptr;
    }

    // Shutdown Paddle
    if (m_Paddle)
    {
        m_Paddle->Shutdown();
        delete m_Paddle;
        m_Paddle = nullptr;
    }

    // Shutdown Color Shader
    if (m_ColorShader)
    {
        m_ColorShader->Shutdown();
        delete m_ColorShader;
        m_ColorShader = nullptr;
    }

    // Shutdown Text Renderer
    if (m_TextRenderer)
    {
        m_TextRenderer->Shutdown();
        delete m_TextRenderer;
        m_TextRenderer = nullptr;
    }

    // Shutdown DirectX
    if (m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = nullptr;
    }

    // Shutdown Input
    if (m_Input)
    {
        delete m_Input;
        m_Input = nullptr;
    }

    // Shutdown Timer
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = nullptr;
    }
}


// Gestisce un frame completo del gioco
bool GameClass::Frame()
{
    if (!m_Input || !m_Timer)
    {
        return false;
    }

    m_Timer->Frame();

    float deltaTime = m_Timer->GetDeltaTime();

    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    if (m_Input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    // I menu vengono gestiti separatamente dal gameplay
    if (IsMenuState())
    {
        if (!HandleMenuInput())
        {
            return false;
        }

        Render();

        return true;
    }

    // Durante il gameplay, P apre il menu di pausa
    bool pIsDown = m_Input->IsKeyDown('P');

    if (pIsDown && !m_pWasDown)
    {
        if (m_GameState == GameState::Playing)
        {
            m_GameState = GameState::Paused;

            m_menuSelectedIndex = 0;

            m_upWasDown = false;
            m_downWasDown = false;
            m_enterWasDown = false;

            UpdateWindowTitle();
        }
    }

    m_pWasDown = pIsDown;

    // In Ready la palla resta agganciata al paddle
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

        CheckPaddleBallCollision(m_Ball);
        CheckPaddleBallCollision(m_SecondBall);

        CheckBallBrickCollision(m_Ball);
        CheckBallBrickCollision(m_SecondBall);

        CheckPaddlePowerUpCollision();

        CheckGameState();
    }

    Render();

    return true;
}


// Registra la pressione di un tasto
void GameClass::KeyDown(unsigned int key)
{
    if (m_Input)
    {
        m_Input->KeyDown(key);
    }
}


// Registra il rilascio di un tasto
void GameClass::KeyUp(unsigned int key)
{
    if (m_Input)
    {
        m_Input->KeyUp(key);
    }
}


// Gestisce il movimento del paddle
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


// Aggiorna palle, PowerUp ed effetti attivi
void GameClass::Update(float deltaTime)
{
    if (m_Ball)
    {
        m_Ball->Update(deltaTime);
    }

    if (m_SecondBall)
    {
        m_SecondBall->Update(deltaTime);
    }

    UpdatePowerUps(deltaTime);

    UpdateActiveEffects(deltaTime);
}


// Disegna scena di gioco, oggetti e UI
void GameClass::Render()
{
    if (!m_D3D || !m_ColorShader)
    {
        return;
    }

    if (m_GameState == GameState::MainMenu)
    {
        m_D3D->BeginScene(0.02f, 0.02f, 0.08f, 1.0f);
    }
    else if (m_GameState == GameState::Ready)
    {
        m_D3D->BeginScene(0.08f, 0.08f, 0.30f, 1.0f);
    }
    else if (m_GameState == GameState::Playing)
    {
        m_D3D->BeginScene(0.1f, 0.1f, 0.4f, 1.0f);
    }
    else if (m_GameState == GameState::Paused)
    {
        m_D3D->BeginScene(0.12f, 0.12f, 0.12f, 1.0f);
    }
    else if (m_GameState == GameState::Win)
    {
        m_D3D->BeginScene(0.1f, 0.35f, 0.1f, 1.0f);
    }
    else
    {
        m_D3D->BeginScene(0.35f, 0.1f, 0.1f, 1.0f);
    }

    // Nel Main Menu disegniamo solo la UI testuale
    if (m_GameState == GameState::MainMenu)
    {
        RenderTextUI();

        m_D3D->EndScene();
        return;
    }

    // Disegna i brick attivi
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

    // Disegna il paddle
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

    // Disegna i powerup attivi
    RenderPowerUps();

    // Disegna la palla principale
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

    // Disegna la seconda palla, se presente
    if (m_SecondBall)
    {
        m_SecondBall->Render(m_D3D->GetDeviceContext());

        m_ColorShader->RenderShader(m_D3D->GetDeviceContext());

        m_D3D->GetDeviceContext()->DrawIndexed(
            m_SecondBall->GetIndexCount(),
            0,
            0
        );
    }

    RenderTextUI();

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


void GameClass::CheckPaddleBallCollision(BallClass* ball)
{
    if (!m_Paddle || !ball)
    {
        return;
    }

    bool isColliding = CheckAABBCollision(
        m_Paddle->GetLeft(),
        m_Paddle->GetRight(),
        m_Paddle->GetTop(),
        m_Paddle->GetBottom(),

        ball->GetLeft(),
        ball->GetRight(),
        ball->GetTop(),
        ball->GetBottom()
    );

    if (!isColliding)
    {
        return;
    }

    /*
        Calcoliamo centro e dimensioni della palla.
    */

    float ballCenterX =
        (ball->GetLeft() + ball->GetRight()) * 0.5f;

    float ballCenterY =
        (ball->GetTop() + ball->GetBottom()) * 0.5f;

    float ballRadius =
        (ball->GetRight() - ball->GetLeft()) * 0.5f;


    /*
        Calcoliamo centro del paddle.
    */

    float paddleCenterX =
        (m_Paddle->GetLeft() + m_Paddle->GetRight()) * 0.5f;

    float paddleHalfWidth =
        (m_Paddle->GetRight() - m_Paddle->GetLeft()) * 0.5f;


    /*
        CASO 1:
        Collisione corretta sopra il paddle.

        La consideriamo collisione superiore solo se:
        - la palla sta scendendo
        - il centro della palla è sopra il top del paddle
        - il centro della palla è compreso orizzontalmente nel paddle

        Questo evita il teletrasporto quando il paddle colpisce la palla di lato.
    */

    bool ballCenterInsidePaddleX =
        ballCenterX >= m_Paddle->GetLeft() &&
        ballCenterX <= m_Paddle->GetRight();

    bool ballCenterAbovePaddleTop =
        ballCenterY >= m_Paddle->GetTop();

    if (ball->IsMovingDown() &&
        ballCenterInsidePaddleX &&
        ballCenterAbovePaddleTop)
    {
        float hitFactor =
            (ballCenterX - paddleCenterX) / paddleHalfWidth;

        ball->BounceFromPaddle(
            m_Paddle->GetTop(),
            hitFactor
        );

        return;
    }


    /*
        CASO 2:
        Collisione laterale.

        Qui NON usiamo BounceFromPaddle(), perché quella funzione
        sposta la palla sopra il paddle.

        Invece:
        - spostiamo la palla fuori dal lato colpito
        - invertiamo la velocità X
    */

    if (ballCenterX < paddleCenterX)
    {
        // La palla è sul lato sinistro del paddle.
        ball->SetPosition(
            m_Paddle->GetLeft() - ballRadius,
            ballCenterY
        );
    }
    else
    {
        // La palla è sul lato destro del paddle.
        ball->SetPosition(
            m_Paddle->GetRight() + ballRadius,
            ballCenterY
        );
    }

    ball->BounceX();
}


void GameClass::CheckBallBrickCollision(BallClass* ball)
{
    if (!ball)
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
            ball->GetLeft(),
            ball->GetRight(),
            ball->GetTop(),
            ball->GetBottom(),

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

            float overlapFromLeft = ball->GetRight() - brick->GetLeft();
            float overlapFromRight = brick->GetRight() - ball->GetLeft();

            float overlapFromBottom = ball->GetTop() - brick->GetBottom();
            float overlapFromTop = brick->GetTop() - ball->GetBottom();

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

            TrySpawnPowerUp(
                brick->GetX(),
                brick->GetY()
            );

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
                ball->BounceX();
            }
            else
            {
                ball->BounceY();
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

    CheckBallsOutOfBounds();

    if (m_GameState == GameState::Ready ||
        m_GameState == GameState::Lose)
    {
        return;
    }

    if (AreAllBricksDestroyed())
    {
        m_GameState = GameState::Win;

        m_menuSelectedIndex = 0;

        m_upWasDown = false;
        m_downWasDown = false;
        m_enterWasDown = false;

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
    ShutdownSecondBall();

    ResetActiveEffects();

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

    ShutdownPowerUps();

    m_lives--;

    if (m_lives <= 0)
    {
        m_lives = 0;
        m_GameState = GameState::Lose;

        m_menuSelectedIndex = 0;

        m_upWasDown = false;
        m_downWasDown = false;
        m_enterWasDown = false;

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
            L"Arkanoid DX11 | Main Menu"
        );
    }
    else if (m_GameState == GameState::Ready)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | Score: %d | Lives: %d",
            m_score,
            m_lives
        );
    }
    else if (m_GameState == GameState::Playing)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | Score: %d | Lives: %d",
            m_score,
            m_lives
        );
    }
    else if (m_GameState == GameState::Paused)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | Paused"
        );
    }
    else if (m_GameState == GameState::Win)
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | You Win | Score: %d",
            m_score
        );
    }
    else
    {
        swprintf_s(
            title,
            L"Arkanoid DX11 | Game Over | Score: %d",
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

    ShutdownSecondBall();

    ResetActiveEffects();

    ShutdownPowerUps();

    ShutdownBricks();
    InitializeBricks();

    ResetRound();

    m_GameState = GameState::Ready;

    UpdateWindowTitle();
}


void GameClass::ShutdownPowerUps()
{
    for (PowerUpClass* powerUp : m_PowerUps)
    {
        if (powerUp)
        {
            powerUp->Shutdown();
            delete powerUp;
        }
    }

    m_PowerUps.clear();
}


void GameClass::UpdatePowerUps(float deltaTime)
{
    for (auto it = m_PowerUps.begin(); it != m_PowerUps.end(); )
    {
        PowerUpClass* powerUp = *it;

        if (!powerUp)
        {
            it = m_PowerUps.erase(it);
            continue;
        }

        if (powerUp->IsActive())
        {
            powerUp->Update(deltaTime);
        }

        /*
            Dopo l'update, il powerup potrebbe essere diventato inattivo:
            - raccolto dal paddle
            - uscito sotto lo schermo
        */

        if (!powerUp->IsActive())
        {
            powerUp->Shutdown();
            delete powerUp;
            powerUp = nullptr;

            it = m_PowerUps.erase(it);
        }
        else
        {
            ++it;
        }
    }
}


void GameClass::RenderPowerUps()
{
    if (!m_D3D || !m_ColorShader)
    {
        return;
    }

    for (PowerUpClass* powerUp : m_PowerUps)
    {
        if (powerUp && powerUp->IsActive())
        {
            powerUp->Render(m_D3D->GetDeviceContext());

            m_ColorShader->RenderShader(m_D3D->GetDeviceContext());

            m_D3D->GetDeviceContext()->DrawIndexed(
                powerUp->GetIndexCount(),
                0,
                0
            );
        }
    }
}


PowerUpType GameClass::GetRandomPowerUpType() const
{
    int randomValue = std::rand() % 4;

    if (randomValue == 0)
    {
        return PowerUpType::PaddleGrow;
    }

    if (randomValue == 1)
    {
        return PowerUpType::PaddleShrink;
    }

    if (randomValue == 2)
    {
        return PowerUpType::BallSpeedUp;
    }

    return PowerUpType::MultiBall;
}


void GameClass::TrySpawnPowerUp(float x, float y)
{
    if (!m_D3D)
    {
        return;
    }

    /*
        Drop chance:
        GameConfig::PowerUpDropChance = 0.05f significa 5%.
    */

    float randomValue =
        static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

    if (randomValue > GameConfig::PowerUpDropChance)
    {
        return;
    }

    PowerUpClass* powerUp = new PowerUpClass();

    if (!powerUp)
    {
        return;
    }

    PowerUpType type = GetRandomPowerUpType();

    if (!powerUp->Initialize(
        m_D3D->GetDevice(),
        x,
        y,
        type
    ))
    {
        powerUp->Shutdown();
        delete powerUp;
        powerUp = nullptr;

        return;
    }

    m_PowerUps.push_back(powerUp);
}


void GameClass::CheckPaddlePowerUpCollision()
{
    if (!m_Paddle)
    {
        return;
    }

    for (PowerUpClass* powerUp : m_PowerUps)
    {
        if (!powerUp || !powerUp->IsActive())
        {
            continue;
        }

        bool isColliding = CheckAABBCollision(
            m_Paddle->GetLeft(),
            m_Paddle->GetRight(),
            m_Paddle->GetTop(),
            m_Paddle->GetBottom(),

            powerUp->GetLeft(),
            powerUp->GetRight(),
            powerUp->GetTop(),
            powerUp->GetBottom()
        );

        if (isColliding)
        {
            
            ApplyPowerUp(powerUp->GetType());

            powerUp->SetActive(false);
        }
    }
}


void GameClass::ApplyPowerUp(PowerUpType type)
{
    if (!m_Paddle || !m_Ball)
    {
        return;
    }

    if (type == PowerUpType::PaddleGrow)
    {
        m_Paddle->SetWidth(GameConfig::PaddleGrowWidth);

        m_isPaddleSizeEffectActive = true;
        m_paddleSizeEffectTimer = GameConfig::PowerUpDuration;
    }
    else if (type == PowerUpType::PaddleShrink)
    {
        m_Paddle->SetWidth(GameConfig::PaddleShrinkWidth);

        m_isPaddleSizeEffectActive = true;
        m_paddleSizeEffectTimer = GameConfig::PowerUpDuration;
    }
    else if (type == PowerUpType::BallSpeedUp)
    {
        m_Ball->SetSpeedMultiplier(GameConfig::BallSpeedMultiplier);

        m_isBallSpeedEffectActive = true;
        m_ballSpeedEffectTimer = GameConfig::PowerUpDuration;
    }
    else if (type == PowerUpType::MultiBall)
    {
        SpawnSecondBall();
    }
}


void GameClass::UpdateActiveEffects(float deltaTime)
{
    /*
        Effetto temporaneo sulla dimensione del paddle.
    */

    if (m_isPaddleSizeEffectActive)
    {
        m_paddleSizeEffectTimer -= deltaTime;

        if (m_paddleSizeEffectTimer <= 0.0f)
        {
            if (m_Paddle)
            {
                m_Paddle->SetWidth(GameConfig::PaddleWidth);
            }

            m_isPaddleSizeEffectActive = false;
            m_paddleSizeEffectTimer = 0.0f;
        }
    }


    /*
        Effetto temporaneo sulla velocità della palla.
    */

    if (m_isBallSpeedEffectActive)
    {
        m_ballSpeedEffectTimer -= deltaTime;

        if (m_ballSpeedEffectTimer <= 0.0f)
        {
            if (m_Ball)
            {
                m_Ball->SetSpeedMultiplier(1.0f);
            }

            if (m_SecondBall)
            {
                m_SecondBall->SetSpeedMultiplier(1.0f);
            }

            m_isBallSpeedEffectActive = false;
            m_ballSpeedEffectTimer = 0.0f;
        }
    }
}


void GameClass::ResetActiveEffects()
{
    if (m_Paddle)
    {
        m_Paddle->SetWidth(GameConfig::PaddleWidth);
    }

    if (m_Ball)
    {
        m_Ball->SetSpeedMultiplier(1.0f);
    }

    if (m_SecondBall)
    {
        m_SecondBall->SetSpeedMultiplier(1.0f);
    }

    m_isPaddleSizeEffectActive = false;
    m_paddleSizeEffectTimer = 0.0f;

    m_isBallSpeedEffectActive = false;
    m_ballSpeedEffectTimer = 0.0f;
}


void GameClass::RenderTextUI()
{
    if (!m_TextRenderer)
    {
        return;
    }

    m_TextRenderer->BeginDraw();

    if (m_GameState == GameState::MainMenu)
    {
        m_TextRenderer->DrawTitle(
            L"ARKANOID DX11",
            90.0f
        );

        m_TextRenderer->DrawMenuItem(
            L"START GAME",
            250.0f,
            m_menuSelectedIndex == 0
        );

        m_TextRenderer->DrawMenuItem(
            L"QUIT",
            315.0f,
            m_menuSelectedIndex == 1
        );

        m_TextRenderer->DrawSmallText(
            L"UP/DOWN to select - ENTER to confirm - ESC to quit",
            420.0f
        );
    }
    else if (m_GameState == GameState::Ready)
    {
        m_TextRenderer->DrawSmallText(
            L"SPACE to launch",
            545.0f
        );
    }
    else if (m_GameState == GameState::Paused)
    {
        m_TextRenderer->DrawTitle(
            L"PAUSED",
            120.0f
        );

        m_TextRenderer->DrawMenuItem(
            L"RESUME",
            250.0f,
            m_menuSelectedIndex == 0
        );

        m_TextRenderer->DrawMenuItem(
            L"QUIT",
            315.0f,
            m_menuSelectedIndex == 1
        );

        m_TextRenderer->DrawSmallText(
            L"UP/DOWN to select - ENTER to confirm",
            420.0f
        );
    }
    else if (m_GameState == GameState::Win)
    {
        m_TextRenderer->DrawTitle(
            L"YOU WIN",
            120.0f
        );

        m_TextRenderer->DrawMenuItem(
            L"RESTART",
            250.0f,
            m_menuSelectedIndex == 0
        );

        m_TextRenderer->DrawMenuItem(
            L"QUIT",
            315.0f,
            m_menuSelectedIndex == 1
        );

        m_TextRenderer->DrawSmallText(
            L"UP/DOWN to select - ENTER to confirm",
            420.0f
        );
    }
    else if (m_GameState == GameState::Lose)
    {
        m_TextRenderer->DrawTitle(
            L"GAME OVER",
            120.0f
        );

        m_TextRenderer->DrawMenuItem(
            L"RESTART",
            250.0f,
            m_menuSelectedIndex == 0
        );

        m_TextRenderer->DrawMenuItem(
            L"QUIT",
            315.0f,
            m_menuSelectedIndex == 1
        );

        m_TextRenderer->DrawSmallText(
            L"UP/DOWN to select - ENTER to confirm",
            420.0f
        );
    }


    /*
        HUD di gioco.

        Lo mostriamo durante:
        - Ready
        - Playing
        - Paused

        Non lo mostriamo nel Main Menu.
        Nei menu Win/Lose per ora abbiamo già il messaggio centrale.
    */

    if (m_GameState == GameState::Ready ||
        m_GameState == GameState::Playing ||
        m_GameState == GameState::Paused)
    {
        wchar_t scoreText[64];
        wchar_t livesText[64];

        swprintf_s(
            scoreText,
            L"Score: %d",
            m_score
        );

        swprintf_s(
            livesText,
            L"Lives: %d",
            m_lives
        );

        m_TextRenderer->DrawBottomLeftText(scoreText);
        m_TextRenderer->DrawBottomRightText(livesText);
    }

    m_TextRenderer->EndDraw();
}


bool GameClass::IsMenuState() const
{
    return m_GameState == GameState::MainMenu ||
        m_GameState == GameState::Paused ||
        m_GameState == GameState::Win ||
        m_GameState == GameState::Lose;
}


bool GameClass::HandleMenuInput()
{
    if (!m_Input)
    {
        return true;
    }

    bool upIsDown =
        m_Input->IsKeyDown(VK_UP) ||
        m_Input->IsKeyDown('W');

    bool downIsDown =
        m_Input->IsKeyDown(VK_DOWN) ||
        m_Input->IsKeyDown('S');

    bool enterIsDown =
        m_Input->IsKeyDown(VK_RETURN);


    /*
        Cambio selezione.

        Tutti i menu hanno due voci:
        - voce 0
        - voce 1

        Quindi basta alternare tra 0 e 1.
    */

    if ((upIsDown && !m_upWasDown) ||
        (downIsDown && !m_downWasDown))
    {
        if (m_menuSelectedIndex == 0)
        {
            m_menuSelectedIndex = 1;
        }
        else
        {
            m_menuSelectedIndex = 0;
        }
    }


    /*
        Conferma con ENTER.
    */

    if (enterIsDown && !m_enterWasDown)
    {
        if (m_GameState == GameState::MainMenu)
        {
            if (m_menuSelectedIndex == 0)
            {
                // START GAME
                StartNewGame();
            }
            else
            {
                // QUIT
                return false;
            }
        }
        else if (m_GameState == GameState::Paused)
        {
            if (m_menuSelectedIndex == 0)
            {
                // RESUME
                m_GameState = GameState::Playing;
                UpdateWindowTitle();
            }
            else
            {
                // QUIT
                return false;
            }
        }
        else if (m_GameState == GameState::Win ||
            m_GameState == GameState::Lose)
        {
            if (m_menuSelectedIndex == 0)
            {
                // RESTART
                ResetGame();
            }
            else
            {
                // QUIT
                return false;
            }
        }
    }

    m_upWasDown = upIsDown;
    m_downWasDown = downIsDown;
    m_enterWasDown = enterIsDown;

    return true;
}


void GameClass::ShutdownSecondBall()
{
    if (m_SecondBall)
    {
        m_SecondBall->Shutdown();
        delete m_SecondBall;
        m_SecondBall = nullptr;
    }
}


void GameClass::SpawnSecondBall()
{
    if (!m_D3D || !m_Ball)
    {
        return;
    }

    /*
        Per ora massimo due palle.
        Se la seconda esiste già, non ne creiamo altre.
    */

    if (m_SecondBall)
    {
        return;
    }

    m_SecondBall = new BallClass();

    if (!m_SecondBall)
    {
        return;
    }

    if (!m_SecondBall->Initialize(
        m_D3D->GetDevice(),
        m_Ball->GetX(),
        m_Ball->GetY(),
        GameConfig::BallSize
    ))
    {
        m_SecondBall->Shutdown();
        delete m_SecondBall;
        m_SecondBall = nullptr;
        return;
    }

    /*
        La seconda palla parte dalla posizione della prima,
        ma con direzione orizzontale opposta.
    */

    float velocityX = -m_Ball->GetVelocityX();
    float velocityY = m_Ball->GetVelocityY();

    if (velocityX > -0.05f && velocityX < 0.05f)
    {
        velocityX = -GameConfig::BallInitialVelocityX;
    }

    m_SecondBall->SetVelocity(
        velocityX,
        velocityY
    );

    /*
        Se il malus BallSpeedUp è già attivo,
        anche la seconda palla deve essere veloce.
    */

    if (m_isBallSpeedEffectActive)
    {
        m_SecondBall->SetSpeedMultiplier(GameConfig::BallSpeedMultiplier);
    }
}


void GameClass::CheckBallsOutOfBounds()
{
    if (!m_Ball)
    {
        return;
    }

    bool mainBallBelow = m_Ball->IsBelowBottom();
    bool secondBallBelow = m_SecondBall && m_SecondBall->IsBelowBottom();

    /*
        Caso 1:
        La seconda palla cade, ma la principale è ancora in gioco.
        Nessuna vita persa.
    */

    if (secondBallBelow && !mainBallBelow)
    {
        ShutdownSecondBall();
        return;
    }

    /*
        Caso 2:
        La palla principale cade, ma la seconda è ancora in gioco.
        Non perdiamo vita: promuoviamo la seconda a palla principale.
    */

    if (mainBallBelow && m_SecondBall && !secondBallBelow)
    {
        m_Ball->Shutdown();
        delete m_Ball;

        m_Ball = m_SecondBall;
        m_SecondBall = nullptr;

        return;
    }

    /*
        Caso 3:
        C'è solo una palla ed è caduta,
        oppure sono cadute entrambe.
        Qui perdiamo una vita.
    */

    if (mainBallBelow)
    {
        ShutdownSecondBall();
        HandleBallLost();
    }
}