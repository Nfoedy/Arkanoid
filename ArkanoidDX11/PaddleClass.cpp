#include "PaddleClass.h"
#include "GameConfig.h"


/*
    PaddleClass gestisce il paddle controllato dal giocatore

    La classe si occupa di:
    - disegnare il paddle come rettangolo 2D
    - muoverlo a sinistra e a destra
    - limitarlo dentro i bordi dello schermo
    - modificarne la larghezza tramite powerup e malus
*/


// Inizializza la velocità del paddle
PaddleClass::PaddleClass()
{
    m_speed = GameConfig::PaddleSpeed;
}


// Distruttore della classe paddle
PaddleClass::~PaddleClass()
{
}


// Inizializza il paddle con posizione e dimensione
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


// Rilascia le risorse del paddle
void PaddleClass::Shutdown()
{
    m_Rect.Shutdown();
}


// Manda il paddle alla pipeline grafica
void PaddleClass::Render(ID3D11DeviceContext* deviceContext)
{
    m_Rect.Render(deviceContext);
}


// Muove il paddle verso sinistra
void PaddleClass::MoveLeft(float deltaTime)
{
    m_Rect.SetPosition(
        m_Rect.GetX() - m_speed * deltaTime,
        m_Rect.GetY()
    );

    ClampToScreen();
}


// Muove il paddle verso destra
void PaddleClass::MoveRight(float deltaTime)
{
    m_Rect.SetPosition(
        m_Rect.GetX() + m_speed * deltaTime,
        m_Rect.GetY()
    );

    ClampToScreen();
}


// Riporta il paddle alla posizione iniziale
void PaddleClass::Reset(float x, float y)
{
    m_Rect.SetPosition(x, y);

    ClampToScreen();
}


// Modifica la larghezza del paddle
void PaddleClass::SetWidth(float width)
{
    m_Rect.SetSize(
        width,
        m_Rect.GetHeight()
    );

    ClampToScreen();
}


// Restituisce il numero di indici da disegnare
int PaddleClass::GetIndexCount() const
{
    return m_Rect.GetIndexCount();
}


// Restituisce il bordo sinistro del paddle
float PaddleClass::GetLeft() const
{
    return m_Rect.GetLeft();
}


// Restituisce il bordo destro del paddle
float PaddleClass::GetRight() const
{
    return m_Rect.GetRight();
}


// Restituisce il bordo superiore del paddle
float PaddleClass::GetTop() const
{
    return m_Rect.GetTop();
}


// Restituisce il bordo inferiore del paddle
float PaddleClass::GetBottom() const
{
    return m_Rect.GetBottom();
}


// Mantiene il paddle dentro i bordi dello schermo
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