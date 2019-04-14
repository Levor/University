#include "wg12864a.h"

extern const FONT_INFO timesNewRoman_16ptFontInfo;
const FONT_INFO *font = &timesNewRoman_16ptFontInfo;

void WG12864A_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	uint16_t e_x = 0, e_y = 0, x, y, d, i;
    signed int dx, dy, i_x, i_y;

    dx = x1-x0;
    dy = y1-y0;
    if (dx > 0) i_x = 1; else
    if (dx == 0) i_x = 0; else
    i_x = -1;

    if (dy > 0) i_y = 1; else
    if (dy == 0) i_y = 0; else
    i_y = -1;

    dx = (dx >= 0) ? dx : -dx;
    dy = (dy >= 0) ? dy : -dy;

    d = (dx > dy) ? dx : dy;

    x = x0;
    y = y0;
    for (i = 0; i < d; i++){
        e_x += dx;
        e_y += dy;
        if (e_x > d){
           e_x -= d;
           x += i_x;
        }
        if (e_y > d){
           e_y -= d;
           y += i_y;
        }
        screen_buf[y / 8][x] |= (1 << (y % 8));
    }
}

void WG12864A_Fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	uint8_t i, j;
	for (i = x0; i < x1; i++){
		for (j = y0; j < y1; j++){
			screen_buf[j / 8][i] |= (1 << (j % 8));
		}
	}
}

uint8_t WG12864A_WriteNum(uint16_t x, uint16_t y, uint16_t num, uint8_t max_pos, uint8_t hide_null){
	uint8_t a = 48, b = 48, c = 48, d = 48, e = 48;

	if(max_pos >= 5)
		while(num >= 10000){
			num -= 10000;
			a++;
		}
	if(max_pos >= 4)
		while(num >= 1000){
			num -= 1000;
			b++;
		}
	if(max_pos >= 3)
		while(num >= 100){
			num -= 100;
			c++;
		}
	if(max_pos >= 2)
		while(num >= 10){
			num -= 10;
			d++;
		}
	while(num > 0){
		num--;
		e++;
	}
	if (hide_null){
		if (a > 48) x = WG12864A_WriteChar(x, y, a);
		if ((a > 48) || (b > 48)) x = WG12864A_WriteChar(x, y, b);
		if ((a > 48) || (b > 48) || (c > 48)) x = WG12864A_WriteChar(x, y, c);
		if ((a > 48) || (b > 48) || (c > 48) || (d > 48)) x = WG12864A_WriteChar(x, y, d);
	} else {
		if (max_pos >= 5) x = WG12864A_WriteChar(x, y, a);
		if (max_pos >= 4) x = WG12864A_WriteChar(x, y, b);
		if (max_pos >= 3) x = WG12864A_WriteChar(x, y, c);
		if (max_pos >= 2) x = WG12864A_WriteChar(x, y, d);
	}
	x = WG12864A_WriteChar(x, y, e);
	return x;
}

uint8_t WG12864A_WriteText(uint8_t x, uint8_t y, const uint8_t *text){
	uint8_t sym;
	while (*text){
		sym = *text++;
		x = WG12864A_WriteChar(x, y, sym);
	}
	return x;
}

uint8_t WG12864A_WriteChar(uint8_t x, uint8_t y, uint8_t symbol){
	uint8_t i, j;
	symbol -= 32;

	if (font -> FontTable[symbol].widthBits <= 8){
		for (i = 0; i < font -> Height; i++){
			for (j = 0; j < font -> FontTable[symbol].widthBits; j++){
				if (font -> FontBitmaps[font -> FontTable[symbol].start + i] & (1 << (7 - j))){
					screen_buf[(y + i) / 8][x + j] |= (1 << ((y + i) % 8));
				} else screen_buf[(y + i) / 8][x + j] &= ~(1 << ((y + i) % 8));
			}
		}
	} else
	if (font -> FontTable[symbol].widthBits <= 16){
		for (i = 0; i < (font -> Height) * 2; i+=2){
			for (j = 0; j < font -> FontTable[symbol].widthBits; j++){
				if (j < 8){
					if (font -> FontBitmaps[font -> FontTable[symbol].start + i] & (1 << (7 - j))){
						screen_buf[(y + (i / 2)) / 8][x + j] |= (1 << ((y + (i / 2)) % 8));
					} else screen_buf[(y + (i / 2)) / 8][x + j] &= ~(1 << ((y + (i / 2)) % 8));
				} else {
					if (font -> FontBitmaps[font -> FontTable[symbol].start + 1 + i] & (1 << (15 - j))){
						screen_buf[(y + (i / 2)) / 8][x + j] |= (1 << ((y + (i / 2)) % 8));
					} else screen_buf[(y + (i / 2)) / 8][x + j] &= ~(1 << ((y + (i / 2)) % 8));
				}
			}
		}
	} else
	if (font -> FontTable[symbol].widthBits <= 24){
		for (i = 0; i < (font -> Height) * 3; i+=3){
			for (j = 0; j < font -> FontTable[symbol].widthBits; j++){
				if (j < 8){
					if (font -> FontBitmaps[font -> FontTable[symbol].start + i] & (1 << (7 - j))){
						screen_buf[(y + (i / 3)) / 8][x + j] |= (1 << ((y + (i / 3)) % 8));
					} else screen_buf[(y + (i / 3)) / 8][x + j] &= ~(1 << ((y + (i / 3)) % 8));
				} else
				if (j < 16) {
					if (font -> FontBitmaps[font -> FontTable[symbol].start + 1 + i] & (1 << (15 - j))){
						screen_buf[(y + (i / 3)) / 8][x + j] |= (1 << ((y + (i / 3)) % 8));
					} else screen_buf[(y + (i / 3)) / 8][x + j] &= ~(1 << ((y + (i / 3)) % 8));
				}else
				if (j < 24) {
					if (font -> FontBitmaps[font -> FontTable[symbol].start + 2 + i] & (1 << (23 - j))){
						screen_buf[(y + (i / 3)) / 8][x + j] |= (1 << ((y + (i / 3)) % 8));
					} else screen_buf[(y + (i / 3)) / 8][x + j] &= ~(1 << ((y + (i / 3)) % 8));
				}
		}
		}
	}
	return x + font -> FontTable[symbol].widthBits + font -> FontSpace;
}

