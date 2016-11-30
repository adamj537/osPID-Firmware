#include <LiquidCrystal.h>
#include "AnalogButton_local.h"

/* Code for state transitions.  Defines the paths between all the states, and
 * which inputs trigger movement between states.	*/
const menuNextState_t nextState [] =
{
//	State			Input			Next State
	{ST_DASHBRD,	BUTTON_BACK,	ST_DASHBRD},
	{ST_DASHBRD,	BUTTON_UP,		ST_DASHBRD},
	{ST_DASHBRD,	BUTTON_DOWN,	ST_CONFIG},
	{ST_DASHBRD,	BUTTON_ENTER,	ST_SP},
  
	{ST_CONFIG,		BUTTON_BACK,	ST_DASHBRD},
	{ST_CONFIG,		BUTTON_UP,		0},

	{ST_RANGE,		0,				ST_PV},

  //This tells StateMachine function to stop.
  //If it is removed, the program will crash!
  {0,		0,		0}
};

/* Code for state functions.  Defines the function performed in each state
 * as well as a text description displayed to the user at each state. */
const menuState_t menu_state [] =
{
//State			//Text		//Function
  {ST_DASHBRD,	"DashBrd",	MenuDashBoard},
  {ST_CONFIG,	"Config",	MenuConfig},
  {ST_ATUNE,	"",			MenuATune},
  {ST_PROFILE,	"Profiles",	MenuProfile},
  {ST_SP,		"S",		MenuSetpoint},
  {ST_IN,		"I",		MenuInput},
  {ST_OUT,		"O",		MenuOutput},
  {ST_MAN,		"M",		MenuManual},
  {ST_P,		"P",		MenuPTerm},
  {ST_I,		"I",		MenuITerm},
  {ST_D,		"D",		MenuDTerm},
  {ST_PV,		"",		MenuPV},

  //This tells the main loop to stop.
  //If it is removed, the program will crash!
  {0,		0,		0}
};

bool editing = false;
bool tuning = false;
byte curMenu = 0;
byte mIndex = 0;
byte mDrawIndex = 0;
byte editDepth = 0;

byte mMain[] = {0,1,2,3};
byte mDash[] = {4,5,6,7};
byte mConfig[] = {8,9,10,11};
byte *mMenu[] = {mMain, mDash, mConfig};
const byte TYPE_NAV=0;
const byte TYPE_VAL=1;
const byte TYPE_OPT=2;
char profname[] = {'N','o',' ','P','r','o','f'};

LiquidCrystal lcd(A1, A0, 4, 7, 8, 9);			//Initialize the LCD
AnalogButton button(A3, 0, 253, 454, 657);		//Initialize the buttons

void MenuDashBoard()
{

}

void MenuConfig ()
{

}

void MenuATune ()
{
  lcd.print(tuning ? F("Cancel ") : F("ATune  "));	//Print menu item.
}

void MenuProfile ()
{
  if(runningProfile)lcd.print(F("Cancel "));		//Print menu item.
  else lcd.print(profname);
}

void MenuSetpoint ()
{

}

void MenuInput ()
{

}

void MenuOutput ()
{

}

void MenuPTerm ()
{

}

void MenuITerm ()
{

}

void MenuDTerm ()
{

}

void MenuPV ()
{

}

void drawLCD()
{
  boolean highlightFirst = (mDrawIndex==mIndex);
  drawItem(0,highlightFirst, mMenu[curMenu][mDrawIndex]);
  drawItem(1,!highlightFirst, mMenu[curMenu][mDrawIndex+1]);
  if(editing) lcd.setCursor(editDepth, highlightFirst?0:1);
}

void drawItem(byte row, boolean highlight, byte index)
{
  char buffer[7];
  lcd.setCursor(0,row);
  double val = 0;
  int dec = 0;
  int num = 0;
  char icon = ' ';
  boolean isNeg = false;
  boolean didneg = false;
  byte decSpot = 0;
  boolean edit = editing && highlightedIndex==index;
  boolean canEdit =! tuning;
  switch(getMenuType(index))
  {
  case TYPE_NAV:
    lcd.print(highlight? '>':' ');
   
   //lcd.print menu name
   
   //break

  case TYPE_VAL:

    // fetch name of item & whether it's editable
    
    lcd.print(edit? '[' : (highlight ? (canEdit ? '>':'|') : ' '));
    
    // display error if item had invalid value
    if(isnan(val))
    { //display an error
      lcd.print(icon);
      lcd.print( now % 2000<1000 ? F(" Error"):F("      ")); 
      return;
    }
    
    for(int i=0;i<dec;i++) val*=10;
    
    num = (int)round(val);
    buffer[0] = icon;
    isNeg = num<0;
    if(isNeg) num = 0 - num;
    didneg = false;
    decSpot = 6-dec;
    if(decSpot==6)decSpot=7;
    for(byte i=6; i>=1;i--)
    {
      if(i==decSpot)buffer[i] = '.';
      else {
        if(num==0)
        {
          if(i>=decSpot-1) buffer[i]='0';
          else if (isNeg && !didneg)
          {
            buffer[i]='-';
            didneg=true;
          }
          else buffer[i]=' ';
        }
        else {
          buffer[i] = num%10+48;
          num/=10;
        }
      }
    }     
    lcd.print(buffer);
    break;
  case TYPE_OPT: 

    lcd.print(edit ? '[': (highlight? '>':' '));    
    switch(index)
    {
    case 7:    
      lcd.print(modeIndex==0 ? F("M Man  "):F("M Auto "));
      break;
    case 11://12:

      lcd.print(ctrlDirection==0 ? F("A Direc"):F("A Rever")); 
      break;
    }

    break;
  default: 
    return;
  }

  //indication of altered state
  if(highlight && (tuning || runningProfile))
  {
    //should we blip?
    if(tuning)
    { 
      if(now % 1500 <500)
      {
        lcd.setCursor(0,row);
        lcd.print('T');
      }
    }
    else //running profile
    {
      if(now % 2000 < 500)
      {
        lcd.setCursor(0,row);
        lcd.print('P');
      }
      else if(now%2000 < 1000)
      {
        lcd.setCursor(0,row);
        char c;
        if(curProfStep<10) c = curProfStep + 48; //0-9
        else c = curProfStep + 65; //A,B...
        lcd.print(c);      
      }  
    }
  }
}

