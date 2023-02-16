# ATtiny84 C++ Script

## Description
The purpose of the ATtiny84 onboard micrcontroller is to perform 5 simple functions: 

1) Drive the EN pin of the boost/buck regulators high, to turn on system power 
2) Safely shutdown the operating system via a push button, and shut off regulator output after a reasonable delay (in this case, 10 seconds)
3) Safely shutdown the operating system when battery voltage depletes less than 3.1v to prevent undervoltage/prolong the battery lifespan. This can be set as low as 2.8v but I would not recommend setting it lower than 3.0v to maintain good battery health 
4) Monitor the BQ24292i charging chip over i2c, including setting the correct parameters/registers for the Retro Lite CM4, like max charge current and voltage cutoffs
5) Allow for specific time delays before performing actions (i.e hold button 3 seconds for power off, 1 second power on. 10 second delay before regulators switched off, 30 second low voltage shutdown delay) - adjustable via the powerOn, powerOff, and shutDown variables
6) Set the CM4 to the lowest possible power down state by driving GLOBAL_EN low after software shutdown

### Install Instructions

The easiest way to flash the ATtiny84 is with an Arduino UNO set up as an ICSP (in-circuit serial programmer) directly from within the Arduino IDE.
https://www.arduino.cc/en/software

Please install the below ATtiny cores for Arduino IDE support and choose the internal 8Mhz clock at 5v when programming your ATtiny84. 
https://github.com/damellis/attiny

For further understanding of the BQ24292i, please check the official datasheet below. Magic number code (i.e directly writing hexadecimal values to the write registers) will be eliminated soon, with proper bitshifting logic to be implemented at a later date. 
https://www.ti.com/lit/ds/symlink/bq24292i.pdf


