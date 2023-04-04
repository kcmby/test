/*
 * modes.h
 *
 *  Created on: 15 сент. 2019 г.
 *      Author: KCM
 */

#ifndef MODES_H_
#define MODES_H_

	#define MAX_SUBMODES		5  // Количество подрежимов

	// режимы
	#define MODE_TEST		0b000		// режим тестирования (при включении)
	#define MODE_WELD		0b001		// режим сварки
	#define MODE_LAMP		0b010		// режим засветки
	#define MODE_SETUP		0b011		// режим настройки
	// подрежимы
	#define SM_WELD_1		0b001		// подрежим сварки с одиночным импульсом
	#define SM_WELD_2		0b010		// подрежим сварки с двойным импульсом
	#define SM_WELD_M		0b011		// подрежим ручной сварки
	#define SM_LAMP_S		0b001		// подрежим засветки с коротким экспонированием
	#define SM_LAMP_L		0b010		// подрежим засветки с длительным экспонированием


#endif /* MODES_H_ */
