/*
 * main.c
 *
 *  Created on: 04 апр. 2023 г.
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

volatile u08 CurID;							// текущий ID режима
volatile u08 CurMode;						// текущий режим работы
volatile u08 CurSubMode;					// подрежим текущего режима
volatile u08 ModeToSet;						// режим для которого производится настройка
//volatile register u08 EncFLAG	asm ("r4");	// флаг чтения энкодера
volatile u08 WeldFLAG;						// флаг "идет сварка"

volatile u16	Second_cnt;	// время на индикаторе в секундах
volatile u32	Pulse_cnt;	// длительность сварки в "тиках" (длительность 1-го импульса)
volatile u32	Pause_cnt;	// длительность паузы в "тиках"
volatile u08	Weld_cnt;	// длительность сварки в ручном режиме

u08	IndData[NUMDIG]; 		// массив данных для индикатора
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
// Инициализация устройств
	InitHAL();
	Encoder_init();
	BUT_Init();
	Sound_Init();
	TM1637_Init();
	RTOS_Init();
	ENABLE_INTERRUPT;

// Тестирование при включении
	CurMode = MODE_TEST;
	//EncFLAG = ENC_FLAG_DOWN;
	//WeldFLAG = WELD_NO;

// Чтение последних данных(настроек)  из EEPROM
	CurID = GetLastID();			// ...LastID
	CurOptions();					// ...последние настройки

	//SetLastID(4);
	//mode_options.d_min = 30;
	//mode_options.d_sec = 00;
	//WriteOptions (&mode_options, CurID);
	RTOS_SetTask (Tsk_Countdown, ONESECOND, ONESECOND);

// Запуск опроса кнопок
	RTOS_SetTask (Tsk_ButPoll, NO_DELAY, KEY_PERIOD);

// Главный цикл диспетчера
	while(1) {
		//wdt_reset();	// Сброс WDT
		RTOS_DispatchTask();

		tmp = BUT_GetBut();		// проверить буфер кнопки
		if (tmp) {
			// если была нажата(отпущена) кнопка
			keyid = tmp & 0x0F;
			if (BUT_GetState(keyid) != BUT_Disable) {
				// ... и кнопка не была ранее запрещена
				event = (tmp & 0xF0) >> 4;
				// соответствующий звук
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

				// ... и переход в соответстующий обработчик
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
				// ... и переход в соответстующий обработчик
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
 * Обработка нажатия кнопок в режиме Сварка
 * Подрежимы:
 * 	-Авто одиночный импульс
 * 	-Авто двойной импульс
 * 	-Ручной
 */

//void ButInWeld(u08 keyid, u08 event)
//{
/*	if ((keyid == BUT_ENC) && (event == BUT_LongPress) ) {
		CurMode = MODE_SETUP;							// переключить на режим настройки
		BUT_Ignor(BUT_MODE);							// игнорировать кн.переключения режимов
		BUT_Ignor(BUT_START);							// игнорировать кн. пуска
//			RTOS_SetTask (BlinkInd_On, NO_DELAY, BLINK_PERIOD);
//			RTOS_SetTask (Tsk_Encoder, 1, 1);				// запустить задачу сканирования энкодера
		return;
	}
	if (CurSubMode == SM_WELD_1) {
		switch(keyid) {
			case BUT_START:
				if (event == BUT_ShortPress) {
					// старт сварки с одиночным импульсом
					Pulse_cnt = Digit_cnt * 100;					// длительность сварки
		//			RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// запуск сварки
				}
				break;
			case BUT_MODE:
				if (event == BUT_LongPress) {
		//			CurMode = MODE_MANUAL;							// переключить на ручной режим
					BUT_Ignor(BUT_ENC);								// игнорировать кн. энкодера
		//			RTOS_SetTask (Tsk_Ind_Man, 1, 25);				// включить мерцание индикатора
				}
				break;
			}
		return;
	}
	if (CurSubMode == SM_WELD_2) {
		switch(keyid) {
			case BUT_START:
				if (event == BUT_ShortPress) {
					Pulse_cnt = Digit_cnt * 100;					// длительность сварки
		//			RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// запуск сварки
				}
				break;
			case BUT_MODE:
				if (event == BUT_LongPress) {
		//			CurMode = MODE_MANUAL;							// переключить на ручной режим
					BUT_Ignor(BUT_ENC);								// игнорировать кн. энкодера
		//			RTOS_SetTask (Tsk_Ind_Man, 1, 25);				// включить мерцание индикатора
				}
				break;
			}
		return;
	}
	if (CurSubMode == SM_WELD_M) {
		switch(keyid) {
			case BUT_START:
				if (event == BUT_ShortPress) {
					Pulse_cnt = Digit_cnt * 100;					// длительность сварки
		//			RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// запуск сварки
				}
				break;
			case BUT_MODE:
				if (event == BUT_LongPress) {
		//			CurMode = MODE_MANUAL;							// переключить на ручной режим
					BUT_Ignor(BUT_ENC);								// игнорировать кн. энкодера
		//			RTOS_SetTask (Tsk_Ind_Man, 1, 25);				// включить мерцание индикатора
				}
				break;
*/
/*			case BUT_ENC:
				if (event == BUT_LongPress) {
					CurMode = MODE_SETUP;							// переключить на режим настройки
					BUT_Ignor(BUT_MODE);							// игнорировать кн.переключения режимов
					BUT_Ignor(BUT_START);							// игнорировать кн. пуска
		//			RTOS_SetTask (BlinkInd_On, NO_DELAY, BLINK_PERIOD);
		//			RTOS_SetTask (Tsk_Encoder, 1, 1);				// запустить задачу сканирования энкодера
				}
			break;
*/
//			}
//		return;
//	}


