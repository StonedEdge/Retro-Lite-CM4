![Image of Retro Lite CM4](https://i.imgur.com/h1mcu5v.jpg)
# Retro Lite CM4: Raspberry Pi Handheld

## Description
The Retro Lite CM4 is a joint project between Dmcke5 and StonedEdge from the BitBuilt forums. The console is powered by a 4000mAh Li-Po and is designed to run with various software images, including but not limited to RetroPie, Lakka or Batocera, directly from an SD card. The project housing was inspired by a Nintendo Switch Lite, but is not a 100% match to the original. The CM4 cannot currently emulate consoles released after the PSP or Dreamcast. This means GameCube, Wii and 3DS are unplayable on this device.  

## Features
- Raspberry Pi 4 SoC: Broadcom BCM2711 quad-core Cortex-A72 @ 1.5GHz (overclockable to 2.1GHz)
- 2GB of LPDDR4-3200 SDRAM
- 5.0GHz IEEE 802.11b/g/n/ac wireless, Bluetooth 5.0
- 800 x 480 HDMI video output (HDMI0 - internal)
- 1080p 60fps HDMI video output (HDMI1 - external)
- Switched to a tempered glass screen protector instead of painted plastic, now is printed
- Fuel gauge for accurate battery SoC monitoring - MAX17055 
- Custom software stack specific to the Retro Lite hardware (HUD)
- Custom menu activated by pushing select + R3, which allows you to calibrate the sticks, see various things like CPU temp, bring up an on screen keyboard for input, and more
- USB-C PD charging capabilities up to 15v (1C charge rate at 2A)
- Stereo Audio Output
- Headphone Jack with automatic switching
- 4000mAh custom lipo, courtesy of [Helder](https://github.com/Helder1981)
- Dual stacked shoulder buttons (L, R, LZ, RZ)
- Safe software/hardware shutdown to prevent corruption to the SD card data
- USB 2.0 2-port downstream hub
- External dock passthrough PCB with 4 port USB-hub

Click the link below to view a more detailed buildlog of the Retro Lite CM4: 

https://bitbuilt.net/forums/index.php?threads/retro-lite-cm4-a-stonededge-and-dmcke5-collaboration.3934/

## Requirements to Build the Retro Lite CM4

### Code 
Code required to complete the handheld includes an ATtiny84 monitor script, which can be flashed via the 1.27mm pitch 6 pin header on the main Retro Lite PCB with an Arduino or similar. Code for the i2s audio amplifier, fuel gauge and OSD functionality can be compiled directly on the CM4 module from the command line. Current code is non-functional, however changes will be pushed shortly when the firmware has been fixed. 

### CAD
The CAD files for the Retro Lite CM4 are not available or open source at this stage. Please do not ask myself or Dmcke5 to release them yet, as we will not do so. There are plans to release the files at a later date. 

### PCB
The Retro Lite CM4 requires 4 custom designed PCBs to be installed, not including the CM4 carrier board. These include: 

a) Retro Lite CM4 AIO carrier board 

b) Retro Lite Controller PCB (Right)

c) Retro Lite Controller PCB (Left)

d) Retro Lite Button Daughterboard

### BOM (Bill of Materials) 
Link to the BoM will be added at a later stage. 
Total BoM to build the Retro Lite CM4 exceeds $250 USD. 

### Contributors 
I'd like to thank these people in no particular order for the awesome work you have done to help me complete this project. 

* Dmcke5 for designing the case assets, including machining our prototypes, code and much more. Wouldn't have been possible to complete this without you 
* YveltalGriffin for suggesting various hardware changes and reviews to PCB/schematics, as well as generally being open and willing to help 
* Juckett D, for creating the Aegis CM4 and offering tips and tricks along the way. Check out his work here: https://github.com/juckettd
