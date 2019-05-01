/*
*	TechFactory.hu
*	LCD kijelzőhöz magyar ékezetes karakter támogatás
*	(LiquidCrystal_I2C class-hoz készült kiegészítő)
*	https://github.com/TechFactoryHU/LCDCharConverter
*/

#ifndef LCDCHARCONVERTER_H 
#define LCDCHARCONVERTER_H
//https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
//version: 1.1.2
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

#ifndef LCDCHRCONVERTER_CHARMAP_SIZE
	#define LCDCHRCONVERTER_CHARMAP_SIZE 8
#endif

class LCDCharConverter {
	public: 
		LCDCharConverter(LiquidCrystal_I2C* lcd);
		void display(char * message);
		char* convert(char * message);
		void reset();
		inline int getLength() { return _len; }
		
	private:
		int _len;
		LiquidCrystal_I2C* _lcd;
		signed char _customCharMap[LCDCHRCONVERTER_CHARMAP_SIZE];
		
		int getRealLength(char* msg);
		char parseChar(char* msg, int* iref);
		char loadCustomChar(int chr_index);
		int getCustomCharSlot(int index);
};
#endif