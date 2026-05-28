#include "BrickClass.h"


BrickClass::BrickClass()
{
    m_isActive = true;
}


BrickClass::~BrickClass()
{
}


bool BrickClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    float width,
    float height,
    float r,
    float g,
    float b
)
{
    m_isActive = true;

    return m_Rect.Initialize(
        device,
        x,
        y,
        width,
        height,
        r,
        g,
        b
    );
}


void BrickClass::Shutdown()
{
    m_Rect.Shutdown();
}


void BrickClass::Render(ID3D11DeviceContext* deviceContext)
{
    m_Rect.Render(deviceContext);
}


int BrickClass::GetIndexCount() const
{
    return m_Rect.GetIndexCount();
}


bool BrickClass::IsActive() const
{
    return m_isActive;
}


void BrickClass::SetActive(bool active)
{
    m_isActive = active;
}


float BrickClass::GetLeft() const
{
    return m_Rect.GetLeft();
}


float BrickClass::GetRight() const
{
    return m_Rect.GetRight();
}


float BrickClass::GetTop() const
{
    return m_Rect.GetTop();
}


float BrickClass::GetBottom() const
{
    return m_Rect.GetBottom();
}