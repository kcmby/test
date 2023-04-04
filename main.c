/*
 * main.c
 *
 *  Created on: 04 ���. 2023 �.
 *      Author: SKudich
 */
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "HAL.h"
#include "ee_special.h"
#include "rtos2.h"
#include "buttons.h"
#include "encoder.h"
#include "sound.h"
#include "TM1637.h"

volatile u08 CurID;							// ������� ID ������
volatile u08 CurMode;						// ������� ����� ������
volatile u08 CurSubMode;					// �������� �������� ������
volatile u08 ModeToSet;						// ����� ��� �������� ������������ ���������
//volatile register u08 EncFLAG	asm ("r4");	// ���� ������ ��������
volatile u08 WeldFLAG;						// ���� "���� ������"

volatile u16	Second_cnt;	// ����� �� ���������� � ��������
volatile u32	Pulse_cnt;	// ������������ ������ � "�����" (������������ 1-�� ��������)
volatile u32	Pause_cnt;	// ������������ ����� � "�����"
volatile u08	Weld_cnt;	// ������������ ������ � ������ ������

u08	IndData[NUMDIG]; 		// ������ ������ ��� ����������
tmode mode_options;

volatile u08 EncData;
void (*Fun2enc)(u08);

//==============================================================================
int main(void)
{
	u08 tmp;
	u08 keyid, event;

#ifndef BUTWITHSOUND
	u16 dur;
	u08 num;
#endif
// ������������� ���������
	InitHAL();
	Encoder_init();
	BUT_Init();
	Sound_Init();
	TM1637_Init();
	RTOS_Init();
	ENABLE_INTERRUPT;

// ������������ ��� ���������
	CurMode = MODE_TEST;
	//EncFLAG = ENC_FLAG_DOWN;
	//WeldFLAG = WELD_NO;

// ������ ��������� ������(��������)  �� EEPROM
	CurID = GetLastID();			// ...LastID
	CurOptions();					// ...��������� ���������

	//SetLastID(4);
	//mode_options.d_min = 30;
	//mode_options.d_sec = 00;
	//WriteOptions (&mode_options, CurID);
	RTOS_SetTask (Tsk_Countdown, ONESECOND, ONESECOND);

// ������ ������ ������
	RTOS_SetTask (Tsk_ButPoll, NO_DELAY, KEY_PERIOD);

// ������� ���� ����������
	while(1) {
		//wdt_reset();	// ����� WDT
		RTOS_DispatchTask();

		tmp = BUT_GetBut();		// ��������� ����� ������
		if (tmp) {
			// ���� ���� ������(��������) ������
			keyid = tmp & 0x0F;
			if (BUT_GetState(keyid) != BUT_Disable) {
				// ... � ������ �� ���� ����� ���������
				event = (tmp & 0xF0) >> 4;
				// ��������������� ����
				dur = ShortSignal;	num = 1;
				switch(event) {
				case BUT_LongPress:
					dur = LongSignal;
					break;
				case BUT_DblPress:
					num = 2;
					break;
				}
				//if (event != BUT_NoEvent && event != BUT_Error)	Sound(dur, num);
				Sound(dur, num);

				// ... � ������� � �������������� ����������
				switch(keyid) {
				case BUT_START:
					ButHandler_Start(event);
					break;
				case BUT_ENC:
					ButHandler_Enc(event);
					break;
				case BUT_MODE:
					ButHandler_Mode(event);
					break;
				}

/*
				// ... � ������� � �������������� ����������
				switch(CurMode) {
				case MODE_WELD:
					if (WeldFLAG == WELD_NO) {
						ButInWeld(keyid, event);
					}
					break;
				case MODE_LAMP:
					ButInLamp(keyid, event);
					break;
				case MODE_SETUP:
					ButInSetup(keyid, event);
					break;
				}
*/
			}
		}
	}
	return 0;
}


/******************************************************************************************
 * ��������� ������� ������ � ������ ������
 * ���������:
 * 	-���� ��������� �������
 * 	-���� ������� �������
 * 	-������
 */

