#include <stm32f10x.h>
#include "math.h"
#include "wg12864a.h"

#define PERIOD_N	25
#define SIZE_BUF	(PERIOD_N * 20)

typedef struct {
	unsigned sample_ready : 1;
} _FLAG;

uint32_t adc_buf[SIZE_BUF];
float mas_U[SIZE_BUF], mas_I[SIZE_BUF], rms_U, rms_I, S, P, cosfi, rash, ufn;
_FLAG flag;
uint16_t time_rashod, time_rashod_buf;
uint8_t antidrebezg;

void DMA1_Channel1_IRQHandler(){
	DMA1 -> IFCR |= DMA_IFCR_CTCIF1;
	TIM3 -> CR1 &= ~TIM_CR1_CEN;
	flag.sample_ready = 1;
}

void Get_I_U(uint32_t *buf, float *U_buf, float *I_buf){
	uint16_t i;
	float temp;
	for (i = 0; i < SIZE_BUF; i++){
		temp = (3.3 * (float)((int16_t)(buf[i] & 0x00000FFF) - 2044)) / 4096.0;				//U_adc
		U_buf[i] =  (353 * temp) / 1.41;													//Uo
		temp = (3.3 * (float)((int16_t)((buf[i] & 0xFFFF0000) >> 16) - 2044)) / 4096.0;		//U_adc
		I_buf[i] = (5 * temp) / 1.65;														//Io
	}
}

float Calc_P(float *U_buf, float *I_buf){
	uint16_t i;
	float res = 0;
	for (i = 0; i < SIZE_BUF; i++){
		res += (U_buf[i] * I_buf[i]);
	}
	return (res / SIZE_BUF);
}

float Calc_RMS(float *in_buf){
	uint16_t i;
	float rms = 0;
	for (i = 0; i < SIZE_BUF; i++){
		rms += (in_buf[i] * in_buf[i]);
	}
	return (sqrt(rms / SIZE_BUF));
}

void EXTI2_IRQHandler(){
	float time;
	time = time_rashod + ((TIM2 -> CNT - time_rashod_buf) / 1000.0);
	time_rashod_buf = TIM2 -> CNT;
	rash = 360.0 / time;
	time_rashod = 0;
	antidrebezg = 1;
	EXTI -> IMR &= ~EXTI_IMR_MR2;
	EXTI -> PR |= EXTI_PR_PR2;
}

void TIM2_IRQHandler(){
	TIM2 -> SR &= ~TIM_SR_UIF;
	time_rashod++;
	if (time_rashod > 720){
		time_rashod = 0;
		rash = 0;
	}
	if (antidrebezg) antidrebezg--;
	else {
		if (EXTI -> PR & EXTI_PR_PR2) EXTI -> PR |= EXTI_PR_PR2;
		EXTI -> IMR |= EXTI_IMR_MR2;
	}
}

