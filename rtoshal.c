/******************************************************************************************
 * rtoshal.c
 *
 *  Created on: 12.09.2017 0:06:38
 *      Author: kcm
 */

#include "rtoshal.h"
#include "commons.h"

volatile u08 arrayTail;                  // "хвост" очереди

/******************************************************************************************
 * Инициализация РТОС, время тика - 1 мс
 */
inline void RTOS_Init()
{
	arrayTail = 0;							// "хвост" в 0

	FLAG_OCFA_Reset							// сбросить флаг
	TIMER_COUNTER 	= TIMER_DefVal;			// загружаем начальное зн. в счетный регистр
	COMPARE_REG	  	= OCRA_TOP;				// загружаем регистр сравнения
	CTC_Mode_On								// включаем режим CTC
	COMPARE_A_IE							// Compare Match A Interrupt Enable
	Timer_RTS_Start							// старт (прескалер - 64)
}
