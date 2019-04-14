#include "delay.h"

void SysTick_Handler(){
	if (delay_counter > 0) delay_counter--;
}

void delay_ms(uint16_t delay){
	SysTick_Config(SystemCoreClock / 1000);
//	NVIC_SetPriority (SysTick_IRQn, 4);
	delay_counter = delay;
	while (delay_counter);
}

void delay_us(uint16_t delay){
	SysTick_Config(SystemCoreClock / 1000000);
//	NVIC_SetPriority (SysTick_IRQn, 4);
	delay_counter = delay;
	while (delay_counter);
}

void delay_init(){
	SysTick_Config(SystemCoreClock / 1000);
//	NVIC_SetPriority (SysTick_IRQn, 4);
	delay_counter = 0;
}
