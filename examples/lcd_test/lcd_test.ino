#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "LCDCharConverter.h"

//https://playground.arduino.cc/Main/I2cScanner/
LiquidCrystal_I2C lcd(0x27,16,2);
LCDCharConverter converter(&lcd);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up");
  
  converter.reset();
  lcd.begin(16,2);
  lcd.init();     
  lcd.setBacklight(1);
  lcd.setCursor(0,0);
  lcd.print(" TechFactory.hu ");
  converter.display("-Árvíztűrő LCD!-", 0, 1);
  delay(4000);
  lcd.clear();
}

void loop() {

    if (Serial.available()>0) {
        //read string from console (utf8 charset)
        String rd = Serial.readStringUntil('\n');
        //remove \n
        rd.remove(rd.length()-1,1);
        
        //clear lcd
        lcd.clear();
        //reset previously loaded custom chars
        converter.reset();
        lcd.setCursor(0,0);
       
        //print out original utf8 string
        lcd.print(&rd[0]);

        //print out converted string with custom loaded chars
        lcd.setCursor(0,1);
        //convert utf8 string and display on LCD (load custom chars from charmap)
        //hungarian letters supported: Áá Éé Íí Őő Úú Űű Öö Üü Óó (extended latin)
        char* msg = converter.convert(&rd[0]);
        for (int i=0; i<converter.getLength();i++) {
            lcd.write(msg[i]);
        }
        //or
        //converter.display( char* message, uint8_t col, uint8_t row);
        
        Serial.println("->OK");
    }
}
