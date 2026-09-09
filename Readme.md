# 📱 Nokia ESP32

Um projeto de **recriação experimental de um sistema Nokia clássico utilizando ESP32**, OLED e joystick.

A ideia é reproduzir a experiência de um celular simples: uma interface baseada em menus, aplicativos, jogos e pequenas funções utilitárias, tudo executado localmente em um microcontrolador.

O projeto foi desenvolvido como uma exploração de **embedded systems, interfaces homem-máquina e arquitetura modular de firmware**.

---

## 🖥️ Visão geral

O sistema possui uma interface gráfica monocromática exibida em um display OLED de `128×64` pixels.

A navegação é realizada por um joystick analógico com botão integrado.

O sistema possui:

* 🎮 Snake
* 🧱 Tetris
* 🚀 Space Impact
* 💬 SMS
* 👥 Contatos
* 🕐 Relógio
* ⏰ Alarme
* 📝 Notas
* ⚙️ Configurações
* 🔌 Desligamento do sistema

A interface possui também uma tela de carregamento entre o menu e cada aplicação, simulando o comportamento de um sistema operacional embarcado.

---

## 🔧 Hardware

### Componentes principais

| Componente       | Especificação      |
| ---------------- | ------------------ |
| Microcontrolador | ESP32              |
| Display          | OLED SSD1306       |
| Resolução        | 128 × 64 pixels    |
| Interface OLED   | I²C                |
| Endereço I²C     | `0x3C`             |
| Controle         | Joystick analógico |
| Comunicação      | I²C / GPIO / ADC   |

### Pinagem

#### OLED

O display utiliza comunicação I²C:

| OLED | ESP32   |
| ---- | ------- |
| SDA  | GPIO 21 |
| SCL  | GPIO 22 |
| VCC  | 3.3 V   |
| GND  | GND     |

#### Joystick

| Função     |    GPIO |
| ---------- | ------: |
| Eixo X     | GPIO 33 |
| Eixo Y     | GPIO 32 |
| Botão (SW) | GPIO 27 |

O botão do joystick utiliza `INPUT_PULLUP`.

---

## 🧠 Arquitetura do firmware

O firmware utiliza uma arquitetura baseada em **máquina de estados finita (FSM)**.

Cada aplicação possui um estado próprio, e o `loop()` principal determina qual módulo deve ser executado.

```text
                    ┌─────────────┐
                    │    MENU     │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │   LOADING   │
                    └──────┬──────┘
                           │
          ┌────────────────┼────────────────┐
          │                │                │
          ▼                ▼                ▼
       ┌───────┐       ┌─────────┐      ┌────────┐
       │ Jogos │       │  Apps   │      │Config  │
       └───────┘       └─────────┘      └────────┘
          │                │                │
          └────────────────┼────────────────┘
                           ▼
                         MENU
```

Os estados principais são:

```cpp
MENU
APP_CONTATOS
APP_CONFIG
APP_ALARME
JOGO_TETRIS
APP_NOTAS
JOGO_SNAKE
JOGO_SPACE
APP_RELOGIO
APP_SMS
LOADING
APP_DESLIGAR
```

Essa abordagem evita concentrar toda a lógica das aplicações em um único arquivo e permite adicionar novos aplicativos ao sistema com maior facilidade.

---

## 📂 Estrutura do projeto

```text
Nokia-ESP32/
│
├── apps/
│   ├── contatos.hpp
│   ├── config.hpp
│   ├── alarme.hpp
│   ├── notas.hpp
│   ├── relogio.hpp
│   └── sms.hpp
│
├── games/
│   ├── space.hpp
│   ├── snake.hpp
│   └── tetris.hpp
│
└── main.ino
```

A separação entre `apps/` e `games/` permite manter cada funcionalidade independente.

---

## 🎮 Interface

O menu apresenta até cinco opções simultaneamente na tela.

O item selecionado é destacado através de uma inversão de cores:

```text
┌──────────────────────────────┐
│              MENU            │
├──────────────────────────────┤
│ Snake                        │
│ Tetris                       │
│ > Space Impact <             │
│ SMS                          │
│ Contatos                     │
│                         ▓    │
└──────────────────────────────┘
```

Uma barra lateral indica a posição atual dentro da lista de aplicativos.

### Controles

* **Joystick para cima:** item anterior
* **Joystick para baixo:** próximo item
* **Botão:** abrir aplicação
* **Aplicações:** utilizam o mesmo joystick para interação

O firmware possui um mecanismo de **debounce baseado em `millis()`**, evitando múltiplos comandos causados por uma única interação física com o joystick.

---

## ⏳ Sistema de carregamento

Antes de iniciar uma aplicação, o sistema apresenta uma tela de carregamento de aproximadamente `1,5 s`.

