#ifndef ENCODER_H
#define ENCODER_H

	#include "commons.h"

	//#define ENC_LIMITS		// см.ниже
	#define ENC_TIKS	 	4		/*количество состояний за один "щелчок"*/

	#define ENCODER_PORT	PORTC	/*регистр порта энкодера*/
	#define PIN_ENC_A			2		/*вывод 0 энкодера*/
	#define PIN_ENC_B			3		/*вывод 1 энкодера*/

	#ifdef ENC_LIMITS
	#define ENC_MIN		1
	#define ENC_MAX		99
	#endif

	extern volatile u08 EncData;      			// Глобальные переменные состояния и данных энкодера

	// возможные состояния выводов энкодера:
	#define state_0      0x00  								/*состояние 0 выводов энкодера*/
	#define state_1      _BV(PIN_ENC_A)             		/*состояние 1 выводов энкодера*/
	#define state_2      _BV(PIN_ENC_B)             		/*состояние 2 выводов энкодера*/
	#define state_3      _BV(PIN_ENC_B) + _BV(PIN_ENC_A)	/*состояние 3 выводов энкодера*/

	/****Прототипы функций ****/
	
	void Encoder_init(void);               		// Функция инициализации энкодера
	u08 Encoder_Scan(void);						// Функция обработки энкодера
	#ifndef ENC_LIMITS
	void SetEncoder_Lim(u08 l_min, u08 l_max);	// Функция установки лимитов энкодера
	#endif

#endif


