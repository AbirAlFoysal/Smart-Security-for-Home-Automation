//Code for Blynk App (ON/OFF Relay,Switch)

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <ArduinoOTA.h>


Servo myservo;

int Status = D5;
int sensor = D6;
int sensor2 = D1;


// #define BLYNK_TEMPLATE_ID "TMPL8sQNV9B7"
// #define BLYNK_DEVICE_NAME "esp project"
// #define BLYNK_AUTH_TOKEN "U133YYmWGnHUEAhc4QqZgTysmP8YMj-3"z
#define BLYNK_TEMPLATE_ID "TMPLb_sUCNVp"
#define BLYNK_DEVICE_NAME "esp project"
// wifi detail.
// Redmi Note 10S
// abir125852

char auth[] = "9_SrHEr7QGTlLyvDem1b1YT6KmpyUh0j";  //Auth Token from Blynk Project
char ssid[] = "Fahim K20 pro";   //Router Name
char pass[] = "fahim999"; //Router Password



BlynkTimer timer;
const int relay1 = D8; // D5-GPIO 14

const int Pin1 = D0;  // D1-GPIO 05

int button1Pin, button2Pin, button3Pin, button4Pin;

int relay1State = LOW;
int button1State = LOW;

void checkPhysicalButton();
// Every time we connect to the cloud...
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V4);
}

//*********************************************
// When App button is pushed - switch the state
BLYNK_WRITE(V4) {
  relay1State = param.asInt();
  //Serial.println(relay1State);
  digitalWrite(relay1, relay1State);
}

void sendSensor()
{
  button1Pin = digitalRead(Pin1);
  //button1
  if (button1Pin == 0) {
    // btnState is used to avoid sequential toggles
    if (button1State != LOW) {

      // Toggle LED state
      relay1State = !relay1State;
      digitalWrite(relay1, relay1State);

      // Update Button Widget
      Blynk.virtualWrite(V4, relay1State);
    }
    button1State = LOW;
  } else {
    button1State = HIGH;
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);
  pinMode(relay1, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(Pin1, INPUT);
  myservo.attach(D7);
  pinMode(sensor, INPUT);// declare sensor as input
  pinMode(sensor2, INPUT);
  pinMode(Status, OUTPUT);  // declare LED as output

  digitalWrite(D4, LOW);
  digitalWrite(relay1, relay1State);

  // Setup a function to be called every 100 ms
  timer.setInterval(100L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
  if (relay1State == 1) {
    int pos;
    int state = digitalRead(sensor); 
    Serial.print("Sensor Data 1:");
    Serial.println(state);
    //edit
    int state2= digitalRead(sensor2);
   
   Serial.print("Sensor Data 2:");
   Serial.println(state2);
    Serial.println(state);
    delay(1000);
    if ((state || state2) == HIGH) {
      digitalWrite (Status, HIGH);
      Serial.println("Motion detected!");

      for (pos = 0; pos <= 180; pos += 2) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
      for (pos = 180; pos >= 0; pos -= 2) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
    else {
      digitalWrite (Status, LOW);
      
      Serial.println("Motion absent!");
    }
  }
  else{digitalWrite (Status, LOW);}
}
