/******************************************************************************
 *
 *	Project Name:	osPID
 *	Designer:	Adam Johnson
 *	Language:	C++
 *	Environment:	Arduino
 *
 *	Description:	The osPID is a PID controller.  This is the main file.
 *
 * TODO:	- Organize setup function by module
 *		- Put LCD stuff in another file
 *		- Put menu stuff in another file and use a state machine
 *		- Put EEPROM stuff in another file (and remove EEPROM stuff
 *		  from input and output files too
 *		- Put serial communication stuff in another file
 *****************************************************************************/

//#define USE_SIMULATION

#include <EEPROM.h>
#include "PID_v1_local.h"
#include "PID_AutoTune_v0_local.h"
#include "InputCard.h"
#include "OutputCard.h"
#include "EEPROMAnything.h"
#include "Menu.h"

uint8_t state = 0;						//state machine variable
uint8_t nextState = 0;					//state machine variable
uint8_t (* StateFunction) (uint8) = 0x00;	//function performed at each state

// ***** PIN ASSIGNMENTS *****
const byte buzzerPin = 3;
const byte systemLEDPin = A2;

unsigned long now;
unsigned long lcdTime;
unsigned long buttonTime;
unsigned long ioTime;
unsigned long serialTime;
boolean sendInfo=true;
boolean sendDash=true;
boolean sendTune=true;
boolean sendInputConfig=true;
boolean sendOutputConfig=true;

double setpoint=250;
double input=250;
double output=50;
double pidInput=250;

double kp = 2;
double ki = 0.5;
double kd = 2;
byte ctrlDirection = 0;
byte modeIndex = 0;
byte highlightedIndex=0;

PID myPID(&pidInput, &output, &setpoint,kp,ki,kd, DIRECT);
InputCard myInput;
byte inputType;
OutputCard myOutput;

double aTuneStep = 20;
double aTuneNoise = 1;
unsigned int aTuneLookBack = 10;
byte ATuneModeRemember = 0;
PID_ATune aTune(&pidInput, &output);

byte curProfStep=0;
byte curType=0;
float curVal=0;
float helperVal=0;
unsigned long helperTime=0;
boolean helperflag=false;
unsigned long curTime=0;

/*Profile declarations*/
const unsigned long profReceiveTimeout = 10000;
unsigned long profReceiveStart=0;
boolean receivingProfile=false;
const int nProfSteps = 15;

byte proftypes[nProfSteps];
unsigned long proftimes[nProfSteps];
float profvals[nProfSteps];
boolean runningProfile = false;

/******************************************************************************
 *
 *	Function:		setup
 *
 *	Description:	This function runs once when the osPID starts up.  It
 *			initializes the peripheral devices on the microcontroller.
 *
 *****************************************************************************/

void setup(void)
{
	Serial.begin(9600);					// Initialize UART.
	lcdTime=10;							// Initialize LCD.
	buttonTime=1;						// Set up analog buttons.
	ioTime=5;
	serialTime=6;
	//windowStartTime=2;
	lcd.begin(8, 2);
	
	lcd.setCursor(0,0);
	lcd.print(F(" osPID   "));
	lcd.setCursor(0,1);
	lcd.print(F(" alpha   "));
	delay(1000);						//Wait for display to refresh
  
	//TODO:  initialize EEPROM to default values?

	myInput.SetSensorType(inputType);	//Initialize input card
	myOutput.InitializeCard();			//Initialize output card

	myPID.SetSampleTime(1000);			//Set up PID library
	myPID.SetOutputLimits(0, 100);
	myPID.SetTunings(kp, ki, kd);
	myPID.SetControllerDirection(ctrlDirection);
	myPID.SetMode(modeIndex);
}

/******************************************************************************
*
* Function:	StateMachine
*
* Description:	This function shifts between the different states of a state
*		machine.  We're using it for a menu defined in Menu.h.
*
* Parameters:	uint8 state - the current state of the machine
*		uint8 input - something that can cause the state to change.
*
******************************************************************************/

 uint8 StateMachine(uint8 state, uint8 input)
 {
  uint8 nextState;	//state machine temperary variable
  uint8 i = 0;		//counter

  nextState = state;	//default action is to stay in the same state

  // Cycle through the state transition table.
  for (i = 0; menu_nextstate[i].state; i++)
  {
    // If a match is found, go to the specified state.
    if ((menu_nextstate[i].state == state) &&
       (menu_nextstate[i].input == input))
    {
      nextState = menu_nextstate[i].nextState;
      break;
    }
  }
	
  return nextState;
}

/******************************************************************************
*
* Function:	loop
*
* Description:	This function runs over and over after the setup function
*		finishes.
*
******************************************************************************/

