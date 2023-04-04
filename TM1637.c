/*
 * TM1637.cpp
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

#include <util/delay.h>
#include "TM1637.h"

	#define TM1637_I2C_COMM1    0x40
	#define TM1637_I2C_COMM2    0xC0
	#define TM1637_I2C_COMM3    0x80

/*****Глобальные переменные для TM1637.c ********/
	u08	Brightness; // яркость индикатора
	u08	IndDots;	// точки для индикатора
	u08	IndBlink;	// мигание разрядов индикатора

	static u08 tube_tab[] = {0x3f, 0x06, 0x5b, 0x4f,
	                            0x66, 0x6d, 0x7d, 0x07,
	                            0x7f, 0x6f, 0x77, 0x7c,
	                            0x39, 0x5e, 0x79, 0x71
	                            }; //0~9,A,b,C,d,E,F

 /*
  uint8_t char2segments(char c)
 {
	  switch (c) {
      	  case '_' : return 0x08;
      	  case '^' : return 0x01; // ВЇ
      	  case '-' : return 0x40;
      	  case '*' : return 0x63; // В°
      	  case ' ' : return 0x00; // space
      	  case 'A' : return 0x77;
      	  case 'b' : return 0x7c;
      	  case 'c' : return 0x58;
      	  case 'C' : return 0x39;
      	  case 'd' : return 0x5e;
      	  case 'E' : return 0x79;
      	  case 'F' : return 0x71;
      	  case 'h' : return 0x74;
      	  case 'H' : return 0x76;
      	  case 'I' : return 0x30;
      	  case 'J' : return 0x0e;
      	  case 'L' : return 0x38;
      	  case 'n' : return 0x54;
      	  case 'N' : return 0x37; // like в€©
      	  case 'o' : return 0x5c;
      	  case 'P' : return 0x73;
      	  case 'q' : return 0x67;
      	  case 'u' : return 0x1c;
      	  case 'U' : return 0x3e;
      	  case 'y' : return 0x66; // =4
	  }
	  return 0;
}
*/
	u08 char2segments(char c)
	{
		  switch (c) {
		  	  case ' ' : return 0x00;
			  case '-' : return 0x40;
			  case 'L' : return 0x38;
			  case 'P' : return 0x73;
			  case 'U' : return 0x3e;
			  default:	 return 0x08;
		  }
		  return 0;
	}


	void TM1637_Init(void)
	{
		DirReg(&IND_PORT) |= (_BV(PIN_CLK) | _BV(PIN_DATA));
		PortReg(&IND_PORT) |= (_BV(PIN_CLK) | _BV(PIN_DATA));
		SetBrightness(BRIGHT_TYPICAL, IND_ON);
		IndDots = DIG_NONE;
		IndBlink = DIG_NONE;
		ClearDisplay();
	}

	// Send start signal to TM1637 (start = when both pins goes low)
	void TM1637_start(void)
	{
		DATA_Low();
		_delay_us (DEFAULT_BIT_DELAY);
	  //CLK_Low()
	}

	// End of transmission (stop = when both pins goes high)
	void TM1637_stop(void)
	{
	  //CLK_Low()
		DATA_Low();
		_delay_us (DEFAULT_BIT_DELAY);
		CLK_High();
		_delay_us (DEFAULT_BIT_DELAY);
		DATA_High();
		_delay_us (DEFAULT_BIT_DELAY);
	}

	u08 WriteByte(u08 wr_data)
	{
		u08 i, ack;

		// Sent 8bit data
		for (i = 0; i < 8; i++) {
			CLK_Low();
			_delay_us (DEFAULT_BIT_DELAY);

			// Set data bit
			if (wr_data & 0x01)
				DATA_High();
			else
				DATA_Low();
			wr_data >>= 1;
			_delay_us (DEFAULT_BIT_DELAY);

			CLK_High();
			_delay_us (DEFAULT_BIT_DELAY);
		}

		// Wait for the ACK
		CLK_Low();									// CLK  в 0
		_delay_us (ASK_BIT_DELAY);
		DATA_High();								// DATA в 1
		_delay_us (ASK_BIT_DELAY);
		CLK_High();									// CLK  в 1
		DirReg(&IND_PORT) &= ~( _BV(PIN_DATA));
		_delay_us (ASK_BIT_DELAY);

		ack = PinReg(&IND_PORT) & _BV(PIN_DATA);
		if (ack == 0) {
			DirReg(&IND_PORT) |=  _BV(PIN_DATA);
			DATA_Low();
		}
		_delay_us (DEFAULT_BIT_DELAY);
		CLK_Low();
		_delay_us (DEFAULT_BIT_DELAY);

		return ack;
	}


 u08 coding_byte(u08 disp_byte)
 {

	 if (disp_byte == 0x7f)	return 0;

	 if ((disp_byte >= 0) && (disp_byte < sizeof(tube_tab)/sizeof(*tube_tab)))
		 return tube_tab[disp_byte];

	 if ( disp_byte >= '0' && disp_byte <= '9' )
		 return tube_tab[(int)(disp_byte)-48]; // char to int (char "0" = ASCII 48)
	 else
		 return char2segments(disp_byte);
 }

 void coding_data(u08 disp_data[], u08 disp_seg[])
 {
	 u08 i, dots, blnk;
	 static u08 tcount = 0;

	 ++tcount;

	 dots = IndDots;						// точки после разрядов
	 blnk = IndBlink;						// мигающие разряды
	 for(i=0; i<NUMDIG; ++i) {
		 disp_seg[i] = coding_byte(disp_data[i]);
		 if (dots & 1)
			 disp_seg[i] += 0x80;
		 if (blnk) {
			 if ((blnk & 1) && (tcount & 1)) {
				 disp_seg[i] = 0;
			 }
			 blnk >>= 1;
		 }
		 dots >>= 1;
	 }
 }


