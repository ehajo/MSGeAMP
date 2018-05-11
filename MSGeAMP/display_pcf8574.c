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

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "display_pcf8574.h"
#include "twi_master.h"

void display_write_command(uint8_t command);
void display_write_nibble(uint8_t data, uint8_t rs);

#define RS (1<<0)
#define RW (1<<1)
#define EN (1<<2)
#define D4 (1<<3)
#define D5 (1<<4)
#define D6 (1<<5)
#define D7 (1<<6)
#define BL (1<<7)

#define PCF_ADDRESS 0b01111110

volatile uint8_t backlight_on = 0;

void display_initialize()
{
	_delay_ms(400);
	i2c_initialize(1);
	display_write_nibble(0x03, 0);
	_delay_ms(5);
	display_write_nibble(0x03, 0);
	_delay_us(110);
	display_write_nibble(0x03, 0);
	_delay_us(110);
	display_write_nibble(0x02, 0);	// Set to 4bit
	_delay_us(40);
	display_write_command(0x28);	// Function set
	_delay_us(40);
	display_write_command(0x0C);	// Display on/off ctrl
	_delay_us(40);
	display_write_command(0x01);	// Clear display
	_delay_ms(2);
	display_write_command(0x06);	// Entry mode set
}

void display_write_command(uint8_t command)
{
	display_write_nibble((command >> 4), 0);
	display_write_nibble(command, 0);
}

void display_write_data(uint8_t data)
{
	display_write_nibble((data >> 4), 1);
	display_write_nibble(data, 1);
}

void display_write_string(char *data)
{
	while(*data != 0)
	{
		display_write_data(*data);
		data++;
	}
}

void display_clear()
{
	display_write_command(0x01);
}

void display_write_nibble(uint8_t data, uint8_t rs)
{
	data &= 0x0f;
	data <<= 3;
	if(rs != 0)
	{
		data |= RS;
	}
	data |= (backlight_on | EN);
	i2c_write(PCF_ADDRESS, data);
	data -= EN;
	i2c_write(PCF_ADDRESS, data);
}

void display_backlight_on()
{
	backlight_on = BL;
}

void display_backlight_off()
{
	backlight_on = 0;
}

void display_set_cursor(uint8_t x, uint8_t y)
{
	if(y != 0)
	{
		display_write_command(0x80 + x + 0x40);
	}
	else
	{
		display_write_command(0x80 + x);
	}
}