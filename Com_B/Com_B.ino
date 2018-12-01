#include <Wire.h>
// this code is for the load cell arduino (receiving cues from LCD arduino and sending mass data)
int msgSize = 10;
String message;

static float mass = 12.09;
static char massArray[8]; // Buffer big enough for 7-character float

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveMsg); // register event
  Serial.begin(9600);           // start serial for output
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveMsg(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char msg[msgSize];
    int i = 0;
    while( i<= msgSize - 1) {
      char c = Wire.read(); // receive byte as a character
      msg[i] = c;
      if (i == msgSize - 1) {
        message = String(msg).substring(0,10);
       // Serial.println(message);
        if (message.equals("ScaleReady")){
          Serial.println("received ready message");
          // Set a boolean
        }
        else if (message == "Speaker ON"){
          Serial.println("received Speaker ON");
          // Set a boolean
        }
        else if (message == "RED LED ON"){
          Serial.println("received RED LED ON");
          // Set a boolean
        }
      }
      i++;
    }
    i = 0;
 
  }
}

void requestEvent() {
  //decimal to string function: float, width, decimal places, array
  dtostrf(mass,5, 2, massArray);
  Wire.write(massArray); // respond with message of 3 bytes
  // we cannot send floats, so we must convert them to Strings?
  // as expected by master
}
