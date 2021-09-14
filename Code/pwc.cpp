#include <stdio.h>
#include <wiringPi.h>


int r1Del = 300;		//OUTPUT pump safety time
int r2Del = 85;			//INPUT pump safety time

int relayPin1 = 1;		//WPi GPIO 1 (RPi0 pin 12) | WPi GPIO 26 (RPi4 pin 32)
int relayPin2 = 4;		//WPi GPIO 4 (RPi0 pin 16) | WPi GPIO 27 (RPi4 pin 36)
const int tFloatPin = 3;	//WPI GPIO 3 (RPi0 pin 15) | WPi GPIO 28 (RPi4 pin 38)
const int bFloatPin = 2;	//WPi GPIO 2 (RPi0 pin 13) | WPi GPIO 29 (RPi4 pin 40)
//NOTE might need to make float pins pulldown or pullup with 'gpio mode [WPi GPIO Pin] up'and down. Guess and check

int fillTimeVar;
int fillTime = fillTimeVar = r2Del;
int emptyTimeVar;
int emptyTime = emptyTimeVar = r1Del;
bool tState, bState;
bool isFull, isEmpty;
bool isError = true;	//assume there's an error untill proven otherwise

int errorCode = 0;

void waterChangeSetup();
void waterChangeRun();
bool isErrorState();
void errorFunc();

int main (void) {
  printf("Beginning PWC process\n");
  if (wiringPiSetup() == -1)
    return 1;
  waterChangeSetup();
  waterChangeRun();

  if (errorCode == 0)
    printf("PWC process complete\n");

  return errorCode;
}


void waterChangeRun() {

  //Serial.println("WATER CHANGE IN PROGRESS");
  emptyTimeVar = emptyTime;
  fillTimeVar = fillTime;
  tState = digitalRead(tFloatPin); //top floater state
  bState = digitalRead(bFloatPin); //bottom floater state
  //Serial.print("tState = "); Serial.print(tState); Serial.print("  bState = "); Serial.println(bState);
  printf("tState = %d, bState = %d\n",tState,bState);


  if (tState == LOW && bState == HIGH) {

    printf("Water Full\n");	//Serial.println("Water Full");
    isFull = true;
    isEmpty = false;
  }

  printf("Activating PHASE I\n");	//Serial.println("Activating PHASE I");
  while (bState != LOW && isFull == true && emptyTimeVar > 0) {    //detects error while emptying

    bState = digitalRead(bFloatPin);
    tState = digitalRead(tFloatPin);

    printf("tState = %d, bState = %d, emptyTimeVar = %d",tState,bState,emptyTimeVar);	//Serial.print("tState = "); Serial.print(tState); Serial.print(" bState = "); Serial.print(bState); Serial.print(" emptyTimeVar = "); Serial.print(emptyTimeVar);

    digitalWrite(relayPin1, LOW);       //turn on phase 1
    if (emptyTimeVar > 1)
      printf(" Emptying out...\n");	//Serial.println(" Emptying out...");
    else {
      printf(" ERROR Empty time took too long\n");	//Serial.println(" ERROR Empty time took too long");
      errorFunc();
    }

    delay(1000);
    emptyTimeVar -= 1;
  }
  digitalWrite(relayPin1, HIGH);          //turn off phase 1
  printf("Turning off PHASE I\n");	//Serial.println("Turning off PHASE I");


  if (bState == LOW && tState == HIGH) {
    printf("Water Empty\n");	//Serial.println("Water Empty");
    isFull = false;
    isEmpty = true;
    //isError = false;
  }

  delay(3000);




  printf("Activating PHASE II\n");	//Serial.println("Activating PHASE II");
  while (tState != LOW && isEmpty == true && fillTimeVar > 0) {    //detects error while filling by keeping track of how long the fill value has been running for

    tState = digitalRead(tFloatPin);
    bState = digitalRead(bFloatPin);
    printf("tState = %d, bState = %d, fillTimeVar = %d",tState,bState,fillTimeVar);	//Serial.print("tState = "); Serial.print(tState); Serial.print("bState = "); Serial.print(bState); Serial.print("fillTimeVar = "); Serial.println(fillTimeVar);

    digitalWrite(relayPin2, LOW);         //turn on phase 2
    if (fillTimeVar > 1)
      printf(" Filling up...\n");	//Serial.println(" Filling up...");
    else {
      printf(" ERROR Fill time took too long\n");	//Serial.println(" ERROR Fill time took too long");
      errorFunc();
    }
    delay(1000);
    fillTimeVar -= 1;
  }
  digitalWrite(relayPin2, HIGH);          //turn off phase 2
  printf("Turning off PHASE II\n");	//Serial.println("Turning off PHASE II");

  isErrorState(); //check if everything looks alright
}


void waterChangeSetup() {
  pinMode(tFloatPin, INPUT); //hard wired as pull-up
  pinMode(bFloatPin, INPUT); //hard wired as pull-up

  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
}

bool isErrorState(){
  if (bState == tState) {
    printf("\nERROR\n*********************bState == tState******************************\n");
    errorFunc();
    return true;
  }
  return false;
}

//error procedure. Stop everything
void errorFunc(){
  delay(200);

  printf("Stopping everyting and reverting to startup configuration\n");
  errorCode = -1;
}
