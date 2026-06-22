#pragma once

// Struct essencial para que o compilador reconheça os dados no .cpp
struct Nota {
  char titulo[20];
  char conteudo[80];
};

// Declaração do app de notas
void rodarNotas();