#include <Wire.h>

// This is the I2C com for the LCD arduino
// GND, A4, A5 need to be connected between the 2 arduinos

char readyMsg[11]="ScaleReady";
char redLedOnMsg[11]="RED LED ON";
char rspeakerOnMsg[11]="Speaker ON";
int msgSize = 5;
float mass;

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
}


void loop() {
// TO request and receive message, e.g. mass
  float receive = requestInfo(5).toFloat();
  Serial.println(receive+1);
  delay(500);

// TO send out messages
////  // Send ready status
//  sendMessage(readyMsg);
//  delay(100);
//
//  // Send speaker ON
//  sendMessage(redLedOnMsg);
//  delay(100);
//
//  // Send RED LED ON
//  sendMessage(speakerOnMsg);
//  delay(100);

}

void sendMessage(char *message){
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(message);
  Wire.endTransmission();    // stop transmitting
}

String requestInfo(int infoSize){
    Wire.requestFrom(8, infoSize);   
    while (Wire.available()) { 
    char msg[infoSize];
    int i = 0;
    while( i<= infoSize - 1) {
      char c = Wire.read(); // receive byte as a character
      msg[i] = c;
      if (i == infoSize - 1) {
        String message = String(msg).substring(0, infoSize);
        Serial.println(message);
        return message;
      }
      i++;
    }
  }
}

