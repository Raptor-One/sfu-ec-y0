#include <Stepper.h>
#include <Servo.h> 

const int stepsPerRevolution = 200;  

#define CAL_STEP_SIZE 10
#define CAL_SPEED 10

#define NUM_SLOTS 10
#define LIMIT_SW_1 7
#define LIMIT_SW_2 6

#define SERVO 3
#define SERVO_UP 127
#define SERVO_DOWN 74

#define PHOTO_1 A0
#define PHOTO_2 A1
#define PHOTO_3 A2
#define MIN_PHOTO_DIFF 200

Stepper stepper(stepsPerRevolution, 8, 9, 10, 11);
Servo servo;
int state = 0;
int currentSlot = 0;
int n = 0;
int slotVals[10];

int totalStepWidth = 0;

void setup() 
{
  Serial.begin(9600);
  
  pinMode(LIMIT_SW_1, INPUT);
  pinMode(LIMIT_SW_2, INPUT);
  
  stepper.setSpeed(CAL_SPEED);
  servo.attach(SERVO);
}

int sorterReadValue(int slot)
{
  return slotVals[slot];
}

int gotoSlot( int slot )
{
  stepper.step( (slot-currentSlot) * (totalStepWidth/NUM_SLOTS) );
  currentSlot = slot;
}

int sorterSwap(int slotA, int slotB)
{
  gotoSlot( slotA );
  servo.write( SERVO_UP );
  gotoSlot( 9 );
  servo.write( SERVO_DOWN );
  gotoSlot( slotB );
  servo.write( SERVO_UP );
  gotoSlot( slotA );
  servo.write( SERVO_DOWN );
  gotoSlot( 9 );
  servo.write( SERVO_UP );
  gotoSlot( slotB );
  servo.write( SERVO_DOWN );
  int tmp = slotVals[slotA];
  slotVals[slotA] = slotVals[slotB];
  slotVals[slotB] = tmp;
}

int p1, p2, p3, max, min, mid;

void loop() 
{
  switch(state)
  {
    case 0: // calibration: start
      if( digitalRead(LIMIT_SW_1) == LOW ) {
        stepper.step(CAL_STEP_SIZE);
      }
      else{
        totalStepWidth = 0;
        state = 1;
        Serial.println("Limit 1 Hit!");
      }
      break;
    case 1: // calibration: measure length
      if( digitalRead(LIMIT_SW_2) == LOW ) {
        stepper.step(-CAL_STEP_SIZE);
        totalStepWidth += CAL_STEP_SIZE;
      }
      else{
        Serial.print("Limit 2 Hit! Total step width: ");
        Serial.println(totalStepWidth);
        currentSlot = 0;
        state = 2;
      }
      break;
    case 2: // read in slot values
      p1 = analogRead(PHOTO_1);
      p2 = analogRead(PHOTO_2);
      p3 = analogRead(PHOTO_3);
      max = max(p1, max(p2, p3));
      min = min(p1, min(p2, p3));
      if(max-min > MIN_PHOTO_DIFF)
      {
        mid = (max-min)/2;
        p1 = p1 > mid ? 1 : 0;
        p2 = p2 > mid ? 1 : 0;
        p3 = p3 > mid ? 1 : 0;
      } else {
        p1 = p2 = p3 = 0;
      }
      slotVals[currentSlot] = (p1 << 2) | (p2 << 1) | p3;
      state = currentSlot < NUM_SLOTS-1 ? 3 : 4;
      break;
    case 3: // move to next slot
      stepper.step(totalStepWidth/NUM_SLOTS);
      currentSlot++;
      state = 2;
      break;
    case 4: // run sort
      n = currentSlot;
      sort(n);
      state = 5;
    case 5: // done
      delay(10);
      break;
  }
 
}


// *******************************************************************************
// ***************** Edit This Function To Implement Sorting *********************
// *******************************************************************************
// Fuctions you can use:
//
// int sorterReadValue(int slot) ;
// Will return the value currrent in the passed in slot index
// Example: int value = sorterReadValue( 3 );
//
// int sorterSwap(int slotA, int slotB) ;
// Will run a sequence that will swap the values of the two passed in slot indexes
// Example: sorterSwap( 1, 2 );
//
void sort(int n)
{
  sorterSwap( 0, 1 ); // as a demonstration, the robot will swap the frist two values (tiles)
}
// *******************************************************************************
