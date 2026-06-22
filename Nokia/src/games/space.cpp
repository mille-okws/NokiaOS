#include "space.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// 1. IMPORTANDO AS VARIÁVEIS GLOBAIS DE HARDWARE E SISTEMA
extern Adafruit_SSD1306 display;
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

extern int estadoAtual;
#define MENU 0 

// 2. VARIÁVEIS DE ESTADO INTERNAS DO SPACE IMPACT
int spaceNaveX = 10;
int spaceNaveY = 28; // Transformado em variável para poder mover a nave verticalmente
int spaceScore = 0;
bool spaceBala = false;
int spaceBalaX = 0, spaceBalaY = 0;
int spaceInimigos[5][2]; // Matriz [x,y] dos inimigos
unsigned long spaceUltimoInimigo = 0;
unsigned long spaceUltimoMove = 0;
bool spaceGameOver = false;

// 3. MECÂNICA E LÓGICA DO GAME
void inicializarSpace() {
  spaceNaveX = 10;
  spaceNaveY = 28;
  spaceScore = 0;
  spaceBala = false;
  spaceGameOver = false;
  for (int i = 0; i < 5; i++) {
    spaceInimigos[i][0] = -20;
    spaceInimigos[i][1] = 0;
  }
}

void rodarSpaceImpact() {
  unsigned long agora = millis();
  
  if (spaceGameOver) {
    display.clearDisplay();
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    display.setCursor(38, 20); display.print("GAME OVER");
    display.setCursor(32, 35); display.print("Score: "); display.print(spaceScore);
    display.setCursor(15, 52); display.print("[Clique] Menu");
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      estadoAtual = MENU;
      ultimoInput = agora;
    }
    return;
  }
  
  int leituraY = analogRead(pinY);
  
  if (leituraY < 500 && spaceNaveY > 10) spaceNaveY -= 2;
  if (leituraY > 3500 && spaceNaveY < 54) spaceNaveY += 2;
  
  if (digitalRead(pinSW) == LOW && !spaceBala && agora - ultimoInput > 300) {
    spaceBala = true;
    spaceBalaX = spaceNaveX + 8;
    spaceBalaY = spaceNaveY + 2;
    ultimoInput = agora;
  }
  
  if (spaceBala) {
    spaceBalaX += 3;
    if (spaceBalaX > 128) spaceBala = false;
  }
  
  if (agora - spaceUltimoInimigo > 1500) {
    for (int i = 0; i < 5; i++) {
      if (spaceInimigos[i][0] < 0) {
        spaceInimigos[i][0] = 128;
        spaceInimigos[i][1] = random(10, 54);
        break;
      }
    }
    spaceUltimoInimigo = agora;
  }
  
  for (int i = 0; i < 5; i++) {
    if (spaceInimigos[i][0] >= 0) {
      spaceInimigos[i][0] -= 2;
      
      // Colisão da Bala com Inimigo
      if (spaceBala && abs(spaceBalaX - spaceInimigos[i][0]) < 6 && 
          abs(spaceBalaY - spaceInimigos[i][1]) < 6) {
        spaceScore += 10;
        spaceInimigos[i][0] = -20;
        spaceBala = false;
      }
      
      // Colisão da Nave com Inimigo
      if (abs(spaceNaveX - spaceInimigos[i][0]) < 6 && 
          abs(spaceNaveY - spaceInimigos[i][1]) < 6) {
        spaceGameOver = true;
      }
      
      if (spaceInimigos[i][0] < -10) {
        spaceInimigos[i][0] = -20;
      }
    }
  }
  
  display.clearDisplay();
  display.setCursor(0, 0); display.print("Score: "); display.print(spaceScore);
  
  // Desenha a Nave (Triângulo)
  display.fillTriangle(spaceNaveX, spaceNaveY, 
                       spaceNaveX, spaceNaveY+6, 
                       spaceNaveX+8, spaceNaveY+3, SSD1306_WHITE);
  
  // Desenha o Tiro
  if (spaceBala) {
    display.fillRect(spaceBalaX, spaceBalaY, 4, 2, SSD1306_WHITE);
  }
  
  // Desenha os Inimigos ativos
  for (int i = 0; i < 5; i++) {
    if (spaceInimigos[i][0] >= 0 && spaceInimigos[i][0] < 128) {
      display.fillRect(spaceInimigos[i][0], spaceInimigos[i][1], 6, 6, SSD1306_WHITE);
    }
  }
}