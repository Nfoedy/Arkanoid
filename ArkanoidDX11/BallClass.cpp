#include "BallClass.h"
#include "GameConfig.h"


BallClass::BallClass()
{
    m_size = 0.0f;

    m_velocityX = GameConfig::BallInitialVelocityX;
    m_velocityY = GameConfig::BallInitialVelocityY;
}


BallClass::~BallClass()
{
}


bool BallClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float size
)
{
    m_size = size;

    return m_Rect.Initialize(
        device,
        x,
        y,
        size,
        size,
        1.0f,
        0.9f,
        0.2f
    );
}


void BallClass::Shutdown()
{
    m_Rect.Shutdown();
}


void BallClass::Update(float deltaTime)
{
    float x = m_Rect.GetX();
    float y = m_Rect.GetY();

    x += m_velocityX * deltaTime;
    y += m_velocityY * deltaTime;

    m_Rect.SetPosition(x, y);

    CheckWallCollision();
}


void BallClass::Render(ID3D11DeviceContext* deviceContext)
{
    m_Rect.Render(deviceContext);
}


int BallClass::GetIndexCount() const
{
    return m_Rect.GetIndexCount();
}


void BallClass::Reset(float x, float y, float velocityX, float velocityY)
{
    m_Rect.SetPosition(x, y);

    m_velocityX = velocityX;
    m_velocityY = velocityY;
}


bool BallClass::IsBelowBottom() const
{
    return GetBottom() < -1.0f;
}


float BallClass::GetLeft() const
{
    return m_Rect.GetLeft();
}


float BallClass::GetRight() const
{
    return m_Rect.GetRight();
}


float BallClass::GetTop() const
{
    return m_Rect.GetTop();
}


float BallClass::GetBottom() const
{
    return m_Rect.GetBottom();
}


bool BallClass::IsMovingDown() const
{
    return m_velocityY < 0.0f;
}


void BallClass::BounceFromPaddle(float paddleTop, float hitFactor)
{
    /*
        Spostiamo la palla appena sopra il paddle.
        Questo evita che rimanga incastrata dentro il paddle.
    */

    m_Rect.SetPosition(
        m_Rect.GetX(),
        paddleTop + (m_size * 0.5f)
    );


    /*
        Limitiamo hitFactor tra -1 e +1.

        hitFactor:
        -1 = lato sinistro del paddle
         0 = centro del paddle
        +1 = lato destro del paddle
    */

    if (hitFactor < -1.0f)
    {
        hitFactor = -1.0f;
    }

    if (hitFactor > 1.0f)
    {
        hitFactor = 1.0f;
    }


    /*
        Dopo aver colpito il paddle,
        la palla deve andare verso l'alto.
    */

    if (m_velocityY < 0.0f)
    {
        m_velocityY *= -1.0f;
    }


    /*
        Velocità orizzontale basata sul punto di impatto.
    */

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


void BallClass::BounceY()
{
    m_velocityY *= -1.0f;
}


void BallClass::BounceX()
{
    m_velocityX *= -1.0f;
}


void BallClass::CheckWallCollision()
{
    float halfSize = m_size * 0.5f;

    float x = m_Rect.GetX();
    float y = m_Rect.GetY();

    // Bordo sinistro.
    if (x - halfSize < -1.0f)
    {
        x = -1.0f + halfSize;
        m_velocityX *= -1.0f;
    }

    // Bordo destro.
    if (x + halfSize > 1.0f)
    {
        x = 1.0f - halfSize;
        m_velocityX *= -1.0f;
    }

    // Bordo alto.
    if (y + halfSize > 1.0f)
    {
        y = 1.0f - halfSize;
        m_velocityY *= -1.0f;
    }

    // Bordo basso:
    // non rimbalziamo più.
    // La sconfitta viene gestita da GameClass.

    m_Rect.SetPosition(x, y);
}


void BallClass::SetPosition(float x, float y)
{
    m_Rect.SetPosition(x, y);
}


void BallClass::SetVelocity(float velocityX, float velocityY)
{
    m_velocityX = velocityX;
    m_velocityY = velocityY;
}


void BallClass::MultiplyVelocity(float factor)
{
    m_velocityX *= factor;
    m_velocityY *= factor;
}