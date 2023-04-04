/*
 * buttons.c
 *
 *  Created on: 24 дек. 2017 г.
 *      Author: KCM
 */

#include "buttons.h"

/******************************************************************************************
 * Переменные модуля
 */
volatile static u08 CurBut;						// номер нажатой кн.
volatile static str_but ButState[MAX_BUTTONS];	// массив флагов кнопок
volatile static u08 BCnt;						// счетчик временных интервалов для анализа событий кнопки

/*************** кольцевой буфер ******************/
static uint8_t buf[BUT_SIZE_BUF];
static uint8_t head, tail, count;
/*************************************************/


/******************************************************************************************
 * Поместить в буфер событие и ID кнопки
 * 	4 ст.бита - событие; 4 мл.бита ID кнопки
 */
static void PutBuf(u08 ev, u08 id)
{
	u08 but;
	but = ev << 4;		// фиксируем событие
	but |= id;			// ... и код кнопки

	if (count < BUT_SIZE_BUF){
	 buf[head] = but;
	 count++;
	 head++;
	 head &= (BUT_SIZE_BUF - 1);
	}
}

/******************************************************************************************
 * Инициализация кнопок
 */
void BUT_Init()
{
	u08 i;

	for(i=0; i<MAX_BUTTONS; i++) {
		ButState[i].enable = YES;  //
		ButState[i].def_val = 1;
		//ButState[i].pre_val = 1;
		ButState[i].pre_val = ButState[i].def_val;
		switch(i) {
		   case BUT_START:
			   DirReg(&KEY_PORT) &= ~_BV(PIN_START);	// инициализируем входы
			   PortReg(&KEY_PORT) |= _BV(PIN_START);	// подключаем внутренние резистры
		      break;
		   case BUT_ENC:
			   DirReg(&KEY_PORT) &= ~_BV(PIN_ENC_D);
			   PortReg(&KEY_PORT) |= _BV(PIN_ENC_D);
		      break;
		   case BUT_MODE:
			   DirReg(&KEY_PORT) &= ~_BV(PIN_MODE);
			   PortReg(&KEY_PORT) |= ~_BV(PIN_MODE);
		      break;
		}
	}
	head = 0;
	tail = 0;
	count = 0;
}

/******************************************************************************************
 * Проверка исходного состояния кнопок
 * 		Если все кнопки в исходном положении - возвращает 0
 * 		Если нет - код кнопки+1, которая находится в другом положении
 */
u08 BUT_IsOk()
{
	u08 i, x;

	for(i=0; i<MAX_BUTTONS; i++) {
		switch(i) {
		   case BUT_START:
			   x = PinReg(&KEY_PORT) & _BV(PIN_START);
		      break;
		   case BUT_ENC:
			   x = PinReg(&KEY_PORT) & _BV(PIN_ENC_D);
		      break;
		   case BUT_MODE:
			   x = PinReg(&KEY_PORT) & _BV(PIN_MODE);
		      break;
		}
		if (x)	x = 1;
		x ^= ButState[i].def_val;
		if (x)
			return i+1;
	}
	return 0;
}

u08 ReadBut(u08 but)
{
	u08 p=0;

	switch(but) {
	   case BUT_START:
		   p = PinReg(&KEY_PORT) & _BV(PIN_START);
	      break;
	   case BUT_ENC:
		   p = PinReg(&KEY_PORT) & _BV(PIN_ENC_D);
	      break;
	   case BUT_MODE:
		   p = PinReg(&KEY_PORT) & _BV(PIN_MODE);
	      break;
	}
	if (p) 	p = 1;
	//else p = 0;

	return p;
}

/******************************************************************************************
 * Анализ состояний кнопки на основе истории соостояний (keyhist)
 * 		вызывается из Tsk_ButPoll()
 * 		Возвращает:
 * 		-если в keyhist достаточно информации - код события (BUT_ShortPress, BUT_LongPress и т.д.)
 * 		-иначе - BUT_Error (информация в keyhist не имеет смысла)
 */
u08 TestHist(u08 keyhist)
{
	u08 h, l, x;

	h = ButState[CurBut].en_hold;
	l = h = ButState[CurBut].en_long;

	x = BUT_Error;
	if (BCnt == TIME_STD) {
		switch(keyhist) {
		   case 0:
			   // кн. отпущена
			   x = BUT_Released;
			  break;
		   case 1:
			   if(h | l)	x = BUT_NoEvent;	// длинное нажатие или удержание в процессе
			   else			x = BUT_Pressed;	// кн. нажата
			  break;
		   case 0b10:
			   // короткое нажатие
			   x = BUT_ShortPress;
		      break;
		   case 0b1010:
			   // двойное нажатие
			   x = BUT_DblPress;
		      break;
		}
	}
	else {
		if(keyhist == 1) {
			// длинное нажатие или удержание
			if(l)   x = BUT_LongPress;
			if(h)   x = BUT_Holded;
		}
	}
	return x;
}


