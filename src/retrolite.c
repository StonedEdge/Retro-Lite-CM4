//Compile with: gcc retrolite.c -o retrolite -lwiringPi
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809 L

//#include <iostream>
#include <inttypes.h>

#include <assert.h>

#include <stdbool.h>

#include <stdio.h>

#include <limits.h>

#include <stdlib.h>

#include <unistd.h>

#include <time.h>

#include <strings.h>

#include "font.h"

#include "imageGraphics.h"

#include "imageLayer.h"

#include "loadpng.h"

#include <unistd.h>

#include <math.h>

#include <SDL.h>

#include <fcntl.h>

#include "bcm_host.h"

#include <wiringPiI2C.h> //sudo apt-get install wiringpi

#include <wiringPi.h>

#include <wiringSerial.h>

#include <softPwm.h>

#include "retrolite.h"

#include "display.h"

#include "max17055.h"

int main() {
  //Local Variables
  int time;

  //Display Setup
  displaySetup();

  //GPIO Setup
  wiringPiSetup();

  pinMode(volUp_button, INPUT);
  pullUpDnControl(volUp_button, PUD_UP);
  pinMode(volDn_button, INPUT);
  pullUpDnControl(volDn_button, PUD_UP);
  pinMode(hpd_detect, INPUT);
  pullUpDnControl(hpd_detect, PUD_UP);
  pinMode(pwr_button, INPUT);
  pinMode(charge_detect, INPUT);
  pinMode(pwm_Brightness, PWM_OUTPUT);
  pinMode(fan_pwm, PWM_OUTPUT);
  pinMode(low_voltage_shutdown, OUTPUT);
  pwmSetMode(PWM_MODE_MS);
  pwmSetRange(101);
  pwmSetClock(10);
  pwmWrite(pwm_Brightness, brightness); //Set brightness to full
  pwmWrite(fan_pwm, fanSpeed); //Set fan speed to default (off)
  digitalWrite(low_voltage_shutdown, LOW);

  //Load Config Values
  loadValues();

  //Max17055 Fuel Gauge Setup
  I2CDevice = wiringPiI2CSetup(I2CAddress);
  max17055_Status = wiringPiI2CReadReg16(I2CDevice, 0x00);
  if (max17055_Status == -1) {
    printf("Max17055 Failed to Connect, Battery information unavailable.\n");
  } else {
    printf("Max17055 Connected.\n");
    max17055Init();
  }

  // Serial Port
  if ((controllerSerial = serialOpen("/dev/ttyACM0", 19200)) < 0) {
    printf("Failed to open Serial Port /dev/ttyACM0\n");
  } else {
    serialFlush(controllerSerial); // Flush all missed inputs
  }

  while (run) {

    time = millis();

    //Battery
    if (charging == false) {
      updateBatteryImage();
    } else {
      batteryChargingDisplay();
    }
    if (batteryLevelChanged) {
      if (batteryIconLayerEN) {
        destroyImageLayer( & batteryIconLayer);
      }
      displayImage(level_Icon, displayX - statusIconSize - 1, 1, & batteryIconLayer, 1);
      batteryIconLayerEN = true;
      batteryLevelChanged = false;
    }
    updateBatteryText();

    //Calibration Overlay
    if (calibrationStep != 0) {
      if (calibrationStep == 1) {
        strncpy(overlayImage, "./overlays/calibration1.png", 60);
      }
      if (calibrationStep == 2) {
        strncpy(overlayImage, "./overlays/calibration2.png", 60);
      }
      if (calibrationStep == 3) {
        strncpy(overlayImage, "./overlays/calibration3.png", 60);
      }
      if (calibrationStepChanged) {
        if (menuLayerEN) {
          destroyImageLayer( & menuLayer);
        }
        menuLayerEN = true;
        displayImage(overlayImage, (displayX / 2) - (overlayImageX / 2), (displayY / 2) - (overlayImageY / 2), & menuLayer, 1);
        calibrationStepChanged = false;
      }
    }

    if (calibrationStep == 3 && calibrationTimerStarted == false) {
      endCalibrationTimer = time;
      calibrationTimerStarted = true;
    }
    if (endCalibrationTimer + 3000 < time && calibrationTimerStarted == true) {
      calibrationStep = 0;
      if (menuLayerEN) {
        destroyImageLayer( & menuLayer);
        menuLayerEN = false;
      }
      calibrationTimerStarted = false;
      //menuEnabled = true;
      menuActive = 0;
      menuUpdated = true;
    }

    //Volume/brightness bar display
    if (volBrightPressed) {
      if (strcmp(last_bar_Icon, bar_Icon) != 0) {
        if (volBrightLayerEN) {
          destroyImageLayer( & volBrightLayer);
        }
        volBrightLayerEN = true;
        displayImage(bar_Icon, 1, 1, & volBrightLayer, 1);
        strncpy(last_bar_Icon, bar_Icon, 60);
      } else {
        if (!volBrightLayerEN) {
          volBrightLayerEN = true;
          displayImage(bar_Icon, 1, 1, & volBrightLayer, 1);
          strncpy(last_bar_Icon, bar_Icon, 60);
        }
      }
      volBrightPressed = false;
    }
    if (iconTimer + 3000 < time) {
      if (volBrightLayerEN) {
        destroyImageLayer( & volBrightLayer);
        volBrightLayerEN = false;
      }
      iconTimer = 0;
    }

    //Low Battery Shut Down
    if (max17055_Status != -1) {
      if (!shutdownTimerStarted) {
        if (getSOC() == 0 && !charging) {
          strncpy(overlayImage, "./overlays/lowbatterywarning.png", 60);
          if (kbdLayerEN) {
            destroyImageLayer( & kbdLayer);
          }
          kbdLayerEN = true;
          displayImage(overlayImage, (displayX / 2) - (overlayImageX / 2), (displayY / 2) - (overlayImageY / 2), & kbdLayer, 1);
          calibrationStepChanged = false;
          shutdownTimer = time;
          shutdownTimerStarted = true;
        }
      } else {
        if (!charging) {
          if (shutdownTimer + 30000 < time) {
            printf("GPIO %d Set high", low_voltage_shutdown);
            digitalWrite(low_voltage_shutdown, HIGH); //Tells the Attiny to initiate shutdown.
          }
        } else {
          if (kbdLayerEN) {
            destroyImageLayer( & kbdLayer);
            shutdownTimerStarted = false;
            kbdLayerEN = false;
          }
        }
      }
    }

    //GPIO for Volume and Power
    if (GPIOTimer + 150 < time) {
      checkGPIO();
      GPIOTimer = time;
    }

    //Controller Serial Interface
    if (controllerSerial != -1) {
      controllerInterface();
    }

    //Set screen brightness
    pwmWrite(pwm_Brightness, map(brightness, 1, 100, 100, 1));

    //Fan Control
    fanControl();

    //Menu
    if (menuEnabled) {
      if (menuUpdated) {
        if (menuDisplayed != menuActive) {
          if (menuLayerEN) {
            destroyImageLayer( & menuLayer);
            menuLayerEN = false;
          }
          if (menuTextLayerEN) {
            destroyImageLayer( & menuTextLayer);
            menuTextLayerEN = false;
          }
          if (menuPointerLayerEN) {
            destroyImageLayer( & menuPointerLayer);
            menuPointerLayerEN = false;
          }
        } else {
          if (menuPointerLayerEN) {
            destroyImageLayer( & menuPointerLayer);
            menuPointerLayerEN = false;
          }
        }
        if (menuActive == 0) {
          mainMenu();
        } else if (menuActive == 1) {
          batteryMenu();
        } else if (menuActive == 2) {
          menuDisplayed = 2;
        } else if (menuActive == 3) {
          osKeyboard = true;
        } else if (menuActive == 4) {
          settingsMenu();
        }
        menuUpdated = false;
      }
    } else {
      if (menuLayerEN) {
        destroyImageLayer( & menuLayer);
        menuLayerEN = false;
      }
      if (menuTextLayerEN) {
        destroyImageLayer( & menuTextLayer);
        menuTextLayerEN = false;
      }
      if (menuPointerLayerEN) {
        destroyImageLayer( & menuPointerLayer);
        menuPointerLayerEN = false;
      }
    }

    //On Screen Keyboard
    if (osKeyboard) {
      //Display Keyboard
      if (keyCase == 1) {
        strncpy(overlayImage, "./keyboard/upperCase.png", 60);
      } else {
        strncpy(overlayImage, "./keyboard/lowerCase.png", 60);
      }
      if (lastKeyCase != keyCase || !kbdLayerEN) {
        if (kbdLayerEN) {
          destroyImageLayer( & kbdLayer);
        }
        kbdLayerEN = true;
        lastKeyCase = keyCase;
        displayImage(overlayImage, (displayX / 2) - (overlayImageX / 2), (displayY / 2) - (overlayImageY / 2), & kbdLayer, 1);
      }
      if (keyboardChanged) {
        //Update highlighted key
        char tempSTR[60];
        sprintf(tempSTR, "./keyboard/%d%d.png", keyboardRow, keyboardCollumn);
        strncpy(keyboardHighlight, tempSTR, 60);

        if (kbdHighlightLayerEN) {
          destroyImageLayer( & kbdHighlightLayer);
        }
        kbdHighlightLayerEN = true;
        displayImage(keyboardHighlight, (displayX / 2) - (overlayImageX / 2), (displayY / 2) - (overlayImageY / 2), & kbdHighlightLayer, 1);
        keyboardChanged = false;
      }
    } else {
      if (!shutdownTimerStarted) {
        if (kbdLayerEN) {
          destroyImageLayer( & kbdLayer);
          kbdLayerEN = false;
        }
        if (kbdHighlightLayerEN) {
          destroyImageLayer( & kbdHighlightLayer);
          kbdHighlightLayerEN = false;
        }
      }
    }

    //Print Ram Usage
    //char buf[BUFSIZ];
    //snprintf(buf, sizeof(buf), "free -h");
    //system(buf);

  }

  result = vc_dispmanx_display_close(display);
  assert(result == 0);

  //---------------------------------------------------------------------
  return 0;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void checkResolution() {
  FILE * resolution;
  char path[35];
  int x = 0;
  int y = 0;
  char * token;

  resolution = popen("fbset -s", "r");

  fgets(path, sizeof(path), resolution); //Read first line
  fgets(path, sizeof(path), resolution); //Read second line
  fgets(path, sizeof(path), resolution); //Read third line

  token = strtok(path, " ");
  while (token != NULL) {
    if (atoi(token) != 0) {
      if (x == 0) {
        x = atoi(token);
      } else {
        if (y == 0) {
          y = atoi(token);
        }
      }

    }
    token = strtok(NULL, " ");
  }
  printf("X Resolution = %d\n", x);
  printf("Y Resolution = %d\n", y);

  displayX = x;
  displayY = y;

  pclose(resolution);
}

void controllerInterface() {
  int value;

  while (serialDataAvail(controllerSerial)) {
    value = serialGetchar(controllerSerial);
    if (value == brightnessUp) {
      if (brightness <= 90) {
        brightness += 10;
        saveValues();
      }

      iconTimer = millis();
      volBrightPressed = true;
    }
    if (value == brightnessDn) {
      if (brightness >= 10) {
        brightness -= 10;
        saveValues();
      }
      iconTimer = millis();
      volBrightPressed = true;
    }
    if (value == menuOpen) {
      menuEnabled = true;
      menuUpdated = true;
      menuSelected = 1;
      printf("Menu Enabled\n");
    }
    if (value == menuClose) {
      menuEnabled = false;
      osKeyboard = false;
      menuActive = 0;
      printf("Menu Disabled\n");
    }
    if (!osKeyboard && menuActive != 1) {
      if (value == osKeyboardUp) {
        //Menu Selection Up
        if (menuSelected > 1 && menuSelected != 7) {
          menuSelected -= 1;
          menuUpdated = true;
        } else if (menuSelected == 7) {
          menuSelected = 5;
          menuUpdated = true;
        }
      }
      if (value == osKeyboardDn) {
        //Menu Selection Down
        if (menuActive != 4) {
          if (menuSelected < 4) {
            menuSelected += 1;
            menuUpdated = true;
          }
        } else {
          if (menuSelected < 5) {
            menuSelected += 1;
            menuUpdated = true;
          } else if (menuSelected == 5) {
            menuSelected = 7;
            menuUpdated = true;
          }
        }
      }
    }
    if (value == osKeyboardLeft) {
      if (menuActive == 4) {
        settingValueChanged(-1);
      }
    }
    if (value == osKeyboardRight) {
      if (menuActive == 4) {
        settingValueChanged(1);
      }
    }
    if (value == osKeyboardSelect) {
      if (calibrationStep == 0) {
        menuOptionSelected();
      }
    }
    /*if(value == calibrationStepOne){
        printf("Calibration Step 1\n");
        calibrationStep = 1;
        calibrationStepChanged = true;
    }*/
    if (value == calibrationStepTwo) {
      printf("Calibration Step 2\n");
      calibrationStep = 2;
      calibrationStepChanged = true;
    }
    if (value == calibrationComplete) {
      printf("Calibration Step 3\n");
      calibrationStep = 3;
      calibrationStepChanged = true;
    }
    //On Screen Keyboard
    if (value == osKeyboardEnabled) {
      osKeyboard = true;
      keyboardChanged = true;
    }
    if (value == osKeyboardDisabled) {
      osKeyboard = false;
      if (kbdHighlightLayerEN) {
        destroyImageLayer( & kbdHighlightLayer);
        kbdHighlightLayerEN = false;
      }
      if (kbdLayerEN) {
        destroyImageLayer( & kbdLayer);
        kbdLayerEN = false;
      }
    }
    if (osKeyboard) {
      if (value == osKeyboardLeft) {
        if (keyboardCollumn > 0) {
          keyboardCollumn--;
        } else {
          if (keyboardRow != 0) {
            keyboardCollumn = 9;
          } else {
            keyboardCollumn = 6;
          }
        }
        keyboardChanged = true;
      }
      if (value == osKeyboardRight) {
        if (keyboardRow != 0) {
          if (keyboardCollumn < 9) {
            keyboardCollumn++;
          } else {
            keyboardCollumn = 0;
          }
        } else {
          if (keyboardCollumn < 6) {
            keyboardCollumn++;
          } else {
            keyboardCollumn = 0;
          }
        }
        keyboardChanged = true;
      }
      if (value == osKeyboardUp) {
        if (keyboardRow == 0) {
          if (keyboardCollumn == 3) {
            keyboardCollumn = 4;
          } else if (keyboardCollumn == 4) {
            keyboardCollumn = 6;
          } else if (keyboardCollumn == 5) {
            keyboardCollumn = 7;
          } else if (keyboardCollumn > 5) {
            keyboardCollumn = 9;
          }
        }
        if (keyboardRow < 4) {
          keyboardRow++;
        } else {
          keyboardRow = 0;
        }
        if (keyboardRow == 0) {
          if (keyboardCollumn == 6) {
            keyboardCollumn = 4;
          } else if (keyboardCollumn == 7) {
            keyboardCollumn = 5;
          } else if (keyboardCollumn > 7) {
            keyboardCollumn = 6;
          } else if (keyboardCollumn == 3 || keyboardCollumn == 4 || keyboardCollumn == 5) {
            keyboardCollumn = 3;
          }
        }
        keyboardChanged = true;
      }
      if (value == osKeyboardDn) {
        if (keyboardRow == 0) {
          if (keyboardCollumn == 4) {
            keyboardCollumn = 6;
          } else if (keyboardCollumn == 5) {
            keyboardCollumn = 7;
          } else if (keyboardCollumn == 6) {
            keyboardCollumn = 9;
          } else if (keyboardCollumn == 3) {
            keyboardCollumn = 4;
          }
        }
        if (keyboardRow > 0) {
          keyboardRow--;
        } else {
          keyboardRow = 4;
        }
        if (keyboardRow == 0) {
          if (keyboardCollumn == 6) {
            keyboardCollumn = 4;
          } else if (keyboardCollumn == 7) {
            keyboardCollumn = 5;
          } else if (keyboardCollumn > 7) {
            keyboardCollumn = 6;
          } else if (keyboardCollumn == 3 || keyboardCollumn == 4 || keyboardCollumn == 5) {
            keyboardCollumn = 3;
          }
        }
        keyboardChanged = true;
      }
      if (value == osKeyboardSelect) {
        if (keyboardRow == 1 && keyboardCollumn == 0) { //Caps Lock
          if (keyCase == 0) {
            keyCase = 1;
          } else {
            keyCase = 0;
          }
        } else {
          if (keyCase == 0) { //Lower Case
            serialPrintf(controllerSerial, "%c", osKeyboardLowercase[keyboardRow][keyboardCollumn]);
          } else { //Upper Case
            serialPrintf(controllerSerial, "%c", osKeyboardUppercase[keyboardRow][keyboardCollumn]);
          }
        }
        keyboardChanged = true;
      }
    }

    if (volBrightPressed) {
      if (brightness == 0) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_0.png", 60);
      } else if (brightness == 10) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_10.png", 60);
      } else if (brightness == 20) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_20.png", 60);
      } else if (brightness == 30) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_30.png", 60);
      } else if (brightness == 40) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_40.png", 60);
      } else if (brightness == 50) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_50.png", 60);
      } else if (brightness == 60) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_60.png", 60);
      } else if (brightness == 70) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_70.png", 60);
      } else if (brightness == 80) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_80.png", 60);
      } else if (brightness == 90) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_90.png", 60);
      } else if (brightness == 100) {
        strncpy(bar_Icon, "./brightnessIcons/brightness_100.png", 60);
      }
    }
  }
}

