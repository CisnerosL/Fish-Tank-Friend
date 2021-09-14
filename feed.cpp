#include <stdio.h>
#include <wiringPi.h>

//use 'gpio readall' for WPi GPIO diagram
#define IN1 5	//WPi GPIO 5  (RPi0 pin 18) | WPi GPIO 1 (RPi4 pin 12)
#define IN2 6	//WPi GPIO 6  (RPi0 pin 22) | WPi GPIO 4 (RPi4 pin 16)
#define IN3 26	//WPi GPIO 26 (RPi0 pin 32) | WPi GPIO 5 (RPi4 pin 18)
#define IN4 27	//WPi GPIO 27 (RPi0 pin 36) | WPi GPIO 6 (RPi4 pin 22)
const int teeth = 2048;
int Steps = 0;			//stage of the stepper coils
bool Direction = true;		//ccw: RHR
int feedNum = 0;		//number of times the feeder has been ran
int compartments = 4;		//number of food compartments

void stepper(int xw);
void shake();
void process();

int main (void) {
  printf("Beginning fish feeding process\n");
  if (wiringPiSetup() == -1)
    return 1;

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  printf("dispensing...");
  fflush(stdout);

  process();

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  printf("\nFish feeding complete\n");
  return 0;
}

void process() {
  Direction = false;
  for (int i = 0; i < ((teeth) / compartments)-1; i++) { //-1 bc 11 does not go into 2048 evenly. /4 so it stops every quarter turn
    stepper(1);
    delayMicroseconds(2400); //800
  }

  printf("shake...");
  fflush(stdout);

  delay(1000);
  shake();
}

void shake() {
  for (int i = 0; i < 20; i++) {		//50//shake duration. Number of times the motor shakes from one side to the other
    for (int j = 0; j < (teeth / 50); j++) {	//64//shake intensity. How far the motor turns in one direction before going the other direction. Smaller number = bigger turns
      stepper(1);
      delayMicroseconds(4000);  //3000//2700); //800
    }
    Direction = !Direction;
  }
}

void stepper(int xw) {
  for (int x = 0; x < xw; x++) {
    switch (Steps) {
      case 0:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      case 1:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 2:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 3:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      default:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
    }

    //SetDirection function
  if (Direction == 1) {   //if direction is true (clockwise)
    Steps++;
  }
  if (Direction == 0) {
    Steps--;
  }
    if (Steps > 3) {
      Steps = 0;
    }
    if (Steps < 0) {
      Steps = 3;
    }
  }
}

