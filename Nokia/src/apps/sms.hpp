#pragma once

// Struct essencial para gerenciar o buffer de texto das mensagens
struct SMS {
  char remetente[20];
  char mensagem[60];
  bool lida;
};

// Declaração do aplicativo de SMS
void rodarSMS();