void loadValues() {
  FILE * fp;
  char buff[255];
  int parsingFailed = false;
  int temp;

  fp = fopen("config", "r");
  if (fp == NULL) {
    printf("Failed to load config file, using defaults\n");
    return;
  }
  fgets(buff, 255, (FILE * ) fp);
  sscanf(buff, "%d %f %d %d %d %d", & batteryCapacity, & lowVoltageThreshold, & cpuFanThreshold, & brightness, & volume, & darkMode);
  fclose(fp);

  //Input Parsing
  if (brightness > 100 || brightness < 1) {
    brightness = 1;
    parsingFailed = true;
  } else {
    temp = brightness / 10;
    brightness = temp * 10;
    parsingFailed = true;
  }
  if (volume > 100 || volume < 0) {
    volume = 100;
    parsingFailed = true;
  } else {
    temp = volume / 10;
    volume = temp * 10;
    parsingFailed = true;
  }
  if (lowVoltageThreshold < 2.8) {
    lowVoltageThreshold = 3.0;
    parsingFailed = true;
  }
  if (parsingFailed) {
    saveValues();
  }
  float test = getVEmpty();
  printf("batteryCapacity: %d\n", batteryCapacity);
  printf("lowVoltageThreshold: %f\n", lowVoltageThreshold);
  printf("cpuFanThreshold: %d\n", cpuFanThreshold);
  printf("brightness: %d\n", brightness);
  printf("volume: %d\n", volume);
  printf("DarkMode: %d\n", darkMode);
  printf("VEmpty: %.2f\n", test);
}

