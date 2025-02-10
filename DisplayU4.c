#include <math.h>
#include "hardware/i2c.h"
#include "include/ssd1306.h"
#include "pico/stdlib.h"
#include "include/font.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include <stdio.h>
#include "matriz_leds.pio.h"

// Definição de pinose portas
#define pino_leds 7
#define PORTA_I2C i2c1
#define PINO_SDA 14
#define PINO_SCL 15
#define endereco_disp 0x3C

// Definição de pinos para LEDs e botões
const uint luz_verde = 11;
const uint luz_azul = 12;
const uint tecla_x = 5;
const uint tecla_y = 6;

// Variáveis para controle de estados e temporização
static volatile uint32_t tempo_ant = 0;
static bool estado_v = false;
static bool estado_a = false;

#define qtd_pixels 25
ssd1306_t display; // Estrutura para manipulação do display
static PIO controlador_pio;
static uint maq_est;
static uint desloc;
static uint32_t dado_led;

//Prototipos de funções
void config_pinos();
static void tratador_irq(uint gpio, uint32_t events);
uint32_t gerar_cores(double b, double r, double g);
void mostrar_digito(int num);

//números da matriz de LEDs:
double numeros[10][25]={
    {0, 1, 1, 1, 0,   
     0, 1, 0, 1, 0,   
     0, 1, 0, 1, 0,   
     0, 1, 0, 1, 0,   
     0, 1, 1, 1, 0}, // 0

    {0, 0, 1, 0, 0,   
     0, 0, 1, 1, 0,   
     0, 0, 1, 0, 0,   
     0, 0, 1, 0, 0,   
     0, 1, 1, 1, 0}, // 1

    {0, 1, 1, 1, 0,   
     0, 1, 0, 0, 0,   
     0, 1, 1, 1, 0,   
     0, 0, 0, 1, 0,   
     0, 1, 1, 1, 0}, // 2

    {0, 1, 1, 1, 0,   
     0, 1, 0, 0, 0,   
     0, 1, 1, 1, 0,   
     0, 1, 0, 0, 0,   
     0, 1, 1, 1, 0}, // 3

    {0, 1, 0, 1, 0,   
     0, 1, 0, 1, 0,   
     0, 1, 1, 1, 0,   
     0, 1, 0, 0, 0,   
     0, 0, 0, 1, 0}, // 4

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0}, // 5

     
    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,  
     0, 1, 1, 1, 0,   
     0, 1, 0, 1, 0,   
     0, 1, 1, 1, 0}, // 6

    {0, 1, 1, 1, 0,
     0, 0, 0, 0, 0,
     0, 0, 1, 0, 0, 
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0}, // 7

    {0, 1, 1, 1, 0, 
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0, 
     0, 1, 0, 1, 0, 
     0, 1, 1, 1, 0}, // 8

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0}   // 9
};

// Configuração inicial dos pinos de entrada e saída
void config_pinos() {
    gpio_init(luz_verde);
    gpio_init(luz_azul);
    gpio_init(tecla_x);
    gpio_init(tecla_y);
    
    gpio_set_dir(luz_verde, GPIO_OUT);
    gpio_set_dir(luz_azul, GPIO_OUT);
    gpio_set_dir(tecla_x, GPIO_IN);
    gpio_set_dir(tecla_y, GPIO_IN);
    
    gpio_pull_up(tecla_x);
    gpio_pull_up(tecla_y);
    
    gpio_put(luz_verde, false);
    gpio_put(luz_azul, false);
}

// Tratador de interrupções dos botões
static void tratador_irq(uint gpio, uint32_t events) {
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());
    
    if (tempo_atual - tempo_ant > 200000) { // Debounce
        tempo_ant = tempo_atual;
        
        if (gpio_get(tecla_x) == 0) {
            estado_v = !estado_v;
            gpio_put(luz_verde, estado_v);
            printf("LED verde %s!", estado_v ? "ligado" : "desligado");
            ssd1306_fill(&display, false);
            ssd1306_draw_string(&display, estado_v ? "LED VERDE LIGADO" : "LED VERDE DESLIGADO", 0, 0);
            ssd1306_send_data(&display);
        } else if (gpio_get(tecla_y) == 0) {
            estado_a = !estado_a;
            gpio_put(luz_azul, estado_a);
            printf("LED azul %s!", estado_a ? "ligado" : "desligado");
            ssd1306_fill(&display, false);
            ssd1306_draw_string(&display, estado_a ? "LED AZUL LIGADO" : "LED AZUL DESLIGADO", 0, 0);
            ssd1306_send_data(&display);
        }
    }
}

// Geração de cores para LEDs usando codificação RGB
uint32_t gerar_cores(double b, double r, double g) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Exibição de um número na matriz de LEDs
void mostrar_digito(int num) {
    for (uint i = 0; i < qtd_pixels; i++) {
        dado_led = gerar_cores(0.0, numeros[num][24 - i], 0.0);
        pio_sm_put_blocking(controlador_pio, maq_est, dado_led);
    }
}

int main() {
    controlador_pio = pio0;
    bool resultado;

    // Configuração do clock do sistema
    resultado = set_sys_clock_khz(128000, false);
    printf("Iniciando a transmissao PIO:\n");
    if (resultado) {
        printf("Clock definido: %ld\n", clock_get_hz(clk_sys));
    }

    // Inicialização da PIO para controle da matriz de LEDs
    desloc = pio_add_program(controlador_pio, &matriz_leds_program);
    maq_est = pio_claim_unused_sm(controlador_pio, true);
    matriz_leds_program_init(controlador_pio, maq_est, desloc, pino_leds);

    // Inicialização do barramento I2C para comunicação com o display
    i2c_init(PORTA_I2C, 400 * 1000);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SDA);
    gpio_pull_up(PINO_SCL);

    // Configuração do display OLED
    ssd1306_init(&display, WIDTH, HEIGHT, false, endereco_disp, PORTA_I2C);
    ssd1306_config(&display);
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);

    stdio_init_all();
    config_pinos();

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(tecla_x, GPIO_IRQ_EDGE_FALL, true, &tratador_irq);
    gpio_set_irq_enabled_with_callback(tecla_y, GPIO_IRQ_EDGE_FALL, true, &tratador_irq);

    while (!stdio_usb_connected()); // Aguarda conexão USB antes de continuar

    while (true) {
        char entrada;
        scanf("%c", &entrada);

        ssd1306_fill(&display, false);

        // Se o caractere for um número, exibe na matriz de LEDs e no display
        if (entrada >= '0' && entrada <= '9') {
            int numero = entrada - '0';
            mostrar_digito(numero);
            ssd1306_draw_char(&display, entrada, 55, 25);
        } else {
            ssd1306_draw_char(&display, entrada, 55, 25);
        }

        ssd1306_send_data(&display);
    }
}
