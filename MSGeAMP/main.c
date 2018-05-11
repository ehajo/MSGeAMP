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

#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "hardware.h"
#include "display_pcf8574.h"

void setup_hardware(void);
uint16_t read_adc(uint8_t);
void usart_send(char);
void usart_send_string(char *);
void calibibration(uint16_t[], uint16_t[], uint8_t, uint8_t);
float normalisation(uint16_t, uint16_t);
void get_config_from_eeprom();
void check_eeprom();
uint16_t get_max_value();
void write_config_to_eeprom();
uint16_t get_temperature();
float interpol(uint16_t ytab[], uint16_t xtab[], float x, int n);


uint8_t EEMEM ee_initalized;
uint16_t EEMEM ee_voltage_table_x[9];
uint16_t EEMEM ee_voltage_table_y[9];
uint16_t EEMEM ee_current_table_x[9];
uint16_t EEMEM ee_current_table_y[9];
uint16_t EEMEM ee_voltage_max;
uint16_t EEMEM ee_current_max;

uint16_t voltage_table_x[9];
uint16_t voltage_table_y[9];
uint16_t current_table_x[9];
uint16_t current_table_y[9];
uint16_t voltage_max, current_max;


volatile float voltage;
volatile float current_soll, current_ist;
volatile uint8_t send_u = 0, send_i = 0;

int main(void)
{
	float voltage_factor, current_factor;
	char str[16];
	//uint16_t temperature;
	setup_hardware();
	check_eeprom();
	get_config_from_eeprom();
	voltage_factor = normalisation(voltage_max, voltage_table_y[8]);
	current_factor = normalisation(current_max, current_table_y[8]);
	//sei();
    while (1) 
    {
		if(!(BUTTON_PORT.IN & BUTTON))
		{
			cli();
			display_clear();
			display_set_cursor(0,0);
			display_write_string("Calibrating U...");
			display_set_cursor(0,1);
			display_write_string("U = 0V -> Button");
			_delay_ms(250);
			while(BUTTON_PORT.IN & BUTTON);
			calibibration(voltage_table_x, voltage_table_y, ADC_VSOLL, ADC_VIST);
			display_set_cursor(0,0);
			display_write_string("Calibrating I...");
			display_set_cursor(0,1);
			display_write_string("I = 0A -> Button");
			while(BUTTON_PORT.IN & BUTTON);
			calibibration(current_table_x, current_table_y, ADC_ISOLL, ADC_IIST);
			write_config_to_eeprom();
			voltage_factor = normalisation(voltage_max, voltage_table_y[8]);
			current_factor = normalisation(current_max, current_table_y[8]);
			display_clear();
			sei();
		}
		cli();
		voltage = read_adc(ADC_VIST);
		voltage = interpol(voltage_table_x, voltage_table_y, voltage, 8);
		voltage = voltage * voltage_factor;
		sei();
		dtostrf((voltage/1000), 4, 1, str);
		if(send_u == 1)
		{
			usart_send_string(str);
			send_u = 0;
		}
		display_set_cursor(0, 0);
		display_write_string("U:");
		display_write_string(str);

		cli();
		current_ist = read_adc(ADC_IIST);
		current_ist = interpol(current_table_x, current_table_y, current_ist, 8);
		current_ist = current_ist * current_factor;
		sei();
		dtostrf((current_ist/1000), 3, 2, str);
		if(send_i == 1)
		{
			usart_send_string(str);
			send_i = 0;
		}
		display_set_cursor(0, 2);
		display_write_string("Ii:");
		display_write_string(str);

		current_soll = read_adc(ADC_ISOLL);
		current_soll = current_soll * current_factor;
		dtostrf((current_soll/1000), 3, 2, str);
		display_set_cursor(8, 2);
		display_write_string("Is:");
		display_write_string(str);

		display_set_cursor(7,1);
		if((current_ist > (current_soll - 100)) && (voltage < 400) && (current_ist > 100))
		{
			LED_ROT_PORT.OUTCLR = LED_ROT;
			display_write_data('!');
		}
		else
		{
			LED_ROT_PORT.OUTSET = LED_ROT;
			display_write_data(' ');
		}
		_delay_ms(75);
    }
}

