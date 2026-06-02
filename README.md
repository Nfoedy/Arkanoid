# ArkanoidDX

ArkanoidDX è un mini-gioco in stile Arkanoid sviluppato in C++ con DirectX 11, Direct2D e DirectWrite per il Master in Computer Game Development dell’Università di Verona.

L’obiettivo del gioco è distruggere tutti i brick presenti nella scena controllando un paddle e facendo rimbalzare una o più palle, evitando che escano dalla parte inferiore dello schermo.

📌 Il progetto è stato sviluppato partendo da zero, con l’obiettivo di comprendere meglio il funzionamento di una finestra Win32, della pipeline grafica di DirectX 11, del game loop, della gestione dell’input, delle collisioni, degli stati di gioco e del rendering del testo.

## 🎮 Concept

Il progetto riprende la struttura classica di Arkanoid: il giocatore controlla un paddle nella parte bassa dello schermo e deve colpire una palla per distruggere una griglia di brick.

Il gioco è stato realizzato senza utilizzare un game engine, costruendo manualmente i principali sistemi di base: rendering 2D, input, aggiornamento tramite delta time, collisioni AABB, gestione degli stati e interfaccia testuale.


## 🕹️ Gameplay

Il giocatore deve distruggere tutti i brick senza perdere tutte le vite disponibili.

La partita inizia dal Main Menu. Una volta avviato il gioco, la palla rimane agganciata al paddle finché il giocatore non la lancia. Durante la partita, alcuni brick possono rilasciare bonus o malus che cadono verso il basso e possono essere raccolti dal paddle.

Funzionalità principali:

* Movimento del paddle
* Palla rotonda renderizzata tramite triangoli
* Griglia di brick distruttibili
* Collisione palla-paddle
* Collisione palla-brick
* Sistema di punteggio
* Sistema di vite
* HUD con score e vite
* Main Menu
* Menu di pausa
* Schermata di vittoria
* Schermata di sconfitta
* Bonus e malus temporanei
* Powerup multiball

## ⭐ Powerup e Malus

Durante il gioco, alcuni brick possono rilasciare un oggetto bonus o malus.

Effetti implementati:

| Tipo          | Effetto                                         |
| ------------- | ----------------------------------------------- |
| Paddle Grow   | Aumenta temporaneamente la larghezza del paddle |
| Paddle Shrink | Riduce temporaneamente la larghezza del paddle  |
| Ball Speed Up | Aumenta temporaneamente la velocità della palla |
| MultiBall     | Genera una seconda palla                        |

Nel caso del multiball, il giocatore perde una vita solo quando tutte le palle sono uscite dallo schermo. Se una sola palla cade mentre l’altra è ancora in gioco, la partita continua normalmente.

## 🎛️ Controlli

| Tasto                | Azione                         |
| -------------------- | ------------------------------ |
| Freccia sinistra / A | Muove il paddle a sinistra     |
| Freccia destra / D   | Muove il paddle a destra       |
| Spazio               | Lancia la palla                |
| P                    | Mette il gioco in pausa        |
| Freccia su / W       | Naviga il menu                 |
| Freccia giù / S      | Naviga il menu                 |
| Invio                | Conferma l’opzione selezionata |
| Esc                  | Esce dal gioco                 |



## 🛠️ Tecnologie utilizzate

* C++
* Win32 API
* DirectX 11
* Direct2D
* DirectWrite
* Git / GitHub

## 🧱 Struttura del progetto

Il progetto è diviso in classi con responsabilità separate.

### Sistemi principali

* `GameClass`
  Gestisce il flusso principale del gioco, gli stati, l’input, l’update, il rendering, le collisioni, score, vite, powerup e multiball.

* `D3DClass`
  Gestisce l’inizializzazione di DirectX 11, device, device context, swap chain, render target e presentazione della scena.

* `InputClass`
  Memorizza lo stato dei tasti della tastiera.

* `TimerClass`
  Calcola il delta time per rendere il movimento indipendente dal frame rate.

### Rendering

* `ColorShaderClass`
  Gestisce lo shader utilizzato per disegnare gli oggetti colorati.

* `RectObject2D`
  Disegna rettangoli 2D usati per paddle, brick e powerup.

* `CircleObject2D`
  Disegna la palla come un cerchio costruito tramite triangoli.

* `TextRendererClass`
  Usa DirectWrite e Direct2D per disegnare testo, menu, istruzioni, score e vite sopra la scena DirectX.

### Oggetti di gameplay

* `PaddleClass`
  Gestisce il paddle, il movimento, il blocco ai bordi dello schermo e la modifica temporanea della larghezza.

* `BallClass`
  Gestisce movimento, rimbalzi, velocità e dati di collisione della palla.

* `BrickClass`
  Rappresenta un brick distruttibile.

* `PowerUpClass`
  Rappresenta un bonus o malus che cade dai brick.



## 📸 Video & Media

Gameplay video: 
