#pragma once

// Estruturas e enums necessários para o funcionamento do jogo
struct Ponto {
  int x, y;
};

enum Direcao { PARADO, CIMA, BAIXO, ESQUERDA, DIREITA };

// Funções públicas que o main vai chamar
void inicializarJogo();
void rodarJogoSnake();

// Função interna de renderização (declarada aqui para organização do .cpp)
void desenharTelaJogo();