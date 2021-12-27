![Image of Retro Lite CM4](https://i.imgur.com/h1mcu5v.jpg)
# Retro Lite CM4: Raspberry Pi Handheld

## Description
The Retro Lite CM4 is a joint project between Dmcke5 and StonedEdge from the BitBuilt forums. The console is powered by a 4000mAh Li-Po and is designed to run with various software images, including but not limited to RetroPie, Lakka or Batocera, directly from an SD card. The project housing was inspired by a Nintendo Switch Lite, but is not a 100% match to the original. The CM4 cannot currently emulate consoles released after the PSP or Dreamcast. This means GameCube, Wii and 3DS are currently unplayable until the Foundation releases a newer version of the Compute Module line. 

## Features
- Raspberry Pi 4 SoC: Broadcom BCM2711 quad-core Cortex-A72 @ 1.5GHz (overclockable to 2.1GHz)
- 2GB of LPDDR4-3200 SDRAM
- 5.0GHz IEEE 802.11b/g/n/ac wireless, Bluetooth 5.0
- 800 x 480 HDMI video output (HDMI0 - internal)
- 1080p 60fps HDMI video output (HDMI1 - external)
- Switched to a tempered glass screen protector instead of painted plastic, now is printed
- Fuel gauge for accurate battery SoC monitoring - MAX17055 
- Custom software stack specific to the Retro Lite hardware (HUD)
- Custom menu, activated by pushing select + R3. You can calibrate the analog sticks, see CPU/GPU temp, bring uponscreen keyboard for input. More features to be added.
- USB-C PD charging capabilities up to 15v (1C charge rate at 2A)
- Stereo Audio Output via i2s audio amplifier (i2c controlled)
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
Code required to complete the handheld includes an ATtiny84 monitor script, which can be flashed via the 1.27mm pitch 6 pin header on the main Retro Lite PCB with an Arduino or similar programmer. Code for the i2s audio amplifier, fuel gauge and OSD functionality can be compiled directly on the CM4 module from the command line. Current code is unfinished as of December 2021, however we are pushing bug fixes when we find them. 

To flash the software to the Retro Lite CM4, you can either download the latest RetroPie Retro Lite custom image, or follow the below steps to get up and running. Plan is to eventually put this into a single script. I have not currently tested our firmware on images other than RetroPie, however I assume other images should also work fine. 

a) Download the latest version of RetroPie for the Raspberry Pi 4/400 here (also compatible with CM4): 
https://retropie.org.uk/download/ 

b) Install the WM8960 I2S audio amplifier script, by running the script here: 
https://github.com/StonedEdge/seeed-voicecard

c) Install the latest version of WiringPi here: 
https://github.com/StonedEdge/WiringPi

d) sudo apt-get install libpng12-dev - install the libpng library

e) cd /home/pi/RetroliteOSD sudo make - compile the Retro Lite CM4 C script

f) Navigate to the /etc directory and add the below lines to allow the code to run in perpetuity 

cd /home/pi/RetroliteOSD

sudo ./retrolite &

sudo ./hdmi_script.sh & 

g) sudo reboot - Reboot to enable all changes 

You should now have all the required software installed for the Retro Lite CM4 to display battery SoC, charging status, WiFi status, bluetooth status and more. The Retro Lite CM4 automatically safely shuts itself down when the voltage of the batteries depletes to 3.1V. The Retro Lite CM4 must reboot to output video to an external monitor via the mini HDMI type-A port. 

-------------------------------------------------------------------------------------------------------

### CAD
The CAD files for the Retro Lite CM4 are not available or open source at this stage. Please do not ask myself or Dmcke5 to release them yet, as we will not do so. There are plans to release the files at a later date. 

-------------------------------------------------------------------------------------------------------

### PCB
The Retro Lite CM4 requires 4 custom designed PCBs to be installed, not including the CM4 carrier board. These include: 

a) Retro Lite CM4 AIO carrier board 

b) Retro Lite Controller PCB (Right)

c) Retro Lite Controller PCB (Left)

d) Retro Lite Button Daughterboard

-------------------------------------------------------------------------------------------------------

### BOM (Bill of Materials) 
Link to the BoM will be added at a later stage. 
Total BoM to build the Retro Lite CM4 exceeds $250 USD for a single unit. 

-------------------------------------------------------------------------------------------------------

### Contributors 
I'd like to thank these people in no particular order for the awesome work you have done to help me complete this project. 

* Dmcke5 for designing the case assets, including machining our prototypes, code and much more. Wouldn't have been possible to complete this without you 
* Gmanmodz for always being an inspiration for me to get into building handhelds and the modding scene 
* YveltalGriffin/Mackie, for suggesting various hardware changes and reviews to PCB/schematics. His help was invaluable in learning new things about PCB design
* Juckett D, for creating the Aegis CM4 and offering tips and tricks along the way. Check out his work here: https://github.com/juckettd