void loop()
{
  uint8 i;			// counter
  
  // Perform state machine actions.
  if (nextState != state)
  {
    // Transition to new state.
    state = nextState;
    
    // Find the new state's function pointer.
    for (i = 0; menuState[i].state; i++)
    {
      if (menuState[i].state == state)
      {
        // Run the state's function.
        nextState = menuState[i].EntryFunction(state);
        break;
      }
    }
  }
  
  // If any key is pressed (or held), clock the state machine, but don't
  // do anything if ther's a state action to complete.
  else if (keys)
  {
    nextState = StateMachine(state, keys);
    keys = 0x00;	// Reset key flags.
  }
  
  now = millis();		//Get a timestamp

  if(now >= buttonTime)		//Process button presses
  {
    switch(button.Get())
    {
    case BUTTON_NONE:
      break;

    case BUTTON_BACK:
      back();
      break;

    case BUTTON_UP:      
      updown(true);
      break;

    case BUTTON_DOWN:
      updown(false);
      break;

    case BUTTON_OK:
      ok();
      break;
    }
    buttonTime += 50;
  }

  bool doIO = (now >= ioTime);	//Figure out if enough time has passed to get a new input sample
  
  if(doIO)			//Process input
  { 
    ioTime+=250;
#ifdef USE_SIMULATION
    DoModel();
    pidInput = input;
#else
    input =  myInput.ReadFromCard();
    if(!isnan(input))pidInput = input;

#endif /*USE_SIMULATION*/
  }
  

  if(tuning)			//Process AutoTune operations
  {
    byte val = (aTune.Runtime());

    if(val != 0)
    {
      tuning = false;
    }

    if(!tuning)			// If tuning is finished, set tuning parameters
    { 
      kp = aTune.GetKp();
      ki = aTune.GetKi();
      kd = aTune.GetKd();
      myPID.SetTunings(kp, ki, kd);
      AutoTuneHelper(false);
      //MemoryBackupTunings();	//TODO:  backup tuning settings
    }
  }
  else
  {
    if(runningProfile) ProfileRunTime();
    //allow the pid to compute if necessary
    myPID.Compute();
  }

  if(doIO)			//Update the output
  {
#ifdef USE_SIMULATION
    theta[29] = output;
#else
    myOutput.WriteToCard(output);
#endif /*USE_SIMULATION*/  
  }

  if(now>lcdTime)		//Update the LCD
  {
    drawLCD();
    lcdTime+=250; 
  }
  
  if(millis() > serialTime)	//Process serial communications
  {
    //SerialReceive();	//TODO:  process serial data received
    //SerialSend();	//TODO:  send serial data
    serialTime += 500;
  }
}

void changeAutoTune()
{
  if(!tuning)
  {
    //initiate autotune
    AutoTuneHelper(true);
    aTune.SetNoiseBand(aTuneNoise);
    aTune.SetOutputStep(aTuneStep);
    aTune.SetLookbackSec((int)aTuneLookBack);
    tuning = true;
  }
  else
  { //cancel autotune
    aTune.Cancel();
    tuning = false;
    AutoTuneHelper(false);
  }
}

void AutoTuneHelper(boolean start)
{

  if(start)
  {
    ATuneModeRemember = myPID.GetMode();
    myPID.SetMode(MANUAL);
  }
  else
  {
    modeIndex = ATuneModeRemember;
    myPID.SetMode(modeIndex);
  } 
}

void StartProfile()
{
  if(!runningProfile)
  {
    //initialize profle
    curProfStep=0;
    runningProfile = true;
    calcNextProf();
  }
}

void StopProfile()
{
  if(runningProfile)
  {
    curProfStep=nProfSteps;
    calcNextProf(); //runningProfile will be set to false in here
  } 
}

void ProfileRunTime()
{
  if(tuning || !runningProfile) return;
  
  boolean gotonext = false;

  //what are we doing?
  if(curType==1) //ramp
  {
    //determine the value of the setpoint
    if(now>helperTime)
    {
      setpoint = curVal;
      gotonext=true;
    }
    else
    {
      setpoint = (curVal-helperVal)*(1-(float)(helperTime-now)/(float)(curTime))+helperVal; 
    }
  }
  else if (curType==2) //wait
  {
    float err = input-setpoint;
    if(helperflag) //we're just looking for a cross
    {

      if(err==0 || (err>0 && helperVal<0) || (err<0 && helperVal>0)) gotonext=true;
      else helperVal = err;
    }
    else //value needs to be within the band for the perscribed time
    {
      if (abs(err)>curVal) helperTime=now; //reset the clock
      else if( (now-helperTime)>=curTime) gotonext=true; //we held for long enough
    }

  }
  else if(curType==3) //step
  {

    if((now-helperTime)>curTime)gotonext=true;
  }
  else if(curType==127) //buzz
  {
    if(now<helperTime)digitalWrite(buzzerPin,HIGH);
    else 
    {
       digitalWrite(buzzerPin,LOW);
       gotonext=true;
    }
  }
  else
  { //unrecognized type, kill the profile
    curProfStep=nProfSteps;
    gotonext=true;
  }

  if(gotonext)
  {
    curProfStep++;
    calcNextProf();
  }
}

void calcNextProf()
{
  if(curProfStep>=nProfSteps) 
  {
    curType=0;
    helperTime =0;
  }
  else
  { 
    curType = proftypes[curProfStep];
    curVal = profvals[curProfStep];
    curTime = proftimes[curProfStep];

  }
  if(curType==1) //ramp
  {
    helperTime = curTime + now; //at what time the ramp will end
    helperVal = setpoint;
  }   
  else if(curType==2) //wait
  {
    helperflag = (curVal==0);
    if(helperflag) helperVal= input-setpoint;
    else helperTime=now; 
  }
  else if(curType==3) //step
  {
    setpoint = curVal;
    helperTime = now;
  }
  else if(curType==127) //buzzer
  {
    helperTime = now + curTime;    
  }
  else
  {
    curType=0;
  }

  if(curType==0) //end
  { //we're done 
    runningProfile=false;
    curProfStep=0;
    Serial.println("P_DN");
    digitalWrite(buzzerPin,LOW);
  } 
  else
  {
    Serial.print("P_STP ");
    Serial.print(int(curProfStep));
    Serial.print(" ");
    Serial.print(int(curType));
    Serial.print(" ");
    Serial.print((curVal));
    Serial.print(" ");
    Serial.println((curTime));
  }

}
