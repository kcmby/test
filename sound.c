/*
 * sound.c
 *
 *  Created on: 18 ˜˜˜. 2019 ˜.
 *      Author: SKudich
 */
#include "sound.h"

/******************************************************************************************
 * ˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜
 */
volatile static u08 NumPic;					// ˜˜˜˜˜˜˜ ˜˜˜ "˜˜˜˜˜˜˜"
volatile static u16 DurPic;					// ˜˜˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜

/******************************************************************************************
 * ˜˜˜˜˜˜˜˜˜˜˜˜˜ Sound
 */
inline void Sound_Init(void)
{

	// TCNT0 ˜ Timer/Counter Register
	// TCCR0A ˜ Timer/Counter Control Register A
	// TCCR0B ˜ Timer/Counter Control Register B
	// OCR0A ˜ Output Compare Register A

	DirReg(&SOUND_PORT) |= _BV(PIN_SND);
	TCNT0 = 0;
	TCCR0A = ((1<<COM0A0)|(1<<WGM01));	// Toggle OC0A on Compare Match, CTC
	OCR0A = Tone;
	TCCR0B = 0;							// Stop Timer0
	//TCCR0B = ((1<<CS01)|(1<<CS00));		// Start, prescaler-64
}

void SetSnd(u16 sig, u08 tim)
{
	Sound_On();

	NumPic = tim;
	DurPic = sig;
}

inline u08 NextSnd(void)
{
	if (NumPic)	--NumPic;
	return NumPic;
}

inline u16 DurationSnd(void)
{
	return DurPic;
}
