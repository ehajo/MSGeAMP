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

 #include "twi_master.h"

 #define CPU_SPEED 3333333
 #define BAUDRATE 100000
 #define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) -5 )
 #define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

void i2c_initialize(uint8_t alternate_pinconfig)
{
	if (alternate_pinconfig != 0)
	{
		PORTMUX_CTRLB |= PORTMUX_TWI0_bm;
	}
	//TWI0.MCTRLA |= TWI_SMEN_bm;
	TWI0.MBAUD = TWI_BAUDSETTING;
	TWI0.MCTRLA |= TWI_ENABLE_bm;
	TWI0.MCTRLB |= TWI_FLUSH_bm;
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

 uint8_t i2c_write(uint8_t slave_address, uint8_t data)
 {
	 TWI0.MADDR = slave_address;
	 while(!(TWI0.MSTATUS&TWI_WIF_bm));

	 TWI0.MDATA = data;
	 while(!(TWI0.MSTATUS&TWI_WIF_bm));

	 TWI0.MCTRLB = 0x03;

	 return 0;
 }

 uint8_t i2c_read(uint8_t slave_address, uint8_t data)
 {
	 uint8_t puffer;

	 TWI0.MADDR = slave_address | 0x01;
	 
	 while(!(TWI0.MSTATUS&TWI_RIF_bm));
	 puffer = TWI0.MDATA;
	 
	 TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;

	 return puffer;
 }