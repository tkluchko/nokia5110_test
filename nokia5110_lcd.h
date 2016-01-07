#ifndef _NOKIA5110_LCD_H_
#define _NOKIA5110_LCD_H_

#define LCD_X_RES 84
#define LCD_Y_RES 48

#define LCD_CACHE_SIZE ((LCD_X_RES * LCD_Y_RES) / 6)

#define LCD_CLK_PIN 	PORTB.0
#define LCD_DATA_PIN 	PORTB.1
#define LCD_DC_PIN 		PORTB.2
#define LCD_RST_PIN 	PORTB.6


void lcd_init(void);
void lcd_contrast(unsigned char contrast);
void lcd_clear(void);
void lcd_clear_area(unsigned int line, unsigned char startX, unsigned char endX);
void lcd_clear_line(unsigned int line);
void lcd_gotoxy(unsigned int x, unsigned int y);
void lcd_putch(char chr);
void lcd_str(char *str);
void lcd_image( flash unsigned char img[]);
void lcd_extrachar(int num);

void writeCharBig (unsigned char x,unsigned char y, unsigned char ch);

#endif



