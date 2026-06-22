#pragma once

// A struct precisa estar aqui para o compilador conhecer o tipo do dado
struct Alarme {
  int hora;
  int minuto;
  bool ativo;
  char nome[15];
};

// Declaração da função principal do módulo
void rodarAlarme();