void setup_hardware()
{
	LED_GN_PORT.OUTCLR = LED_GN;
	LED_GN_PORT.DIRSET = LED_GN;
	LED_ROT_PORT.OUTSET = LED_ROT;
	LED_ROT_PORT.DIRSET = LED_ROT;
	BUTTON_PORT.BUTTON_CTRL = PORT_PULLUPEN_bm;
	BUTTON_PORT.OUTSET = BUTTON;
	SHTDWN_PORT.OUTSET = SHTDWN;
	SHTDWN_PORT.DIRSET = SHTDWN;

	// initalize ADC:
	ADC0_CTRLA &= ~(ADC_RESSEL_bm);		// 10 bit resolution
	ADC0_CTRLB |= (ADC_SAMPNUM1_bm);	// accumulate 4 results
	ADC0_CTRLC |= (ADC_REFSEL0_bm) | (ADC_SAMPCAP_bm) | (ADC_PRESC0_bm) | (ADC_PRESC1_bm);		// VDD as reference
	ADC0_CTRLA |= ADC_ENABLE_bm;		// enable it

	// initalize USART
	PORTB.OUTSET = (PIN2_bm | PIN0_bm);
	PORTB.DIRSET = (PIN2_bm | PIN0_bm);
	USART0.BAUD = 1387.5;
	USART0.CTRLC = (USART_CHSIZE0_bm | USART_CHSIZE1_bm);
	USART0.CTRLA = (USART_RS4850_bm);
	USART0.CTRLB = (USART_RXEN_bm | USART_TXEN_bm);
	
	// initalize display
	display_initialize();
}

uint16_t get_temperature()
{
	uint16_t adc_reading = 0;
	uint8_t ctrlc, ctrlb;
	int8_t  sigrow_offset = SIGROW.TEMPSENSE1;
	uint8_t sigrow_gain = SIGROW.TEMPSENSE0;
	VREF.CTRLA |= VREF_ADC0REFSEL0_bm;
	ctrlc = ADC0.CTRLC;
	ctrlb = ADC0.CTRLB;
	ADC0.CTRLC = 0;
	ADC0_CTRLC |= (ADC_SAMPCAP_bm) | (ADC_PRESC0_bm) | (ADC_PRESC1_bm);
	ADC0_CTRLB = 0;
	adc_reading = read_adc(ADC_TEMP);
	uint32_t temp = adc_reading - sigrow_offset;
	temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
	temp += 0x80; // Add 1/2 to get correct rounding on division below
	temp >>= 8; // Divide result to get Kelvin
	uint16_t temperature_in_K = temp;
	ADC0_CTRLC = ctrlc;
	ADC0_CTRLB = ctrlb;
	return temperature_in_K;
}

void usart_send(char data)
{
	while(!(USART0.STATUS & USART_DREIF_bm));
	USART0.TXDATAL = data;
}

void usart_send_string(char * data)
{
	uint8_t i = 0;
	while(data[i])
	{
		usart_send(data[i]);
		i++;
	}
}

uint16_t read_adc(uint8_t channel)
{
	ADC0_MUXPOS = channel;
	ADC0_COMMAND = ADC_STCONV_bm;
	while(ADC0_COMMAND & ADC_STCONV_bm)
	{}
	ADC0_COMMAND = ADC_STCONV_bm;
	while(ADC0_COMMAND & ADC_STCONV_bm)
	{}
	return ADC0_RES;
}

