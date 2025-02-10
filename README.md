# Projeto: Controle de LEDs e Display OLED com Raspberry Pi Pico

## Descrição
Este projeto implementa o controle de uma matriz de LEDs WS2812 e um display OLED SSD1306 utilizando a placa Raspberry Pi Pico. O sistema também permite o acionamento de LEDs individuais e a exibição de caracteres na matriz de LEDs e no display OLED. O projeto inclui o uso de interrupções para botões físicos, debounce via software e comunicação I2C.

## Funcionalidades
- Controle de LEDs WS2812 via PIO (Programmable I/O)
- Exibição de caracteres e números na matriz de LEDs
- Controle de LEDs individuais (verde e azul) acionados por botões
- Exibição de mensagens no display OLED SSD1306
- Tratamento de interrupções para acionamento dos botões
- Debounce via software para evitar acionamentos falsos
- Interface USB para comunicação com o computador

## Componentes Utilizados
- **Raspberry Pi Pico**
- **Matriz de LEDs WS2812**
- **Display OLED SSD1306 (I2C)**
- **Botões de entrada**
- **LEDs indicadores (verde e azul)**
- **Resistores pull-up para os botões**

## Conexões
| Componente         | Pino do Raspberry Pi Pico |
|--------------------|--------------------------|
| Matriz WS2812     | GPIO 7                    |
| Display OLED SDA  | GPIO 14                   |
| Display OLED SCL  | GPIO 15                   |
| LED Verde         | GPIO 11                   |
| LED Azul          | GPIO 12                   |
| Botão X          | GPIO 5                    |
| Botão Y          | GPIO 6                    |

## Instalação e Execução
### Requisitos
- Raspberry Pi Pico com firmware do SDK do RP2040 instalado
- Compilador C/C++ para ARM (como o `arm-none-eabi-gcc`)
- Biblioteca Pico SDK configurada
- Biblioteca para o display OLED SSD1306
- Biblioteca PIO para controle dos LEDs WS2812

### Compilação e Upload do Código
1. Clone este repositório:
   ```bash
   git clone https://github.com/Neto-Jaime/Tarefa_interrupcao.git
   cd Tarefa_interrupcao
   ```
2. Configure o ambiente do Raspberry Pi Pico (caso ainda não tenha feito):
   ```bash
   export PICO_SDK_PATH=/caminho/para/pico-sdk
   ```
3. Crie um diretório de build e compile o código:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Conecte o Raspberry Pi Pico em modo de bootloader (pressionando `BOOTSEL` ao conectar via USB).
5. Copie o arquivo `.uf2` gerado para a unidade do Pico.

## Uso
- **Acionamento dos LEDs:** Pressione os botões `X` e `Y` para ligar/desligar os LEDs verde e azul, respectivamente.
- **Exibição de números na matriz de LEDs:** Digite um número pelo terminal serial USB para exibi-lo na matriz de LEDs.
- **Mensagens no display OLED:** O estado dos LEDs (ligado/desligado) é mostrado no display OLED automaticamente.

## Autor
Projeto desenvolvido por **Jaime Neto**


