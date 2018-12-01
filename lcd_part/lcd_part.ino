#include <font_6x8.h>
#include <font_big.h>
#include <LCD4884.h>
 
#define MENU_X  0      // 0-83
#define MENU_Y  0       // 0-5

// joystick number
#define LEFT_KEY 0
#define CENTER_KEY 1
#define DOWN_KEY 2
#define RIGHT_KEY 3
#define UP_KEY 4

//keypad debounce parameter
#define DEBOUNCE_MAX 15
#define DEBOUNCE_ON  10
#define DEBOUNCE_OFF 3 

#define NUM_KEYS 5

int  adc_key_val[5] ={
  50, 200, 400, 600, 800 };

// debounce counters
byte button_count[NUM_KEYS];
// button status - pressed/released
byte button_status[NUM_KEYS];
// button on flags for user program 
byte button_flag[NUM_KEYS];

//turn on the display at first
bool display_turn_off = false;
 
void setup()
{
  // setup interrupt-driven keypad arrays  
  // reset button arrays
  for(byte i=0; i<NUM_KEYS; i++){
    button_count[i]=0;
    button_status[i]=0;
    button_flag[i]=0;
  }

  // Setup timer2 -- Prescaler/256
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);
  TCCR2B = (1<<CS22)|(1<<CS21);      

  ASSR |=(0<<AS2);

  // Use normal mode  
  TCCR2A =0;    
  //Timer2 Overflow Interrupt Enable  
  TIMSK2 |= (0<<OCIE2A);
  TCNT2=0x6;  // counting starts from 6;  
  TIMSK2 = (1<<TOIE2);    



  SREG|=1<<SREG_I;

  lcd.LCD_init();
  lcd.LCD_clear();

  //menu initialization
  init_MENU();
  display_turn_off = false;  

  lcd.backlight(ON);//Turn on the backlight
  //lcd.backlight(OFF); // Turn off the backlight  
}
 
void init_MENU(void)
{
  lcd.LCD_clear();
  lcd.LCD_write_string(MENU_X, 0, "Patient Name: ", MENU_HIGHLIGHT);
  lcd.LCD_write_string(MENU_X, 1, "Jenny Yu", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, 2, "Drug Name: ", MENU_HIGHLIGHT);
  lcd.LCD_write_string(MENU_X, 3, "methotrexate", MENU_NORMAL);
  lcd.LCD_write_string(MENU_X, 4, "Dosage:", MENU_HIGHLIGHT);
  lcd.LCD_write_string(MENU_X, 5, "500 mg", MENU_NORMAL);
}

void init_MENU_TWO(void)
{
  lcd.LCD_clear();
  lcd.LCD_write_string(MENU_X, 0, "Prescribed amount: ", MENU_HIGHLIGHT);
}
 
void loop()
{
  byte i;
  for(i=0; i<NUM_KEYS; i++){
    if(button_flag[i] !=0){

      button_flag[i]=0;  // reset button flag
      switch(i){
        case CENTER_KEY:
          lcd.LCD_clear();
          if(display_turn_off == false){
            init_MENU_TWO();
            display_turn_off = true;
          }
          else {
            init_MENU();
            display_turn_off = false;
          }
          break;
      }
    }
  }
}

// The followinging are interrupt-driven keypad reading functions
// which includes DEBOUNCE ON/OFF mechanism, and continuous pressing detection


// Convert ADC value to key number
char get_key(unsigned int input)
{
  char k;

  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {

      return k;
    }
  }

  if (k >= NUM_KEYS)
    k = -1;     // No valid key pressed

  return k;
}

void update_adc_key(){
  int adc_key_in;
  char key_in;
  byte i;

  adc_key_in = analogRead(0);
  key_in = get_key(adc_key_in);
  for(i=0; i<NUM_KEYS; i++)
  {
    if(key_in==i)  //one key is pressed 
    { 
      if(button_count[i]<DEBOUNCE_MAX)
      {
        button_count[i]++;
        if(button_count[i]>DEBOUNCE_ON)
        {
          if(button_status[i] == 0)
          {
            button_flag[i] = 1;
            button_status[i] = 1; //button debounced to 'pressed' status
          }

        }
      }

    }
    else // no button pressed
    {
      if (button_count[i] >0)
      {  
        button_flag[i] = 0;  
        button_count[i]--;
        if(button_count[i]<DEBOUNCE_OFF){
          button_status[i]=0;   //button debounced to 'released' status
        }
      }
    }

  }
}

// Timer2 interrupt routine -
// 1/(160000000/256/(256-6)) = 4ms interval

ISR(TIMER2_OVF_vect) {  
  TCNT2  = 6;
  update_adc_key();
}