//}

/******************************************************************************************
 * Обработка нажатия кнопок в режиме Засветка
 * Подрежимы:
 * 	-Короткая засветка (до 9 мин.59 сек.)
 * 	-Длительная засветка для маски (от 10 мин.)
 */
/*
void ButInLamp(u08 keyid, u08 event)
{
	return;
}
*/
/******************************************************************************************
 * Обработка нажатия кнопок в режиме Setup
 */
/*
void ButInSetup(u08 keyid, u08 event)
{
	return;
}
*/

void ButHandler_Start(u08 event)
{
	// сварка в автоматическом режиме
	if ((event == BUT_ShortPress) && (CurSubMode !=SM_WELD_M)) {
		if (WeldFLAG == WELD_NO)
			Power();
		return;
	}
	// сварка в ручном режиме
	if (CurSubMode == SM_WELD_M) {
		switch(event) {
			case BUT_Pressed:
				Second_cnt = 0;
				Pulse_cnt = MAX_WELDING;
				RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// запуск сварки
				RTOS_SetTask (Tsk_Countup, ONESECOND, ONESECOND);
				break;
			case BUT_Released:
				if (WeldFLAG != WELD_NO) {
					Tsk_Symistor_Off();
					RTOS_DeleteTask(Tsk_Symistor_Off);
					RTOS_DeleteTask(Tsk_Countup);
				}
				//ShowCurSettings(); 							// вывод на индикатор параметров
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
 * Включить симистор и подать сеть в нагрузку
 */
void Power()
{
	// В каком режиме
	if (CurMode == MODE_LAMP) {
		// время на индикаторе перевести в секунды
		Time2Second();

		/* запретить все кнопки кроме BUT_MODE
		 * (в этом режиме это стоп)
		 */

		// поднять флаги (если нужно)

		// запустить задачу на включение семистора
		Pulse_cnt = Second_cnt * 1000;					// длительность сварки
		RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// запуск сварки

		// запустить задачу на обратный отсчет
		RTOS_SetTask (Tsk_Countdown, ONESECOND, ONESECOND);
	}
	else {
		switch(CurSubMode) {
			case SM_WELD_1:
				Pulse_cnt = IndData[3] * 10;
				RTOS_SetTask (Tsk_Symistor1_On, 1, NO_PERIOD);	// запуск сварки
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
/* очередной запуск после срабатывания энкодера */
		// вычисляем ID выбранного подрежима
		tmp_id = CurID + (EncData - mode_options.submode);


	}
	else {
		first ^= 1;
		if (first) {
			/* кнопка энкодера была нажата 1-й раз (первый запуск) */
			Display_Dig(mode_options.name);					// вывод на инд. название подрежима
			// лимиты для енкодера
			if (mode_options.mode == MODE_WELD) 	enc_max = 3;
			else 									enc_max = 2;
			SetEncoder_Lim(1, enc_max);

			EncData = mode_options.submode;					// текущее значение для енкодера
			BUT_Ignor(BUT_MODE);							// игнорировать кн.переключения режимов
			BUT_Ignor(BUT_START);							// игнорировать кн. пуска
			// здесь ище может быть мигание;
			Fun2enc = GhangeSubMode;						// вернуться сюда после срабатывания энкодера
			RTOS_SetTask (Tsk_Encoder, 1, 1);				// запустить задачу сканирования энкодера
		}
		else {
			/* кнопка энкодера была нажата 2-й раз (сохранение выбора) */
			RTOS_DeleteTask(Tsk_Encoder);
			if (tmp_id != CurID) {
				CurID = tmp_id;
//				eeprom_write_byte (&LastID, tmp_id);
//				if (mode_options.mode == MODE_WELD) 	eeprom_write_byte (&LastWeldID, tmp_id);
//				else 									eeprom_write_byte (&LastLampID, tmp_id);
				ReadCurOptions();		// ...последние настройки
				BUT_Activ(BUT_MODE);
				BUT_Activ(BUT_START);
			}
		}
	}

}

void CurOptions(void)
{
	//u08 tmp;

	ReadCurOptions();		// ...последние настройки
	//CurMode = mode_options.mode;
	//CurSubMode = mode_options.submode;
	//tmp = ForSafety();
	ForSafety();

// Вывод текущей инф. на индикатор
	Display_Dig(mode_options.name);		// в течении 1сек. название режима
	_delay_ms(ONESECOND);
	SetDots(mode_options.dots);			// короткий сигнал
	Sound(ShortSignal, 1);

	ShowCurSettings(); 					// вывод на индикатор тек. параметров
}

void ReadCurOptions(void)
{
	ReadOptions (&mode_options, CurID);		// ...последние настройки
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
	// вывод на индикатор параметров
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
	SetBlink(digit);													// мигающие разряды
	//RTOS_SetTask (Tsk_IndBlink_On, NO_DELAY, BLINK_PERIOD, NULL);		// запуск мигания определенных разрядов индикатора
	RTOS_SetTask (Tsk_IndBlink_On, NO_DELAY, BLINK_PERIOD);		// запуск мигания определенных разрядов индикатора
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
 * Задачи вызываемые через диспетчер
 ******************************************************************************************/

/******************************************************************************************
 * Звук
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

	Sound_On();	// Вкл. звук

	t = DurationSnd();
	//RTOS_SetTask (Tsk_Sound_Off, t, NO_PERIOD, NULL);	// ...и Выкл через t
	RTOS_SetTask (Tsk_Sound_Off, t, NO_PERIOD);	// ...и Выкл через t
}

/******************************************************************************************
 * Индикатор
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
 * Включить симистор
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
 * Включить симистор на 1-й импульс
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
 * Отработать паузу перед 2-м импульсом
 */
void Tsk_Symistor_pause()
{
	Pow_Off();
	RTOS_SetTask (Tsk_Symistor1_On, Pause_cnt, NO_PERIOD); // ... и после паузы включить на 2-й

	Led_on();
}


/******************************************************************************************
 * Выключить симистор
 */
void Tsk_Symistor_Off()
{
	Pow_Off();

	Led_off();
	//BUT_Activ(BUT_START);
	if (CurSubMode != SM_WELD_M) {
		// если сварка проходит в автоматическом режиме
		BUT_Activ(BUT_ENC);
	}
	else {
		// если сварка проходит в ручном режиме
		RTOS_DeleteTask(Tsk_Countup);			// остановить счетчик длительности сварки
//		RTOS_SetTask (Tsk_Ind_Man, 800, 25);		// включить мерцание индикатора (с задержкой)
	}
	ShowCurSettings(); 							// вывод на индикатор параметров
	WeldFLAG = WELD_NO;
}

/******************************************************************************************
 * Запуск задачи сканирования энкодера
 */
void Tsk_Encoder()
{
	if (Encoder_Scan()) {
		(*Fun2enc)(1);	// вернутся в функцию, запустившую задачу сканирования энкодера
	}
}



/******************************************************************************************
 * Короткие задачи вызываемые в прерывании
 ******************************************************************************************/
/*
void I_PowerOff()
{
	Pow_Off();
}
*/
