#pragma once

// Estrutura do contato precisa estar no cabeçalho 
// para que outros arquivos saibam como ela é
struct Contato {
  char nome[20];
  char numero[16];
};

// Declaração da função que o arquivo principal vai chamar
void rodarContatos();