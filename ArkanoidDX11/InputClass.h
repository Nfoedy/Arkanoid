#pragma once


class InputClass
{
public:
    InputClass();
    ~InputClass();

    // Inizializza lo stato di tutti i tasti
    void Initialize();

    // Segna un tasto come premuto
    void KeyDown(unsigned int key);

    // Segna un tasto come rilasciato
    void KeyUp(unsigned int key);

    // Controlla se un tasto è attualmente premuto
    bool IsKeyDown(unsigned int key) const;

private:
    // Array di 256 booleani
    // Ogni posizione rappresenta un possibile tasto
    // true  = tasto premuto
    // false = tasto non premuto
    bool m_keys[256];
};