/*
 * MSGeAMP.c
 * 
 * Copyright (c) 2018 Hannes Jochriem, eHaJo
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 


#ifndef DISPLAY_PCF8574_H_
#define DISPLAY_PCF8574_H_

void display_initialize(void);
void display_backlight_on(void);
void display_backlight_off(void);
void display_write_data(uint8_t data);
void display_set_cursor(uint8_t x, uint8_t y);
void display_write_string(char *data);
void display_clear(void);

uint8_t line1[16];
uint8_t line2[16];


#endif /* DISPLAY_PCF8574_H_ */