void saveValues() {
  FILE * fptr;
  fptr = fopen("config", "w");
  if (fptr == NULL) {
    printf("Error Logging Values! \n");
    return;
  } else {
    fprintf(fptr, "%d %.1f %d %d %d %d", batteryCapacity, lowVoltageThreshold, cpuFanThreshold, brightness, volume, darkMode);
  }
  fclose(fptr);
}

void updateBatteryImage() {
  char currentIcon[60];
  strncpy(currentIcon, level_Icon, 60);
  if (max17055_Status != -1) { //Check if Max17055 is connected
    currentSOC = getSOC();
    if (currentSOC > 90) {
      strncpy(level_Icon, "./batteryIcons/battery_full_white_18dp.png", 60);
    } else if (currentSOC > 80) {
      strncpy(level_Icon, "./batteryIcons/battery_90_white_18dp.png", 60);
    } else if (currentSOC > 70) {
      strncpy(level_Icon, "./batteryIcons/battery_80_white_18dp.png", 60);
    } else if (currentSOC > 60) {
      strncpy(level_Icon, "./batteryIcons/battery_70_white_18dp.png", 60);
    } else if (currentSOC > 50) {
      strncpy(level_Icon, "./batteryIcons/battery_60_white_18dp.png", 60);
    } else if (currentSOC > 40) {
      strncpy(level_Icon, "./batteryIcons/battery_50_white_18dp.png", 60);
    } else if (currentSOC > 30) {
      strncpy(level_Icon, "./batteryIcons/battery_40_white_18dp.png", 60);
    } else if (currentSOC > 20) {
      strncpy(level_Icon, "./batteryIcons/battery_30_white_18dp.png", 60);
    } else if (currentSOC > 10) {
      strncpy(level_Icon, "./batteryIcons/battery_20_white_18dp.png", 60);
    } else if (currentSOC > 0) {
      strncpy(level_Icon, "./batteryIcons/battery_10_white_18dp.png", 60);
    } else {
      strncpy(level_Icon, "./batteryIcons/battery_0_white_18dp.png", 60);
    }
  } else {
    strncpy(level_Icon, "./batteryIcons/battery_error_18d.png", 60);
  }
  if (strcmp(currentIcon, level_Icon) != 0) {
    //printf("Level Icon Changed\n");
    batteryLevelChanged = true;
  }
}

