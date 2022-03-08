![Image of Retro Lite CM4](https://i.imgur.com/h1mcu5v.jpg)
# Retro Lite CM4: Raspberry Pi Handheld

## Description
The Retro Lite CM4 is a joint project between Dmcke5 and StonedEdge. The console is powered by a 4000mAh Li-Po and is designed to run with various software images, including but not limited to RetroPie, Lakka or Batocera, directly from an SD card or via eMMC storage. The project housing was inspired by a Nintendo Switch Lite, but is not a 100% match to the original. The CM4 cannot currently emulate 64-bit consoles released after the PSP or Dreamcast. This means GameCube, Wii and 3DS are currently unplayable until the Foundation releases a newer version of the Compute Module line. 64-bit OS can be flashed to the SD card, however performance is still awful due to GPU bottlenecks. 64-bit OS also now breaks the deprecated WiringPi library, so for now, our handheld only works on 32-bit Raspberry Pi OS, until a newer GPIO library is used. 

## Features
- Machined aluminum 6061 housing (green/purple), custom machined and anodized by Dmcke5
- Raspberry Pi 4 SoC: Broadcom BCM2711 quad-core Cortex-A72 @ 1.5GHz (ARM CPU overclockable to 2.1GHz, GPU overclockable to 850MHz)
- 2GB of LPDDR4-3200 SDRAM
- 5.0GHz IEEE 802.11b/g/n/ac wireless, Bluetooth 5.0
- Custom AIO 4-layer PCB, with CM4 mezzanines, 5v boost rated at 2.1A continuous current, 3.3v buck, USB2422 hub, BQ24292i, MAX17055 fuel gauge, ATtiny84, WM8960 amp, TFP401 encoder, SD card slot, USB-C hardware and mini HDMI type A
- 2x controller PCBs with FFC cables
- 800 x 480 (5:3 aspect ratio) IPS display
- HDMI video output (HDMI0 - internal)
- 1080p 60fps HDMI video output (HDMI1 - external) via the mini HDMI port
- Tempered glass screen protector
- Fuel gauge for accurate battery SoC monitoring - MAX17055, custom battery indicator shown in steps of 10 icons, plus %SoC value, via dispmanx APIs on a TFT HDMI driven panel
- Charging indicator
- HDMI switching scripts, to shutdown and reboot to external HDMI with an overclock applied
- ATMEGA32u4 internal USB 2.0 HID controller
- Brightness and volume HUD adjustment, in steps of 10 icons. Brightness is adjusted by holding select + up/down on the DPAD
- Custom menu, activated by pushing select + R3. The menu consists of 4 sections (1. Battery Information, 2. Controller Calibration, 3. Onscreen Keyboard, 4. Settings Menu) -   courtesy of Dmcke5's hard work and ideas :D
- USB-C PD charging capabilities up to 15v (0.5C charge rate at 2A) with an original Nintendo Switch charger
- Stereo Audio Output via i2s WM8960 Wolfson audio amplifier
- Headphone jack, with automatic switching
- 4000mAh custom lipo design, providing 4 hours of gameplay
- Dual stacked shoulder buttons (L, R, LZ, RZ)
- 2x switch analog sticks, range programmable in software
- Resin casted ABXY, DPAD, start+select, shoulder buttons with acetal spring hinge + silicone membranes
- Safe software/hardware shutdown to prevent corruption to the SD card data (low voltage shutdown included)
- USB 2.0 2-port downstream hub
- External dock passthrough PCB with 4 port USB-hub (unfinished)

Click the link below to view a more detailed buildlog of the Retro Lite CM4: 
https://bitbuilt.net/forums/index.php?threads/retro-lite-cm4-a-stonededge-and-dmcke5-collaboration.3934/

## Requirements to Build the Retro Lite CM4

### Code 
Code has now been split into various header files and the retrolite.c code has now been made easier to read and condensed for the end-user. Code required to complete the handheld includes an ATtiny84 monitor script, which can be flashed via the 1.27mm pitch 6 pin header on the main Retro Lite PCB with an Arduino or similar programmer. Code for the i2s audio amplifier, fuel gauge and OSD functionality can be compiled directly on the CM4 module from the command line. Current code is unfinished as of January 2022, however we are pushing bug fixes when we find them - expect a fully functioning RetroPie image to be shared in the future from this repository. 

To flash the software to the Retro Lite CM4, you can either download the latest RetroPie Retro Lite custom image, or follow the below steps to get up and running. Plan is to eventually put this into a single script. I have not currently tested our firmware on images other than RetroPie, however I assume other images should also work fine. 

a) Download the latest version of RetroPie for the Raspberry Pi 4/400 here (also compatible with CM4): 
https://retropie.org.uk/download/ 

