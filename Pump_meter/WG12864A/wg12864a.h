#include <stm32f10x.h>
#include "fontStructures.h"

#define WG_FIRST		0x01
#define WG_SECOND		0x02
#define WG_ALL			0x03

#define WG_DI_HI		GPIOA -> BSRR = GPIO_BSRR_BS3
#define WG_DI_LO		GPIOA -> BSRR = GPIO_BSRR_BR3
#define WG_E_HI			GPIOA -> BSRR = GPIO_BSRR_BS4
#define WG_E_LO			GPIOA -> BSRR = GPIO_BSRR_BR4
#define WG_DB0_HI		GPIOA -> BSRR = GPIO_BSRR_BS5
#define WG_DB0_LO		GPIOA -> BSRR = GPIO_BSRR_BR5
#define WG_DB1_HI		GPIOA -> BSRR = GPIO_BSRR_BS6
#define WG_DB1_LO		GPIOA -> BSRR = GPIO_BSRR_BR6
#define WG_DB2_HI		GPIOA -> BSRR = GPIO_BSRR_BS7
#define WG_DB2_LO		GPIOA -> BSRR = GPIO_BSRR_BR7
#define WG_DB3_HI		GPIOB -> BSRR = GPIO_BSRR_BS0
#define WG_DB3_LO		GPIOB -> BSRR = GPIO_BSRR_BR0
#define WG_DB4_HI		GPIOB -> BSRR = GPIO_BSRR_BS1
#define WG_DB4_LO		GPIOB -> BSRR = GPIO_BSRR_BR1
#define WG_DB5_HI		GPIOB -> BSRR = GPIO_BSRR_BS2
#define WG_DB5_LO		GPIOB -> BSRR = GPIO_BSRR_BR2
#define WG_DB6_HI		GPIOB -> BSRR = GPIO_BSRR_BS10
#define WG_DB6_LO		GPIOB -> BSRR = GPIO_BSRR_BR10
#define WG_DB7_HI		GPIOB -> BSRR = GPIO_BSRR_BS11
#define WG_DB7_LO		GPIOB -> BSRR = GPIO_BSRR_BR11
#define WG_CS1_HI		GPIOB -> BSRR = GPIO_BSRR_BS12
#define WG_CS1_LO		GPIOB -> BSRR = GPIO_BSRR_BR12
#define WG_CS2_HI		GPIOB -> BSRR = GPIO_BSRR_BS13
#define WG_CS2_LO		GPIOB -> BSRR = GPIO_BSRR_BR13
#define WG_RST_HI		GPIOB -> BSRR = GPIO_BSRR_BS14
#define WG_RST_LO		GPIOB -> BSRR = GPIO_BSRR_BR14

uint8_t screen_buf[8][128];

void WG12864A_Init();
void WG12864A_Send_Cmd(uint8_t cmd, uint8_t cs);
void WG12864A_Send_Data(uint8_t data, uint8_t cs);
uint8_t WG12864A_WriteText(uint8_t x, uint8_t y, const uint8_t *text);
uint8_t WG12864A_WriteChar(uint8_t x, uint8_t y, uint8_t symbol);
void WG12864A_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void WG12864A_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
uint8_t WG12864A_WriteNum(uint16_t x, uint16_t y, uint16_t num, uint8_t max_pos, uint8_t hide_null);
void WG12864A_Clear();
void WG12864A_DrawBuf();
void WG12864A_ClearBuf();

