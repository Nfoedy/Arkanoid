#include "PaddleClass.h"


PaddleClass::PaddleClass()
{
    m_speed = 1.2f;
}


PaddleClass::~PaddleClass()
{
}


bool PaddleClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float width,
    float height
)
{
    return m_Rect.Initialize(
        device,
        x,
        y,
        width,
        height,
        0.9f,
        0.9f,
        1.0f
    );
}


void PaddleClass::Shutdown()
{
    m_Rect.Shutdown();
}


void PaddleClass::Render(ID3D11DeviceContext* deviceContext)
{
    m_Rect.Render(deviceContext);
}


void PaddleClass::MoveLeft(float deltaTime)
{
    m_Rect.SetPosition(
        m_Rect.GetX() - m_speed * deltaTime,
        m_Rect.GetY()
    );

    ClampToScreen();
}


void PaddleClass::MoveRight(float deltaTime)
{
    m_Rect.SetPosition(
        m_Rect.GetX() + m_speed * deltaTime,
        m_Rect.GetY()
    );

    ClampToScreen();
}


void PaddleClass::Reset(float x, float y)
{
    m_Rect.SetPosition(x, y);

    ClampToScreen();
}


int PaddleClass::GetIndexCount() const
{
    return m_Rect.GetIndexCount();
}


float PaddleClass::GetLeft() const
{
    return m_Rect.GetLeft();
}


float PaddleClass::GetRight() const
{
    return m_Rect.GetRight();
}


float PaddleClass::GetTop() const
{
    return m_Rect.GetTop();
}


float PaddleClass::GetBottom() const
{
    return m_Rect.GetBottom();
}


void PaddleClass::ClampToScreen()
{
    float halfWidth = m_Rect.GetWidth() * 0.5f;

    float x = m_Rect.GetX();
    float y = m_Rect.GetY();

    if (x - halfWidth < -1.0f)
    {
        x = -1.0f + halfWidth;
    }

    if (x + halfWidth > 1.0f)
    {
        x = 1.0f - halfWidth;
    }

    m_Rect.SetPosition(x, y);
}