void WG12864A_Send_Cmd(uint8_t cmd, uint8_t cs){
	uint16_t i;
	WG_DI_LO;
	if (cs & 0x01) WG_CS1_HI; else WG_CS1_LO;
	if (cs & 0x02) WG_CS2_HI; else WG_CS2_LO;
	if (cmd & 0x01) WG_DB0_HI; else WG_DB0_LO;
	if (cmd & 0x02) WG_DB1_HI; else WG_DB1_LO;
	if (cmd & 0x04) WG_DB2_HI; else WG_DB2_LO;
	if (cmd & 0x08) WG_DB3_HI; else WG_DB3_LO;
	if (cmd & 0x10) WG_DB4_HI; else WG_DB4_LO;
	if (cmd & 0x20) WG_DB5_HI; else WG_DB5_LO;
	if (cmd & 0x40) WG_DB6_HI; else WG_DB6_LO;
	if (cmd & 0x80) WG_DB7_HI; else WG_DB7_LO;
	WG_E_HI;
	for (i = 0; i < 1000; i++);
	WG_E_LO;
	WG_CS1_LO;
	WG_CS2_LO;
}

void WG12864A_Send_Data(uint8_t data, uint8_t cs){
	uint16_t i;
	WG_DI_HI;
	if (cs & 0x01) WG_CS1_HI; else WG_CS1_LO;
	if (cs & 0x02) WG_CS2_HI; else WG_CS2_LO;
	if (data & 0x01) WG_DB0_HI; else WG_DB0_LO;
	if (data & 0x02) WG_DB1_HI; else WG_DB1_LO;
	if (data & 0x04) WG_DB2_HI; else WG_DB2_LO;
	if (data & 0x08) WG_DB3_HI; else WG_DB3_LO;
	if (data & 0x10) WG_DB4_HI; else WG_DB4_LO;
	if (data & 0x20) WG_DB5_HI; else WG_DB5_LO;
	if (data & 0x40) WG_DB6_HI; else WG_DB6_LO;
	if (data & 0x80) WG_DB7_HI; else WG_DB7_LO;
	WG_E_HI;
	for (i = 0; i < 1000; i++);
	WG_E_LO;
	WG_CS1_LO;
	WG_CS2_LO;
}

void WG12864A_Clear(){
	uint8_t i, j;
	WG12864A_Send_Cmd(0x40, WG_ALL);	//X = 0;
	WG12864A_Send_Cmd(0xB8, WG_ALL);	//Y = 0;
	for (j = 0; j < 8; j++){
		WG12864A_Send_Cmd(0xB8 + j, WG_ALL);
		for (i = 0; i < 64; i++){
			WG12864A_Send_Data(0x00, WG_ALL);
		}
	}
}

void WG12864A_DrawBuf(){
	uint8_t i, j;
	WG12864A_Send_Cmd(0x40, WG_ALL);	//X = 0;
	WG12864A_Send_Cmd(0xB8, WG_ALL);	//Y = 0;
	for (i = 0; i < 8; i++){
		WG12864A_Send_Cmd(0xB8 + i, WG_ALL);
		for (j = 0; j < 64; j++){
			WG12864A_Send_Data(screen_buf[i][j], WG_FIRST);
			WG12864A_Send_Data(screen_buf[i][j + 64], WG_SECOND);
		}
	}
}

void WG12864A_ClearBuf(){
	uint8_t i, j;
	for (i = 0; i < 8; i++){
		for (j = 0; j < 128; j++){
			screen_buf[i][j] = 0;
		}
	}
}

void WG12864A_Init(){
	uint16_t i;
	RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
	GPIOA -> CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
	GPIOA -> CRL |= (GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_MODE6 | GPIO_CRL_MODE7);
	GPIOB -> CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2);
	GPIOB -> CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_CNF11 | GPIO_CRH_CNF12 | GPIO_CRH_CNF13 | GPIO_CRH_CNF14);
	GPIOB -> CRL |= (GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2);
	GPIOB -> CRH |= (GPIO_CRH_MODE10 | GPIO_CRH_MODE11 | GPIO_CRH_MODE12 | GPIO_CRH_MODE13 | GPIO_CRH_MODE14);
	WG_RST_LO;
	for (i = 0; i < 65000; i++);
	WG_RST_HI;
	WG12864A_Send_Cmd(0x3F, WG_ALL);	//ON
	WG12864A_Send_Cmd(0xC0, WG_ALL);	//NO_FLIP
	WG12864A_ClearBuf();
	WG12864A_DrawBuf();
}
