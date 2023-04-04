/*
 * ee_special.c
 *
 *  Created on: 14 сент. 2019 г.
 *      Author: KCM
 */

	#include "ee_special.h"
	#include "TM1637.h"

	u08	LastXXX EEMEM = 200;						// мусор
	u08 eepromstring[] EEMEM ={"n_o_t_h_i_n_g\0"};	// мусор
	u08 kcm[] EEMEM = {'K','C','M','-','2','0','1','9',' ','U','n','i','T','i','m','e','r',' ','1','.','0'};

	/********Данные в EEPROM **********/
	u08	LastID EEMEM = 4;
	u08	LastWeldID EEMEM = 1;
	u08	LastLampID EEMEM = 4;
	tmode ee_opt [MAX_SUBMODES] EEMEM = {
			{MODE_WELD, SM_WELD_1, "U-1 ", 0, 0, 8, 0, 0, DIG_4},
			{MODE_WELD, SM_WELD_2, "U-2 ", 0, 0, 5, 2, 4, DIG_2+DIG_3+DIG_4},
			{MODE_WELD, SM_WELD_M, "U-P ", 0, 0, 8, 0, 0, DIG_4},
			{MODE_LAMP, SM_LAMP_S, "L-1 ", 4, 0, 0, 0, 0, DIG_2},
			{MODE_LAMP, SM_LAMP_L, "L-2 ", 33, 10, 0, 0, 0, DIG_2}
	};

	/********Прототипы функций **********/

	inline u08 GetLastID(void)
	{
		return eeprom_read_byte (&LastID);
	}

	inline u08 GetLastWeldID(void)
	{
		return eeprom_read_byte (&LastWeldID);
	}

	inline u08 GetLastLampID(void)
	{
		return eeprom_read_byte (&LastLampID);
	}

	inline void SetLastID(u08 id)
	{
		eeprom_write_byte (&LastID, id);
	}

	inline u08 ForSafety(void)
	{
		return eeprom_read_byte (&LastXXX);
	}

	void ReadOptions (tmode *buffer, u08 id)
	{
		eeprom_read_block (buffer, &ee_opt[id], sizeof(tmode));
	}

	void WriteOptions (tmode *buffer, u08 id)
	{
		eeprom_write_block (buffer, &ee_opt[id], sizeof(tmode));
	}
/*
	void SetOption (u08 id, u08 *opt, u08 val)
	{
		eeprom_write_byte (&ee_opt[id]->, val);
	}
*/
