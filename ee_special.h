/*
 * ee_special.h
 *
 *  Created on: 14 сент. 2019 г.
 *      Author: KCM
 */

#ifndef EE_SPECIAL_H_
#define EE_SPECIAL_H_

	#include <avr/eeprom.h> 	// работа с eeprom
	#include "commons.h"
	#include "modes.h"

	typedef struct tmode
	{
		//u08 id;				// id записи
		u08 mode;			// режим таймера
		u08 submode;		// подрежим таймера
		u08 name[4];		// название режима на индикаторе
		u08 d_min;			// длительность (минут)
		u08 d_sec;			// длительность (секунд)
		u08 pulse1;			// длительность 1-го импульса (в периодах)
		u08 pauza;			// пауза между импульсами (в периодах)
		u08 pulse2;			// длительность 2-го импульса (в периодах)
		//u08 enc_min;		// минимальное значение для настройки
		//u08 enc_max;		// максимальное значение для настройки
		u08 dots;			// позиция точки на индикаторе
	}tmode;


	u08 GetLastID(void);
	void SetLastID(u08 id);
	u08 GetLastLampID(void);
	u08 GetLastWeldID(void);
	u08 ForSafety(void);
	void ReadOptions (tmode *buffer, u08 id);
	void WriteOptions (tmode *buffer, u08 id);


#endif /* EE_SPECIAL_H_ */
