//  Demo function:The application method to drive the stepper motor.
//  Hareware:Stepper motor - 24BYJ48,Seeed's Motor Shield v2.0
//  Author:Frankie.Chu
//  Date:20 November, 2012

//for lcd
#include "Wire.h"
#include "LiquidCrystal.h"

#define MOTOR_CLOCKWISE      0
#define MOTOR_ANTICLOCKWISE  1
/******Pins definitions*************/
#define MOTORSHIELD_IN1	8
#define MOTORSHIELD_IN2	11
#define MOTORSHIELD_IN3	12
#define MOTORSHIELD_IN4	13
#define CTRLPIN_A		9
#define CTRLPIN_B		10

const unsigned char stepper_ctrl[]={0x27,0x36,0x1e,0x0f};
struct MotorStruct
{
	int8_t speed;
	uint8_t direction;
};
MotorStruct stepperMotor;
unsigned int number_of_steps = 200;
const int stepsPerRevolution = 200; //same as above
const float revolutionsToOpen = 7.7; //the number of revolutions needed to open door
const unsigned int steps_to_open = 1000;

const int unlatchedCount = 10000;  //how long between opeing and closing
//lcd constants
// Connect via i2c, default address #0 (A0-A2 not jumpered)
LiquidCrystal lcd(0);

//relay init
int relayPin = 3;
int relayState = LOW;  //everything is off

//when pir is triggered, we want to give he user a reasonable time to present rfid
//if maxOnCount is exceeded, power things down
const int maxOnCount = 100;
int onCount = 0;

//pir init
int pirPin = 4;
int priState = LOW;
int pirVal = 0;

//rfid constants
int RFIDResetPin = 7;

//Register your RFID tags here
char tag1[13] = "4200442D173C";
char tag2[13] = "420048E6FD11";
char tag3[13] = "420044391827";
char tag4[13] = "01023101093A";
char tag5[13] = "01023C0A4376";
char tag6[13] = "01023C000E31";
char tag7[13] = "01023C0A3207";
char tag8[13] = "1A004116317C";
char tag9[13] = "1E009A81F9FC";
char tag10[13] = "1A004162261F";

/**********************************************************************/
/*Function: Get the stepper motor rotate                               */
/*Parameter:-int steps,the total steps and the direction the motor rotates.*/
/*			if steps > 0,rotates anticlockwise,			   			   */
/*			if steps < 0,rotates clockwise.           				   */
/*Return:	void                      							      */
void step(int steps)
{
	int steps_left = abs(steps)*4;
	int step_number;
	int millis_delay = 60L * 1000L /number_of_steps/(stepperMotor.speed + 50);
	if (steps > 0) 
	{
		stepperMotor.direction= MOTOR_ANTICLOCKWISE;
		step_number = 0; 
	}
    else if (steps < 0) 
	{
		stepperMotor.direction= MOTOR_CLOCKWISE;
		step_number = number_of_steps;
	}
	else return;
	while(steps_left > 0) 
	{
		PORTB = stepper_ctrl[step_number%4];
		delay(millis_delay);
		if(stepperMotor.direction== MOTOR_ANTICLOCKWISE)
		{
			step_number++;
		    if (step_number == number_of_steps)
		    	step_number = 0;
		}
		else 
		{
			step_number--;
		    if (step_number == 0)
		    	step_number = number_of_steps;
		}
		steps_left --;
		
	}
}
void initialize()
{
	pinMode(MOTORSHIELD_IN1,OUTPUT);
	pinMode(MOTORSHIELD_IN2,OUTPUT);
	pinMode(MOTORSHIELD_IN3,OUTPUT);
	pinMode(MOTORSHIELD_IN4,OUTPUT);
	pinMode(CTRLPIN_A,OUTPUT);
	pinMode(CTRLPIN_B,OUTPUT);
	stop();
	stepperMotor.speed = 25;
	stepperMotor.direction = MOTOR_CLOCKWISE;
}
/*******************************************/
void stop()
{
	/*Unenble the pin, to stop the motor. */
	digitalWrite(CTRLPIN_A,LOW);
    digitalWrite(CTRLPIN_B,LOW);
}

void setup()
{
  Serial.begin(9600);
  
  initialize();//Initialization for the stepper motor.

  //pir setup
  pinMode(pirPin, INPUT);

  //rfid setup
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
  
  //setup lcd
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Welcome!!!");
}


//rfid functions
void readRFIDReader() {
  char tagString[13];
  int index = 0;
  boolean reading = false;
  
  while(Serial.available()){

    int readByte = Serial.read(); //read next available byte

    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }

  checkTag(tagString); //Check if it is a match
  clearTag(tagString); //Clear the char of all value
  resetReader(); //reset the RFID reader
}

