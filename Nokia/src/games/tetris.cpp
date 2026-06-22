#include "tetris.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// 1. IMPORTANDO AS VARIÁVEIS GLOBAIS DO HARDWARE
extern Adafruit_SSD1306 display;
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

extern int estadoAtual;
#define MENU 0 

// Definições exclusivas do Tetris
#define TETRIS_LARGURA 10
#define TETRIS_ALTURA 16
#define TETRIS_BLOCO 3
#define TETRIS_OFFSET_X 10
#define TETRIS_OFFSET_Y 8

// Variáveis internas do jogo
int tetrisGrid[TETRIS_ALTURA][TETRIS_LARGURA];
int tetrisPeca[4][4];
int tetrisPosX = 3, tetrisPosY = 0;
int tetrisTipo = 0;
int tetrisRotacao = 0;
int tetrisScore = 0;
int tetrisLinhas = 0;
unsigned long tetrisUltimoMove = 0;
int tetrisVelocidade = 500;
bool tetrisGameOver = false;

const int tetrisPecas[7][4][4] = {
  // I
  {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
  // O
  {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
  // T
  {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}},
  // S
  {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
  // Z
  {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
  // J
  {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}},
  // L
  {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}}
};

// 2. LOGICA DO JOGO

void inicializarTetris() {
  for (int y = 0; y < TETRIS_ALTURA; y++)
    for (int x = 0; x < TETRIS_LARGURA; x++)
      tetrisGrid[y][x] = 0;
  
  tetrisScore = 0;
  tetrisLinhas = 0;
  tetrisGameOver = false;
  tetrisVelocidade = 500;
  novaPecaTetris();
}

void novaPecaTetris() {
  tetrisTipo = random(7);
  tetrisRotacao = 0;
  tetrisPosX = 3;
  tetrisPosY = 0;
  
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      tetrisPeca[y][x] = tetrisPecas[tetrisTipo][y][x];
  
  if (colideTetris(tetrisPosX, tetrisPosY)) {
    tetrisGameOver = true;
  }
}

bool colideTetris(int px, int py) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (tetrisPeca[y][x]) {
        int gx = px + x;
        int gy = py + y;
        if (gx < 0 || gx >= TETRIS_LARGURA || gy >= TETRIS_ALTURA) return true;
        if (gy >= 0 && tetrisGrid[gy][gx]) return true;
      }
    }
  }
  return false;
}

void fixarPecaTetris() {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (tetrisPeca[y][x] && tetrisPosY + y >= 0) {
        tetrisGrid[tetrisPosY + y][tetrisPosX + x] = 1;
      }
    }
  }
  
  for (int y = TETRIS_ALTURA - 1; y >= 0; y--) {
    bool completa = true;
    for (int x = 0; x < TETRIS_LARGURA; x++) {
      if (!tetrisGrid[y][x]) {
        completa = false;
        break;
      }
    }
    
    if (completa) {
      tetrisLinhas++;
      tetrisScore += 100;
      for (int yy = y; yy > 0; yy--)
        for (int x = 0; x < TETRIS_LARGURA; x++)
          tetrisGrid[yy][x] = tetrisGrid[yy-1][x];
      y++; 
    }
  }
  
  novaPecaTetris();
}

void rotacionarTetris() {
  int temp[4][4];
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      temp[y][x] = tetrisPeca[3-x][y];
  
  int pecaBackup[4][4];
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      pecaBackup[y][x] = tetrisPeca[y][x];
  
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      tetrisPeca[y][x] = temp[y][x];
  
  if (colideTetris(tetrisPosX, tetrisPosY)) {
    for (int y = 0; y < 4; y++)
      for (int x = 0; x < 4; x++)
        tetrisPeca[y][x] = pecaBackup[y][x];
  }
}

void rodarTetris() {
  unsigned long agora = millis();
  
  if (tetrisGameOver) {
    display.clearDisplay();
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    display.setCursor(38, 15); display.print("GAME OVER");
    display.setCursor(25, 28); display.print("Score: "); display.print(tetrisScore);
    display.setCursor(25, 38); display.print("Lines: "); display.print(tetrisLinhas);
    display.setCursor(15, 52); display.print("[Clique] Menu");
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      estadoAtual = MENU;
      ultimoInput = agora;
    }
    return;
  }
  
  int leituraY = analogRead(pinY);
  int leituraX = analogRead(pinX);
  
  if (leituraX < 500 && agora - ultimoInput > debounce) {
    if (!colideTetris(tetrisPosX - 1, tetrisPosY)) tetrisPosX--;
    ultimoInput = agora;
  }
  if (leituraX > 3500 && agora - ultimoInput > debounce) {
    if (!colideTetris(tetrisPosX + 1, tetrisPosY)) tetrisPosX++;
    ultimoInput = agora;
  }
  if (leituraY > 3500 && agora - ultimoInput > debounce) {
    if (!colideTetris(tetrisPosX, tetrisPosY + 1)) tetrisPosY++;
    ultimoInput = agora;
  }
  
  if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
    rotacionarTetris();
    ultimoInput = agora;
  }
  
  if (agora - tetrisUltimoMove > tetrisVelocidade) {
    tetrisUltimoMove = agora;
    if (!colideTetris(tetrisPosX, tetrisPosY + 1)) {
      tetrisPosY++;
    } else {
      fixarPecaTetris();
    }
  }
  
  display.clearDisplay();
  display.setCursor(75, 0); display.print("SC:"); display.print(tetrisScore);
  display.setCursor(75, 10); display.print("LN:"); display.print(tetrisLinhas);
  
  display.drawRect(TETRIS_OFFSET_X-1, TETRIS_OFFSET_Y-1, 
                   TETRIS_LARGURA*TETRIS_BLOCO+2, TETRIS_ALTURA*TETRIS_BLOCO+2, 
                   SSD1306_WHITE);
  
  for (int y = 0; y < TETRIS_ALTURA; y++) {
    for (int x = 0; x < TETRIS_LARGURA; x++) {
      if (tetrisGrid[y][x]) {
        display.fillRect(TETRIS_OFFSET_X + x*TETRIS_BLOCO, 
                         TETRIS_OFFSET_Y + y*TETRIS_BLOCO, 
                         TETRIS_BLOCO-1, TETRIS_BLOCO-1, SSD1306_WHITE);
      }
    }
  }
  
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (tetrisPeca[y][x] && tetrisPosY + y >= 0) {
        display.fillRect(TETRIS_OFFSET_X + (tetrisPosX+x)*TETRIS_BLOCO, 
                         TETRIS_OFFSET_Y + (tetrisPosY+y)*TETRIS_BLOCO, 
                         TETRIS_BLOCO-1, TETRIS_BLOCO-1, SSD1306_WHITE);
      }
    }
  }
}