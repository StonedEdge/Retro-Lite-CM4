#!/bin/bash
### BEGIN INIT INFO
# Provides:          startup.sh
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start daemon at boot time
# Description:       Enable service provided by daemon.
### END INIT INFO

#. /lib/lsb/init-functions

# HDMI Switcher
# Purpose: To switch between the internal HDMI0 driven screen and the external HDMI1 monitor. 
# Created by: Benjamin Todd 
# Date: 2021/12/17

# Variables 
REBOOT_REQUIRED=false
CONFIG=/boot/config.txt
LOADMOD=/etc/modules

screenCheck(){
	# Detects whether external HDMI1 HPD is high and external video should be output. Then, setup config.txt accordingly
	DISPLAYS=$(tvservice -v 7 -s)
	if [[ "${DISPLAYS}" == *"0xa"* ]]; then
		if [ -e $CONFIG ] && grep -q -E "^hdmi_drive:0=1$" $CONFIG; then
			# Turn on HDMI 1920 x 1080p video
			sudo sed -i "s|^hdmi_group:0=2$|hdmi_group:1=2|" $CONFIG
			sudo sed -i "s|^hdmi_mode:0=87$|hdmi_mode:1=82|" $CONFIG
            sudo sed -i "s|^hdmi_cvt:0=800 480 60 6 0 0 0$|#hdmi_cvt:0=800 480 60 6 0 0 0|" $CONFIG
            sudo sed -i "s|^hdmi_force_hotplug:0=1$|#hdmi_force_hotplug:0=1|" $CONFIG
			sudo sed -i "s|^hdmi_drive:0=1$|hdmi_drive:1=2|" $CONFIG
            # Turn off i2s audio 
            sudo sed -i "s|^dtparam=i2s=on$|#dtparam=i2s=on|" $CONFIG
			sudo sed -i "s|^dtoverlay=i2s-mmap$|#dtoverlay=i2s-mmap|" $CONFIG
			sudo sed -i "s|^#dtparam=audio=on$|dtparam=audio=on|" $CONFIG
            # Turn off backlight
            sudo sed -i "s|^gpio=12=pd,dl,op$|gpio=12=pu,dh,op|" $CONFIG

			sudo cp ~/asound.conf.hdmi /etc/asound.conf
			if [ -e $LOADMOD ] && grep -q "^#snd-bcm2835" $LOADMOD; then
				#Turn on HDMI audio 
				sudo sed -i "s|^#snd-bcm2835|snd-bcm2835|" $LOADMOD
			fi
			REBOOT_REQUIRED=true
		fi
	else 
        if [[ "${DISPLAYS}" == *"0x9"* || *"Invalid"* ]]; then
			if [ -e $CONFIG ] && grep -q -E "^hdmi_drive:1=2$" $CONFIG; then
				# Turn on HDMI 800x480p video
				sudo sed -i "s|^hdmi_group:1=2$|hdmi_group:0=2|" $CONFIG
				sudo sed -i "s|^hdmi_mode:1=82$|hdmi_mode:0=87|" $CONFIG
				sudo sed -i "s|^#hdmi_cvt:0=800 480 60 6 0 0 0$|hdmi_cvt:0=800 480 60 6 0 0 0|" $CONFIG
				sudo sed -i "s|^#hdmi_force_hotplug:0=1$|hdmi_force_hotplug:0=1|" $CONFIG
				sudo sed -i "s|^hdmi_drive:1=2$|hdmi_drive:0=1|" $CONFIG
				# Turn on i2s audio                    
				sudo sed -i "s|^#dtoverlay=i2s-mmap$|dtoverlay=i2s-mmap|" $CONFIG
				sudo sed -i "s|^#dtparam=i2s=on$|dtparam=i2s=on|" $CONFIG
				sudo sed -i "s|^dtparam=audio=on$|#dtparam=audio=on|" $CONFIG
				# Turn on backlight 
				sudo sed -i "s|^gpio=12=pu,dh,op$|gpio=12=pd,dl,op|" $CONFIG

				sudo cp ~/asound.conf.lcd /etc/asound.conf
				if [ -e $LOADMOD ] && grep -q "^snd-bcm2835" $LOADMOD; then
						sudo sed -i "s|^snd-bcm2835|#snd-bcm2835|" $LOADMOD
				fi
				REBOOT_REQUIRED=true
			fi
		fi
	fi
}

start(){
	screenCheck
	if $REBOOT_REQUIRED; then
		reboot
	fi
}

start

exit 0