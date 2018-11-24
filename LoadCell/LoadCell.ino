#include <HX711.h> 
//necessary library for this code to work
 
#define DOUT  3 // data out to pin 3
#define CLK  2 // clock to pin 2
float rawMass[25];
int counter = 0;
float currMass = 0;
 
HX711 scale(DOUT, CLK);
 
//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor = -314418;
 
//=============================================================================================
//                         SETUP
//=============================================================================================
void setup() {
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
 
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
 
  Serial.print("Reading: ");
  currMass = scale.get_units()*1000;
  Serial.print(currMass, 1); // convert kg to mg
  Serial.print(" g");
  Serial.println();
   
   float maxMass = 0;
   
   //if we have 25 values (5 seconds have passed)
   if (counter==24)
   {
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
   else
   {
    // add value to array
    rawMass[counter] = currMass;
   }

  // wait for 1/5 of a second
  delay(200);
  counter++;
}
//=============================================================================================