//void ButInWeld(u08 keyid, u08 event)
//{
/*	if ((keyid == BUT_ENC) && (event == BUT_LongPress) ) {
		CurMode = MODE_SETUP;							// ����������� �� ����� ���������
		BUT_Ignor(BUT_MODE);							// ������������ ��.������������ �������
		BUT_Ignor(BUT_START);							// ������������ ��. �����
//			RTOS_SetTask (BlinkInd_On, NO_DELAY, BLINK_PERIOD);
//			RTOS_SetTask (Tsk_Encoder, 1, 1);				// ��������� ������ ������������ ��������
		return;
	}
	if (CurSubMode == SM_WELD_1) {
		switch(keyid) {
			case BUT_START:
				if (event == BUT_ShortPress) {
					// ����� ������ � ��������� ���������
					Pulse_cnt = Digit_cnt * 100;					// ������������ ������
		//			RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// ������ ������
				}
				break;
			case BUT_MODE:
				if (event == BUT_LongPress) {
		//			CurMode = MODE_MANUAL;							// ����������� �� ������ �����
					BUT_Ignor(BUT_ENC);								// ������������ ��. ��������
		//			RTOS_SetTask (Tsk_Ind_Man, 1, 25);				// �������� �������� ����������
				}
				break;
			}
		return;
	}
	if (CurSubMode == SM_WELD_2) {
		switch(keyid) {
			case BUT_START:
				if (event == BUT_ShortPress) {
					Pulse_cnt = Digit_cnt * 100;					// ������������ ������
		//			RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// ������ ������
				}
				break;
			case BUT_MODE:
				if (event == BUT_LongPress) {
		//			CurMode = MODE_MANUAL;							// ����������� �� ������ �����
					BUT_Ignor(BUT_ENC);								// ������������ ��. ��������
		//			RTOS_SetTask (Tsk_Ind_Man, 1, 25);				// �������� �������� ����������
				}
				break;
			}
		return;
	}
	if (CurSubMode == SM_WELD_M) {
		switch(keyid) {
			case BUT_START:
				if (event == BUT_ShortPress) {
					Pulse_cnt = Digit_cnt * 100;					// ������������ ������
		//			RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// ������ ������
				}
				break;
			case BUT_MODE:
				if (event == BUT_LongPress) {
		//			CurMode = MODE_MANUAL;							// ����������� �� ������ �����
					BUT_Ignor(BUT_ENC);								// ������������ ��. ��������
		//			RTOS_SetTask (Tsk_Ind_Man, 1, 25);				// �������� �������� ����������
				}
				break;
*/
/*			case BUT_ENC:
				if (event == BUT_LongPress) {
					CurMode = MODE_SETUP;							// ����������� �� ����� ���������
					BUT_Ignor(BUT_MODE);							// ������������ ��.������������ �������
					BUT_Ignor(BUT_START);							// ������������ ��. �����
		//			RTOS_SetTask (BlinkInd_On, NO_DELAY, BLINK_PERIOD);
		//			RTOS_SetTask (Tsk_Encoder, 1, 1);				// ��������� ������ ������������ ��������
				}
			break;
*/
//			}
//		return;
//	}


//}

/******************************************************************************************
 * ��������� ������� ������ � ������ ��������
 * ���������:
 * 	-�������� �������� (�� 9 ���.59 ���.)
 * 	-���������� �������� ��� ����� (�� 10 ���.)
 */
/*
void ButInLamp(u08 keyid, u08 event)
{
	return;
}
*/
/******************************************************************************************
 * ��������� ������� ������ � ������ Setup
 */
/*
void ButInSetup(u08 keyid, u08 event)
{
	return;
}
*/

void ButHandler_Start(u08 event)
{
	// ������ � �������������� ������
	if ((event == BUT_ShortPress) && (CurSubMode !=SM_WELD_M)) {
		if (WeldFLAG == WELD_NO)
			Power();
		return;
	}
	// ������ � ������ ������
	if (CurSubMode == SM_WELD_M) {
		switch(event) {
			case BUT_Pressed:
				Second_cnt = 0;
				Pulse_cnt = MAX_WELDING;
				RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// ������ ������
				RTOS_SetTask (Tsk_Countup, ONESECOND, ONESECOND);
				break;
			case BUT_Released:
				if (WeldFLAG != WELD_NO) {
					Tsk_Symistor_Off();
					RTOS_DeleteTask(Tsk_Symistor_Off);
					RTOS_DeleteTask(Tsk_Countup);
				}
				//ShowCurSettings(); 							// ����� �� ��������� ����������
				break;
		}
	}
	return;
}
void ButHandler_Enc(u08 event)
{

	return;
}
void ButHandler_Mode(u08 event)
{
	if (event == BUT_DblPress) {
		CurMode ^= 0b11;
		if (CurMode == MODE_LAMP)
			CurID = GetLastLampID();
		else
			CurID = GetLastWeldID();
		CurOptions();
		return;
	}
	if (event == BUT_ShortPress) {
		Tsk_Symistor_Off();
		RTOS_DeleteTask(Tsk_Symistor_Off);
		RTOS_DeleteTask(Tsk_Countup);
	}

	return;
}




/******************************************************************************************
 * �������� �������� � ������ ���� � ��������
 */
