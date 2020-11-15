//*****************************************************************************
// background_taks.h
//
// fitnesswedgroup2
//
// Authors: Max Young, Reto Schori,  Matthew Johnson
// Last modified: 9/05/2020
//
//Includes all of the background tasks needed to implement the fitness tracker
//*****************************************************************************

#include <circBufT_int32_t.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "utils/ustdlib.h"
#include "../OrbitOLED/OrbitOLEDInterface.h"
#include "../OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "readAcc.h"
#include "buttons4.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#ifndef BACKGROUND_TAKS_H_
#define BACKGROUND_TAKS_H_
#define BUF_SIZE 16
#define KM_TO_MILE_FACTOR 0.621371
#define STEP_THRESHOLD 98304 //~1.5G (1G = 256**2 (raw), 1.5*256**2 = 98304)
enum stepStates {ABOVE_THRESHOLD=0, BELOW_THRESHOLD};
enum displayDataStates {STEPS, DISTANCE};
enum displayDistanceFormats {KM, MILES};



//*****************************************************************************
// Display steps updates the OrbitOLED screen with the current step count. Also
// displays distance in miles and km depending on the current states
//*****************************************************************************
void
displaySteps();

//*****************************************************************************
// Handles all button press events, includes the four tiva buttons and switch on
// the booster pack, relies on buttons4.c having been initialized.
//*****************************************************************************
void handleButtonPress();

//*****************************************************************************
// Takes and acc_norm and compares it to 1.5G, uses a state matching to determine if a
// new step has occurred
//*****************************************************************************
void updateStepCount(int32_t accl_norm);

//*****************************************************************************
// reads in the passed circbuff pointers and calculates the accl norm
// return the norm of all 3 accelerometers
//*****************************************************************************
uint32_t getAcclNorm(circBuf_t* acc_x_buffer,circBuf_t* acc_y_buffer,circBuf_t* acc_z_buffer);



#endif /* BACKGROUND_TAKS_H_ */
