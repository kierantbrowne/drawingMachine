/*Drawing Machine alpha
  by: Kieran Browne
  date: 21st April 2014
  last modified: 23rd April 2014 


  Motor A on the left
  Motor B on the right
*/
#include <Servo.h>
Servo servo;

float px, py; // where the system is drawing
const float MOTORSEPARATION =152.0; // motor separation (all values given in notches
const float AX = 0.0; // motor A x coord
const float BX = AX+MOTORSEPARATION; // motor B x coord
float mAr = 100.0 ; // dist between m1 and weight
float mBr = 100.0 ; // dist between m1 and weight
// motors at full speed turn 81rpm, gears have 16 notches
const float NPM = 0.0216; // notches per millisecond
boolean draw = false;
// Clockwise and counter-clockwise definitions.
// Depending on how you wired your motors, you may need to swap.
#define CW  0
#define CCW 1

// Motor definitions to make life easier:
#define MOTOR_A 0
#define MOTOR_B 1

// Pin Assignments //
// Don't change these! These pins are statically defined by shield layout
const byte PWMA = 11;  // PWM control (speed) for motor A
const byte PWMB = 10; // PWM control (speed) for motor B
const byte DIRA = 13; // Direction control for motor A
const byte DIRB = 12; // Direction control for motor B

// type
int tt = 40;
int tm = 44;
int tb = 50;
int st;

//dir
byte adir;
byte bdir;

// potential fix to the height loss problem
int pulselength = 10;
int offset = 2; // it is likely that if steppers were used this would not be necessary

void setup()
{
  setupArdumoto(); // set pins for dc motors
  // find initial pointer location
  px = (sq(mAr)-sq(mBr)-sq(AX)+sq(BX))/(2*(BX-AX));
  py =  sqrt(sq(mAr)-sq(px-AX));
  servo.attach(2); // set pin for servo standoff
  drawing(false); // dont draw until pointer is positioned
  delay(30000);
}

void loop()
{
  // E
  st = 18;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st,tb);
  movePointerTo(st+10,tb);
  drawing(false);
  movePointerTo(st,tm);
  drawing(true);
  movePointerTo(st+8,tm);
  drawing(false);
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st+10,tt);
  // V
  st += 12;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st+4,tb);
  movePointerTo(st+8,tt);
  drawing(false);
  // O
  st +=12;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st,tb);
  movePointerTo(st+8,tb);
  movePointerTo(st+8,tt);
  movePointerTo(st,tt);
  drawing(false);
  // L
  st +=14;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st,tb);
  movePointerTo(st+6,tb);
  drawing(false);
  // U
  st +=12;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st,tb);
  movePointerTo(st+6,tb);
  movePointerTo(st+6,tt);
  drawing(false);
  // T
  st +=12;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st+8,tt);
  drawing(false);
  movePointerTo(st+4,tt);
  drawing(true);
  movePointerTo(st+4,tb);
  drawing(false);
  // I
  st +=10;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st,tb);
  drawing(false);
  // O
  st +=6;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tt);
  drawing(true);
  movePointerTo(st,tb);
  movePointerTo(st+8,tb);
  movePointerTo(st+8,tt);
  movePointerTo(st,tt);
  drawing(false);
  // N
  st +=14;
  drawing(false); // dont draw until pointer is positioned
  movePointerTo(st,tb);
  drawing(true);
  movePointerTo(st,tt);
  movePointerTo(st+8,tb);
  movePointerTo(st+8,tt);
  drawing(false);
}

void movePointerTo(float newX, float newY){
  
  float newAradius = sqrt(sq(newY)+sq(newX-AX));
  float newBradius = sqrt(sq(newY)+sq(newX-BX));
  
  float changeA = newAradius - mAr;
  float changeB = newBradius - mBr;
  
  pulse(changeA,changeB); 
  
  delay(500);
  
  mAr = newAradius; //update r values
  mBr = newBradius;
}

void pulse(float changeA, float changeB){
  int apow=pulselength; int bpow=pulselength;
  if (changeA > 0) adir = 0; else {adir = 1; apow+=offset;}
  if (changeB > 0) {bdir = 0; bpow +=0;} else bdir = 1;
  for (int i=0 ; i<max(abs(changeA),abs(changeB)); i++){
        if(abs(changeA)>=i) driveArdumoto(MOTOR_A, adir, 255);
        delay(apow);
        stopArdumoto(MOTOR_A);
        delay(10);
        if(abs(changeB)>=i) driveArdumoto(MOTOR_B, bdir, 255);
        delay(bpow);
        stopArdumoto(MOTOR_B);
        delay(10);
  }
}
      
        
    

void drawing(boolean d){
  if ((draw == false)&&(d == true)){
    for(int pos = 90; pos>=10; pos-=1){                                
      servo.write(pos); delay(15); 
    }
    draw = true;
  }else if((draw == true)&&(d == false)){
    for(int pos = 10; pos < 90; pos += 1){
      servo.write(pos); delay(15);
    } 
    draw = false;
  }
}


/*
///////////////////////////// 

ARDUMOTO LIB
by Jim Lindblom

/////////////////////////////
*/

// driveArdumoto drives 'motor' in 'dir' direction at 'spd' speed
void driveArdumoto(byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_A)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
  else if (motor == MOTOR_B)
  {
    digitalWrite(DIRB, dir);
    analogWrite(PWMB, spd);
  }  
}

// stopArdumoto makes a motor stop
void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}

// setupArdumoto initialize all pins
void setupArdumoto()
{
  // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}