void checkTag(char tag[]){
///////////////////////////////////
//Check the read tag against known tags
// badly done - need to check against an array, this way stinks
///////////////////////////////////

  if(strlen(tag) == 0) return; //empty, no need to contunue

  if(compareTag(tag, tag1)){ // if matched tag1, do this
    lightLED(2);
    rotateStepper();

  }else if(compareTag(tag, tag2)){ //if matched tag2, do this
    lightLED(3);

  }else if(compareTag(tag, tag3)){
    lightLED(4);

  }else if(compareTag(tag, tag4)){
    lightLED(5);

  }else if(compareTag(tag, tag5)){
    lightLED(6);

  }else if(compareTag(tag, tag6)){
    lightLED(7);

  }else if(compareTag(tag, tag7)){
    lightLED(8);

  }else if(compareTag(tag, tag8)){
    lightLED(9);

  }else if(compareTag(tag, tag9)){
    lightLED(10);

  }else if(compareTag(tag, tag10)){
    lightLED(11);

  }else{
    Serial.println(tag); //read out any unknown tag
  }

}

void lightLED(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////
  Serial.println(pin);

  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}

void resetReader(){
///////////////////////////////////
//Reset the RFID reader to read again.
///////////////////////////////////
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

void clearTag(char one[]){
///////////////////////////////////
//clear the char array by filling with null - ASCII 0
//Will think same tag has been read otherwise
///////////////////////////////////
  for(int i = 0; i < strlen(one); i++){
    one[i] = 0;
  }
}

boolean compareTag(char one[], char two[]){
///////////////////////////////////
//compare two value to see if same,
//strcmp not working 100% so we do this
///////////////////////////////////

  if(strlen(one) == 0) return false; //empty

  for(int i = 0; i < 12; i++){
    if(one[i] != two[i]) return false;
  }

  return true; //no mismatches
}

void releaseStepper() {
  //we don't want the stepper to stay in the locked state.
  //should use an array of stepper pins ... later
  lcd.clear();
  //lcd.setBacklight(HIGH);
  lcd.setCursor(0, 1);
  lcd.print("Releasing Motor");
  
  digitalWrite(8, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
}

void rotateStepper() {
  
  lcd.clear();
  //lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print(" Access Granted");
  lcd.setCursor(0, 1);
  lcd.print("  Opening Door");
  
  
   Serial.println("clockwise");
   
   float steps = stepsPerRevolution * revolutionsToOpen;
  //myStepper.step(steps);
  //step(steps);
  step(steps_to_open);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Door Open,   ");
  lcd.setCursor(0, 1);
  lcd.print("  Please Enter  ");
  
  delay(unlatchedCount);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Closing Door");
   // return handle to where it should be
   
  Serial.println("counterclockwise");
  
 
  //myStepper.step(-steps);
  //step(-steps);
  step(-steps_to_open);
  
  //lcd.setBacklight(HIGH);

  //now power down the relay
  //release the door
  releaseStepper();
}


void loop()
{
  
  //delay(10000);
  /*
	step(100);//Stepper motors rotate anticlockwise 200 steps.
	delay(1000);
	step(-100);//Stepper motors rotate clockwise 200 steps.
	delay(1000);
*/
  onCount++;
  //check relay state to see if everything is on
  if (relayState) {
    //everything should be on, so we can check the rfid
    
    //check pir, if HIGH, reset stayOn counter
    pirVal = digitalRead(pirPin);
    if (pirVal == HIGH) {
      //we have motion, reset counter to give user more time
      onCount = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    Welcome,");
      lcd.setCursor(0, 1);
      lcd.print("   Swipe RFID");
    } else if (onCount > maxOnCount) {
      //we have waited for a valid rfid long enough, power down
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    Goodbye!");
      delay(1000);
      digitalWrite(relayPin, LOW);
      relayState = LOW;
      
      //release stepper so it isn't drawing
      releaseStepper();
      
      delay(1000);
      lcd.setBacklight(LOW);
    } else {
      //just a loop with no motion, no rfid
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Are You There?");
      lcd.setCursor(0, 1);
      lcd.print(maxOnCount - onCount);
    }
    
    //now check rfid
    readRFIDReader();
    
  } else {
    //check pir, if HIGH then turn on relay and turn relayState to on
    pirVal = digitalRead(pirPin);
    if (pirVal == HIGH) {
      lcd.clear();
      lcd.setBacklight(HIGH);
      //we have motion, power up
      onCount = 0;
      //now power everything up
      digitalWrite(relayPin, HIGH);
      relayState = HIGH;
      
    
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("    Welcome!");
      lcd.setCursor(0, 1);
      lcd.print("Powering on unit");
      
      //will now start checking rfid next time around
    }
  }
}
