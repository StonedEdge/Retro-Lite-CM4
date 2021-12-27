# ATtiny84 Script

## Description
The purpose of the ATtiny84 onboard micrcontroller is to perform XXX main functions: 

1) Activate power to the boost/buck regulators to turn on system power 
2) Safely shutdown the operating system via a push button 
3) Safely shutdown the operating system when battery voltage depletes less than 3.1v to prevent undervoltage/prolong the battery lifespan
4) Monitor the BQ24292i charging chip over i2c, including setting the correct parameters for the Retro Lite CM4, like max charge current and voltage cutoffs
5) Allow for time delays before perorming actions (i.e hold button 3 seconds for power off, 1 second power on. 10 second delay before regulators switched off, 30 second low voltage shutdown delay)



