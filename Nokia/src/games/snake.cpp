#include "snake.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// 1. IMPORTANDO AS VARIÁVEIS GLOBAIS DE HARDWARE E SISTEMA
extern Adafruit_SSD1306 display;
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

extern int estadoAtual;
#define MENU 0 

// Definições exclusivas do campo do Snake
#define TAMANHO_BLOCO 4
#define LARGURA_CAMPO 128
#define ALTURA_CAMPO 56

// 2. VARIÁVEIS DE ESTADO INTERNAS DO JOGO
Direcao dirAtual = PARADO;
Ponto cobra[100];
int tamanhoCobra = 3;
Ponto comida;
int pontuacao = 0;
bool gameOver = false;
unsigned long ultimoMovimentoCobra = 0;
const unsigned long velocidadeCobra = 120;

// 3. MECÂNICA E LÓGICA DO GAME
void inicializarJogo() {
  tamanhoCobra = 3;
  pontuacao = 0;
  gameOver = false;
  dirAtual = PARADO;

  int centroX = (LARGURA_CAMPO / 2 / TAMANHO_BLOCO) * TAMANHO_BLOCO;
  int centroY = (8 + (ALTURA_CAMPO / 2) / TAMANHO_BLOCO) * TAMANHO_BLOCO;

  for (int i = 0; i < tamanhoCobra; i++) {
    cobra[i].x = centroX - (i * TAMANHO_BLOCO);
    cobra[i].y = centroY;
  }

  comida.x = (random(0, LARGURA_CAMPO / TAMANHO_BLOCO) * TAMANHO_BLOCO);
  comida.y = 8 + (random(0, ALTURA_CAMPO / TAMANHO_BLOCO) * TAMANHO_BLOCO);
}

void rodarJogoSnake() {
  unsigned long tempoAtual = millis();

  if (gameOver) {
    static bool telaGameOverDesenhada = false;
    if (!telaGameOverDesenhada) {
      display.clearDisplay();
      display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
      display.setCursor(38, 15); display.print("GAME OVER");
      display.setCursor(32, 32); display.print("Pontos: "); display.print(pontuacao);
      display.setCursor(15, 48); display.print("[Clique] Menu");
      display.display();
      telaGameOverDesenhada = true;
    }

    if (digitalRead(pinSW) == LOW && tempoAtual - ultimoInput > debounce) {
      telaGameOverDesenhada = false;
      estadoAtual = MENU;
      ultimoInput = tempoAtual;
    }
    return;
  }

  int leituraY = analogRead(pinY);
  int leituraX = analogRead(pinX);
  
  if (leituraY < 500 && dirAtual != BAIXO) dirAtual = CIMA;
  else if (leituraY > 3500 && dirAtual != CIMA) dirAtual = BAIXO;
  else if (leituraX < 500 && dirAtual != ESQUERDA) dirAtual = DIREITA;
  else if (leituraX > 3500 && dirAtual != DIREITA) dirAtual = ESQUERDA;

  if (tempoAtual - ultimoMovimentoCobra > velocidadeCobra) {
    ultimoMovimentoCobra = tempoAtual;

    if (dirAtual == PARADO) {
      desenharTelaJogo();
      return;
    }

    Ponto posicaoAnteriorRabo = cobra[tamanhoCobra - 1];

    for (int i = tamanhoCobra - 1; i > 0; i--) {
      cobra[i] = cobra[i - 1];
    }

    if (dirAtual == CIMA) cobra[0].y -= TAMANHO_BLOCO;
    else if (dirAtual == BAIXO) cobra[0].y += TAMANHO_BLOCO;
    else if (dirAtual == ESQUERDA) cobra[0].x -= TAMANHO_BLOCO;
    else if (dirAtual == DIREITA) cobra[0].x += TAMANHO_BLOCO;

    if (cobra[0].x < 0 || cobra[0].x >= LARGURA_CAMPO || cobra[0].y < 8 || cobra[0].y >= 64) {
      gameOver = true;
    }

    for (int i = 1; i < tamanhoCobra; i++) {
      if (cobra[0].x == cobra[i].x && cobra[0].y == cobra[i].y) {
        gameOver = true;
      }
    }

    if (cobra[0].x == comida.x && cobra[0].y == comida.y) {
      pontuacao += 10;
      if (tamanhoCobra < 100) {
        tamanhoCobra++;
        cobra[tamanhoCobra - 1] = posicaoAnteriorRabo;
      }
      comida.x = (random(0, LARGURA_CAMPO / TAMANHO_BLOCO) * TAMANHO_BLOCO);
      comida.y = 8 + (random(0, ALTURA_CAMPO / TAMANHO_BLOCO) * TAMANHO_BLOCO);
    }

    desenharTelaJogo();
  }
}

void desenharTelaJogo() {
  display.clearDisplay();
  display.setCursor(5, 0); display.print("SCORE: "); display.print(pontuacao);
  display.drawLine(0, 7, 128, 7, SSD1306_WHITE);
  display.fillRect(comida.x, comida.y, TAMANHO_BLOCO, TAMANHO_BLOCO, SSD1306_WHITE);
  
  for (int i = 0; i < tamanhoCobra; i++) {
    display.fillRect(cobra[i].x, cobra[i].y, TAMANHO_BLOCO, TAMANHO_BLOCO, SSD1306_WHITE);
  }
}