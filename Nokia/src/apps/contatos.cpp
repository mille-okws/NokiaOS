#include "contatos.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h> // Se certifique de incluir a biblioteca do display aqui

// 1. DIZENDO AO COMPILADOR QUE ESSAS VARIÁVEIS EXISTEM NO ARQUIVO PRINCIPAL
extern Adafruit_SSD1306 display; 
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

// Se o 'estadoAtual' e 'MENU' forem globais do menu principal:
extern int estadoAtual; 
#define MENU 0 // Ou inclua o cabeçalho onde MENU está definido

// 2. OS DADOS DOS CONTATOS (Ficam escondidos aqui dentro agora)
Contato contatos[8] = {
  {"Mae Coruja", "(62) 99999-0001"},
  {"Pai", "(62) 99999-0002"},
  {"Amigo", "(62) 98888-0003"},
  {"Trabalho", "(62) 3333-4444"},
  {"Pizza", "(62) 3030-3030"},
  {"Policia", "190"},
  {"Bombeiros", "193"},
  {"SAMU", "192"}
};

int contatoSelecionado = 0;
bool contatoVisualizando = false;

// 3. A SUA FUNÇÃO INTEGRAL
void rodarContatos() {
  unsigned long agora = millis();
  
  if (contatoVisualizando) {
    display.clearDisplay();
    display.fillRect(0, 0, 128, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(5, 2);
    display.print(contatos[contatoSelecionado].nome);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(15, 28);
    display.print(contatos[contatoSelecionado].numero);
    
    display.drawLine(0, 50, 128, 50, SSD1306_WHITE);
    display.setCursor(20, 54);
    display.print("[Clique] Voltar");
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      contatoVisualizando = false;
      ultimoInput = agora;
    }
  } else {
    int leituraY = analogRead(pinY);
    
    if (leituraY < 500 && agora - ultimoInput > debounce) {
      contatoSelecionado--;
      if (contatoSelecionado < 0) contatoSelecionado = 7;
      ultimoInput = agora;
    }
    if (leituraY > 3500 && agora - ultimoInput > debounce) {
      contatoSelecionado++;
      if (contatoSelecionado > 7) contatoSelecionado = 0;
      ultimoInput = agora;
    }
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      contatoVisualizando = true;
      ultimoInput = agora;
    }
    
    int leituraX = analogRead(pinX);
    if (leituraX < 500 && agora - ultimoInput > 300) {
      estadoAtual = MENU;
      ultimoInput = agora;
    }
    
    display.clearDisplay();
    display.fillRect(0, 0, 128, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(16, 2);
    display.print("..:: CONTATOS ::..");
    display.setTextColor(SSD1306_WHITE);
    
    int inicio = contatoSelecionado - 2;
    if (inicio < 0) inicio = 0;
    if (inicio > 3) inicio = 3;
    
    for (int i = 0; i < 5; i++) {
      int idx = inicio + i;
      if (idx > 7) break;
      
      int y = 15 + i*9;
      if (idx == contatoSelecionado) {
        display.fillRect(0, y-1, 128, 9, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      
      display.setCursor(5, y);
      display.print(contatos[idx].nome);
    }
    display.setTextColor(SSD1306_WHITE);
  }
}