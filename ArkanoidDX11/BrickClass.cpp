#include "BrickClass.h"


/*
    BrickClass gestisce un singolo brick del livello

    La classe si occupa di:
    - disegnare il brick come rettangolo 2D
    - salvare se il brick è attivo o distrutto
    - fornire i dati necessari per collisioni e spawn dei powerup
*/


// Inizializza lo stato base del brick
BrickClass::BrickClass()
{
    m_isActive = true;
}


// Distruttore della classe brick
BrickClass::~BrickClass()
{
}


// Inizializza il brick con posizione, dim e colore
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


// Rilascia le risorse del brick
void BrickClass::Shutdown()
{
    m_Rect.Shutdown();
}


// Manda il brick alla pipeline grafica
void BrickClass::Render(ID3D11DeviceContext* deviceContext)
{
    m_Rect.Render(deviceContext);
}


// Restituisce il numero di indici da disegnare
int BrickClass::GetIndexCount() const
{
    return m_Rect.GetIndexCount();
}


// Controlla se il brick è ancora attivo
bool BrickClass::IsActive() const
{
    return m_isActive;
}


// Imposta lo stato attivo del brick
void BrickClass::SetActive(bool active)
{
    m_isActive = active;
}


// Restituisce il bordo sinistro del brick
float BrickClass::GetLeft() const
{
    return m_Rect.GetLeft();
}


// Restituisce il bordo destro del brick
float BrickClass::GetRight() const
{
    return m_Rect.GetRight();
}


// Restituisce il bordo superiore del brick
float BrickClass::GetTop() const
{
    return m_Rect.GetTop();
}


// Restituisce il bordo inferiore del brick
float BrickClass::GetBottom() const
{
    return m_Rect.GetBottom();
}


// Restituisce la posizione X del brick
float BrickClass::GetX() const
{
    return (GetLeft() + GetRight()) * 0.5f;
}


// Restituisce la posizione Y del brick
float BrickClass::GetY() const
{
    return (GetTop() + GetBottom()) * 0.5f;
}