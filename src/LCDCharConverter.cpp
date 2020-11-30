/*
*	TechFactory.hu
*	LCD kijelzőhöz magyar ékezetes karakter támogatás
*	(LiquidCrystal_I2C class-hoz készült kiegészítő)
*/

#include "LCDCharConverter.h"
#include "LCDCharmap.h"

LCDCharConverter::LCDCharConverter(LiquidCrystal_I2C* lcd) {
	_lcd = lcd;
}

void LCDCharConverter::display(char* message, uint8_t col, uint8_t row) {
	//konvertáljuk a bejövő utf8 kódolású karaktereket ASCII-ra
	char* converted = convert(message);
	//majd ezt már ki tudjuk írni a kijelzőre
	_lcd->setCursor(col,row); 
	for (int i=0; i<_len; i++) {
	  _lcd->write(converted[i]);
	}
	free(converted);
	_len = 0;
}

char* LCDCharConverter::convert(char* message) {
	char* newmessage;
	//kikérjük az új méretet (ami már nem utf8)
	_len = getRealLength(message);
	//lefoglaljuk a szükséges memóriát
	newmessage = (char*) malloc(_len);
	
	//eredeti utf8 szöveg hosszúsága
	size_t len = strlen(message);
	//végloopoljuk az eredeti utf8-as szöveget és
	//bemapeljük az utf8 karaktereket az lcd egyedi karakter készletébe. 
	//(Jelenleg csak a magyar ékezetes kis nagy betűket rajzoltuk me)
	//Max 7 egyedi karaktert lehet egy idejűleg megjeleníteni, a többi utf8 karakter helyett ?-et jelenítünk meg.
	int x = 0;
	for (int i=0; i<len; i++) {
		newmessage[x] = parseChar(message, &i);
		x++;
	}
	
	return newmessage;
}

void LCDCharConverter::reset() {
	for (int i=0; i<LCDCHRCONVERTER_CHARMAP_SIZE; i++) {
        _customCharMap[i] = -1;
    }
}

char LCDCharConverter::parseChar(char* message, int* iref) {
   int i = (int)*iref;
   //1 byte
   if ((byte)message[i] >= 0x01 && (byte)message[i] <= 0x7F) {
       return message[i];
   //2 byte 
   }else if((byte)message[i] >= 0xC2 && (byte)message[i] <= 0xDF) {
      if ((byte)message[i] == 0xC2) {
         i++; *iref = i;
         //° (fok) => karakter (0xB0) -> 223
         if ((byte)message[i] == 0xB0) { return 223; }
      }else if((byte)message[i] == 0xC3) {
           i++; *iref = i;
           switch ((byte)message[i]) {
              //81 - Á
              case 0x81: return loadCustomChar(0); break;
              //89 - É
              case 0x89: return loadCustomChar(1); break;
              //8D - Í
              case 0x8D: return loadCustomChar(2); break;
              //9A - Ú
              case 0x9A: return loadCustomChar(3); break;
              //9A - Ü
              case 0x9C: return loadCustomChar(4); break;
              //93 - Ó
              case 0x93: return loadCustomChar(5); break;
              //96 - Ö
              case 0x96: return loadCustomChar(6); break;  
              //A1 - á
              case 0xA1: return loadCustomChar(9); break;  
              //A9 - é
              case 0xA9: return loadCustomChar(10); break;  
              //AD - í
              case 0xAD: return loadCustomChar(11); break;  
              //BA - ú
              case 0xBA: return loadCustomChar(12); break;  
              //BC - ü
              //az ü szerepel az eredeti karaktertáblában, ne pazaroljunk helyet
              case 0xBC: return 245; break;
              //case 0xBC: return loadCustomChar(13); break;  
              //B3 - ó
              case 0xB3: return loadCustomChar(14); break;  
              //B6 - ö
              //az ö szerepel az eredeti karaktertáblában, ne pazaroljunk helyet
              case 0xB6: return 239; break;
              //case 0xB6: return loadCustomChar(15); break;  
           }

      }else if((byte)message[i] == 0xC5) {
          i++; *iref = i;
          switch ((byte)message[i]) {
              //90 - Ő
              case 0x90: return loadCustomChar(7); break;
              //B0 - Ű
              case 0xB0: return loadCustomChar(8); break;
              //91 - ő
              case 0x91: return loadCustomChar(16); break;  
              //B1 - ű
              case 0xB1: return loadCustomChar(17); break;  
          }
     
      }
   //3 byte
   }else if((byte)message[i] >= 0xE0 && (byte)message[i] <= 0xEF) {}
   //4 byte
   else if((byte)message[i] >= 0xF0) {}
   //ismeretlen karakter []
   return 255;
}

int LCDCharConverter::getRealLength(char* message) {
	size_t len = strlen(message);
	int nl = 0;
	for (int i=0; i<len; i++) {
	   //1 byte karakter
	   if ((byte)message[i] >= 0x01 && (byte)message[i] <= 0x7F) {
		   nl++;
	   //2 byte karakter
	   }else if((byte)message[i] >= 0xC2 && (byte)message[i] <= 0xDF) {
		   nl++; i++;
	   } 
	   //3 byte karakter
	   else if((byte)message[i] >= 0xE0 && (byte)message[i] <= 0xEF) {
		   nl++; i+=2;
	   }
	   //4 byte karakter
	   else if((byte)message[i] >= 0xF0) {
		   nl++; i+=3;
	   }
	   //ismeretlen, 1 byte-t számolunk 
	   else {
		  nl++;
	   }
	}
	return nl;
}

char LCDCharConverter::loadCustomChar(int chr_index) {
	int index = getCustomCharSlot(chr_index);
    if (index != -1) {
        return (char)index;
    }
    //nem lehetett betolteni a karaktert (elfogyott a hely)
    //helyette ? jelet teszünk ki a kijelzőre
    return 63;
}

int LCDCharConverter::getCustomCharSlot(int index) {
    //keressunk helyet a betoltendo karakternek
    //ellenorízzük azt is, hogy korábban betöltöttük e már, így biztosan csak 1x használjuk fel a helyet adott karakterre
    for (int i=0; i<LCDCHRCONVERTER_CHARMAP_SIZE; i++) {
        if (_customCharMap[i] == index || _customCharMap[i] == -1) {
            if (_customCharMap[i] == -1) {
                _lcd->createChar(i, LCDCHRCONVERTER_CUSTOM_CHARS[index]);
                _customCharMap[i] = index;
            }
            return i;
        }
    }
    
    //nincs tobb hely
    return -1;
}

