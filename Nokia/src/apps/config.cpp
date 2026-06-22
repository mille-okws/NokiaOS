#include "config.hpp"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// 1. IMPORTANDO AS VARIÁVEIS GLOBAIS DO HARDWARE
extern Adafruit_SSD1306 display;
extern const int pinX, pinY, pinSW;
extern unsigned long ultimoInput;
extern const unsigned long debounce;

// Máquina de estados do menu principal
extern int estadoAtual;
#define MENU 0 // Defina o valor correspondente ao seu MENU principal

// 2. IMPORTANDO OS ITENS DE CONFIGURAÇÃO (Definidos no main ou criados aqui)
// Se você já tiver essa lista no main, use o extern assim:
extern const char* configItens[6]; 

/* Dica: Se preferir tirar essa lista do main e deixar ela trancada aqui dentro, 
basta remover o 'extern' da linha de cima e declarar o array direto:
const char* configItens[6] = {"Brilho", "Som", "Bluetooth", "Sobre", "Resetar", "Sair"};
*/
// =====================================
// CONFIG
// =====================================
const char* configItens[] = {
  "Brilho: Alto",
  "Som: Ligado",
  "Vibrar: Deslig",
  "Tema: Classic",
  "Idioma: PT-BR",
  "Sobre"
};
// Variável interna de controle do menu de config
int configSelecionado = 0;

// 3. SUA FUNÇÃO DE CONFIGURAÇÃO ISOLADA
void rodarConfig() {
  unsigned long agora = millis();
  
  int leituraY = analogRead(pinY);
  
  if (leituraY < 500 && agora - ultimoInput > debounce) {
    configSelecionado--;
    if (configSelecionado < 0) configSelecionado = 5;
    ultimoInput = agora;
  }
  if (leituraY > 3500 && agora - ultimoInput > debounce) {
    configSelecionado++;
    if (configSelecionado > 5) configSelecionado = 0;
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
  display.setCursor(30, 2);
  display.print("CONFIGURACOES");
  display.setTextColor(SSD1306_WHITE);
  
  int inicio = configSelecionado - 2;
  if (inicio < 0) inicio = 0;
  if (inicio > 1) inicio = 1;
  
  for (int i = 0; i < 5; i++) {
    int idx = inicio + i;
    if (idx > 5) break;
    
    int y = 15 + i*9;
    
    if (idx == configSelecionado) {
      display.fillRect(0, y-1, 128, 9, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    
    display.setCursor(5, y);
    display.print(configItens[idx]);
  }
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(10, 56);
  display.print("v0.1 - Nokia Clone");
}