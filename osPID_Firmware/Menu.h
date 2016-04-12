/*****************************************************************************
*
* Title:	Menu.h
*
* Description:	Contains definitions for Menu structure.  The menu is a big
*	state machine.  Rather than using a large switch statement or a bunch
*	of complex functions to react to each input event, there are enumerated
*	types which define each state, the paths between the states, and the
*	inputs which cause the states to change.  This implementation was
*	chosen in hope that the resulting state machine will be easily
*	modified.  While this is a fairly common way of implementing a state
*	machine, this particular flavor was inspired by application code for
*	Atmel's AVR Butterfly dev kit.
*
****************************************************************************/

#ifndef MENU_H
#define MENU_H

//Menu states
enum {
  // Dashboard Menu
  ST_DASHBRD = 1,	//need to make sure no state is 0
  ST_SP, ST_PV, ST_OUT, ST_MAN,
  
  // Input Menu
  ST_INPUT, ST_TYPE, ST_TH_R, ST_TH_T, ST_TH_BETA, ST_TH_V
  
  // PID Menu
  ST_PID, ST_P, ST_I, ST_D, ST_ACTION, ST_HYSTERESIS,
  
  // Output Menu
  ST_OUTPUT, ST_WINDOW, ST_RELAY
};

/* Code for state transitions.  Defines the paths between all the states, and
 * which inputs trigger movement between states.	*/

typedef struct
{
  uint8 state;
  uint8 input;
  uint8 nextState;
} MENU_NEXTSTATE;

extern const rom MENU_NEXTSTATE menu_nextstate[];

/* Code for state functions.  Defines the function performed in each state
 * as well as a text description displayed to the user at each state. */

typedef struct
{
  uint8 state;
  const rom char *text;
  uint8 (* StateFunction) (uint8 input);
} MENU_STATE;

extern const rom MENU_STATE menu_state[];

//State function initializations
uint8 MenuPV(uint8 input);
uint8 MenuRange(uint8 input);
uint8 MenuSP(uint8 input);
uint8 MenuSecr(uint8 input);
uint8 MenuPressureUnit(uint8 input);
uint8 MenuVelUnit(uint8 input);
uint8 MenuFlowUnit(uint8 input);
uint8 MenuFlowRange(uint8 input);
uint8 MenuKFactor(uint8 input);
uint8 MenuAreaShape(uint8 input);
uint8 MenuDiameter(uint8 input);
uint8 MenuXDim(uint8 input);
uint8 MenuYDim(uint8 input);
uint8 MenuCtrl(uint8 input);
uint8 Menu1SP(uint8 input);
uint8 MenuAlarm(uint8 input);
uint8 MenuAlarmReset(uint8 input);
uint8 MenuAlarmInhibit(uint8 input);
uint8 MenuAlarmDelay(uint8 input);
uint8 MenuPeak(uint8 input);
uint8 MenuValley(uint8 input);
uint8 MenuResolution(uint8 input);
uint8 MenuPercentDisplay(uint8 input);
uint8 MenuDampening(uint8 input);
uint8 MenuAdv(uint8 input);
uint8 MenuPol(uint8 input);
uint8 MenuPoh(uint8 input);
uint8 MenuMaintSP1(uint8 input);
uint8 MenuMaintSP2(uint8 input);
uint8 MenuAddress(uint8 input);
uint8 MenuWriteEnable(uint8 input);
uint8 MenuZero(uint8 input);
uint8 MenuSpan(uint8 input);

#endif