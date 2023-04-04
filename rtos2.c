#include <avr/interrupt.h>
#include "rtos2.h"
#include "commons.h"

/******************************************************************************************
 * Переменные модуля
 */
volatile static task TaskArray[MAX_TASKS];      // очередь задач
extern u08 arrayTail;                  			// "хвост" очереди
//volatile u08 tcount;							// служебный счетчик таймера


/******************************************************************************************
 * Добавление задачи в список
 */
#ifdef INTFUNC
void RTOS_SetTask (void (*taskFunc)(void), u32 taskDelay, u32 taskPeriod, void (*IFunc)(void))
{
   u08 i;
   
   //if(!taskFunc) return;
   for(i = 0; i < arrayTail; i++) {                   // поиск задачи в текущем списке
      if(TaskArray[i].pFunc == taskFunc) {            // если нашли, то обновляем переменные
         DISABLE_INTERRUPT;

         TaskArray[i].delay  = taskDelay;
         TaskArray[i].period = taskPeriod;
         TaskArray[i].run    = 0;
         TaskArray[i].pIFunc = IFunc;

         RESTORE_INTERRUPT;
         return;                                      // обновив, выходим
      }
   }

   if (arrayTail < MAX_TASKS) {                       // если такой задачи в списке нет
      DISABLE_INTERRUPT;							  // и есть место,то добавляем
      
      TaskArray[arrayTail].pFunc  = taskFunc;
      TaskArray[arrayTail].delay  = taskDelay;
      TaskArray[arrayTail].period = taskPeriod;
      TaskArray[arrayTail].run    = 0;
      TaskArray[i].pIFunc = IFunc;

      arrayTail++;                                    // увеличиваем "хвост"
      RESTORE_INTERRUPT;
   }
}
#else
void RTOS_SetTask (void (*taskFunc)(void), u32 taskDelay, u32 taskPeriod)
{
   u08 i;

   //if(!taskFunc) return;
   for(i = 0; i < arrayTail; i++) {                   // поиск задачи в текущем списке
      if(TaskArray[i].pFunc == taskFunc) {            // если нашли, то обновляем переменные
         DISABLE_INTERRUPT;

         TaskArray[i].delay  = taskDelay;
         TaskArray[i].period = taskPeriod;
         TaskArray[i].run    = 0;

         RESTORE_INTERRUPT;
         return;                                      // обновив, выходим
      }
   }

   if (arrayTail < MAX_TASKS) {                       // если такой задачи в списке нет
      DISABLE_INTERRUPT;							  // и есть место,то добавляем

      TaskArray[arrayTail].pFunc  = taskFunc;
      TaskArray[arrayTail].delay  = taskDelay;
      TaskArray[arrayTail].period = taskPeriod;
      TaskArray[arrayTail].run    = 0;

      arrayTail++;                                    // увеличиваем "хвост"
      RESTORE_INTERRUPT;
   }
}
#endif

/******************************************************************************************
 * Удаление задачи из списка
 */
void RTOS_DeleteTask (void (*taskFunc)(void))
{
	u08 i;
   
	for (i=0; i<arrayTail; i++) {							// проходим по списку задач
		if(TaskArray[i].pFunc == taskFunc) {				// если задача в списке найдена
			DISABLE_INTERRUPT;
			if(i != (arrayTail - 1)) {						// переносим последнюю задачу
				TaskArray[i] = TaskArray[arrayTail - 1];	// на место удаляемой
			}
			arrayTail--;									// уменьшаем указатель "хвоста"
			RESTORE_INTERRUPT;
			return;
		}
	}
}

/******************************************************************************************
 * Диспетчер РТОС, вызывается в main
 */

void RTOS_DispatchTask()
{
	u08 i;
	void (*function) (void);

	for (i=0; i<arrayTail; i++) {							// проходим по списку задач
		if (TaskArray[i].run == 1) {						// если флаг на выполнение взведен,
															// запоминаем задачу, т.к. во
			function = TaskArray[i].pFunc;					// время выполнения может
															// измениться индекс
			if(TaskArray[i].period == 0) {
															// если период равен 0
				RTOS_DeleteTask(TaskArray[i].pFunc);		// удаляем задачу из списка,
			}
			else {
				TaskArray[i].run = 0;						// иначе снимаем флаг запуска
				if(!TaskArray[i].delay) {					// если задача не изменила задержку
															// задаем ее
					TaskArray[i].delay = TaskArray[i].period-1; 
				}											// задача для себя может сделать паузу
			}
			(*function)();	// выполняем задачу
		}
	}
}

/******************************************************************************************
 * Читать через сколько сработает функция
 */
/*
u32 RTOS_ReadDelay (void (*function)(void))
{
	u08 i, nf;
	u32 delay;

	nf = 0;
	delay = 0;
	for (i=0; i<arrayTail; i++) {							// проходим по списку задач
		if(TaskArray[i].pFunc == function) {				// если задача в списке найдена
			nf = 1;
			DISABLE_INTERRUPT;
			delay = TaskArray[i].delay;						// запоминаем и возвращаем результат
			RESTORE_INTERRUPT;
//TLed_on();
			break;
		}
	}
	if (nf)
		function = NULL;								// если задача не найдена

	return delay;
}
*/

/******************************************************************************************
 * Таймерная служба РТОС (прерывание аппаратного таймера)
 */
ISR(RTOS_ISR) 
{
	u08 i;
   
	for (i=0; i<arrayTail; i++) {		// проходим по списку задач
		if  (TaskArray[i].delay) {		// если время до выполнения не истекло
			TaskArray[i].delay--;		// уменьшаем время
		}
		else {
			TaskArray[i].run = 1;		// иначе взводим флаг запуска,
#ifdef INTFUNC
			if(TaskArray[i].pIFunc) {
				(*TaskArray[i].pIFunc)();
//				Pow_Off();
			}
#endif
		}
	}
}