void Power()
{
	// � ����� ������
	if (CurMode == MODE_LAMP) {
		// ����� �� ���������� ��������� � �������
		Time2Second();

		/* ��������� ��� ������ ����� BUT_MODE
		 * (� ���� ������ ��� ����)
		 */

		// ������� ����� (���� �����)

		// ��������� ������ �� ��������� ���������
		Pulse_cnt = Second_cnt * 1000;					// ������������ ������
		RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// ������ ������

		// ��������� ������ �� �������� ������
		RTOS_SetTask (Tsk_Countdown, ONESECOND, ONESECOND);
	}
	else {
		switch(CurSubMode) {
			case SM_WELD_1:
				Pulse_cnt = IndData[3] * 10;
				RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// ������ ������
				break;
			case SM_WELD_2:
				Pulse_cnt = IndData[3] * 10;
				Pause_cnt = IndData[2] * 10;
				RTOS_SetTask (Tsk_Symistor2_On, 1, NO_PERIOD);
				Pulse_cnt = IndData[1] * 10;
				break;
			//case SM_WELD_M:
			//	break;
		}
	}

}

void GhangeSubMode(u08 flag)
{
	static u08 first = 0;
	static u08 tmp_id = 0;
	u08 enc_max;
	//s08 x;

	if (flag) {
/* ��������� ������ ����� ������������ �������� */
		// ��������� ID ���������� ���������
		tmp_id = CurID + (EncData - mode_options.submode);


	}
	else {
		first ^= 1;
		if (first) {
			/* ������ �������� ���� ������ 1-� ��� (������ ������) */
			Display_Dig(mode_options.name);					// ����� �� ���. �������� ���������
			// ������ ��� ��������
			if (mode_options.mode == MODE_WELD) 	enc_max = 3;
			else 									enc_max = 2;
			SetEncoder_Lim(1, enc_max);

			EncData = mode_options.submode;					// ������� �������� ��� ��������
			BUT_Ignor(BUT_MODE);							// ������������ ��.������������ �������
			BUT_Ignor(BUT_START);							// ������������ ��. �����
			// ����� ��� ����� ���� �������;
			Fun2enc = GhangeSubMode;						// ��������� ���� ����� ������������ ��������
			RTOS_SetTask (Tsk_Encoder, 1, 1);				// ��������� ������ ������������ ��������
		}
		else {
			/* ������ �������� ���� ������ 2-� ��� (���������� ������) */
			RTOS_DeleteTask(Tsk_Encoder);
			if (tmp_id != CurID) {
				CurID = tmp_id;
//				eeprom_write_byte (&LastID, tmp_id);
//				if (mode_options.mode == MODE_WELD) 	eeprom_write_byte (&LastWeldID, tmp_id);
//				else 									eeprom_write_byte (&LastLampID, tmp_id);
				ReadCurOptions();		// ...��������� ���������
				BUT_Activ(BUT_MODE);
				BUT_Activ(BUT_START);
			}
		}
	}

}

void CurOptions(void)
{
	//u08 tmp;

	ReadCurOptions();		// ...��������� ���������
	//CurMode = mode_options.mode;
	//CurSubMode = mode_options.submode;
	//tmp = ForSafety();
	ForSafety();

// ����� ������� ���. �� ���������
	Display_Dig(mode_options.name);		// � ������� 1���. �������� ������
	_delay_ms(ONESECOND);
	SetDots(mode_options.dots);			// �������� ������
	Sound(ShortSignal, 1);

	ShowCurSettings(); 					// ����� �� ��������� ���. ����������
}

void ReadCurOptions(void)
{
	ReadOptions (&mode_options, CurID);		// ...��������� ���������
	CurMode = mode_options.mode;
	CurSubMode = mode_options.submode;
}

void Sound(u16 sig, u08 tim)
{
	//Sound_On
	SetSnd(sig, tim);
	Tsk_Sound_On ();
}

void ShowCurSettings(void)
{
	// ����� �� ��������� ����������
	switch(mode_options.mode) {
	case MODE_WELD:
		IndData[0] = 'P';
		if (CurSubMode == SM_WELD_1) {
			IndData[1] = ' ';
			IndData[2] = ' ';
			IndData[3] = mode_options.pulse1;
		}
		if (CurSubMode == SM_WELD_2) {
			IndData[1] = mode_options.pulse1;
			IndData[2] = mode_options.pauza;
			IndData[3] = mode_options.pulse2;
		}
		if (CurSubMode == SM_WELD_M) {
			IndData[1] = ' ';
			IndData[2] = 0;
			IndData[3] = 0;
		}
		break;
	case MODE_LAMP:
		Opt2Ind(mode_options.d_min, mode_options.d_sec);
		break;
	}
	Display_Dig(IndData);
}

void BlinkDigit_On(u08 digit)
{
	SetBlink(digit);													// �������� �������
	//RTOS_SetTask (Tsk_IndBlink_On, NO_DELAY, BLINK_PERIOD, NULL);		// ������ ������� ������������ �������� ����������
	RTOS_SetTask (Tsk_IndBlink_On, NO_DELAY, BLINK_PERIOD);		// ������ ������� ������������ �������� ����������
}

