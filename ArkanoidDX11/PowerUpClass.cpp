#include "PowerUpClass.h"
#include "GameConfig.h"


PowerUpClass::PowerUpClass()
{
    m_type = PowerUpType::PaddleGrow;
    m_isActive = true;
    m_fallSpeed = GameConfig::PowerUpFallSpeed;
}


PowerUpClass::~PowerUpClass()
{
}


bool PowerUpClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    PowerUpType type
)
{
    m_type = type;
    m_isActive = true;

    /*
        Colore:
        - bonus verde
        - malus rosso
    */

    float r = 0.0f;
    float g = 1.0f;
    float b = 0.0f;

    if (m_type == PowerUpType::PaddleShrink ||
        m_type == PowerUpType::BallSpeedUp)
    {
        r = 1.0f;
        g = 0.0f;
        b = 0.0f;
    }

    return m_Rect.Initialize(
        device,
        x,
        y,
        GameConfig::PowerUpSize,
        GameConfig::PowerUpSize,
        r,
        g,
        b
    );
}


void PowerUpClass::Shutdown()
{
    m_Rect.Shutdown();
}


void PowerUpClass::Update(float deltaTime)
{
    if (!m_isActive)
    {
        return;
    }

    float x = m_Rect.GetX();
    float y = m_Rect.GetY();

    y -= m_fallSpeed * deltaTime;

    m_Rect.SetPosition(x, y);

    /*
        Se il powerup esce sotto lo schermo,
        lo disattiviamo.
    */

    if (m_Rect.GetTop() < -1.0f)
    {
        m_isActive = false;
    }
}


void PowerUpClass::Render(ID3D11DeviceContext* deviceContext)
{
    if (!m_isActive)
    {
        return;
    }

    m_Rect.Render(deviceContext);
}


int PowerUpClass::GetIndexCount() const
{
    return m_Rect.GetIndexCount();
}


bool PowerUpClass::IsActive() const
{
    return m_isActive;
}


void PowerUpClass::SetActive(bool active)
{
    m_isActive = active;
}


PowerUpType PowerUpClass::GetType() const
{
    return m_type;
}


float PowerUpClass::GetLeft() const
{
    return m_Rect.GetLeft();
}


float PowerUpClass::GetRight() const
{
    return m_Rect.GetRight();
}


float PowerUpClass::GetTop() const
{
    return m_Rect.GetTop();
}


float PowerUpClass::GetBottom() const
{
    return m_Rect.GetBottom();
}