// Sets the brightness of the display.
//
// The setting takes effect when a command is given to change the data being
// displayed.
//
//brightness A number from 0 (lowes brightness) to 7 (highest brightness)
//param turn Turn display on or off
void SetBrightness(u08 brightness, u08 turn)
{
	Brightness = (brightness & 0x7);
	if(turn)	Brightness |= 0x08;
}

inline void SetDots(u08 dots)
{
	IndDots = dots;
}

inline void SetBlink(u08 dig)
{
	IndBlink = dig;
}

void Display_RAW(u08 *data)
{
	u08 k;

    // Write COMM1
	TM1637_start();
	WriteByte(TM1637_I2C_COMM1);
	TM1637_stop();

	// Write COMM2 + first digit address
	TM1637_start();
	WriteByte(TM1637_I2C_COMM2);

	// Write the data bytes
	for (k=0; k < NUMDIG; k++)
	  WriteByte(data[k]);

	TM1637_stop();

	// Write COMM3 + brightness
	TM1637_start();
	WriteByte(TM1637_I2C_COMM3 + Brightness);
	TM1637_stop();
}

void Display_Dig(u08 *dig)
{
	u08 seg[NUMDIG];

	coding_data(dig, seg);
	Display_RAW(seg);
}

void ClearDisplay(void)
{
	u08 seg[] = {0, 0, 0, 0};

	Display_RAW(seg);
}

/*
// Display function.Write to full-screen.
void TM1637_display(int8_t disp_data[])
{
  int8_t seg_data[NUMDIG];
  uint8_t i;

  for (i = 0; i < NUMDIG; i++)
    seg_data[i] = disp_data[i];

  coding(seg_data);
  start();              // Start signal sent to TM1637 from MCU
  WriteByte(ADDR_AUTO); // Command1: Set data
  stop();
  start();
  WriteByte(cmd_set_addr); // Command2: Set address (automatic address adding)

  for (i = 0; i < NUMDIG; i++)
    WriteByte(seg_data[i]); // Transfer display data (8 bits x num_of_digits)

  stop();
  start();
  WriteByte(cmd_disp_ctrl); // Control display
  stop();
}

// ******************************************
void TM1637_display(uint8_t bit_addr, int8_t disp_data)
{
  int8_t seg_data;

  seg_data = coding(disp_data);
  start();               // Start signal sent to TM1637 from MCU
  WriteByte(ADDR_FIXED); // Command1: Set data
  stop();
  start();
  WriteByte(bit_addr | 0xc0); // Command2: Set data (fixed address)
  WriteByte(seg_data);        // Transfer display data 8 bits
  stop();
  start();
  WriteByte(cmd_disp_ctrl); // Control display
  stop();
}

//--------------------------------------------------------

void TM1637_displayNum(float num, int decimal, bool show_minus)
{
  // Displays number with decimal places (no decimal point implementation)
  // Colon is used instead of decimal point if decimal == 2
  // Be aware of int size limitations (up to +-2^15 = +-32767)

  int number = fabs(num) * pow(10, decimal);

  for (int i = 0; i < NUMDIG - (show_minus && num < 0 ? 1 : 0); ++i)
  {
    int j = NUMDIG - i - 1;

    if (number != 0)
      display(j, number % 10);
    else
      display(j, 0x7f); // display nothing

    number /= 10;
  }

  if (show_minus && num < 0)
    display(0, '-'); // Display '-'

  if (decimal == 2)
    point(true);
  else
    point(false);
}

void TM1637_displayStr(char str[], uint16_t loop_delay)
{
  for (int i = 0; i < strlen(str); i++) {
    if (i + 1 > NUMDIG) {
      delay(loop_delay); //loop delay
      for (int d = 0; d < NUMDIG; d++) {
        display(d, str[d + i + 1 - NUMDIG]); //loop display
      }
    } else {
      display(i, str[i]);
    }
  }

  // display nothing
  for (int i = strlen(str); i < NUMDIG; i++) {
    display(i, 0x7f);
  }
}

void TM1637_clearDisplay(void)
{
  display(0x00, 0x7f);
  display(0x01, 0x7f);
  display(0x02, 0x7f);
  display(0x03, 0x7f);
}

// To take effect the next time it displays.
void TM1637_set(uint8_t brightness, uint8_t set_data, uint8_t set_addr)
{
  cmd_set_data = set_data;
  cmd_set_addr = set_addr;
  //Set the brightness and it takes effect the next time it displays.
  cmd_disp_ctrl = 0x88 + brightness;
}

// Whether to light the clock point ":".
// To take effect the next time it displays.
void TM1637_point(boolean PointFlag)
{
  _PointFlag = PointFlag;
}

void TM1637_coding(int8_t disp_data[])
{
  for (uint8_t i = 0; i < NUMDIG; i++)
    disp_data[i] = coding(disp_data[i]);
}

int8_t TM1637_coding(int8_t disp_data)
{
  if (disp_data == 0x7f)
    disp_data = 0x00; // Clear digit
  else if (disp_data >= 0 && disp_data < int(sizeof(tube_tab)/sizeof(*tube_tab)))
    disp_data = tube_tab[disp_data];
  else if ( disp_data >= '0' && disp_data <= '9' )
    disp_data = tube_tab[int(disp_data)-48]; // char to int (char "0" = ASCII 48)
  else
    disp_data = char2segments(disp_data);
  disp_data += _PointFlag == POINT_ON ? 0x80 : 0;

  return disp_data;
}

void BitDelay(void)
{
  //delayMicroseconds(50);
	_delay_us (DEFAULT_BIT_DELAY);
}
*/