/******************************************************************************************
 * Переодический опрос кнопок
 * 		вызывается из диспетчера
 * 		переодичность вызова см. KEY_PERIOD
 */
void Tsk_ButPoll()
{
	u08 i, p;
	static u08 ButGo = NO;		// NO-все кн. в неизменном состоянии, YES-была нажата(отпущена) кнопка
	static u08 BHist = 0;		// история состояний нажатой кн.

// Проверка нажата ли любая из кнопок
	if(ButGo == NO) {
		for(i=0; i<MAX_BUTTONS; i++) {
			if (ButState[i].enable == NO)	continue;
			// проверяем очередную кнопку
			p = ReadBut(i);
			// сравниваем с предыдущим значением
			if (p != ButState[i].pre_val) {
				// если состояние кн. изменилось...
				ButGo = YES;
				CurBut = i;							// сохраняем номер кн.
				ButState[i].change = YES;			// установить признак изменения
				BHist = 0;							// историю кн. в исх.состояние
				BCnt = 0;							// обнулить счетчик времени
				break;
			}
		}
		return;
	}

// Прошлый раз была нажата кнопка
	p = ReadBut(CurBut);				// очередное чтение кнопки
	++BCnt;

	if (ButState[CurBut].change == YES) {
		// прошлый раз изменилось состояние кнопки
		if (p != ButState[CurBut].pre_val) {
			// ...и кн. в том же состоянии, что и прошлый раз (дребезг завершен)
			ButState[CurBut].pre_val = p;		// сохраняем это состояние
			ButState[CurBut].change = NO;		// снимаем признак изменения
			p ^= ButState[CurBut].def_val;		// (p=1-кн. нажата, p=0-кн. отпущена)

			if (ButState[CurBut].isswich) {		// если это переключатель
				p |= SW_Off;
				PutBuf(p, CurBut);				// ... то событие в буфер
				ButGo = NO;
				return;
			}
			else {								// если это просто кн.
				BHist = BHist >> 1;				// ...готовим переменную истории событий
				BHist |= p;						// ...и заносим состояние
				BCnt = 0;
			}
		}
		else {
			// ...а теперь предыдущих изменений не наблюдается (помеха?)
			ButState[CurBut].change = NO;
			if (BHist == 0) {
				ButGo = NO;
				return;
			}
		}
	}
	else {
		// прошлый раз состояние кнопки не изменилось
		if (p != ButState[CurBut].pre_val) {
			// ... а на этом шаге состояние изменилось
			ButState[CurBut].change = YES;			// установить признак изменения
		}
	}

// Достигнуты ли предельные значения счетчика времени анализа нажатия(ий) кн.
	if((BCnt == TIME_STD) || (BCnt == TIME_LNG)) {
		// анализируем BHist
		p = TestHist(BHist);
		switch(p) {
		   case BUT_NoEvent:
			   // продолжить анализ
		      break;
		   case BUT_Error:
			   // ошибка
			   ButGo = NO;
		      break;
		   default:
			   // событие
			   PutBuf(p, CurBut);
			   ButGo = NO;
		}
	}
	return;
}
/******************************************************************************************
 * Читать из буфера событие и ID кнопки
 *   	4 ст.бита - событие; 4 мл.бита ID кнопки
 *   	Если буфер пуст возврать 0
 */
u08 BUT_GetBut(void)
{
  u08 but = 0;

  if (count){
     but = buf[tail];
     count--;
     tail++;
     tail &= (BUT_SIZE_BUF - 1);
  }

  return but;
}

/******************************************************************************************
 * Читать статус кнопки
 */
inline u08 BUT_GetState(u08 keyid)
{
	return ButState[keyid].enable;
}

/******************************************************************************************
 * Не анализировать состояние кнопки
 */
inline void BUT_Ignor(u08 keyid)
{
	ButState[keyid].enable = NO;
}

/******************************************************************************************
 * Анализировать состояние кнопки
 */
inline void BUT_Activ(u08 keyid)
{
	ButState[keyid].enable = YES;
}
