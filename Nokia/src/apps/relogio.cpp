#include "relogio.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// 1. IMPORTANDO AS VARIÁVEIS GLOBAIS DE HARDWARE E SISTEMA
extern Adafruit_SSD1306 display;
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

extern int estadoAtual;
#define MENU 0 

// 2. VARIÁVEIS DE ESTADO INTERNAS DO RELÓGIO
int relogioHora = 14;
int relogioMinuto = 30;
int relogioSegundo = 0;
unsigned long relogioUltimoTick = 0;
bool relogioEditando = false;
int relogioCampoEditando = 0; // 0=hora, 1=minuto

// 3. LOGICA DO APLICATIVO
void rodarRelogio() {
  unsigned long agora = millis();
  
  // Incremento do relógio (apenas quando não estamos editando)
  if (agora - relogioUltimoTick > 1000 && !relogioEditando) {
    relogioUltimoTick = agora;
    relogioSegundo++;
    if (relogioSegundo >= 60) {
      relogioSegundo = 0;
      relogioMinuto++;
      if (relogioMinuto >= 60) {
        relogioMinuto = 0;
        relogioHora++;
        if (relogioHora >= 24) relogioHora = 0;
      }
    }
  }
  
  int leituraX = analogRead(pinX);
  int lecturaY = analogRead(pinY);
  
  if (!relogioEditando) {
    // Entrar no modo de edição
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      relogioEditando = true;
      relogioCampoEditando = 0;
      ultimoInput = agora;
    }
    
    // Voltar para o menu principal
    if (leituraX < 500 && agora - ultimoInput > 300) {
      estadoAtual = MENU;
      ultimoInput = agora;
    }
  } else {
    // Alternar entre campos (Hora/Minuto)
    if (leituraX < 500 && agora - ultimoInput > debounce) {
      relogioCampoEditando = 1 - relogioCampoEditando;
      ultimoInput = agora;
    }
    if (leituraX > 3500 && agora - ultimoInput > debounce) {
      relogioCampoEditando = 1 - relogioCampoEditando;
      ultimoInput = agora;
    }
    
    // Incrementar valores
    if (lecturaY < 500 && agora - ultimoInput > debounce) {
      if (relogioCampoEditando == 0) {
        relogioHora++;
        if (relogioHora >= 24) relogioHora = 0;
      } else {
        relogioMinuto++;
        if (relogioMinuto >= 60) relogioMinuto = 0;
      }
      ultimoInput = agora;
    }
    
    // Decrementar valores
    if (lecturaY > 3500 && agora - ultimoInput > debounce) {
      if (relogioCampoEditando == 0) {
        relogioHora--;
        if (relogioHora < 0) relogioHora = 23;
      } else {
        relogioMinuto--;
        if (relogioMinuto < 0) relogioMinuto = 59;
      }
      ultimoInput = agora;
    }
    
    // Salvar e sair do modo edição
    if (digitalRead(pinSW) == LOW && agora - ultimoInput > debounce) {
      relogioEditando = false;
      relogioSegundo = 0;
      ultimoInput = agora;
    }
  }
  
  // 4. RENDERIZAÇÃO DA INTERFACE GRÁFICA
  display.clearDisplay();
  display.fillRect(0, 0, 128, 11, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(40, 2);
  display.print("RELOGIO");
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(3);
  int cursorX = 15;
  int cursorY = 25;
  
  // Piscar Hora em modo inversão
  if (relogioEditando && relogioCampoEditando == 0 && (millis() / 500) % 2) {
    display.setTextColor(SSD1306_BLACK);
    display.fillRect(cursorX-2, cursorY-2, 38, 26, SSD1306_WHITE);
  }
  display.setCursor(cursorX, cursorY);
  if (relogioHora < 10) display.print("0");
  display.print(relogioHora);
  display.setTextColor(SSD1306_WHITE);
  
  // Separador de dois pontos
  display.setCursor(cursorX + 40, cursorY);
  display.print(":");
  
  // Piscar Minuto em modo inversão
  if (relogioEditando && relogioCampoEditando == 1 && (millis() / 500) % 2) {
    display.setTextColor(SSD1306_BLACK);
    display.fillRect(cursorX + 56, cursorY-2, 38, 26, SSD1306_WHITE);
  }
  display.setCursor(cursorX + 58, cursorY);
  if (relogioMinuto < 10) display.print("0");
  display.print(relogioMinuto);
  display.setTextColor(SSD1306_WHITE);
  
  // Segundos menores no canto superior direito do relógio
  display.setTextSize(1);
  display.setCursor(cursorX + 98, cursorY + 10);
  if (relogioSegundo < 10) display.print("0");
  display.print(relogioSegundo);
  
  // Rodapé dinâmico de ação
  display.setCursor(10, 55);
  if (relogioEditando) {
    display.print("[Clique] Salvar");
  } else {
    display.print("[Clique] Ajustar");
  }
}