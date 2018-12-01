#include <HX711.h> 
//necessary library for this code to work
 
#define DOUT  3 // data out to pin 3
#define CLK  2 // clock to pin 2
float rawMass[25];
int counter = 0;
float currMass = 0;

//define LEDs
int readyPin = 9;
int waitingPin = 10;
int removePin = 11;
boolean readyLED = LOW;
boolean waitingLED = LOW;
boolean removeLED = LOW;

 
HX711 scale(DOUT, CLK);
 
//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor = -314418;
 
//=============================================================================================
//                         SETUP
//=============================================================================================
void setup() {
  pinMode(readyPin, readyLED);
  pinMode(waitingPin, waitingLED);
  pinMode(removePin, removeLED);
  
  Serial.begin(9600);
  Serial.println("Press t for tare");
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
 
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  scale.tare(); //Reset the scale to 0
  
  
//  if(Serial.available())
//   {
//    char temp = Serial.read();
//    if(temp == 't' || temp =='T')
//      scale.tare();  //Reset the scale to zero
//   }
}
 
//=============================================================================================
//                         LOOP
//=============================================================================================
void loop() {
  //if they tell us to, the counter will be 0
  boolean ready = true;
  if(ready == true)
  {
    counter = 0;
    readyLED = HIGH;
    waitingLED = LOW;
    removeLED = LOW;
  }

  
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  currMass = scale.get_units()*1000;
  Serial.print(currMass, 1); // convert kg to mg
  Serial.print(" g");
  Serial.println();
   
   float maxMass = 0;
   
   //if we have 25 values (5 seconds have passed)
   else if (counter==24)
   {
    readyLED = LOW;
    waitingLED = LOW;
    removeLED = HIGH;
    //find maximum mass out of all 25 values
    for (int i=0; i<25; i++)
    {
      if(rawMass[i] > maxMass)
      {
        maxMass = rawMass[i];
      }
    }
    counter = 0;
    //send maxMass to LCD
    Serial.println();
    Serial.print("Max Mass: ");
    Serial.print(maxMass);
    Serial.println(" g");
    Serial.println();
   }

   // if we have not been measuring mass for 5 seconds
   else //counter can only be from 2 to 23
   {
    //check if user took vial off and put it back on

    //first method to do this
    //we need to compare the current value or see if it's 0, things are more complicated because there is a range for values of "nothing"
    if(currMass <=0.5 && currMass >=(-0.5)) // so this means the user took it off unless we can do it in a smarter way (currently, this is just for functionality)
    {
      counter = 0; //start from the beginning
    }
    
    // second method
    // if difference between current value and previous value is greater or equal to half of the current value (meaning there is a huge difference suddenly)
    if((rawMass[counter] - rawMass[counter-1]) >= (0.5*rawMass[counter]))
    {
      counter = 0; //start from the beginning
    }
    
    else 
    {
      readyLED = LOW;
      waitingLED = HIGH;
      removeLED = LOW;
      // add value to array
      rawMass[counter] = currMass;
    }    
   }
   
  // turn on/off appropriate LEDs
  digitalWrite(readyPin, readyLED);
  digitalWrite(waitingPin, waitingLED);
  digitalWrite(removePIn, removeLED);
  
  // wait for 1/5 of a second
  delay(200);
  counter++;
}
//=============================================================================================
