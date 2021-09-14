#include <stdio.h>
#include <wiringPi.h>

#define IN1 1   //WPi GPIO 1 (RPi4 pin 12) use 'gpio readall' for WPi GPIO diagram
#define IN2 4   //WPi GPIO 4 (RPi4 pin 16)
#define IN3 5   //WPi GPIO 5 (RPi4 pin 18)
#define IN4 6   //WPi GPIO 6 (RPi4 pin 22)

const int clkPin = 23;	//WPi GPIO 23 (RPi4 pin 33)
const int dtPin = 24;	//WPi GPIO 24 (RPi4 pin 35)
const int swPin = 25;	//WPi GPIO 25 (RPi4 pin 37)

const int teeth = 2048;
int Steps = 0;                  //stage of the stepper coils
bool Direction = true;          //ccw: RHR
const int MULTIPLIER = teeth * 2 / 80; //makes the encoder knob turn the stepper faster
int encoderVal = 0;
int encoderValI = 1;


int getEncoderTurn(void);
void stepper(int xw);

int main (void) {
  printf("Beginning set fish feeder process\n");
  if (wiringPiSetup() == -1)
    return 1;

  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT);
  //pullUpDnControl(swPin, PUD_UP); //pull-up resistor for switch. Fun fact: theres a bug in WPi where the command for pull-up resistrs doesn't work

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  printf("Please ajdust rotary encoder to the desired position\n");
  //fflush(stdout);

  printf("swPin: %d\n", digitalRead(swPin));

  while (digitalRead(swPin) != LOW) { //put a resistor from 3.3V to swPin to stop the program. WPi pullup resistors command doesn't work

   // printf("swPin: %d\n", digitalRead(swPin));

    int change = getEncoderTurn();
    encoderVal = encoderVal + change;

    if (encoderVal != encoderValI) {
      //Serial.println(encoderVal * -1);  //outputs the direction the encoder is being turned (CCW+)
      int rotate = encoderVal - encoderValI;

      printf("%d",rotate); //outputs the current value of the encoder (gets smaller if rotary encoder is being turned CCW and bigger if CW)
      printf(" ");
      fflush(stdout);

      if (rotate == 1) {
        Direction = true;   //sets the direction the motor rotates to clockwise
        for (int i = 0; i < (MULTIPLIER); i++) { //the *2 is for the way the motor is being turned. /4 so it stops every quarter turn
          stepper(1);
          delayMicroseconds(5000); //800
        }
      } else {
        Direction = false;  //sets the direction the motor rotates to counter clockwise
        for (int i = 0; i < (MULTIPLIER); i++) { //the *2 is for the way the motor is being turned. /4 so it stops every quarter turn
          stepper(1);
          delayMicroseconds(5000); //800
        }
      }
      encoderValI = encoderVal;
    }
  }

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  printf("\nFish feeder set\n");
  return 0;
}


int getEncoderTurn(void) {  //function to detect if the rotary encoder is being turned CW or CCW
  static int oldA = HIGH;           //set the oldA as HIGH
  static int oldB = HIGH;           //set the oldB as HIGH
  int result = 0;
  int newA = digitalRead(clkPin);   //read the value of clkPin to newA
  int newB = digitalRead(dtPin);    //read the value of dtPin to newB
  if (newA != oldA || newB != oldB) { //if the value of clkPin or the dtPin has changed
    if (oldA == HIGH && newA == LOW)//something has changed
      result = (oldB * 2 - 1);
  }
  oldA = newA;
  oldB = newB;
  return result;
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
