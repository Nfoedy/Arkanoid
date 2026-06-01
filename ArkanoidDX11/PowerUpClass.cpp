#include "PowerUpClass.h"
#include "GameConfig.h"


/*
    PowerUpClass gestisce bonus e malus che cadono dai brick

    La classe si occupa di:
    - creare il powerup con tipo, posizione e colore
    - aggiornarne la caduta verso il basso
    - disegnarlo come rettangolo 2D
    - disattivarlo quando viene raccolto o esce dallo schermo
*/


// Inizializza i valori base del powerup
PowerUpClass::PowerUpClass()
{
    m_type = PowerUpType::PaddleGrow;
    m_isActive = true;
    m_fallSpeed = GameConfig::PowerUpFallSpeed;
}


// Distruttore della classe powerup
PowerUpClass::~PowerUpClass()
{
}


// Inizializza il powerup con posizione e tipo
bool PowerUpClass::Initialize(
    ID3D11Device* device,
    float x,
    float y,
    PowerUpType type
)
{
    m_type = type;
    m_isActive = true;

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


// Rilascia le risorse del powerup
void PowerUpClass::Shutdown()
{
    m_Rect.Shutdown();
}


// Aggiorna la caduta del powerup
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

    if (m_Rect.GetTop() < -1.0f)
    {
        m_isActive = false;
    }
}


// Manda il powerup alla pipeline grafica
void PowerUpClass::Render(ID3D11DeviceContext* deviceContext)
{
    if (!m_isActive)
    {
        return;
    }

    m_Rect.Render(deviceContext);
}


// Restituisce il numero di indici da disegnare
int PowerUpClass::GetIndexCount() const
{
    return m_Rect.GetIndexCount();
}


// Controlla se il powerup è attivo
bool PowerUpClass::IsActive() const
{
    return m_isActive;
}


// Imposta lo stato attivo del powerup
void PowerUpClass::SetActive(bool active)
{
    m_isActive = active;
}


// Restituisce il tipo del powerup
PowerUpType PowerUpClass::GetType() const
{
    return m_type;
}


// Restituisce il bordo sinistro del powerup
float PowerUpClass::GetLeft() const
{
    return m_Rect.GetLeft();
}


// Restituisce il bordo destro del powerup
float PowerUpClass::GetRight() const
{
    return m_Rect.GetRight();
}


// Restituisce il bordo superiore del powerup
float PowerUpClass::GetTop() const
{
    return m_Rect.GetTop();
}


// Restituisce il bordo inferiore del powerup
float PowerUpClass::GetBottom() const
{
    return m_Rect.GetBottom();
}