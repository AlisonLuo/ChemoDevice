#include <font_6x8.h>
#include <font_big.h>
#include <LCD4884.h>
 
#define MENU_X  0      // 0-83
#define MENU_Y  0       // 0-5
 
void setup()
{
 
lcd.LCD_init();
lcd.LCD_clear();
 
 
init_MENU();
}
 
void init_MENU(void)
{
 
byte i;
lcd.LCD_clear();
lcd.LCD_write_string(MENU_X, 0, "Patient Name: ", MENU_HIGHLIGHT);
lcd.LCD_write_string(MENU_X, 1, "Jenny Yu", MENU_NORMAL);
lcd.LCD_write_string(MENU_X, 2, "Drug Name: ", MENU_HIGHLIGHT);
lcd.LCD_write_string(MENU_X, 3, "methotrexate", MENU_NORMAL);
lcd.LCD_write_string(MENU_X, 4, "Dosage:", MENU_HIGHLIGHT);
lcd.LCD_write_string(MENU_X, 5, "500 mg", MENU_NORMAL);
}
 
void loop()
{
 
}
