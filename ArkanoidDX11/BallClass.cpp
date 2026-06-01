#include "BallClass.h"
#include "GameConfig.h"


/*
    BallClass gestisce la palla del gioco.

    La classe si occupa di:
    - disegnare la palla come cerchio 2D
    - aggiornare posizione e velocità
    - gestire i rimbalzi sui bordi dello schermo
    - gestire il rimbalzo sul paddle
    - fornire i dati necessari per collisioni e multiball
*/


// Inizializza i valori base della palla
BallClass::BallClass()
{
    m_size = 0.0f;

    m_velocityX = GameConfig::BallInitialVelocityX;
    m_velocityY = GameConfig::BallInitialVelocityY;

    m_speedMultiplier = 1.0f;
}


// Distruttore della classe palla
BallClass::~BallClass()
{
}


// Inizializza la palla come cerchio 2D
bool BallClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float size
)
{
    m_size = size;

    return m_Circle.Initialize(
        device,
        x,
        y,
        size * 0.5f,
        1.0f,
        0.9f,
        0.2f,
        32
    );
}


// Rilascia le risorse della palla
void BallClass::Shutdown()
{
    m_Circle.Shutdown();
}


// Aggiorna posizione e collisioni con i bordi
void BallClass::Update(float deltaTime)
{
    float x = m_Circle.GetX();
    float y = m_Circle.GetY();

    x += m_velocityX * m_speedMultiplier * deltaTime;
    y += m_velocityY * m_speedMultiplier * deltaTime;

    m_Circle.SetPosition(x, y);

    CheckWallCollision();
}


// Manda la palla alla pipeline grafica
void BallClass::Render(ID3D11DeviceContext* deviceContext)
{
    m_Circle.Render(deviceContext);
}


// Restituisce il numero di indici da disegnare
int BallClass::GetIndexCount() const
{
    return m_Circle.GetIndexCount();
}


// Resetta posizione e velocità della palla
void BallClass::Reset(float x, float y, float velocityX, float velocityY)
{
    m_Circle.SetPosition(x, y);

    m_velocityX = velocityX;
    m_velocityY = velocityY;
}


// Controlla se la palla è uscita dal basso
bool BallClass::IsBelowBottom() const
{
    return GetBottom() < -1.0f;
}


// Restituisce il bordo sinistro della palla
float BallClass::GetLeft() const
{
    return m_Circle.GetLeft();
}


// Restituisce il bordo destro della palla
float BallClass::GetRight() const
{
    return m_Circle.GetRight();
}


// Restituisce il bordo superiore della palla
float BallClass::GetTop() const
{
    return m_Circle.GetTop();
}


// Restituisce il bordo inferiore della palla
float BallClass::GetBottom() const
{
    return m_Circle.GetBottom();
}


// Controlla se la palla si sta muovendo verso il basso
bool BallClass::IsMovingDown() const
{
    return m_velocityY < 0.0f;
}


// Gestisce il rimbalzo della palla sul paddle
void BallClass::BounceFromPaddle(float paddleTop, float hitFactor)
{
    m_Circle.SetPosition(
        m_Circle.GetX(),
        paddleTop + (m_size * 0.5f)
    );

    if (hitFactor < -1.0f)
    {
        hitFactor = -1.0f;
    }

    if (hitFactor > 1.0f)
    {
        hitFactor = 1.0f;
    }

    if (m_velocityY < 0.0f)
    {
        m_velocityY *= -1.0f;
    }

    const float maxHorizontalSpeed = GameConfig::BallMaxHorizontalSpeed;
    const float minHorizontalSpeed = GameConfig::BallMinHorizontalSpeed;

    if (hitFactor > -0.15f && hitFactor < 0.15f)
    {
        if (m_velocityX < 0.0f)
        {
            m_velocityX = -minHorizontalSpeed;
        }
        else
        {
            m_velocityX = minHorizontalSpeed;
        }
    }
    else
    {
        m_velocityX = hitFactor * maxHorizontalSpeed;
    }
}


// Inverte la direzione verticale della palla
void BallClass::BounceY()
{
    m_velocityY *= -1.0f;
}


// Inverte la direzione orizzontale della palla
void BallClass::BounceX()
{
    m_velocityX *= -1.0f;
}


// Gestisce il rimbalzo sui bordi dello schermo
void BallClass::CheckWallCollision()
{
    float halfSize = m_size * 0.5f;

    float x = m_Circle.GetX();
    float y = m_Circle.GetY();

    if (x - halfSize < -1.0f)
    {
        x = -1.0f + halfSize;
        m_velocityX *= -1.0f;
    }

    if (x + halfSize > 1.0f)
    {
        x = 1.0f - halfSize;
        m_velocityX *= -1.0f;
    }

    if (y + halfSize > 1.0f)
    {
        y = 1.0f - halfSize;
        m_velocityY *= -1.0f;
    }

    m_Circle.SetPosition(x, y);
}


// Imposta la posizione della palla
void BallClass::SetPosition(float x, float y)
{
    m_Circle.SetPosition(x, y);
}


// Imposta la velocità della palla
void BallClass::SetVelocity(float velocityX, float velocityY)
{
    m_velocityX = velocityX;
    m_velocityY = velocityY;
}


// Imposta il moltiplicatore di velocità della palla
void BallClass::SetSpeedMultiplier(float multiplier)
{
    m_speedMultiplier = multiplier;
}


// Restituisce la posizione X della palla
float BallClass::GetX() const
{
    return m_Circle.GetX();
}


// Restituisce la posizione Y della palla
float BallClass::GetY() const
{
    return m_Circle.GetY();
}


// Restituisce la velocità orizzontale della palla
float BallClass::GetVelocityX() const
{
    return m_velocityX;
}


// Restituisce la velocità verticale della palla
float BallClass::GetVelocityY() const
{
    return m_velocityY;
}