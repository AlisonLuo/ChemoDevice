#include "LiquidCrystal.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
String data = "Jane Doe, Mitoxantrone, 50 mg, Not safe";

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  
}

void loop() {
  displayData(data);
//  lcd.print("Drug: Mitoxantrone");
//  lcd.setCursor(0,1);
//  lcd.print("Current dose: 50 mg");
//  delay(1500);
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("50 mg");
//  lcd.setCursor(0,1);
//  lcd.print("Not a safe dosage!");
//  delay(1000);
//  lcd.clear();
}

void displayData(string)
{
  // separate the string into substrings
  
  
}
