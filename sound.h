/*
 * sound.h
 *
 *  Created on: 18 июл. 2019 г.
 *      Author: SKudich
 */

#ifndef SOUND_H_
#define SOUND_H_

#include "commons.h"

#define SOUND_PORT		PORTD	// порт звука
#define PIN_SND			6		// пин звука

#define Tone			0x31	// частота звука
#define ShortSignal		50					// длительность короткого сигнала
#define LongSignal		500					// длительность длинного сигнала
#define PauseSignal		200					// пауза между сигналами

// 8-bit Timer/Counter0
#define Sound_On()	do{TCCR0B = ((1<<CS01)|(1<<CS00));} while(0)	// вкл. пищалку (делитель 64)
#define Sound_Off()	do{TCCR0B = 0; TCNT0 = 0;} while(0)				// выкл. пищалку

/******************************************************************************************
 * Прототипы фукнций
 */
void Sound_Init(void);
void SetSnd(u16 signal, u08 times);
u08 NextSnd(void);
u16 DurationSnd(void);

#endif /* SOUND_H_ */