boolean changeflag=false;

void back()
{
  if(editing)
  { //decrease the depth and stop editing if required

    editDepth--;
    if(getMenuType(highlightedIndex)==TYPE_VAL)
    {
      if(editDepth==7-getValDec(highlightedIndex))editDepth--; //skip the decimal  
    }
    if(editDepth<3)
    {
      editDepth=0;
      editing= false;
      lcd.noCursor();
    }
  }
  else
  { //if not editing return to previous menu. currently this is always main
    //depending on which menu we're coming back from, we may need to write to the eeprom
    if(changeflag)
    {
      if(curMenu==1)
      { 
        //MemoryBackupDash();	//TODO:  backup dashboard settings
      }
      else if(curMenu==2) //tunings may have changed
      {
        //MemoryBackupTunings();	//TODO:  backup tuning settings
        myPID.SetTunings(kp,ki,kd);
        myPID.SetControllerDirection(ctrlDirection);
      }
      changeflag=false;
    }
    if(curMenu!=0)
    { 
      highlightedIndex = curMenu-1; //make sure the arrow is on the menu they were in
      mIndex=curMenu-1;
      curMenu=0;
      mDrawIndex=0;

    }
  }
}

void updown(bool up)
{

  if(editing)
  {
    changeflag = true;
    byte decdepth;
    double adder;
    switch(getMenuType(highlightedIndex))
    {
    case TYPE_VAL:
      decdepth = 7 - getValDec(highlightedIndex);
      adder=1;
      if(editDepth<decdepth-1)for(int i=editDepth;i<decdepth-1;i++)adder*=10;
      else if(editDepth>decdepth)for(int i=decdepth;i<editDepth;i++)adder/=10;

      if(!up)adder = 0-adder;

      double *val, minimum, maximum;
      switch(highlightedIndex)
      {
      case 4: 
        val=&setpoint; 
        break;
      case 6:  
        val=&output; 
        break;
      case 8:  
        val=&kp; 
        break;
      case 9:  
        val=&ki; 
        break;
      case 10:  
        val=&kd; 
        break;
      }
      
      minimum = getValMin(highlightedIndex);
      maximum = getValMax(highlightedIndex);
      (*val)+=adder;
      if((*val)>maximum)(*val)=maximum;
      else if((*val)<minimum)(*val)=minimum;
      break; 
    case TYPE_OPT:
      switch(highlightedIndex)
      {
      case 7:
        modeIndex= (modeIndex==0?1:0);
        /*mode change code*/
        myPID.SetMode(modeIndex);
        break;
      case 11://12:
        ctrlDirection = (ctrlDirection==0?1:0);
        Serial.println(ctrlDirection);
        break;
      }

      break;
    }

  }
  else
  {
    if(up)
    {
      if(mIndex>0)
      {
        mIndex--;
        mDrawIndex=mIndex;
      }
    }
    else
    {
      byte limit = 3;// (curMenu==2 ? 4 : 3); 
      if(mIndex<limit)
      {
        mDrawIndex =mIndex;
        mIndex++;
      }
    }
    highlightedIndex = mMenu[curMenu][mIndex];
  }
}

void ok()
{
  if(editing)
  {
    byte dec = getValDec(highlightedIndex);
    if(getMenuType(highlightedIndex) == TYPE_VAL &&(editDepth<6 || (editDepth==6 && dec!=1)))
    {
      editDepth++;
      if(editDepth==7-dec)editDepth++; //skip the decimal
    }
  }
  else
  {
    switch(highlightedIndex)
    {
    case 0: 
      curMenu=1;
      mDrawIndex=0;
      mIndex=0; 
      highlightedIndex = 4; //setpoint
      changeflag = false;
      break;
    case 1: 
      curMenu=2;
      mDrawIndex=0;
      mIndex=0; 
      highlightedIndex = 8; //kp
      changeflag = false;
      break;
    case 2: 
      changeAutoTune();/*autotune code*/
      break;
    case 3: 
      if(runningProfile)StopProfile();
      else StartProfile();

      break;
    case 5: /*nothing for input*/
      break;
    case 6: 
      if(modeIndex==0 && !tuning) editing=true; 
      break; //output
    case 4: //setpoint
    case 8: //kp
    case 9: //ki
    case 10: //kd
//    case 11: //windowsize
    case 11: //12: //direction
      editing=true;
      break; //verify this is correct
    case 7: 
      if(!tuning) editing=true; 
      break; //mode
    }
    if(editing)
    {
      editDepth=3;
      lcd.cursor();
    }
  }
}