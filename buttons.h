/*
 * buttons.h
 *
 *  Created on: 13 авг. 2019 г.
 *      Author: KCM
 *      Опрос кнопок производится каждые KEY_PERIOD
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

	//#define BUTWITHSOUND		// соответствующий звук при добавлении кн. в буфер

	#include "commons.h"
//	#include "buttonsdef.h"

	#define KEY_PERIOD		10	// периодичность опроса кнопок (10mS)
	#define MAX_BUTTONS		3	// Количество кнопок
	enum buttons {BUT_START,BUT_ENC,BUT_MODE};

		//PORT Defines
	//#define ENC_PORT		PORTC	// порт энкодера
	#define KEY_PORT		PORTD	// порт кнопок кправления

	//#define PIN_ENC_A		1	// +сигнал A энкодера 				(PC1)
	//#define PIN_ENC_B		0	// +сигнал B энкодера 				(PC0)
	#define PIN_START		0	// кнопка Пуск						(PD0)
	#define PIN_ENC_D		1	// кнопка энкодера			 		(PD1)
	#define PIN_MODE		2	// тумблер Автомат-Ручной			(PD2)


	typedef struct {u08 enable		:  1;	// проверять ли состояние кн. (1-Да, 0-Нет)
					u08 change		:  1;	// признак изменения состояния кнопки
					u08 isswich		:  1;	// признак переключателя (не кнопки)
					u08 en_long		:  1;	// для кн. разрешено длинное нажатие
					u08 en_hold		:  1;	// для кн. разрешено удержание
					u08 def_val		:  1;	// состояние кнопки по умолчанию
					u08 pre_val		:  1;	// предыдущее состояние кнопки
	} str_but;

	#define BUT_Disable			NO
	#define BUT_Enable			YES

	// временные интервалы
	#define TIME_STD			40		// для фиксации одиночного(двойного) нажатия (40 * 10mS = 400 mS)
	#define TIME_LNG			150		// для фиксации длинного нажатия (150 * 10mS = 1,5 S)
	//#define TIME_ERR			255		// предельный временной интервал (255 * 10mS = 2,5 S)

	// события кнопки (не изменять)
	#define BUT_NoEvent			0b000	// нет событий
	#define BUT_ShortPress		0b001	// короткое нажатие
	#define BUT_LongPress		0b010	// длинное нажатие
	#define BUT_DblPress		0b011	// двойное нажатие
	#define SW_Off				0b100	// переключатель в положении 0
	#define SW_On				0b101	// переключатель в положении 1
	#define BUT_Released		0b100	// кн. была отпущена
	#define BUT_Pressed			0b101	// кн. была нажата
	#define BUT_Holded			0b110	// кн. удерживается
	#define BUT_Error			0xFF	// неизвесное собыние

	/*размер буфера событий.
	Его значение должно быть
	кратно степени двойки (2, 4, 8, 16...).*/
	#define BUT_SIZE_BUF	2

	/**************** пользовательские функции *****************/

	/*инициализация.
	вызывается в начале программы*/
	void BUT_Init(void);

	/*опрос кнопок/входов.
	вызывается периодически (период 10mS)*/
	void Tsk_ButPoll(void);

	/*взять событие и ID кнопки из буфера.
	4 ст.бита - событие; 4 мл.бита ID кнопки.*/
	u08 BUT_GetBut(void);

	/* Читать статус кнопки */
	u08 BUT_GetState(u08);


	/* Не анализировать состояние кнопки
	*/
	void BUT_Ignor(u08);

	/* Анализировать состояние кнопки
	*/
	void BUT_Activ(u08);

	/* Проверка исходного состояния кнопок
	 * 		Если все кнопки в исходном положении - возвращает 0
	 * 		Если нет - код кнопки, которая находится в другом положении */
	u08 BUT_IsOk(void);

#endif /* BUTTONS_H_ */