void calibibration(uint16_t table_x[], uint16_t table_y[], uint8_t adc_soll, uint8_t adc_ist)
{
	uint16_t temp;
	char str[3];

	//display_clear();

	do {
		temp = read_adc(adc_soll);
	} while(temp > 100);
	table_x[0] = temp;
	table_y[0] = read_adc(adc_ist);
	display_set_cursor(0,1);
	display_write_string("Point 0 done.   ");
	
	for(uint8_t i = 1; i<8; i++)
	{
		do {
			temp = read_adc(adc_soll);
		} while((temp < ((i*512)-8)) || (temp > (i*512)));
		table_x[i] = temp;
		table_y[i] = read_adc(adc_ist);
		display_set_cursor(0,1);
		display_write_string("Point ");
		itoa(i, str, 10);
		display_write_string(str);
		display_write_string(" done.");
	}

	do {
		temp = read_adc(adc_soll);
	} while(temp < 4050);
	_delay_ms(500);
	table_x[8] = read_adc(adc_soll);
	_delay_ms(1);
	table_y[8] = read_adc(adc_ist);
	display_set_cursor(0,1);
	display_write_string("Point 8 done.");
}

float normalisation(uint16_t norm_value, uint16_t max_value)
{
	float temp;
	temp = norm_value;
	temp /= (float)max_value;
	return temp;
}

void get_config_from_eeprom()
{
	// get U/I-lookups:
	eeprom_read_block((void*)voltage_table_x, (const void*)ee_voltage_table_x, 18);
	eeprom_read_block((void*)voltage_table_y, (const void*)ee_voltage_table_y, 18);
	eeprom_read_block((void*)current_table_x, (const void*)ee_current_table_x, 18);
	eeprom_read_block((void*)current_table_y, (const void*)ee_current_table_y, 18);
	current_max = eeprom_read_word((uint16_t *)&ee_current_max);
	voltage_max = eeprom_read_word((uint16_t *)&ee_voltage_max);
}

void write_config_to_eeprom()
{
	// get U/I-lookups:
	eeprom_update_block((const void*)voltage_table_x, (void*)&ee_voltage_table_x, 18);
	eeprom_update_block((const void*)voltage_table_y, (void*)&ee_voltage_table_y, 18);
	eeprom_update_block((const void*)current_table_x, (void*)&ee_current_table_x, 18);
	eeprom_update_block((const void*)current_table_y, (void*)&ee_current_table_y, 18);
	eeprom_update_word(&ee_current_max, current_max);
	eeprom_update_word(&ee_voltage_max, voltage_max);
}

void check_eeprom()
{
	uint16_t temp[9] = {0, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4096};
	if(eeprom_read_byte((uint8_t *)&ee_initalized) != 0x15)
	{
		usart_send_string("initializing EEPROM \n");
		eeprom_update_block((const void*)temp, (void*)ee_voltage_table_x, 18);
		eeprom_update_block((const void*)temp, (void*)ee_voltage_table_y, 18);
		eeprom_update_block((const void*)temp, (void*)ee_current_table_x, 18);
		eeprom_update_block((const void*)temp, (void*)ee_current_table_y, 18);
		eeprom_update_word(&ee_voltage_max, 14000ul);
		eeprom_update_word(&ee_current_max, 2500ul);
		eeprom_update_byte(&ee_initalized, 0x15);
	}
}

float interpol(uint16_t ytab[], uint16_t xtab[], float x, int n)
{
	float y, m, deltax, deltay;
	int i;

	i=0;
	while((x > xtab[i+1]) && (i < (n-1))){
		i++;
	}
	deltax=(float)xtab[i+1]-(float)xtab[i];
	deltay=(float)ytab[i+1]-(float)ytab[i];
	m=deltay/deltax;
	y=m*(x-(float)xtab[i])+(float)ytab[i];
	return y;
}

ISR(USART0_RXC_vect)
{
	switch(USART0_RXDATAL)
	{
		case 'v':
			send_u = 1;
			break;
		case 'i':
			send_i = 1;
			break;
		default:
			break;
	}
}