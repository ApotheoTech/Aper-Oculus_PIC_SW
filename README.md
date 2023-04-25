# Aper-Oculus_PIC_SW
This repository provides the PIC software as a starting point for power and reset control on the Aper Oculus. You can modify as needed using a Microchip programmer like the ICD4 or equivalent.

# How to program:
Aper-Oculus connector J15 provides a "Cortex JTAG/SWD 10 Pin Mini" interface that can be connected to a PICkit 4 Target Adapter board, and then a suitable programmer (like the ICD4 or newer).
This allows for active debugging of PIC during operation.

* You must have MPLAB X IDE installed, with an 8 bit compiler. This project utilzes the MPLAB Code Configurator for easy code generation and pin assignment.
* Connect J4 of PICkit 4 target board to J15 of Aper-Oculus. Power up Aper-Oculus via DIP-SWITCH J10 (Ensure 5.5mm power supply of 12V is connected to board first)
* Connect Suitable MicroChip programmer to board.

# Current Status:
The current push is working on development boards in a naive fashion, powering up all supplies, and deasserting PS_POR_B as expected. 
TODO:
* Add I2C probing for FMC to probe FMC EEPROM to find requested VADJ
* Add I2C read/write to VADJ VREG to change voltage during power up before setting PS_POR_B high
* Possibly add UART for a later spin to communicate with Kria SOM for board status?
