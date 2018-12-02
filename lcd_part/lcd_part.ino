#include <HX711.h>
#include "pitches.h"

#define DOUT 8
#define CLK 9
float massArray [10];
int count = 0;
float currMass = 0;
float sum = 0;
char prescribed_amount[20] = "35";
char checked_result[20];

// define LEDs
int readyPin = 11;
int waitingPin = 12;

boolean readyLED = LOW;
boolean waitingLED = LOW;

HX711 scale(DOUT, CLK);

float calibration_factor = -311418;

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
byte page = 0;
byte prev_page = 0;
 
void setup()
{
  //LEDs
  pinMode(readyPin, OUTPUT);
  pinMode(waitingPin, OUTPUT);
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
  init_MENU(page, prev_page);

  lcd.backlight(ON);//Turn on the backlight
  Serial.begin(9600);
  Serial.println("Press t for tare:");
  scale.set_scale();
  scale.tare();
  
}


void init_MENU(byte page_number, byte prev_page)
{
  if (page_number != prev_page) {
    lcd.LCD_clear();
  }
  
  if (page_number == 0) {
    lcd.LCD_write_string(MENU_X, 0, "Patient Name: ", MENU_HIGHLIGHT);
    lcd.LCD_write_string(MENU_X, 1, "Jenny Yu", MENU_NORMAL);
    lcd.LCD_write_string(MENU_X, 2, "Drug Name: ", MENU_HIGHLIGHT);
    lcd.LCD_write_string(MENU_X, 3, "methotrexate", MENU_NORMAL);
    lcd.LCD_write_string(MENU_X, 4, "Dosage:", MENU_HIGHLIGHT);
    lcd.LCD_write_string(MENU_X, 5, "500 mg", MENU_NORMAL);
  }
  else if (page_number == 1) {
    lcd.LCD_write_string(10, 0, "Prescribed", MENU_NORMAL);
    lcd.LCD_write_string(20, 1, "Amount", MENU_NORMAL);
    lcd.LCD_write_string_big(20, 3, prescribed_amount, MENU_NORMAL);
    lcd.LCD_write_string(70, 5, "g", MENU_NORMAL);
  }
  else if (page_number == 2) {
    lcd.LCD_write_string(MENU_X, 0, checked_result, MENU_HIGHLIGHT);
  }
  
}
 
void loop()
{
  byte i;
  for(i=0; i<NUM_KEYS; i++){
    if(button_flag[i] !=0){

      button_flag[i]=0;  // reset button flag
      switch(i){
        case LEFT_KEY:
          if(page >= 0 && page < 3){
            if (page == 0) {
              prev_page = 0;
              init_MENU(page,prev_page);
              break;
            }
            else {
              prev_page = page;
              page = page - 1;
            }
            init_MENU(page, prev_page);
          }
          break;
        case RIGHT_KEY:
          if(page >= 0 && page < 3){
            if (page == 2) {
              prev_page = 2;
              init_MENU(page, prev_page);
              break;
            }
            else {
              prev_page = page;
              page = page + 1;
            }
            init_MENU(page, prev_page);
          }
          break;
      }
    }
  }

  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  currMass = scale.get_units()*1000;
  Serial.print(currMass, 1); // convert kg to mg
  Serial.print(" g");
  Serial.println();

  if(currMass > -1 && currMass < 1)
  {
    readyLED = HIGH;
    waitingLED = LOW;
    digitalWrite(readyPin, readyLED);
    digitalWrite(waitingPin, waitingLED);

  }
  else if(currMass > 1)
  {
    waitingLED = HIGH;
    readyLED = LOW;
    digitalWrite(readyPin, readyLED);
    digitalWrite(waitingPin, waitingLED);
    sum = sum + currMass;
    float massAvg = 0;
    
    if(count==10)
    {
      massAvg = sum/11;
      Serial.print("Average Mass: ");
      Serial.println(massAvg);
      double prescribed_double = atof(prescribed_amount);
      if((massAvg < prescribed_double+2) && massAvg > (prescribed_double-2))
      {
        // display ok
        strcpy(checked_result, "okay!");
        init_MENU(2,prev_page);
        Serial.println("display ok");
      }
      else
      {
        //display ERROR and massAvg
        // turn on SPEAKER
        strcpy(checked_result, "error");
         init_MENU(2,prev_page);
        Serial.println("Speaker is on + ERROR");
        for (int thisNote = 0; thisNote < 100; thisNote++) {
              tone(10,  NOTE_C4, 500);
          }
        
      }
      count = 0;
      sum = 0;
    }
    else {
      count++;
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