b) Install the WM8960 I2S audio amplifier script, by running the script here: 
https://github.com/StonedEdge/seeed-voicecard

c) Install the latest version of WiringPi here: 
https://github.com/StonedEdge/WiringPi

d) sudo apt-get install libpng12-dev - install the libpng library

e) cd /home/pi/RetroliteOSD sudo make - compile the Retro Lite CM4 C script

f) Navigate to the /etc directory and add the below lines to allow the code to run at startup to the rc.local file

cd /home/pi/RetroliteOSD
sudo ./retrolite &
sudo ./hdmi_script.sh & 

g) sudo reboot - Reboot to enable all changes 

You should now have all the required software installed for the Retro Lite CM4 to display battery SoC, charging status, WiFi status, bluetooth status and more. The Retro Lite CM4 automatically safely shuts itself down when the voltage of the batteries depletes to 3.1V - adjustable directly from the settings calibration menu. The Retro Lite CM4 must reboot to output video to an external monitor via the mini HDMI type-A port, which is handled by the hdmi_switch.sh script. 

-------------------------------------------------------------------------------------------------------

### CAD
The CAD files for the Retro Lite CM4 are not available or open source at this stage. Please do not ask myself or Dmcke5 to release them yet, as we will not do so. There are plans to release the files at a later date. We've worked hard on the case design and don't want someone stealing our assets!

-------------------------------------------------------------------------------------------------------

### PCB
The Retro Lite CM4 requires 4 custom designed PCBs to be installed, not including the CM4 carrier board. These include: 

a) Retro Lite CM4 AIO carrier board 

b) Retro Lite Controller PCB (Right)

c) Retro Lite Controller PCB (Left)

d) Retro Lite Button Daughterboard

Gerbers to be ordered from your board house of choice will be uploaded at a later date.

-------------------------------------------------------------------------------------------------------

### BOM (Bill of Materials) 
Link to the BoM will be added at a later stage. 
Total BoM to build the Retro Lite CM4 exceeds $250 USD for a single unit, more if you machine the case in aluminum. 

-------------------------------------------------------------------------------------------------------
### New Revision Features
Features that I would like to add to the handheld (welcome for others to assist) include the following:

a) HDMI over USB-C, using the TPS65987D and the TUSB546A-DCI high speed mux. 

b) OTG integration (power perihperals through the onboard USB-C port). 

c) DSI 5.5" display (struggle to know of a way to get this working currently on the Pi). 

Feel free to make a pull request with any changes you would like seen to the handheld. 

-------------------------------------------------------------------------------------------------------


### Contributors 
I'd like to thank these people in no particular order for the awesome work you have done to help me complete this project. 

* Dmcke5 for designing the case assets, including machining/anodizing the prototypes, doing most of the code and much more. Wouldn't have been possible to complete this without you - a team effort
* YveltalGriffin/Mackie, for suggesting various hardware changes and reviews to PCB/schematics. His help was invaluable in learning new things about PCB design
* Juckett D, for creating the Aegis CM4 and offering tips and tricks along the way. Check out his work here: https://github.com/juckettd
