#include "notas.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// 1. IMPORTANDO AS VARIÁVEIS GLOBAIS DE HARDWARE
extern Adafruit_SSD1306 display;
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

extern int estadoAtual;
#define MENU 0 

// 2. DADOS E ESTADOS INTERNOS DO BLOCO DE NOTAS
Nota notas[3] = {
  {"Lista Compras", "Leite, Pao, Cafe, Manteiga"},
  {"Senhas", "WiFi: nokia2025"},
  {"Ideias", "Criar jogo retro"}
};

int notaSelecionada = 0;
bool notaVisualizando = false;

// 3. A FUNÇÃO COMPLETA ISOLADA
void rodarNotas() {
  unsigned long agora = millis();
  
  if (notaVisualizando) {
    display.clearDisplay();
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(5, 1);
    display.print(notas[notaSelecionada].titulo);
    display.setTextColor(SSD1306_WHITE);
    
    int linhaY = 15;
    int charsPorLinha = 21;
    char* txt = notas[notaSelecionada].conteudo;
    int len = strlen(txt);
    
    for (int i = 0; i < len; i += charsPorLinha) {
      if (linhaY > 50) break;
      char linha[22];
      strncpy(linha, txt + i, charsPorLinha);
      linha[min(charsPorLinha, len - i)] = '\0';
      display.setCursor(3, linhaY);
      display.print(linha);
      linhaY += 9;
    }
    
    display.setCursor(20, 56);
    display.print("[Clique] Voltar");
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      notaVisualizando = false;
      ultimoInput = agora;
    }
  } else {
    int leituraY = analogRead(pinY);
    
    if (leituraY < 500 && agora - ultimoInput > debounce) {
      notaSelecionada--;
      if (notaSelecionada < 0) notaSelecionada = 2;
      ultimoInput = agora;
    }
    if (leituraY > 3500 && agora - ultimoInput > debounce) {
      notaSelecionada++;
      if (notaSelecionada > 2) notaSelecionada = 0;
      ultimoInput = agora;
    }
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      notaVisualizando = true;
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
    display.setCursor(45, 2);
    display.print("NOTAS");
    display.setTextColor(SSD1306_WHITE);
    
    for (int i = 0; i < 3; i++) {
      int y = 18 + i*14;
      
      if (i == notaSelecionada) {
        display.fillRect(0, y-2, 128, 12, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      
      display.setCursor(5, y);
      display.print("> ");
      display.print(notas[i].titulo);
    }
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(20, 56);
    display.print("[Clique] Abrir");
  }
}