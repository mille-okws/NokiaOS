#include "sms.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// 1. IMPORTANDO AS VARIÁVEIS GLOBAIS DE HARDWARE E SISTEMA
extern Adafruit_SSD1306 display;
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

extern int estadoAtual;
#define MENU 0 

// 2. VARIÁVEIS DE ESTADO E BANCO DE DADOS LOCAL DE SMS
SMS mensagens[5] = {
  {"Mae", "Vem jantar, a comida ta pronta!", false},
  {"Operadora", "Seu saldo de recarga esta expirando.", true},
  {"Contatinho", "Ei, vai fazer o que hoje a noite?", false},
  {"Chefe", "Preciso do relatorio na minha mesa ja.", true},
  {"Banco", "Token de seguranca: 482910. Nao compartilhe.", false}
};

int smsSelecionada = 0;
bool smsVisualizando = false;

// 3. EXECUÇÃO DO APLICATIVO
void rodarSMS() {
  unsigned long agora = millis();
  
  if (smsVisualizando) {
    display.clearDisplay();
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(5, 1);
    display.print(mensagens[smsSelecionada].remetente);
    display.setTextColor(SSD1306_WHITE);
    
    int linhaY = 15;
    int charsPorLinha = 21;
    char* msg = mensagens[smsSelecionada].mensagem;
    int len = strlen(msg);
    
    for (int i = 0; i < len; i += charsPorLinha) {
      char linha[22];
      strncpy(linha, msg + i, charsPorLinha);
      linha[min(charsPorLinha, len - i)] = '\0';
      display.setCursor(3, linhaY);
      display.print(linha);
      linhaY += 10;
    }
    
    display.setCursor(5, 55); display.print("[Clique] Voltar");
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      mensagens[smsSelecionada].lida = true; // Marca como lida ao sair da visualização
      smsVisualizando = false;
      ultimoInput = agora;
    }
  } else {
    int leituraY = analogRead(pinY);
    
    if (leituraY < 500 && agora - ultimoInput > debounce) {
      smsSelecionada--;
      if (smsSelecionada < 0) smsSelecionada = 4;
      ultimoInput = agora;
    }
    if (leituraY > 3500 && agora - ultimoInput > debounce) {
      smsSelecionada++;
      if (smsSelecionada > 4) smsSelecionada = 0;
      ultimoInput = agora;
    }
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      smsVisualizando = true;
      ultimoInput = agora;
    }
    
    int leituraX = analogRead(pinX);
    if (leituraX < 500 && agora - ultimoInput > 300) {
      estadoAtual = MENU;
      ultimoInput = agora;
    }
    
    display.clearDisplay();
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(40, 1); display.print("MENSAGENS");
    display.setTextColor(SSD1306_WHITE);
    
    for (int i = 0; i < 5; i++) {
      int y = 14 + i*10;
      if (i == smsSelecionada) {
        display.fillRect(0, y-1, 128, 9, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      
      display.setCursor(3, y);
      if (!mensagens[i].lida) display.print("*");
      else display.print(" ");
      display.print(mensagens[i].remetente);
    }
    display.setTextColor(SSD1306_WHITE);
  }
}