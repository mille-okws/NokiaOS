#include "alarme.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Garanta que o 'const' e o 'unsigned long' combinem perfeitamente com o main:
extern Adafruit_SSD1306 display;
extern const int pinX;
extern const int pinY;
extern const int pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce; 
extern int estadoAtual;
#define MENU 0 

// 2. DADOS E ESTADOS INTERNOS DO MÓDULO ALARME
Alarme alarmes[3] = {
  {7, 0, true, "Acordar"},
  {12, 30, false, "Almoco"},
  {22, 0, false, "Dormir"}
};

int alarmeSelecionado = 0;
bool alarmeEditando = false;

// 3. A FUNÇÃO COMPLETA ISOLADA
void rodarAlarme() {
  unsigned long agora = millis();
  
  if (!alarmeEditando) {
    int leituraY = analogRead(pinY);
    
    if (leituraY < 500 && agora - ultimoInput > debounce) {
      alarmeSelecionado--;
      if (alarmeSelecionado < 0) alarmeSelecionado = 2;
      ultimoInput = agora;
    }
    if (leituraY > 3500 && agora - ultimoInput > debounce) {
      alarmeSelecionado++;
      if (alarmeSelecionado > 2) alarmeSelecionado = 0;
      ultimoInput = agora;
    }
    
    int leituraX = analogRead(pinX);
    if (leituraX > 3500 && agora - ultimoInput > debounce) {
      alarmes[alarmeSelecionado].ativo = !alarmes[alarmeSelecionado].ativo;
      ultimoInput = agora;
    }
    
    if (leituraX < 500 && agora - ultimoInput > 300) {
      estadoAtual = MENU;
      ultimoInput = agora;
    }
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      alarmeEditando = true;
      ultimoInput = agora;
    }
  } else {
    int leituraY = analogRead(pinY);
    
    if (leituraY < 500 && agora - ultimoInput > debounce) {
      alarmes[alarmeSelecionado].hora++;
      if (alarmes[alarmeSelecionado].hora >= 24) alarmes[alarmeSelecionado].hora = 0;
      ultimoInput = agora;
    }
    if (leituraY > 3500 && agora - ultimoInput > debounce) {
      alarmes[alarmeSelecionado].hora--;
      if (alarmes[alarmeSelecionado].hora < 0) alarmes[alarmeSelecionado].hora = 23;
      ultimoInput = agora;
    }
    
    int leituraX = analogRead(pinX);
    if (leituraX < 500 && agora - ultimoInput > debounce) {
      alarmes[alarmeSelecionado].minuto--;
      if (alarmes[alarmeSelecionado].minuto < 0) alarmes[alarmeSelecionado].minuto = 59;
      ultimoInput = agora;
    }
    if (leituraX > 3500 && agora - ultimoInput > debounce) {
      alarmes[alarmeSelecionado].minuto++;
      if (alarmes[alarmeSelecionado].minuto >= 60) alarmes[alarmeSelecionado].minuto = 0;
      ultimoInput = agora;
    }
    
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      alarmeEditando = false;
      ultimoInput = agora;
    }
  }
  
  display.clearDisplay();
  display.fillRect(0, 0, 128, 11, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(42, 2);
  display.print("ALARMES");
  display.setTextColor(SSD1306_WHITE);
  
  for (int i = 0; i < 3; i++) {
    int y = 15 + i*15;
    
    if (i == alarmeSelecionado) {
      display.drawRect(2, y-2, 124, 14, SSD1306_WHITE);
    }
    
    display.setCursor(6, y);
    if (alarmes[i].hora < 10) display.print("0");
    display.print(alarmes[i].hora);
    display.print(":");
    if (alarmes[i].minuto < 10) display.print("0");
    display.print(alarmes[i].minuto);
    
    display.setCursor(50, y);
    display.print(alarmes[i].nome);
    
    display.setCursor(100, y);
    display.print(alarmes[i].ativo ? "[ON]" : "[--]");
  }
  
  display.setCursor(5, 56);
  if (alarmeEditando) {
    display.print("[Clique] Salvar");
  } else {
    display.print("[<>] On/Off [v] Edit");
  }
}