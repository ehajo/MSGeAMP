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


#ifndef HARDWARE_H_
#define HARDWARE_H_

#define LED_ROT_PORT	PORTC
#define LED_ROT			PIN3_bm
#define LED_GN_PORT		PORTC
#define LED_GN			PIN2_bm
#define BUTTON_PORT		PORTC
#define BUTTON			PIN1_bm
#define BUTTON_CTRL		PIN1CTRL
#define SHTDWN_PORT		PORTA
#define SHTDWN			PIN7_bm

#define ADC_VIST	0x06
#define ADC_IIST	0x03
#define ADC_VSOLL	0x04
#define ADC_ISOLL	0x05
#define ADC_TEMP	0x1E


#endif /* HARDWARE_H_ */