void batteryChargingDisplay() {
  unsigned int time = millis();
  if (chargeIconTimer + 1000 < time) {
    chargeIconTimer = time;
    batteryLevelChanged = true;
    if (chargeStep < 4) {
      chargeStep += 1;
    } else {
      chargeStep = 0;
    }
  }
  if (chargeStep == 0) {
    strncpy(level_Icon, "./batteryIcons/battery_charge_0_18dp.png", 60);
  } else if (chargeStep == 1) {
    strncpy(level_Icon, "./batteryIcons/battery_charge_1_18dp.png", 60);
  } else if (chargeStep == 2) {
    strncpy(level_Icon, "./batteryIcons/battery_charge_2_18dp.png", 60);
  } else if (chargeStep == 3) {
    strncpy(level_Icon, "./batteryIcons/battery_charge_3_18dp.png", 60);
  } else if (chargeStep == 4) {
    strncpy(level_Icon, "./batteryIcons/battery_charge_4_18dp.png", 60);
  }
}

void checkGPIO() {
  char buf[BUFSIZ];
  if (hpd_change == true) {
    if (digitalRead(hpd_detect) == 1) {
      snprintf(buf, sizeof(buf), "amixer -M set Speaker %d%%", 90); //Playback
      //printf("%s\n", buf); //Debug
      system(buf);
      hpd_change = false;
    }
  }

  if (hpd_change == false) {
    if (digitalRead(hpd_detect) == 0) {
      snprintf(buf, sizeof(buf), "amixer -M set Speaker %d%%", 0);
      //printf("%s\n", buf); //Debug
      system(buf);
      hpd_change = true;
    }
  }

  //bool volumeChange = false;
  if (digitalRead(volUp_button) == 0) {
    if (volume <= 90) {
      volume += 10;
      snprintf(buf, sizeof(buf), "amixer -M set Playback %d%%", volume); //Playback
      //printf("%s\n", buf); //Debug
      system(buf);
      //volumeChange = true;
    }
    iconTimer = millis();
    volBrightPressed = true;
  }
  if (digitalRead(volDn_button) == 0) {
    if (volume >= 10) {
      volume -= 10;
      snprintf(buf, sizeof(buf), "amixer -M set Playback %d%%", volume); //Playback
      //printf("%s\n", buf); //Debug
      system(buf);
      //volumeChange = true;
    }
    iconTimer = millis();
    volBrightPressed = true;
  }
  if (digitalRead(pwr_button) == 1) {
    printf("Shutdown\n");
    system("sudo halt"); //Shut Down
  }
  if (volBrightPressed) {
    saveValues();

    if (volume == 0) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_0.png", 60);
    } else if (volume == 10) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_10.png", 60);
    } else if (volume == 20) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_20.png", 60);
    } else if (volume == 30) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_30.png", 60);
    } else if (volume == 40) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_40.png", 60);
    } else if (volume == 50) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_50.png", 60);
    } else if (volume == 60) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_60.png", 60);
    } else if (volume == 70) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_70.png", 60);
    } else if (volume == 80) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_80.png", 60);
    } else if (volume == 90) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_90.png", 60);
    } else if (volume == 100) {
      strncpy(bar_Icon, "./volumeIcons/volume_bar_100.png", 60);
    }
  }
  if (digitalRead(charge_detect) == 0) { //If charging
    charging = true;
  } else {
    charging = false;
  }
}

