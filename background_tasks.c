//*****************************************************************************
// background_taks.h
//
// fitnesswedgroup2
//
// Created on: 7/05/2020
// Authors: Max Young, Reto Schori,  Matthew Johnson
// Last modified: 9/05/2020
//
// Includes all of the background tasks needed to implement the fitness tracker
//
//*****************************************************************************


#include "background_tasks.h"

// *******************************************************
// Globals to module
// *******************************************************
uint16_t step_count = 0;
int16_t step_state = BELOW_THRESHOLD; //Either below or above
int16_t display_data = STEPS; //Either steps or distance
int16_t distance_format = KM; //Either km or miles


//*****************************************************************************
// Display step count or distance, depends of global var display_data.
//*****************************************************************************
void
displaySteps(){
    char string[17];  // 16 characters across the display
    char unit[6] = "KM";
    
    // Use display_data to decide whether to display steps or distance
    if (display_data == STEPS){
        usnprintf (string, sizeof(string), "step count: %d", step_count);
        OLEDStringDraw (string, 0, 0);
    }
    else if(display_data == DISTANCE){
        
        // Calculate distance from steps using factor of 0.9
        float distance = ((float)step_count * 0.9)/1000; // km

        // If we want miles then convert distance from km to miles
        if(distance_format == MILES){
            strncpy(unit, "Mile", sizeof(unit));
            distance = distance * KM_TO_MILE_FACTOR; //Conversion
        }

        // Use snprintf so can nicely display with decimal point
        snprintf(string, sizeof(string), "Distance: (%s)", unit);
        OLEDStringDraw (string, 0, 0);
        snprintf(string, sizeof(string), "%.3f", distance);
        OLEDStringDraw (string, 0, 1);

    }
}


//*****************************************************************************
// Handles all button press events, includes the four tiva buttons and switch on
// the booster pack, relies on buttons4.c having been initialized.
//*****************************************************************************
void handleButtonPress(){
    updateButtons ();
    uint8_t butStates[4];
    // Button states we are checking for
    butStates[LEFT] = checkButton (LEFT);
    butStates[RIGHT] = checkButton (RIGHT);
    butStates[UP] = checkButton (UP);
    butStates[DOWN] = checkButton (DOWN);
    uint8_t swt_left_state = checkSwitch(LEFT_SWT);

    // Toggles between display steps or distance if left or right button pushed
    if (butStates[LEFT] == PUSHED || butStates[RIGHT] == PUSHED){
        OrbitOledClear(); // Clean display
        switch(display_data){ // FSM, whatever we currently on, switch.
        case (STEPS):
            display_data = DISTANCE;
            break;

        case DISTANCE:
            display_data = STEPS;
            break;
        }
    }

    // Reset step count on long press of down button
    if (checkButtonLongPress(DOWN, butStates[DOWN]) == 1){
        OrbitOledClear();
        step_count = 0;
    }

    // Test mode is entered if SW1 is ON
    if (swt_left_state == ON){
        // Up button will add 100 steps
        if (butStates[UP] == PUSHED){
            step_count = step_count + 100;
            }
        // Down button will subtract 500 steps
        if (butStates[DOWN] == PUSHED){
            if (step_count >= 500){
                step_count = step_count -500;
            }
            else{ // Condition so dont get negative counts
                step_count = 0;
            }
        }
    }
    // Normal operation for up button
    else{
        if (butStates[UP] == PUSHED && display_data == DISTANCE){
            OrbitOledClear();
            switch(distance_format){ // FSM, whatever state we in, switch.
            case MILES:
                distance_format = KM;
                break;

            case KM:
                distance_format = MILES;
                break;

            }
        }
    }
}


//*****************************************************************************
// Takes an acc_norm and compares it to 1.5G, uses a state matchine to determine if a
// new step has occurred
//*****************************************************************************
void updateStepCount(int32_t accl_norm){
    switch(step_state){
    case ABOVE_THRESHOLD:
        // If we were above the threshold but are now below, update.
        if (accl_norm < STEP_THRESHOLD){
            step_state = BELOW_THRESHOLD;
        }
        break;

    case BELOW_THRESHOLD:
        // Only increase step count if we were below threshold and are now above
        if (accl_norm >= STEP_THRESHOLD){
            step_state = ABOVE_THRESHOLD;
            step_count++;
        }
        break;
    }
}


//*****************************************************************************
// reads in the passed circbuff pointers and calculates the accl norm
// return the norm of all 3 accelerometers
//*****************************************************************************
uint32_t getAcclNorm(circBuf_t* acc_x_buffer,circBuf_t* acc_y_buffer,circBuf_t* acc_z_buffer){
    uint16_t i=0;
    int16_t x_sum = 0;
    int16_t z_sum = 0; 
    int16_t y_sum = 0;
    for (i = 0; i<BUF_SIZE; i++){
        x_sum = x_sum + readCircBuf(acc_x_buffer);
        z_sum = z_sum + readCircBuf(acc_z_buffer);
        y_sum = y_sum + readCircBuf(acc_y_buffer);
    }
    // Calculate average by dividing sum of entries by num of entries
    x_sum = x_sum / BUF_SIZE;
    y_sum = y_sum / BUF_SIZE;
    z_sum = z_sum / BUF_SIZE;
    
    // Return norm is addition of all sums squared
    return x_sum*x_sum + y_sum*y_sum + z_sum*z_sum;
}


