#ifndef HAL_H
#define HAL_H

	//#include <avr/io.h>
//	#include <avr/interrupt.h>
//	#include <avrlibtypes.h>
	//#include "avrlibdefs.h"
	//#include "avrlibtypes.h"
	//#include <avr/eeprom.h> 	// работа с eeprom
//	#include "avr/pgmspace.h"
//	#include <avr/wdt.h>
//	#include <util/delay.h>
	//#include "commons.h"

	//Clock Config
	//#define F_CPU 8000000L

	//System Timer Config
//	#define Prescaler	  		64
//	#define	TimerDivider  		(F_CPU/Prescaler/1000)		// 1 mS


#ifdef __AVR_AT90S8515__
	//PORT Defines
	#define IND_PORT		PORTA	// порт цифрового индикатора
	#define LED_PORT		PORTB	// порт светодиода
	#define MOC_PORT		PORTC	// порт симистора
	#define POZ_PORT		PORTC	// порт управления разрядами индикатора
	//PIN Defines
	#define PIN_LPOZ		5	// база транзистора (мл.разряд индикатора)
	#define PIN_HPOZ		4	// база транзистора (ст.разряд индикатора)
	#define PIN_LED			5	// светодиод режима
	#define PIN_MOC			3	// управление драйвером симистора
#endif

#ifdef __AVR_ATmega328P__
	//PORT Defines
	#define LED_PORT		PORTC	// порт светодиода
	#define TLED_PORT		PORTB	// порт тестового светодиода
	#define MOC_PORT		PORTC	// порт симистора

	//PIN Defines
	#define PIN_LED			1	// +светодиод режима 				(PC1)
	#define PIN_TLED		5	// +светодиод тестовый 				(PB5)
	#define PIN_MOC			0	// +управление драйвером симистора 	(PC0)
#endif

	
	#define Led_on()		do{PortReg(&LED_PORT) |=  _BV(PIN_LED);} while(0)
	#define Led_off()		do{PortReg(&LED_PORT) &=  ~(_BV(PIN_LED));} while(0)
	#define Led_xor()		do{PortReg(&LED_PORT) ^=  _BV(PIN_LED);} while(0)
	#define TLed_on()		do{PortReg(&TLED_PORT) |=  _BV(PIN_TLED);} while(0)
	#define TLed_off()		do{PortReg(&TLED_PORT) &=  ~(_BV(PIN_TLED));} while(0)
	#define Pow_On()		do{PortReg(&MOC_PORT) |=  _BV(PIN_MOC);} while(0)
	#define Pow_Off()		do{PortReg(&MOC_PORT) &=  ~(_BV( PIN_MOC));} while(0)


	extern void InitHAL(void);


#endif
