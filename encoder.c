//Набор функций для обслуживания инкрементального энкодера
//Автор s_black - www.embed.com.ua
//июль 2010 г. 	

#include "encoder.h"

#ifndef ENC_LIMITS
u08 ENC_MIN;
u08 ENC_MAX;
#endif

inline void Encoder_init (void)//Функция инициализации энкодера
{
	/****инициализируем входы для подключения энкодера ****/
	DirReg(&ENCODER_PORT)  &= ~(_BV(PIN_ENC_A));
	DirReg(&ENCODER_PORT)  &= ~(_BV(PIN_ENC_B));
	/****подключаем внутренние резистры ****/
	PortReg(&ENCODER_PORT) |= _BV(PIN_ENC_A) | _BV(PIN_ENC_B);
}

//#ifdef ENC_LIMITS
u08 Encoder_Scan(void)//Функция обработки энкодера
{
    static u08 New, EncPlus, EncMinus;//Переменные нового значения энкодера, промежуточные переменные + и -
    static u08 EncState;
	uint8_t ret = 0;

    New = PinReg(&ENCODER_PORT) & (_BV(PIN_ENC_B) | _BV(PIN_ENC_A));// Считываем настоящее положение энкодера

    if(New != EncState)//Если значение изменилось по отношению к прошлому
    {
        switch(EncState) //Перебор прошлого значения энкодера
	    {
	    case state_2:if(New == state_3) EncPlus++;//В зависимости от значения увеличиваем
		             if(New == state_0) EncMinus++;//Или уменьшаем
		       break;
	    case state_0:if(New == state_2) EncPlus++;
		             if(New == state_1) EncMinus++;
		       break;
	    case state_1:if(New == state_0) EncPlus++;
		             if(New == state_3) EncMinus++;
		       break;
	    case state_3:if(New == state_1) EncPlus++;
		             if(New == state_2) EncMinus++;
		       break;
        default:break;
	    }

		if(EncPlus == ENC_TIKS) //если прошёл один "щелчок"
		{
		    if(EncData++ >= ENC_MAX) EncData = ENC_MAX;	//увеличиваем значение, следим, чтобы не выйти за границы верхнего
			EncPlus = 0;
			ret = 1;
		}

		if(EncMinus == ENC_TIKS) //если прошёл один "щелчок"
		{
		    if(EncData-- <= ENC_MIN) EncData = ENC_MIN;	//уменьшаем значение, следим чтобы не выйти за границы нижнего пределов
			EncMinus = 0;
			ret = 1;
		}
        EncState = New;	// Записываем новое значение предыдущего состояния
	}
	return ret;
}
//#else
#ifndef ENC_LIMITS
inline void SetEncoder_Lim(u08 l_min, u08 l_max)
{
	ENC_MIN = l_min;
	ENC_MAX = l_max;
}
#endif
/*
u08 Encoder_Scan(u08 l_min, u08 l_max)		//Функция обработки энкодера
{
    static u08 New, EncPlus, EncMinus;//Переменные нового значения энкодера, промежуточные переменные + и -
    static u08 EncState;
	uint8_t ret = 0;
 
    New = PinReg(&ENCODER_PORT) & (_BV(PIN_ENC_B) | _BV(PIN_ENC_A));// Считываем настоящее положение энкодера
 
    if(New != EncState)//Если значение изменилось по отношению к прошлому
    {
        switch(EncState) //Перебор прошлого значения энкодера
	    {
	    case state_2:if(New == state_3) EncPlus++;//В зависимости от значения увеличиваем
		             if(New == state_0) EncMinus++;//Или уменьшаем  
		       break;
	    case state_0:if(New == state_2) EncPlus++;
		             if(New == state_1) EncMinus++; 
		       break;
	    case state_1:if(New == state_0) EncPlus++;
		             if(New == state_3) EncMinus++; 
		       break;
	    case state_3:if(New == state_1) EncPlus++;
		             if(New == state_2) EncMinus++; 
		       break;
        default:break;
	    }
		
		if(EncPlus == ENC_TIKS) //если прошёл один "щелчок"
		{
		    if(EncData++ >= l_max) EncData = l_max;//увеличиваем значение, следим, чтобы не выйти за границы верхнего
			EncPlus = 0;
			ret = 1;
		}
		
		if(EncMinus == ENC_TIKS) //если прошёл один "щелчок"
		{
		    if(EncData-- <= l_min) EncData = l_min;//уменьшаем значение, следим чтобы не выйти за границы нижнего пределов
			EncMinus = 0;
			ret = 1;
		}
        EncState = New;	// Записываем новое значение предыдущего состояния
	}
	return ret;
}
*/
//#endif