int main(void)
{
	uint8_t text_pos;
	uint16_t temp;
	uint32_t i;

	RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_ADC2EN | RCC_APB2ENR_AFIOEN;
	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM2EN;
	RCC -> AHBENR |= RCC_AHBENR_DMA1EN;

	flag.sample_ready = 0;

	GPIOA -> CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF0 | GPIO_CRL_CNF1);

	//==================ADC========================================
	RCC->CFGR |= RCC_CFGR_ADCPRE;
	RCC->CFGR |= RCC_CFGR_ADCPRE_DIV4;
	//==================CAL========================================
	ADC1 -> CR2 |= ADC_CR2_CAL;
	while (!(ADC1 -> CR2 & ADC_CR2_CAL));
	ADC2 -> CR2 |= ADC_CR2_CAL;
	while (!(ADC2 -> CR2 & ADC_CR2_CAL));

	ADC2 -> CR2 = ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG;
	ADC2 -> SQR3 = 1;
	ADC1 -> CR2 = ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG | ADC_CR2_DMA;
	ADC1 -> SQR3 = 0;
	ADC1 -> CR1 |= ADC_CR1_DUALMOD_1 | ADC_CR1_DUALMOD_2;
	ADC2 -> SMPR2 = ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1;
	ADC1 -> SMPR2 = ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_1;
	ADC2 -> CR2 |= ADC_CR2_ADON;
	ADC1 -> CR2 |= ADC_CR2_ADON;

	TIM3 -> CR2 |= TIM_CR2_MMS_1;
	TIM3 -> ARR = 1000;						//1kHz
	TIM3 -> PSC = 56 - 1;

	DMA1_Channel1 -> CPAR = (uint32_t)&ADC1 -> DR;
	DMA1_Channel1 -> CMAR = (uint32_t)&adc_buf[0];
	DMA1_Channel1 -> CNDTR = SIZE_BUF;
	DMA1_Channel1 -> CCR = DMA_CCR1_PL_1 | DMA_CCR1_MSIZE_1 | DMA_CCR1_PSIZE_1 | DMA_CCR1_MINC | DMA_CCR1_CIRC | DMA_CCR1_TCIE | DMA_CCR1_EN;
	NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	TIM3 -> CR1 |= TIM_CR1_CEN;

	GPIOA -> CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
	GPIOA -> CRL |= GPIO_CRL_CNF2_1;
	GPIOA -> ODR |= GPIO_ODR_ODR2;

	WG12864A_Init();
	WG12864A_WriteText(15, 20, "анаеп 5");
	WG12864A_DrawBuf();
	for (i = 0; i < 5000000; i++);

	TIM2 -> PSC = 56000 - 1;
	TIM2 -> ARR = 1000;
	TIM2 -> DIER |= TIM_DIER_UIE;
	TIM2 -> CR1 |= TIM_CR1_CEN;
	NVIC_SetPriority(TIM2_IRQn, 2);
	NVIC_EnableIRQ(TIM2_IRQn);

	EXTI -> IMR = EXTI_IMR_MR2;
	EXTI -> RTSR = EXTI_FTSR_TR2;
	NVIC_SetPriority(EXTI2_IRQn, 3);
	NVIC_EnableIRQ(EXTI2_IRQn);

	IWDG -> KR = 0x5555;
	IWDG -> PR = IWDG_PR_PR_1;
	IWDG -> RLR = 0xFFF;
	IWDG -> KR = 0xCCCC;

    while(1)
    {
    	IWDG -> KR = 0xAAAA;
    	if (flag.sample_ready){
    		Get_I_U(adc_buf, mas_U, mas_I);
    		flag.sample_ready = 0;
    		TIM3 -> CR1 |= TIM_CR1_CEN;
    		rms_U = Calc_RMS(mas_U);
    		rms_I = Calc_RMS(mas_I);
    		if (rms_I >= 0.5){
				S = 3 * rms_U * rms_I;
				P = 3 * fabs(Calc_P(mas_U, mas_I));
				cosfi = P / S;
				if (rash > 0) ufn = (S / (rash * 1000));
    		} else {
    			S = 0;
    			P = 0;
    			cosfi = 1;
    			ufn = 0;
    		}
    		WG12864A_ClearBuf();
    		text_pos = WG12864A_WriteText(0, 0, "I:");
    		temp = rms_I;
    		text_pos = WG12864A_WriteNum(text_pos, 0, temp, 2, 1);
    		text_pos = WG12864A_WriteChar(text_pos, 0, ',');
    		temp = (rms_I - temp) * 10;
    		text_pos = WG12864A_WriteNum(text_pos, 0, temp, 1, 0);
    		text_pos = WG12864A_WriteText(55, 0, "s:");
    		text_pos = WG12864A_WriteNum(text_pos, 0, S, 5, 1);
    		text_pos = WG12864A_WriteText(30, 20, "cos:");
    		if (cosfi == 1){
    			text_pos = WG12864A_WriteChar(text_pos, 20, '1');
    		} else {
    			text_pos = WG12864A_WriteChar(text_pos, 20, '0');
    			text_pos = WG12864A_WriteChar(text_pos, 20, ',');
    			temp = cosfi * 100;
    			text_pos = WG12864A_WriteNum(text_pos, 20, temp, 2, 0);
    		}
    		text_pos = WG12864A_WriteText(0, 40, "q:");
    		text_pos = WG12864A_WriteNum(text_pos, 40, rash, 3, 1);
    		text_pos = WG12864A_WriteText(55, 40, "СМ:");
    		if (ufn > 10){
    			text_pos = WG12864A_WriteNum(text_pos, 40, ufn, 3, 1);
    		} else {
    			temp = ufn;
    			text_pos = WG12864A_WriteNum(text_pos, 40, temp, 1, 1);
    			text_pos = WG12864A_WriteChar(text_pos, 40, ',');
    			temp = (ufn - temp) * 100;
    			text_pos = WG12864A_WriteNum(text_pos, 40, temp, 2, 0);
    		}
    		WG12864A_DrawLine(0, 22, 127, 22);
    		WG12864A_DrawLine(0, 42, 127, 42);
    		WG12864A_DrawLine(53, 0, 53, 22);
    		WG12864A_DrawLine(53, 42, 53, 63);
    		WG12864A_DrawBuf();
    	}
    }
}
