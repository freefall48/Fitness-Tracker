//*****************************************************************************
//
// fitnessTracker.c
//
// fitnesswedgroup2
//
// Implements a step counter using accelerometer and display steps and distance
// on the ORBIT Booster pack OLED.
// Setting SW1 on the Booster pack up enters test mode where the up and down
// buttons add and remove steps.
// Left and Right buttons toggle between steps and distance
// Long press on the down button resets step count and distance to zero.
//
// Authors: Max Young, Reto Schori,  Matthew Johnson
// Last modified: 9/05/2020
//*****************************************************************************




#include <circBufT_int32_t.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "../OrbitOLED/OrbitOLEDInterface.h"
#include "../OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "readAcc.h"
#include "buttons4.h"
#include "UART.h"
#include "delay.h"
#include "background_tasks.h"


//*****************************************************************************
// Globals
//*****************************************************************************
enum TASK_STATES{READY, SERVICED, WAIT};
circBuf_t acc_x_buffer;
circBuf_t acc_y_buffer;
circBuf_t acc_z_buffer;
uint16_t SAMPLE_RATE_HZ = 96;// Humans step at aprox. 3Hz and buff size is 16
uint8_t update_display = WAIT;                                    // 2*3*16 = 96
uint8_t calc_accl_norm = WAIT;
uint8_t handle_button_press = WAIT;
uint8_t get_accl_data = WAIT;
uint16_t update_display_count = 0;                                    
uint16_t calc_accl_norm_count = 0;
uint16_t handle_button_press_count = 0;
uint16_t get_accl_data_count = 0;

//*****************************************************************************
// Constants
//*****************************************************************************
#define DISPLAY_FREQ_HZ 10
#define CALC_NORM_FREQ_HZ 96
#define HANDLE_BUTTON_FREQ_HZ 50
#define GET_ACCL_DATA_FREQ_HZ 96

//*****************************************************************************
// The interrupt handler for the SysTick interrupt.
// Handles flags for whether tasks should be executed depending on their freq
//*****************************************************************************
void
SysTickIntHandler(void)
{
    // Every systick we update count, this controls freq that tasks are done.
    update_display_count++;
    calc_accl_norm_count++;
    handle_button_press_count++;
    get_accl_data_count++;

    // If a task should be executed then reset count and toggle flag 
    // for the round robin in main.
    
    if(update_display_count >= (SAMPLE_RATE_HZ / DISPLAY_FREQ_HZ)){
        update_display = READY;
        update_display_count = 0;
    }

    if(calc_accl_norm_count >= ( SAMPLE_RATE_HZ / CALC_NORM_FREQ_HZ)){
        calc_accl_norm = READY;
        calc_accl_norm_count =0;
    }

    if(handle_button_press_count >= (SAMPLE_RATE_HZ / HANDLE_BUTTON_FREQ_HZ)){
        handle_button_press = READY;
        handle_button_press_count = 0;
    }

    if(get_accl_data_count >= (SAMPLE_RATE_HZ / GET_ACCL_DATA_FREQ_HZ)){
        get_accl_data = READY;
        get_accl_data_count = 0;
    }
}


//*****************************************************************************
// Initialization functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

//     Set up the period for the SysTick timer.  The SysTick timer period is
//     set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


//*****************************************************************************
// Main function initializes display, accelerometer, buffers and button and then
// handles background tasks in a round robin fashion.
//*****************************************************************************
int
main(void)
{
    // Initialisations
    IntMasterDisable();
    initAccl();
    initClock ();
    OLEDInitialise ();
    initCircBuf (&acc_z_buffer, BUF_SIZE);
    initCircBuf (&acc_x_buffer, BUF_SIZE);
    initCircBuf (&acc_y_buffer, BUF_SIZE);
    initButtons ();
    initialiseUSB_UART ();
    
    // Enable interrupts to the processor.
    IntMasterEnable();
    
    // Main loop services all background tasks by polling if they are ready.
    while(1){
        
        // If task is ready then reset flag and execute the task.
        
        if(update_display == READY){ 
            displaySteps();
            update_display = WAIT;
        }
        if(calc_accl_norm == READY){
            //Calculate the norm and check if there has been a step
            uint32_t accl_norm = getAcclNorm(&acc_x_buffer, &acc_y_buffer, &acc_z_buffer);
            updateStepCount(accl_norm);
            calc_accl_norm = WAIT;
        }
        if(handle_button_press == READY){
            handleButtonPress();
            handle_button_press = WAIT;
        }
        if(get_accl_data == READY){
            // Get latest accl data and write to the buffers.
            vector3_t data =  getAcclData();
            writeCircBuf(&acc_x_buffer,  data.x);
            writeCircBuf(&acc_z_buffer,  data.z);
            writeCircBuf(&acc_y_buffer,  data.y);
            get_accl_data = WAIT;
        }
    }
}
