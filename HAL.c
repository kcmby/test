#include "HAL.h"
#include "commons.h"

inline void InitHAL(void)
{
	// Настройка портов (на выход)
	DirReg(&LED_PORT) 	= _BV(PIN_LED);
	DirReg(&TLED_PORT) 	= _BV(PIN_TLED);
	DirReg(&MOC_PORT) 	= _BV(PIN_MOC);

	//PortReg(&KEY_PORT) |= _BV(PIN_START) | _BV(PIN_MODE);//подключаем внутренние резистры
}