const int temp_fan_min = 50;
const int temp_fan_full = 75;
const int pwm_fan_min = 30;
const int pwm_fan_max = 101;
const int hysteresis = 5;
const float Kp = 1;
const float Ki = 0.03;
const float Kd = 0.1;

static float lastError = 0;
static float integral = 0;

void fanControl() {
    float millideg;
    FILE *thermal;

    thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    fscanf(thermal, "%f", &millideg);
    fclose(thermal);
    float systemp = (int)(millideg / 10.0f) / 100.0f;

    float error = systemp - cpuFanThreshold;
    integral += error;
    float derivative = error - lastError;
    lastError = error;                                                                                     
    float pid = Kp * error + Ki * integral + Kd * derivative;

    if (systemp <= cpuFanThreshold + hysteresis) {
        fanSpeed = 0;
    }
    else if (temp_fan_min <= systemp - hysteresis && systemp - hysteresis <= temp_fan_full) {
        fanSpeed = (pwm_fan_min*(temp_fan_full-systemp+hysteresis) + pwm_fan_max*(systemp-hysteresis-temp_fan_min))/((temp_fan_full-temp_fan_min));
    }
    else if (systemp - hysteresis > temp_fan_full) {
        fanSpeed = pwm_fan_max;
    }

    fanSpeed += (int)pid;
    fanSpeed = fmaxf(fminf(fanSpeed, pwm_fan_max), 0);

    pwmWrite(fan_pwm, fanSpeed);
}
