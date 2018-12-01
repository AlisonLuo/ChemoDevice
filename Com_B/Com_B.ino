#include <Wire.h>
// this code is for the load cell arduino (receiving cues from LCD arduino and sending mass data)
int msgSize = 10;
String message;

static float mass = 12.09;
static char massArray[8]; // Buffer big enough for 7-character float

// LED code
int readyPin = 11;
int waitingPin = 10;
int removePin = 9;
boolean readyLED = LOW;
boolean waitingLED = LOW;
boolean removeLED = LOW;

boolean scaleReady = false;
boolean waiting = false;
boolean removeMass = false;
boolean error = false;

void setup() {
 // LED code
  pinMode(readyPin, OUTPUT);
  pinMode(waitingPin, OUTPUT);
  pinMode(removePin, OUTPUT);
  
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveMsg); // register event
  Serial.begin(9600);           // start serial for output
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
  if(scaleReady == true)
  {
    Serial.println("put on the mass");
    readyLED = HIGH;
    waitingLED = LOW;
    removeLED = LOW;
  }

  else if (waiting == true)
  {
     Serial.println("currently waiting");
    readyLED = LOW;
    waitingLED = HIGH;
    removeLED = LOW;
  }

  else if (removeMass == true)
  {
    Serial.println("please remove the mass");
    readyLED = LOW;
    waitingLED = LOW;
    removeLED = HIGH;
  }
  // turn on/off appropriate LEDs
//  Serial.println("ready LED");
//  Serial.println(readyLED);
  
  digitalWrite(readyPin, readyLED);
  digitalWrite(waitingPin, waitingLED);
  digitalWrite(removePin, removeLED);

  delay(2000); // placeholder for detecting mass change
  // if mass changed from 0 to __: (optional)
  waitingLED = HIGH;
  readyLED = LOW;
  digitalWrite(readyPin, readyLED);
  digitalWrite(waitingPin, waitingLED);
  digitalWrite(removePin, removeLED);
  delay(4000); // placeholder for taking measurements from load cell

  // call requestEvent function
  mass = 77.88;
  requestEvent();
  waitingLED = LOW;
  removeLED = HIGH;
  digitalWrite(readyPin, readyLED);
  digitalWrite(waitingPin, waitingLED);
  digitalWrite(removePin, removeLED);
  delay(5000); // if mass becomes 0

  // receive speaker on/off
  // on: play tone
  // off: don't
  if(error == true)
  {
    readyLED = LOW;
    waitingLED = LOW;
    removeLED = LOW;
    digitalWrite(readyPin, readyLED);
    digitalWrite(waitingPin, waitingLED);
    digitalWrite(removePin, removeLED);
    Serial.println("Speaker is on");
    delay(2000);
  }
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
          scaleReady = true;
          waiting = false;
          // Set a boolean
        }
        else if (message.equals("Speaker ON")){
          Serial.println("received Speaker ON");
          error = true;
        }
        else if (message.equals("Remove mass")){
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
  waiting = true;
  scaleReady = false;
  
}
