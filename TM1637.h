/*
 * TM1637.h
 * A library for the 4 digit display
 * 
 * TM1637 chip datasheet:
 * https://www.mcielectronics.cl/website_MCI/static/documents/Datasheet_TM1637.pdf
 *
 * Copyright (c) 2012 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : Frankie.Chu
 * Create Time: 9 April,2012
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TM1637_h
#define TM1637_h

	#include "commons.h"

	#define IND_PORT			PORTB	// порт цифрового индикатора
	#define PIN_CLK				2		// пин CLK
	#define PIN_DATA			3		// пин DATA

	#define DEFAULT_BIT_DELAY 	1
	#define ASK_BIT_DELAY	  	100
	#define NUMDIG 				4 		// Разрядность индикатора
	#define BLINK_PERIOD		300		// Периодичность мигания разрядов (в ms)


	#define CLK_Low()		do{PortReg(&IND_PORT) &=  ~(_BV(PIN_CLK));} while(0)
	#define CLK_High()		do{PortReg(&IND_PORT) |=  _BV(PIN_CLK);} while(0)
	#define DATA_Low()		do{PortReg(&IND_PORT) &=  ~(_BV(PIN_DATA));} while(0)
	#define DATA_High()		do{PortReg(&IND_PORT) |=  _BV(PIN_DATA);} while(0)


	/*******************Definitions for TM1637*********************/
	//#define ADDR_AUTO 0x40
	//#define ADDR_FIXED 0x44

	//#define STARTADDR 0xc0

	/**************Definitions for brightness**********************/
	#define BRIGHT_DARKEST 0
	#define BRIGHT_TYPICAL 2
	#define BRIGHTEST 7

//		 --0x01(A)--
// 		|           |
//	  0x20(F)     0x02(B)
// 		|           |
//		 --0x40(G)--
//		|           |
//	  0x10(E)     0x04(C)
//		|           |
//		 --0x08(D)--

	#define SEG_A   0x01
	#define SEG_B   0x02
	#define SEG_C   0x04
	#define SEG_D   0x08
	#define SEG_E   0x10
	#define SEG_F   0x20
	#define SEG_G   0x40

	#define IND_ON	1
	#define IND_OFF	0

	/*****Definitions for the clock point and dots *******/
	#define DOTCLOK_ON 	1
	#define DOTCLOK_OFF 0

	/*****Номера разрядов индикатора (для мигания, точки) ****/
	#define DIG_NONE   0
	#define DIG_1   0b0001
	#define DIG_2   0b0010
	#define DIG_3   0b0100
	#define DIG_4   0b1000
	#define DIG_ALL 0b1111

	void TM1637_Init(void);               			// Инициализация индикатора
	void SetBrightness(u08 brightness, u08 turn);	// Установка яркости индикатора и вкл/выкл индикатора
	void SetDots(u08 dots);							//
	void SetBlink(u08 dig);
	void Display_RAW(u08 *data);
	void Display_Dig(u08 *dig);
	void ClearDisplay(void);

  //u08 cmd_set_data;
  //u08 cmd_set_addr;
  //u08 cmd_disp_ctrl;
  //boolean _PointFlag;            //_PointFlag=1:the clock point on
  //void TM1637(u08, u08);
  //u08 WriteByte(u08 wr_data); 					// Write 8bit data to tm1637
  //void start(void);              				// Send start bits
  //void stop(void);               				// Send stop bits
  //void display(int8_t DispData[]);
  //void display(u08 BitAddr, int8_t DispData);
  //void displayNum(float num, int decimal = 0, bool show_minus = true);
  //void displayStr(char str[],  uint16_t loop_delay = 500);
  //void clearDisplay(void);
 // void set(u08 = BRIGHT_TYPICAL, u08 = 0x40, u08 = 0xc0); //To take effect the next time it displays.
  //void point(boolean PointFlag);                                      //whether to light the clock point ":".To take effect the next time it displays.
  //void coding(int8_t DispData[]);
  //s08 coding(int8_t DispData);
  //void BitDelay(void);
  //u08 char2segments(char c);


  //u08 clkpin;
  //u08 datapin;

#endif
