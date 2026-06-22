#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "apps/contatos.hpp"
#include "apps/config.hpp"
#include "apps/alarme.hpp"
#include "apps/notas.hpp"
#include "apps/relogio.hpp"
#include "apps/sms.hpp"
#include "games/space.hpp"
#include "games/snake.hpp"
#include "games/tetris.hpp"

// =====================================
// PROTÓTIPOS DE APLICATIVOS EXTERNOS
// =====================================
void inicializarJogo();   // Snake
void inicializarTetris(); // Tetris
void inicializarSpace();  // Space Impact

// =====================================
// PROTÓTIPOS DAS FUNÇÕES LOCAIS
// =====================================
void atualizarMenu();
void desenharMenu();
void gerenciarLoading();
void rodarDesligar();

// =====================================
// OLED
// =====================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =====================================
// JOYSTICK
// =====================================
int pinX = 33;
int pinY = 32;
int pinSW = 27;

// =====================================
// MENU DE APLICATIVOS
// =====================================
const char* apps[] = {
  "Snake",
  "Tetris",
  "Space Impact",
  "SMS",
  "Contatos",
  "Relogio",
  "Alarme",
  "Notas",
  "Config",
  "Desligar"
};

const int TOTAL_APPS = 10; // Atualizado para 10 itens ativos
int itemSelecionado = 0;

// =====================================
// ESTADOS DO SISTEMA OPERACIONAL
// =====================================
enum {
  MENU = 0,
  APP_CONTATOS = 1,
  APP_CONFIG = 2,
  APP_ALARME = 3,
  JOGO_TETRIS = 4,
  APP_NOTAS = 5,
  JOGO_SNAKE = 6,
  JOGO_SPACE = 7,
  APP_RELOGIO = 8,
  APP_SMS = 9,
  LOADING,
  APP_DESLIGAR
};

int estadoAtual = MENU;

// =====================================
// CONTROLE DE TIMERS E DEBOUNCE
// =====================================
unsigned long ultimoInput = 0;
unsigned long debounce = 180;
unsigned long tempoInicioApp = 0;
bool appInicializado = false;
const unsigned long tempoCarregamento = 1500;

// =====================================
// SETUP
// =====================================
void setup() {
  Serial.begin(115200);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Falha OLED");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  
  randomSeed(analogRead(34));
}

// =====================================
// LOOP PRINCIPAL
// =====================================
void loop() {
  switch (estadoAtual) {
    case MENU:             atualizarMenu(); desenharMenu(); break;
    case LOADING:          gerenciarLoading(); break;
    case JOGO_SNAKE:       rodarJogoSnake(); break;
    case JOGO_TETRIS:      rodarTetris(); break;
    case JOGO_SPACE:       rodarSpaceImpact(); break;
    case APP_SMS:          rodarSMS(); break;
    case APP_CONTATOS:     rodarContatos(); break;
    case APP_RELOGIO:      rodarRelogio(); break;
    case APP_ALARME:       rodarAlarme(); break;
    case APP_NOTAS:        rodarNotas(); break;
    case APP_CONFIG:       rodarConfig(); break;
    case APP_DESLIGAR:     rodarDesligar(); break;
    default:               estadoAtual = MENU; break;
  }
  display.display();
}

// =====================================
// MENU
// =====================================
void atualizarMenu() {
  int leituraY = analogRead(pinY);
  unsigned long agora = millis();

  if (leituraY < 500 && agora - ultimoInput > debounce) {
    itemSelecionado--;
    if (itemSelecionado < 0) itemSelecionado = TOTAL_APPS - 1;
    ultimoInput = agora;
  }

  if (leituraY > 3500 && agora - ultimoInput > debounce) {
    itemSelecionado++;
    if (itemSelecionado >= TOTAL_APPS) itemSelecionado = 0;
    ultimoInput = agora;
  }

  if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
    ultimoInput = agora;
    appInicializado = false;
    estadoAtual = LOADING;
  }
}

void desenharMenu() {
  display.clearDisplay();
  
  display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(52, 1);
  display.print("MENU");
  display.setTextColor(SSD1306_WHITE);

  int inicioLista = itemSelecionado - 2;
  if (inicioLista < 0) inicioLista = 0;
  if (inicioLista > TOTAL_APPS - 5) inicioLista = TOTAL_APPS - 5;

  for (int i = 0; i < 5; i++) {
    int indice = inicioLista + i;
    if (indice >= TOTAL_APPS) break;
    int y = 14 + (i * 10);

    if (indice == itemSelecionado) {
      display.fillRect(0, y - 1, 122, 9, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(5, y);
    display.print(apps[indice]);
  }

  display.drawRect(123, 12, 4, 40, SSD1306_WHITE);
  int barraY = map(itemSelecionado, 0, TOTAL_APPS - 1, 13, 43);
  display.fillRect(124, barraY, 2, 8, SSD1306_WHITE);
}

// =====================================
// LOADING
// =====================================
void gerenciarLoading() {
  if (!appInicializado) {
    tempoInicioApp = millis();
    appInicializado = true;
  }

  unsigned long tempoDecorrido = millis() - tempoInicioApp;

  if (tempoDecorrido < tempoCarregamento) {
    display.clearDisplay();
    display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(12, 12);
    display.print("Abrindo...");
    
    display.drawRoundRect(10, 26, 108, 16, 3, SSD1306_WHITE);
    display.setCursor(16, 30);
    display.print(apps[itemSelecionado]);
    
    int larguraBarra = map(tempoDecorrido, 0, tempoCarregamento, 0, 104);
    display.fillRect(12, 45, larguraBarra, 4, SSD1306_WHITE);
  } else {
    appInicializado = false;
    
    if (strcmp(apps[itemSelecionado], "Snake") == 0) {
      inicializarJogo();
      estadoAtual = JOGO_SNAKE;
    } else if (strcmp(apps[itemSelecionado], "Tetris") == 0) {
      inicializarTetris();
      estadoAtual = JOGO_TETRIS;
    } else if (strcmp(apps[itemSelecionado], "Space Impact") == 0) {
      inicializarSpace();
      estadoAtual = JOGO_SPACE;
    } else if (strcmp(apps[itemSelecionado], "SMS") == 0) {
      estadoAtual = APP_SMS;
    } else if (strcmp(apps[itemSelecionado], "Contatos") == 0) {
      estadoAtual = APP_CONTATOS;
    } else if (strcmp(apps[itemSelecionado], "Relogio") == 0) {
      estadoAtual = APP_RELOGIO;
    } else if (strcmp(apps[itemSelecionado], "Alarme") == 0) {
      estadoAtual = APP_ALARME;
    } else if (strcmp(apps[itemSelecionado], "Notas") == 0) {
      estadoAtual = APP_NOTAS;
    } else if (strcmp(apps[itemSelecionado], "Config") == 0) {
      estadoAtual = APP_CONFIG;
    } else if (strcmp(apps[itemSelecionado], "Desligar") == 0) {
      estadoAtual = APP_DESLIGAR;
    } else {
      estadoAtual = MENU;
    }
  }
}

// =====================================
// DESLIGAR
// =====================================
void rodarDesligar() {
  display.clearDisplay();
  display.drawRoundRect(10, 10, 108, 44, 4, SSD1306_WHITE);
  
  display.setCursor(25, 20);
  display.print("Desligando...");
  
  display.setCursor(40, 35);
  display.print("Nokia");
  display.display();
  
  delay(2000);
  
  display.clearDisplay();
  display.display();
  
  while (true) {
    if (digitalRead(pinSW) == LOW) {
      delay(500);
      estadoAtual = MENU;
      return;
    }
    delay(100);
  }
}