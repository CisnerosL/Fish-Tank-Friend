#include <stdio.h>
#include <wiringPi.h>

#define IN1 1   //WPi GPIO 1 (RPi4 pin 12) use 'gpio readall' for WPi GPIO diagram
#define IN2 4   //WPi GPIO 4 (RPi4 pin 16)
#define IN3 5   //WPi GPIO 5 (RPi4 pin 18)
#define IN4 6   //WPi GPIO 6 (RPi4 pin 22)
const int teeth = 2048;
int Steps = 0;                  //stage of the stepper coils
bool Direction = true;          //ccw: RHR
int feedNum = 0;                //number of times the feeder has been ran

void stepper(int xw);
void shake();
void process();

int main (void) {
  printf("Beginning first fish feeding process\n");
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

  printf("\nFirst fish feeding complete\n");
  return 0;
}

void process() {
  Direction = false;
  for (int i = 0; i < ((teeth) / 8); i++) { //rotates only an eith turn since the initial oreintation has no openings on the bottom
    stepper(1);
    delayMicroseconds(2400); //800
  }

  printf("shake...");
  fflush(stdout);

  delay(1000);
  shake();
}

void shake() {
  for (int i = 0; i < 50 ; i++) {
    for (int j = 0; j < (teeth / 64); j++) {
      stepper(1);
      delayMicroseconds(3000);  //2700); //800
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
