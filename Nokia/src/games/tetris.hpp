#pragma once

// Funções que o menu principal (main) precisa acessar
void inicializarTetris();
void rodarTetris();

// Declaramos as funções internas aqui também para que o compilador se organize dentro do .cpp
void novaPecaTetris();
bool colideTetris(int px, int py);
void fixarPecaTetris();
void rotacionarTetris();