![Image of Retro Lite CM4](https://i.imgur.com/h1mcu5v.jpg)
# Retro Lite CM4: Raspberry Pi Handheld

## Build Guide  

<p align="center">
   <img src="https://i.imgur.com/qu4UXJB.png"/>
</p>

A new build guide is coming soon! Stay tuned and watch this space in early 2023 for a full procurement/build guide for the Retro Lite CM4. 
The guide will cover:

1) Ordering/procuring parts 
2) Best suppliers to order from 
3) Total P&L/expected costs from the project 
4) Assembly guide
5) Firmware flashing guide (OSD, Controller and Safe Shutdown Scripts)

## Description
 
The Retro Lite CM4 is a joint project between Dmcke5 and StonedEdge. The console is powered by a 4000mAh Li-Po and is designed to run with various software images, including but not limited to RetroPie, Lakka or Batocera, directly from an SD card or via eMMC storage. The project housing was inspired by a Nintendo Switch Lite, but is not a 100% match to the original. The CM4 cannot currently emulate most 32/64-bit consoles released after the PSP or Dreamcast. This means GameCube, Wii and 3DS are currently unplayable until the Foundation releases a newer version of the Compute Module line. wiringPi is now a deprecated GPIO library however does work with arm64 and aarch64, so you can install 64-bit RetroPie if you'd like. 

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
- External dock passthrough PCB with 4 port USB-hub - have now started uploading dock assets

Click the link below to view a more detailed buildlog of the Retro Lite CM4: 
https://bitbuilt.net/forums/index.php?threads/retro-lite-cm4-a-stonededge-and-dmcke5-collaboration.3934/

## Requirements to Build the Retro Lite CM4

### Code 

Code has now been split into various header files and the retrolite.c code has now been made easier to read and condensed for the end-user. Code required to complete the handheld includes an ATtiny84 monitor script, which can be flashed via the 1.27mm pitch 6 pin header on the main Retro Lite PCB with an Arduino or similar programmer. Code for the i2s audio amplifier, fuel gauge and OSD functionality can be compiled directly on the CM4 module from the command line. Current code is unfinished as of January 2022, however we are pushing bug fixes when we find them - expect a fully functioning RetroPie image to be shared in the future from this repository (actually, just found out this is not allowed due to the licensing of certain emulators, so will create install scripts at a later date). 

To flash the software to the Retro Lite CM4, you can either download the latest RetroPie Retro Lite custom image, or follow the below steps to get up and running. Plan is to eventually put this into a single script. I have not currently tested our firmware on images other than RetroPie, however I assume other images should also work fine. 

a) Download the latest version of RetroPie for the Raspberry Pi 4/400 here (also compatible with CM4): 
https://retropie.org.uk/download/ 

b) Run the WM8960 I2S audio amplifier script and install audio drivers:
https://github.com/StonedEdge/seeed-voicecard

c) Install the latest version of WiringPi, compatible with both 64/32-bit OS here: 
https://github.com/StonedEdge/WiringPi

d) Add config.txt to the /boot directory (overwrite)

e) cd /home/pi/RetroliteOSD sudo make - compile the Retro Lite CM4 C code and generate binary

f) Navigate to the /etc directory and add the below lines to the rc.local file, to allow the code to run every time at startup 

cd /home/pi/RetroliteOSD
sudo ./retrolite &
sudo ./hdmi_script.sh & 

g) sudo reboot - Reboot to enable all changes 

You should now have all the required software installed for the Retro Lite CM4 to display battery SoC, charging status, WiFi status, bluetooth status and more. The Retro Lite CM4 automatically safely shuts itself down when the voltage of the batteries depletes to 3.1V - adjustable directly from the settings calibration menu. The Retro Lite CM4 must reboot to output video to an external monitor via the mini HDMI type-A port, which is handled by the hdmi_switch.sh script. 

-------------------------------------------------------------------------------------------------------

### CAD
The CAD files for the Retro Lite CM4 are now available and open source! Please do give credit where due as a lot of work has gone into these designs.
You can check out my project partner's work here: https://github.com/dmcke5

-------------------------------------------------------------------------------------------------------

### PCB
The Retro Lite CM4 requires 4 custom designed PCBs to be installed, not including the CM4 carrier board. These include: 

a) Retro Lite CM4 AIO carrier board 

b) Retro Lite Controller PCB (Right)

c) Retro Lite Controller PCB (Left)

d) Retro Lite Button Daughterboard

Gerbers to be ordered from your board house of choice have now been uploaded! Recommended to use JLCPCB for fast turnaround using their 4-layer service.

-------------------------------------------------------------------------------------------------------

### BOM (Bill of Materials) 
Preliminary BOMs for all 4 PCBs have been added including cost. You will have to source your own parts however most can be found on Digikey/Mouser.

-------------------------------------------------------------------------------------------------------
### New Revision Features
Features that I would like to add to the handheld (welcome for others to assist) include the following:

a) HDMI over USB-C, using the TPS65987D and the TUSB546A-DCI high speed mux. 

b) OTG integration (power perihperals through the onboard USB-C port). 

c) DSI 5.5" display (struggle to know of a way to get this working currently on the Pi). 

Feel free to make a pull request with any changes you would like seen to the handheld. 

-------------------------------------------------------------------------------------------------------
### Optional Docking Station 
![Image of Retro Lite CM4](https://i.imgur.com/LP2ecQt.png)

I am currently working on a small docking station for this project that uses an OLED display to display various cool things, as well as charge the console + passthrough HDMI video. It's designed to work with RetroPie and use a RP2040 microcontroller, written in C (Pico) and Python (CM4). I will upload the code and further instructions on how to get it working over USB with the CM4 at a later date. The aim is to keep this cross compatible with several different projects however the dock integrates the OLED screen into the custom dock PCB. 

-------------------------------------------------------------------------------------------------------
### Contributors 
I'd like to thank these people in no particular order for the awesome work you have done to help me complete this project. 

* Dmcke5 for designing the case assets, including machining/anodizing the prototypes, doing most of the code and much more. Wouldn't have been possible to complete this without you - a team effort
* YveltalGriffin/Mackie, for suggesting various hardware changes and reviews to PCB/schematics. His help was invaluable in learning new things about PCB design
* Juckett D, for creating the Aegis CM4 and offering tips and tricks along the way. Check out his work here: https://github.com/juckettd
* Aurelio Manarra, for always helping me out with my non EE questions and various other things ᕕ( ᐛ )ᕗ
