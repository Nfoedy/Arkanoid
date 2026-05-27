#include "InputClass.h"


InputClass::InputClass()
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
    // All'inizio nessun tasto è premuto
    for (int i = 0; i < 256; i++)
    {
        m_keys[i] = false;
    }
}


void InputClass::KeyDown(unsigned int key)
{
    // Controlla che il codice del tasto sia valido
    if (key < 256)
    {
        m_keys[key] = true;
    }
}


void InputClass::KeyUp(unsigned int key)
{
    // Controlla che il codice del tasto sia valido
    if (key < 256)
    {
        m_keys[key] = false;
    }
}


bool InputClass::IsKeyDown(unsigned int key) const
{
    // Se il tasto è valido, ritorniamo il suo stato
    if (key < 256)
    {
        return m_keys[key];
    }

    // Se il codice non è valido, consideriamo il tasto non premuto
    return false;
}