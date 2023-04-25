/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18LF47K42
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
//#include "mcc_generated_files/interrupt_manager.h"
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "shared_variables.h"

/*
                         Main application
 */

volatile uint8_t tmr1_delay_complete = 0;
volatile uint8_t tmr3_delay_complete = 0;
//volatile uint8_t tmr3_delay_started  = 0;

void main(void)
{
    // Initialize the device
    // Using 16MHz internal clock for below
    SYSTEM_Initialize();
    
    //Setup used registers for state internal to device
    uint8_t     tmr1_started = 0;
    uint8_t     tmr3_started = 0;
    
    //Initialize all open Drain ports
    IO_RD0_SetOpenDrain();              //PS_POR_B_3V3_L to Mosfet (active high grounds PS_POR_B to FPGA)
    IO_RC6_SetOpenDrain();              //CAM1_RESET_N to MIPI Channel 1 (Close to FMC), pulled high on Aper-Oculus
    IO_RD1_SetOpenDrain();              //CAM0_RESET_N to MIPI Channel 0 (Close to Display Connectors), pulled high on Aper-Oculus
    IO_RD2_SetOpenDrain();              //SD_RESET_N (3.3V) pulled high on Aper-Oculus
    
    //Since RC0 is an output to a voltage converter, we must drive this high or low, we can not open drain it. 
    //IO_RC0_SetOpenDrain()
    
    //Note, since we setup VADJ_EN as an output, we have to turn that on manually as well
    IO_RB3_SetOpenDrain();
    IO_RB3_SetHigh();
    
    //Must ensure dir is low 
    IO_RC2_SetLow();
    
    //Set all PS resets to a known value 
    IO_RC0_SetLow();
    IO_RD2_SetLow();
    
    //Set all PL resets to a known value
    IO_RC6_SetLow();
    IO_RD1_SetLow();
    
    //Set PS_POR_B_3V3_L to high impedance
    IO_RD0_SetHigh();                   //We want high impedance, pull up so PS_POR_B is pulled down to GND.

    // Enable the Global Interrupts for timers
    INTERRUPT_GlobalInterruptEnable();
    
    //Begin program, check for CC_PL_PGOOD and CC_PS_PGOOD, if both are high, then we can deassert PS_POR_B_3V3_L
    while (1)
    {
        // Check if CC_PS_PGOOD is high and we have not waited 25ms
        if(!IO_RA2_PORT && !tmr1_delay_complete && !tmr1_started) {
            TMR1_StartTimer();
            tmr1_started = 1;           //Ensure we don't start timer over and over again 
        }
        
        if(!IO_RA3_PORT && IO_RD5_PORT && !tmr3_delay_complete && !tmr3_started) {  //PL relies on VADJ_PG going high, not 5V sourceable so use 3.3V line 
            TMR3_StartTimer();
            tmr3_started = 1;
        }
        
        //Deassert bidirectional drain teh moment CC_PS_PGOOD is high 
        if(!IO_RA2_PORT) {
            IO_RD0_SetLow();                                                        //Set low to pull down gate of Q21, so PS_POR_B goes high 
        }
        
        //Normal reset operation for this condition for PS ports
        //CC_PS_PGOOD must be high (i.e. our port is low due to voltage level conversion with FET)
        //25ms timer must be finished
        //Must deassert the PS_POR_B line (monitored on RC1, set on RD0)
        if(!IO_RA2_PORT && tmr1_delay_complete && IO_RC1_PORT) {
            IO_RC0_LAT = IO_RA7_PORT;       //Port 30, USB RESET, since both are active low or high, set them here! (Low propagates through)
            IO_RD2_LAT = IO_RA6_PORT;       //Port 36, SD RESET
        }
        else {
            IO_RC0_SetLow();
            IO_RD2_SetLow();
        }
        
        //Normal reset operation for this condition for PL ports
        if(!IO_RA3_PORT && IO_RD5_PORT && tmr3_delay_complete) {
            IO_RC6_LAT = IO_RD7_PORT;       //If high, high impedance, if low, assert low, Camera 1 Reset
            IO_RD1_LAT = IO_RD6_PORT;       //If high, high impedance, if low, assert low, Camera 0 Reset
        }
        else {
            IO_RC6_SetLow();
            IO_RD2_SetLow();
        }
    }
}
/**
 End of File
*/