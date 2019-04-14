#include <stm32f10x.h>

static volatile uint16_t delay_counter;

void delay_ms(uint16_t delay);
void delay_us(uint16_t delay);
void delay_init();
