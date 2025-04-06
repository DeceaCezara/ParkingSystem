
#include <Wire.h>
#include <DFRobot_RGBLCD1602.h>
#include <Servo.h>

// Initialize the RGB LCD (with address 0x60 and 16x2 display)
DFRobot_RGBLCD1602 lcd(0x60, 16, 2);   

// Initialize Servo
Servo myservo;

// Pin Definitions for Ultrasonic Sensors
const int trig1 = 5; // Trigger for Ultrasonic Sensor 1
const int echo1 = 2; // Echo for Ultrasonic Sensor 1
const int trig2 = 7; // Trigger for Ultrasonic Sensor 2
const int echo2 = 3; // Echo for Ultrasonic Sensor 2

// Variables
int Slot = 4;           // Total number of parking slots
int flag1 = 0;
int flag2 = 0;

// Function to measure distance using an ultrasonic sensor
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);  // Ensure the trigger pin is low
  delayMicroseconds(2);        // Wait for 2 microseconds
  digitalWrite(trigPin, HIGH); // Send a pulse
  delayMicroseconds(10);       // Wait for 10 microseconds
  digitalWrite(trigPin, LOW);  // End the pulse
  
  long duration = pulseIn(echoPin, HIGH);  // Measure the time the pulse takes
  long distance = duration * 0.034 / 2;   // Convert to centimeters
  return distance;
}

void setup() {
  Serial.begin(9600);  // Start serial communication
  
  // Initialize LCD
  lcd.init();
  lcd.setRGB(0, 255, 0); // Set the initial backlight to green
  lcd.print("     WELCOME!    ");
  
  // Initialize pins for ultrasonic sensors
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  
  // Initialize Servo
  myservo.attach(4);
  myservo.write(100);  // Set initial servo position (barrier closed)
  
  delay(2000);
  lcd.clear();
}

void loop() {
  // Measure distance from ultrasonic sensors
  long distance1 = measureDistance(trig1, echo1);
  long distance2 = measureDistance(trig2, echo2);

  // Check for car at entry (Sensor 1)
  if (distance1 < 10 && flag1 == 0) { // If distance is less than 10 cm
    if (Slot > 0) {
      flag1 = 1;  // Set flag for car detected at entry
      if (flag2 == 0) {
        myservo.write(0);  // Open barrier
        Slot = Slot - 1;   // Decrease available slots
      }
    } else {
      // Parking full message
      lcd.setCursor(0, 0);
      lcd.print("    SORRY :(    ");
      lcd.setCursor(0, 1);
      lcd.print("  Parking Full  ");
      delay(3000);
      lcd.clear();
    }
  }

  // Check for car at exit (Sensor 2)
  if (distance2 < 10 && flag2 == 0) { // If distance is less than 10 cm
    flag2 = 1;  // Set flag for car detected at exit
    if (flag1 == 0) {
      myservo.write(0);  // Open barrier
      Slot = Slot + 1;   // Increase available slots
    }
  }

  // Reset flags and close barrier after both cars are processed
  if (flag1 == 1 && flag2 == 1) {
    delay(1000);  // Wait for 1 second
    myservo.write(100);  // Close barrier
    flag1 = 0;
    flag2 = 0;
  }

  // Display remaining slots on the LCD
  lcd.setCursor(0, 0);
  lcd.print("    WELCOME!    ");
  lcd.setCursor(0, 1);
  lcd.print("Slot Left: ");
  lcd.print(Slot);
  delay(500);
}