void BlinkDigit_Off()
{
	SetBlink(DIG_NONE);
	RTOS_DeleteTask (Tsk_IndBlink_On);
	Display_Dig(IndData);
}

void Sek2Ind(u16 sek)
{
	u08 s, m;

	s = sek % 60;
	m = sek / 60;

	Opt2Ind(m, s);
//	IndData[3] = s % 10;
//	IndData[2] = s / 10;
//	IndData[1] = m % 10;
//	IndData[0] = m / 10;
}

void Opt2Ind(u08 m, u08 s)
{
		IndData[3] = s % 10;
		IndData[2] = s / 10;
		IndData[1] = m % 10;
		IndData[0] = m / 10;
}

void Time2Second()
{
	Second_cnt = (IndData[0]*10 + IndData[1])*60 + IndData[2]*10 + IndData[3];
}

/******************************************************************************************
 * ������ ���������� ����� ���������
 ******************************************************************************************/

/******************************************************************************************
 * ����
 */
void Tsk_Sound_Off ()
{
	Sound_Off();

	if (NextSnd())
		//RTOS_SetTask (Tsk_Sound_On, PauseSignal, NO_PERIOD, NULL);
		RTOS_SetTask (Tsk_Sound_On, PauseSignal, NO_PERIOD);
	else
		RTOS_DeleteTask (Tsk_Sound_On);
}

void Tsk_Sound_On ()
{
	u32 t;

	Sound_On();	// ���. ����

	t = DurationSnd();
	//RTOS_SetTask (Tsk_Sound_Off, t, NO_PERIOD, NULL);	// ...� ���� ����� t
	RTOS_SetTask (Tsk_Sound_Off, t, NO_PERIOD);	// ...� ���� ����� t
}

/******************************************************************************************
 * ���������
 */
void Tsk_IndBlink_On ()
{
	Display_Dig(IndData);
}

void Tsk_Countdown()
{
	//u16 sek;

	//sek = (IndData[0]*10 + IndData[1])*60 + IndData[2]*10 + IndData[3];
	if(Second_cnt)	{
		--Second_cnt;
		Sek2Ind(Second_cnt);
		Display_Dig(IndData);
		if (Second_cnt <= 10) 	Sound(ShortSignal, 1);
	}
	else {
		RTOS_DeleteTask (Tsk_Countdown);
		Sound(LongSignal, 3);
	}
}


void Tsk_Countup()
{
	++Second_cnt;
	Sek2Ind(Second_cnt);
	Display_Dig(IndData);
}

/******************************************************************************************
 * �������� ��������
 */
void Tsk_Symistor1_On()
{
	Pow_On();
	RTOS_SetTask (Tsk_Symistor_Off, Pulse_cnt, NO_PERIOD);

	Led_on();
	WeldFLAG = WELD_YES;
	if (CurMode != MODE_LAMP)
		BUT_Ignor(BUT_MODE);
	BUT_Ignor(BUT_ENC);
}

/******************************************************************************************
 * �������� �������� �� 1-� �������
 */
void Tsk_Symistor2_On()
{
	Pow_On();
	RTOS_SetTask (Tsk_Symistor_pause, Pulse_cnt, NO_PERIOD);

	Led_on();
	WeldFLAG = WELD_YES;
	BUT_Ignor(BUT_MODE);
	BUT_Ignor(BUT_ENC);
}


/******************************************************************************************
 * ���������� ����� ����� 2-� ���������
 */
void Tsk_Symistor_pause()
{
	Pow_Off();
	RTOS_SetTask (Tsk_Symistor1_On, Pause_cnt, NO_PERIOD); // ... � ����� ����� �������� �� 2-�

	Led_on();
}


/******************************************************************************************
 * ��������� ��������
 */
void Tsk_Symistor_Off()
{
	Pow_Off();

	Led_off();
	//BUT_Activ(BUT_START);
	if (CurSubMode != SM_WELD_M) {
		// ���� ������ �������� � �������������� ������
		BUT_Activ(BUT_ENC);
	}
	else {
		// ���� ������ �������� � ������ ������
		RTOS_DeleteTask(Tsk_Countup);			// ���������� ������� ������������ ������
//		RTOS_SetTask (Tsk_Ind_Man, 800, 25);		// �������� �������� ���������� (� ���������)
	}
	ShowCurSettings(); 							// ����� �� ��������� ����������
	WeldFLAG = WELD_NO;
}

/******************************************************************************************
 * ������ ������ ������������ ��������
 */
void Tsk_Encoder()
{
	if (Encoder_Scan()) {
		(*Fun2enc)(1);	// �������� � �������, ����������� ������ ������������ ��������
	}
}



/******************************************************************************************
 * �������� ������ ���������� � ����������
 ******************************************************************************************/
/*
void I_PowerOff()
{
	Pow_Off();
}
*/
