#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "LCDCharConverter.h"

//https://playground.arduino.cc/Main/I2cScanner/
LiquidCrystal_I2C lcd(0x27,16,2);
LCDCharConverter converter(&lcd);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up");
  
  lcd.begin(16,2);
  converter.reset();
  lcd.setBacklight(1);
  lcd.init();     
}

void loop() {
    //watch commands from serial
    if (Serial.available()>0) {
        String rd = Serial.readStringUntil('\n');
        //remove \n
        rd.remove(rd.length()-1,1);
        Serial.print("------------");
        Serial.println(rd);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(rd);
        
        lcd.setCursor(0,1);
        char* out = &rd[0];
        for (int i=0; i<strlen(out);i++) {
          lcd.print(out[i], HEX);
          lcd.print(" ");
          Serial.print(out[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
    }
}