```text
┌──────────────────────────────┐
│                              │
│        Abrindo...            │
│      ┌──────────────┐        │
│      │    Snake     │        │
│      └──────────────┘        │
│      ███████████░░░          │
│                              │
└──────────────────────────────┘
```

Além de reproduzir visualmente a experiência de um celular clássico, essa etapa cria uma transição clara entre os estados do sistema.

---

## 📦 Bibliotecas

O projeto utiliza principalmente:

### Adafruit SSD1306

Driver para o display OLED SSD1306.

### Adafruit GFX

Biblioteca gráfica utilizada para desenhar texto, retângulos e outros elementos na tela.

### Wire

Biblioteca utilizada para comunicação I²C.

Instalação pelo Arduino IDE:

```text
Adafruit SSD1306
Adafruit GFX Library
```

---

## ⚙️ Funcionamento

O `setup()` inicializa:

1. Comunicação serial
2. GPIOs
3. OLED
4. Gerador de números aleatórios

O `loop()` funciona como o núcleo do sistema:

```cpp
switch (estadoAtual) {

    case MENU:
        atualizarMenu();
        desenharMenu();
        break;

    case LOADING:
        gerenciarLoading();
        break;

    case JOGO_SNAKE:
        rodarJogoSnake();
        break;

    case JOGO_TETRIS:
        rodarTetris();
        break;

    case JOGO_SPACE:
        rodarSpaceImpact();
        break;

    ...
}
```

Dessa forma, somente a aplicação correspondente ao estado atual é executada.

---

## 🔌 Inicialização de aplicativos

Os aplicativos são inicializados de maneira independente.

Por exemplo:

```cpp
if (strcmp(apps[itemSelecionado], "Snake") == 0) {
    inicializarJogo();
    estadoAtual = JOGO_SNAKE;
}
```

Enquanto a execução do jogo ocorre através de:

```cpp
case JOGO_SNAKE:
    rodarJogoSnake();
    break;
```

Isso separa a etapa de **inicialização** da etapa de **execução**, permitindo que cada aplicação mantenha sua própria lógica.

---

## 🔌 Modo de desligamento

O sistema possui um estado específico para desligamento.

Ao selecionar `Desligar`, o dispositivo apresenta:

```text
┌──────────────────────────────┐
│                              │
│       Desligando...          │
│                              │
│            Nokia             │
│                              │
└──────────────────────────────┘
```

Após a animação, o sistema entra em um estado de espera até que o botão seja pressionado novamente.

---

## 🚧 Status

| Funcionalidade | Status |
| -------------- | ------ |
| OLED           | ✅      |
| Menu           | ✅      |
| Joystick       | ✅      |
| Navegação      | ✅      |
| Loading screen | ✅      |
| Snake          | ✅      |
| Tetris         | ✅      |
| Space Impact   | ✅      |
| SMS            | 🚧     |
| Contatos       | 🚧     |
| Relógio        | 🚧     |
| Alarme         | 🚧     |
| Notas          | 🚧     |
| Configurações  | 🚧     |
| Desligamento   | ✅      |

---

## 🚀 Possíveis melhorias

Algumas funcionalidades que podem ser adicionadas futuramente:

* [ ] Persistência de dados na memória flash
* [ ] Sistema de arquivos
* [ ] RTC para relógio real
* [ ] Vibração através de motor háptico
* [ ] Sons através de buzzer
* [ ] Gerenciamento de bateria
* [ ] Tela inicial personalizada
* [ ] Sistema de temas
* [ ] Mais jogos
* [ ] Sistema de pontuação persistente
* [ ] Animações de transição
* [ ] Menu de configurações funcional
* [ ] Comunicação Bluetooth
* [ ] Comunicação Wi-Fi
* [ ] Sistema de mensagens entre dispositivos
* [ ] Sleep mode para economia de energia

---

## 🎯 Objetivo do projeto

Mais do que reproduzir um celular antigo, o projeto serve como um laboratório para experimentar:

* Firmware embarcado
* Máquinas de estados
* Interfaces homem-máquina
* Displays gráficos
* Leitura de entradas analógicas
* Debounce de botões
* Arquitetura modular
* Desenvolvimento de jogos em microcontroladores
* Gerenciamento de recursos limitados

O projeto transforma um ESP32 em um pequeno **sistema embarcado multifuncional**, inspirado na simplicidade dos celulares Nokia clássicos.

---

## 🛠️ Tecnologias

```text
C++
ESP32
Arduino
I²C
GPIO
ADC
OLED
SSD1306
Embedded Systems
Finite State Machine
```

---

## 📜 Licença

Este projeto é experimental e foi desenvolvido para fins de estudo, prototipagem e exploração de sistemas embarcados.
