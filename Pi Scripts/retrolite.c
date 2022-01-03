//Compile with: gcc retrolite.c -o retrolite -lwiringPi
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

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
#include "keycodes.h"

#include "bcm_host.h"
#include <wiringPiI2C.h> //sudo apt-get install wiringpi
#include <wiringPi.h>
#include <wiringSerial.h>
#include <softPwm.h>

#define true 1
#define false 0

volatile bool run = true;

//I2C Registers
static int Status = 0x00;
static int VCell = 0x09;
//static int AvgVCell = 0x19;
static int Current = 0x0A;
//static int AvgCurrent = 0x0B;
static int RepSOC = 0x06;
static int RepCap = 0x05;
static int TimeToEmpty = 0x11;
static int DesignCap = 0x18;

//GPIO Pins
static int volUp_button = 5; //Volume Up Button BCM = 19 wPi = 24
static int volDn_button = 16; //Volume Down Button BCM = 15 wPi = 16
static int pwr_button = 6;   //Software Power Button from ATTINY BCM = 25 wPi = 6
static int charge_detect = 7; //Pin goes low when charger is connected. BCM = 4 wPi = 7
static int pwm_Brightness = 26; //BCM = 12 wPi = 26
static int fan_pwm = 23; //BCM = 13, wPi = 23
static int low_voltage_shutdown = 27; // BCM = 16, wPi = 27
static int headphone_detect = 15; //BCM = 14, wPi = 15

//MAX17055
static int I2CAddress = 0x36; //0x36 Fuel Gauge, 0x48 ADS1115;
int I2CDevice;
int batteryCapacity = 4000; //0xFA0;

//Display
int displayX = 800;
int displayY = 480;
static int statusIconSize = 18; //Size of status icon images
static int fontWidth = 8;
static int overlayImageX = 500; //Number of X pixels in overlay image
static int overlayImageY = 250; //Number of Y pixels in overlay image
int brightness = 0; //Default Full brightness
int darkMode = 0; //Default light text/battery mode
//PNGview Variables
uint32_t displayNumber;
VC_IMAGE_TYPE_T type;
DISPMANX_DISPLAY_HANDLE_T display;
DISPMANX_MODEINFO_T info;
static int layer = 30000;
int result;
DISPMANX_UPDATE_HANDLE_T update;
//Layers
IMAGE_LAYER_T infoLayer;
int infoLayerEN = false;
IMAGE_LAYER_T kbdLayer;
int kbdLayerEN = false;
IMAGE_LAYER_T kbdHighlightLayer;
int kbdHighlightLayerEN = false;
IMAGE_LAYER_T batteryIconLayer;
int batteryIconLayerEN = false;
IMAGE_LAYER_T batteryPercentLayer;
int batteryPercentLayerEN = false;
IMAGE_LAYER_T volBrightLayer;
int volBrightLayerEN = false;


//Controller Serial
int controllerSerial = -1;
int calibrationStep = 0;
int calibrationStepChanged = false;
static int brightnessUp = 0x00;
static int brightnessDn = 0x01;
static int calibrationStepOne = 0x02;
static int calibrationStepTwo = 0x03;
static int calibrationComplete = 0x04;
static int osKeyboardEnabled = 0x05;
static int osKeyboardLeft = 0x06;
static int osKeyboardRight = 0x07;
static int osKeyboardUp = 0x08;
static int osKeyboardDn = 0x09;
static int osKeyboardSelect = 0x10;
static int osKeyboardDisabled = 0x11;
int osKeyboard = false;
int keyboardChanged = false;
int keyboardDemo = false; //Change to true for a demo of the keyboard
int keyCase = 0; //0 = LOWER CASE, 1 = UPPER CASE
int lastKeyCase = 2;
int keyboardRow = 0; //0-4 : 5 Rows in total
int keyboardCollumn = 0; //0-10 : 10 Collumns in total (except first row only has 7)


//Variables
//static int i2c_ch = 1;
//static float resistSensor = 0.01;
static float capacity_multiplier_mAH = (5e-3)/0.01;
static float current_multiplier_mV = (1.5625e-3)/0.01;
static float voltage_multiplier_V = 7.8125e-5;
static float time_multiplier_Hours = 5.625/3600.0;
static float percentage_multiplier = 1.0/256.0;
int ioError = false;
float ChargeVoltage = 4.2;
int max17055_Status = 0;
float currentTime = 0;
float currentSOC = 0;
char level_Icon[60];// = "./batteryIcons/battery_full_white_18dp.png";
char overlayImage[60];// = "./overlays/calibration3.png";
char bar_Icon[60];// = "./volumeIcons/volume_bar_0.png";
char last_bar_Icon[60];
char keyboardHighlight[60];// = "./keyboard/00.png";
int controllerConnected = 0;
int volume = 100;
int chargeStep = 0; //Used for charging animation
int charging = false;
int cpuFanThreshold = 65; //Temperature fan starts running at. Default 65
int fanSpeed = 0;
float lowVoltageThreshold = 3.1; //If voltage drops below this value, engage auto shut down.
int batteryLevelChanged = false;
int volBrightPressed = false;
//char tempSTR[60];
char batteryText[5];

//Timers
unsigned int chargeIconTimer;
unsigned int endCalibrationTimer;
unsigned int iconTimer = 0;
int calibrationTimerStarted = false;
int shutdownTimerStarted = false;
unsigned int shutdownTimer;
unsigned int GPIOTimer;
unsigned int demoTimer;
unsigned int testTimer;
int testBool = true;

//Colours
static RGBA8_T clearColor = {0,0,0,0};
//static RGBA8_T backgroundColour = { 0, 0, 0, 100 };
static RGBA8_T whiteColour = { 255, 255, 255, 255 };
static RGBA8_T blackColour =  { 0, 0, 0, 255 };
//static RGBA8_T greenColour = { 0, 255, 0, 200 };
//static RGBA8_T redColour = { 255, 0, 0, 200 };


//Functions
void updateInfo(IMAGE_LAYER_T*, char[]);
void getInput();
void clearLayer(IMAGE_LAYER_T*);
void updateBattery(float, IMAGE_LAYER_T*);
char *getcwd(char *buf, size_t size);
void controllerInterface();
void loadValues();
void saveValues();
void updateBatteryImage();
void batteryChargingDisplay();
void checkGPIO();
void updateBatteryText();
void fanControl();
void checkResolution();
void displayImage(char address[60], int xPos, int yPos, IMAGE_LAYER_T* imgLayer, int layerOffset);
void displayText(char address[60], int xPos, int yPos, IMAGE_LAYER_T* imgLayer, int layerOffset);
void displaySetup();

float getSOC(){
    int SOC_raw = wiringPiI2CReadReg16(I2CDevice,RepSOC);
    return SOC_raw * percentage_multiplier;
}

int getCapacity(){
    int capacity_raw = wiringPiI2CReadReg16(I2CDevice,DesignCap);
    capacity_raw = capacity_raw * capacity_multiplier_mAH;
    return (capacity_raw);
}

int getRemainingCapacity(){
    int remainingCapacity_raw = wiringPiI2CReadReg16(I2CDevice,RepCap);
    return (remainingCapacity_raw);
}

float getInstantaneousCurrent(){
    int current_raw = wiringPiI2CReadReg16(I2CDevice,Current);
    return current_raw * current_multiplier_mV;
}

float getInstantaneousVoltage(){
    int voltage_raw = wiringPiI2CReadReg16(I2CDevice,VCell);
    return voltage_raw * voltage_multiplier_V;
}

float getTimeToEmpty(){
    int TTE_raw = wiringPiI2CReadReg16(I2CDevice,TimeToEmpty);
    return TTE_raw * time_multiplier_Hours;
}

void max17055Init(){
    int StatusPOR = wiringPiI2CReadReg16(I2CDevice,0x00)&0x0002;
    int currentCapacity = getCapacity();
    if(currentCapacity != batteryCapacity){
        printf("Updating battery Capacity from %dmah to %dmah\n",currentCapacity, batteryCapacity);
        StatusPOR = 1; //Force re-config
    }
    if(StatusPOR == 0){ //If no re-config neccessary
        getCapacity();
        getSOC();
    } else { //Initial Config
        while(wiringPiI2CReadReg16(I2CDevice, 0x3D)&1){
            delay(10);
        }
        wiringPiI2CWriteReg16(I2CDevice, 0x18, (batteryCapacity/capacity_multiplier_mAH)); //Write DesignCap
        wiringPiI2CWriteReg16(I2CDevice, 0x45, (int)(batteryCapacity/capacity_multiplier_mAH)/32); //Write dQAcc
        wiringPiI2CWriteReg16(I2CDevice, 0x1E, 0x666);//256mA); //Write IchgTerm
        wiringPiI2CWriteReg16(I2CDevice, 0x3A, 0x9B00);//3.1V); //Write VEmpty
        int HibCFG = wiringPiI2CReadReg16(I2CDevice,0xBA); //Store original HibCFG value
        wiringPiI2CWriteReg16(I2CDevice, 0x60, 0x90); //Exit Hibernate Mode Step 1
        wiringPiI2CWriteReg16(I2CDevice, 0xBA, 0x0); //Exit Hibernate Mode Step 2
        wiringPiI2CWriteReg16(I2CDevice, 0x60, 0x0); //Exit Hibernate Mode Step 3

        if(ChargeVoltage > 4.275){
            wiringPiI2CWriteReg16(I2CDevice, 0x46,(int)((batteryCapacity/capacity_multiplier_mAH)/32)*51200/(batteryCapacity/capacity_multiplier_mAH));
            wiringPiI2CWriteReg16(I2CDevice, 0xDB, 0x8400);
        } else {
            wiringPiI2CWriteReg16(I2CDevice, 0x46,(int)((batteryCapacity/capacity_multiplier_mAH)/32)*44138/(batteryCapacity/capacity_multiplier_mAH));
            wiringPiI2CWriteReg16(I2CDevice, 0xDB, 0x8000);
        }
        
        while(wiringPiI2CReadReg16(I2CDevice,0xDB) & 0x8000){
            delay(10);
        }
        wiringPiI2CWriteReg16(I2CDevice, 0xBA, HibCFG);

        max17055_Status = wiringPiI2CReadReg16(I2CDevice,0x00);
        wiringPiI2CWriteReg16(I2CDevice, 0x00, Status & 0xFFFD);
    }
}

void clearLayer(IMAGE_LAYER_T *layer){
    IMAGE_T *image = &(layer->image);
    clearImageRGB(image, &clearColor);
    changeSourceAndUpdateImageLayer(layer);
}

void displayImage(char address[60], int xPos, int yPos, IMAGE_LAYER_T *imgLayer, int layerOffset){
    if (loadPng(&(imgLayer->image), address) == false) {
        fprintf(stderr, "%s \n", address);
    }
    createResourceImageLayer(imgLayer, layer + layerOffset);
    update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementImageLayerOffset(imgLayer, xPos, yPos, display, update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);
}

void displayText(char text[60], int xPos, int yPos, IMAGE_LAYER_T *imgLayer, int layerOffset){
    initImageLayer(imgLayer, displayX, displayY, type);
    if(darkMode){
        drawStringRGB(0, 0, text, &blackColour, &(imgLayer->image));
    } else {
        drawStringRGB(0, 0, text, &whiteColour, &(imgLayer->image));
    }
    
    createResourceImageLayer(imgLayer, layer + layerOffset);
    update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementImageLayerOffset(imgLayer, xPos, yPos, display, update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);
}

void updateBatteryText(){
    char temp[5];
    int xOffset = 3;
    strncpy(temp, batteryText, 5);

    //clearImageRGB(&(infoLayer->image), &clearColor);
    snprintf(batteryText, sizeof(batteryText),"%.0f%%", currentSOC);
    if(currentSOC >  99.5){
        xOffset = 4;
    } else if(currentSOC < 9.5){
        xOffset = 2;
    }
    if(strcmp(batteryText,temp) != 0){
        if(batteryPercentLayerEN){
            destroyImageLayer(&batteryPercentLayer);
        }
        displayText(batteryText,  displayX - statusIconSize - (fontWidth * xOffset) -4, 3, &batteryPercentLayer, 1);
        batteryPercentLayerEN = true;
    }
}

void displaySetup(){
    checkResolution();
    displayNumber = 0;

    type = VC_IMAGE_RGBA32;

    bcm_host_init();

    display = vc_dispmanx_display_open(displayNumber);
    assert(display != 0);

    result = vc_dispmanx_display_get_info(display, &info);
    assert(result == 0);
    //return result;
}

int main(){
    //Local Variables
    int time;

    //Display Setup
    displaySetup();

    //GPIO Setup
    wiringPiSetup();
    
    pinMode(volUp_button, INPUT); pullUpDnControl(volUp_button, PUD_UP);
    pinMode(volDn_button, INPUT); pullUpDnControl(volDn_button, PUD_UP);
    pinMode(pwr_button, INPUT);                               
    pinMode(headphone_detect, INPUT); pullUpDnControl(headphone_detect, PUD_UP);
    pinMode(charge_detect, INPUT);
    pinMode(pwm_Brightness, PWM_OUTPUT);
    pinMode(fan_pwm, PWM_OUTPUT);
    pinMode(low_voltage_shutdown, OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(101);
    pwmSetClock(192);
    pwmWrite(pwm_Brightness, brightness); //Set brightness to full
    pwmWrite(fan_pwm, fanSpeed); //Set fan speed to default (off)
    digitalWrite(low_voltage_shutdown, LOW);
    
    //Max17055 Fuel Gauge Setup
    I2CDevice = wiringPiI2CSetup(I2CAddress);
    max17055_Status = wiringPiI2CReadReg16(I2CDevice,0x00);
    if(max17055_Status == -1){
        printf("Max17055 Failed to Connect, Battery information unavailable.\n");
    } else {
        printf("Max17055 Connected.\n");
        max17055Init();
    }

    //Serial Port
    if((controllerSerial = serialOpen ("/dev/ttyACM0", 19200)) < 0) {
        printf("Failed to open Serial Port /dev/ttyACM0\n");
    } else {
        serialFlush(controllerSerial); //Flush all missed inputs
    }

    loadValues();

    while (run) {
        time = millis();

        //Battery
        if(charging == false){
            updateBatteryImage();
        } else {
            batteryChargingDisplay();
        }
        if(batteryLevelChanged){
            if(batteryIconLayerEN){
                destroyImageLayer(&batteryIconLayer);
            }
            displayImage(level_Icon, displayX - statusIconSize - 1, 1, &batteryIconLayer, 1);
            batteryIconLayerEN = true;
            batteryLevelChanged = false;
        }
        updateBatteryText();

        //Calibration Overlay
        if(calibrationStep != 0){
            if(calibrationStep == 1){
                strncpy(overlayImage, "./overlays/calibration1.png", 60);
            }
            if(calibrationStep == 2){
                strncpy(overlayImage, "./overlays/calibration2.png", 60);
            }
            if(calibrationStep == 3){
                strncpy(overlayImage, "./overlays/calibration3.png", 60);
            }
            if(calibrationStepChanged){
                if(kbdLayerEN){
                    destroyImageLayer(&kbdLayer);
                }
                kbdLayerEN = true;
                displayImage(overlayImage, (displayX /2) - (overlayImageX / 2), (displayY /2) - (overlayImageY / 2), &kbdLayer, 1);
                calibrationStepChanged = false;
            }
        }
        
        if(calibrationStep == 3 && calibrationTimerStarted == false){
            endCalibrationTimer = time;
            calibrationTimerStarted = true;
        }
        if(endCalibrationTimer + 3000 < time && calibrationTimerStarted == true){
            calibrationStep = 0;
            if(kbdLayerEN){
                destroyImageLayer(&kbdLayer);
                kbdLayerEN = false;
            }
            calibrationTimerStarted = false;
        }

        //Volume/brightness bar display
        if(volBrightPressed){
            if(strcmp(last_bar_Icon, bar_Icon) != 0){
                if(volBrightLayerEN){
                    destroyImageLayer(&volBrightLayer);
                }
                volBrightLayerEN = true;
                displayImage(bar_Icon, 1, 1, &volBrightLayer, 1);
                strncpy(last_bar_Icon, bar_Icon, 60);
            } else {
                if(!volBrightLayerEN){
                    volBrightLayerEN = true;
                    displayImage(bar_Icon, 1, 1, &volBrightLayer, 1);
                    strncpy(last_bar_Icon, bar_Icon, 60);
                }
            }
            volBrightPressed = false;
        }
        if(iconTimer + 3000 < time){
            if(volBrightLayerEN){
                destroyImageLayer(&volBrightLayer);
                volBrightLayerEN = false;
            }
            iconTimer = 0;
        }

        //Low Battery Shut Down
        if(max17055_Status != -1){
            if(!shutdownTimerStarted){
                if(getSOC() == 0){
                    strncpy(overlayImage, "./overlays/lowbatterywarning.png", 60);
                    if(kbdLayerEN){
                        destroyImageLayer(&kbdLayer);
                    }
                    kbdLayerEN = true;
                    displayImage(overlayImage, (displayX /2) - (overlayImageX / 2), (displayY /2) - (overlayImageY / 2), &kbdLayer, 1);
                    calibrationStepChanged = false;
                    shutdownTimer = time;
                    shutdownTimerStarted = true;
                }
            } else {
                if(shutdownTimer + 30000 < time){
                    printf("GPIO %d Set high", low_voltage_shutdown);
                    digitalWrite(low_voltage_shutdown, HIGH); //Tells the Attiny to initiate shutdown.
                }
            }
        }

        //Uncomment for Battery Logging
        //printf("Voltage = %f\n", getInstantaneousVoltage());
        //printf("Design Capacity = %d\n", getCapacity());
        //printf("Remaining Capacity = %d\n", getRemainingCapacity());
        //printf("Current = %f\n", getInstantaneousCurrent());
        //printf("SOC = %f\n", getSOC());

        //GPIO for Volume and Power
        if(GPIOTimer + 150 < time){
            checkGPIO();
            GPIOTimer = time;
        }
        
        //Controller Serial Interface
        if(controllerSerial != -1){
            controllerInterface();
        }
        

        //Set screen brightness
        pwmWrite(pwm_Brightness, brightness);

        //Fan Control
        //fanControl();

        //On Screen Keyboard
        if(osKeyboard){
            //Display Keyboard
            if(keyCase == 1){
                strncpy(overlayImage, "./keyboard/upperCase.png", 60);
            } else {
                strncpy(overlayImage, "./keyboard/lowerCase.png", 60);
            }
            if(lastKeyCase != keyCase || !kbdLayerEN){
                if(kbdLayerEN){
                    destroyImageLayer(&kbdLayer);
                }
                kbdLayerEN = true;
                lastKeyCase = keyCase;
                displayImage(overlayImage, (displayX /2) - (overlayImageX / 2), (displayY /2) - (overlayImageY / 2), &kbdLayer, 1);
            }
            if(keyboardChanged){
                //Update highlighted key
                char tempSTR[60];
                sprintf(tempSTR, "./keyboard/%d%d.png", keyboardRow, keyboardCollumn);
                strncpy(keyboardHighlight, tempSTR, 60);

                if(kbdHighlightLayerEN){
                    destroyImageLayer(&kbdHighlightLayer);
                }
                kbdHighlightLayerEN = true;
                displayImage(keyboardHighlight, (displayX /2) - (overlayImageX / 2), (displayY /2) - (overlayImageY / 2), &kbdHighlightLayer, 1);
                keyboardChanged = false;
            }

        }
            
        //Print Ram Usage
        //char buf[BUFSIZ];
        //snprintf(buf, sizeof(buf), "free -h");
        //system(buf);

    }

    //destroyImageLayer(&infoLayer);
    //destroyImageLayer(&batteryLayer);
    //destroyImageLayer(&wimageLayer);

    //destroyImageLayer(&infoTextLayer);
    //destroyImageLayer(&keyboardLayer);
    //destroyImageLayer(&bimageLayer);
    //destroyImageLayer(&cimageLayer);

    result = vc_dispmanx_display_close(display);
    assert(result == 0);

    
    //---------------------------------------------------------------------
    return 0;
}

void checkResolution(){
    FILE *resolution;
    char path[35];
    int x = 0;
    int y = 0;
    char *token;

    resolution = popen("fbset -s", "r");

    fgets(path, sizeof(path), resolution); //Read first line
    fgets(path, sizeof(path), resolution);  //Read second line
    fgets(path, sizeof(path), resolution);  //Read third line

    token = strtok(path, " ");
    while(token != NULL){
        if(atoi(token) != 0){
            if(x == 0){
                x = atoi(token);
            } else {
                if(y == 0){
                    y  = atoi(token);
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

void controllerInterface(){
    int value;

    while(serialDataAvail(controllerSerial)){
        value = serialGetchar(controllerSerial);
        if(value == brightnessUp){
            if(brightness <= 90){
                brightness += 10;
                saveValues();
            }
            iconTimer = millis();
            volBrightPressed = true;
        }
        if(value == brightnessDn){
            if(brightness >= 10){
                brightness -= 10;
                saveValues();
            }
            iconTimer = millis();
            volBrightPressed = true;
        }
        if(value == calibrationStepOne){
            printf("Calibration Step 1\n");
            calibrationStep = 1;
            calibrationStepChanged = true;
        }
        if(value == calibrationStepTwo){
            printf("Calibration Step 2\n");
            calibrationStep = 2;
            calibrationStepChanged = true;
        }
        if(value == calibrationComplete){
            printf("Calibration Step 3\n");
            calibrationStep = 3;
            calibrationStepChanged = true;
        }
        //On Screen Keyboard
        if(value == osKeyboardEnabled){
            osKeyboard = true;
            keyboardChanged = true;
        }
        if(value == osKeyboardDisabled){
            osKeyboard = false;
            if(kbdHighlightLayerEN){
                destroyImageLayer(&kbdHighlightLayer);
                kbdHighlightLayerEN = false;
            }
            if(kbdLayerEN){
                destroyImageLayer(&kbdLayer);
                kbdLayerEN = false;
            }
        }
        if(value == osKeyboardLeft){
            if(keyboardCollumn > 0){
                keyboardCollumn--;
            } else {
                if(keyboardRow != 0){
                    keyboardCollumn = 9;
                } else {
                    keyboardCollumn = 6;
                }
            }
            keyboardChanged = true;
        }
        if(value == osKeyboardRight){
            if(keyboardRow != 0){
                if(keyboardCollumn < 9){
                    keyboardCollumn++;
                } else {
                    keyboardCollumn = 0;
                }
            } else {
                if(keyboardCollumn < 6){
                    keyboardCollumn++;
                } else {
                    keyboardCollumn = 0;
                }
            }
            keyboardChanged = true;
        }
        if(value == osKeyboardUp){
            if(keyboardRow == 0){
                if(keyboardCollumn == 3){
                    keyboardCollumn = 4;
                } else if(keyboardCollumn == 4){
                    keyboardCollumn = 6;
                } else if(keyboardCollumn == 5){
                    keyboardCollumn = 7;
                } else if(keyboardCollumn > 5){
                    keyboardCollumn = 9;
                }
            }
            if(keyboardRow < 4){
                keyboardRow++;
            } else {
                keyboardRow = 0;
            }
            if(keyboardRow == 0){
                if(keyboardCollumn == 6){
                    keyboardCollumn = 4;
                } else if(keyboardCollumn == 7){
                    keyboardCollumn = 5;
                } else if(keyboardCollumn > 7){
                    keyboardCollumn = 6;
                } else if(keyboardCollumn == 3 || keyboardCollumn == 4 || keyboardCollumn == 5){
                    keyboardCollumn = 3;
                }
            }
            keyboardChanged = true;
        }
        if(value == osKeyboardDn){
            if(keyboardRow == 0){
                if(keyboardCollumn == 4){
                    keyboardCollumn = 6;
                } else if(keyboardCollumn == 5){
                    keyboardCollumn = 7;
                } else if(keyboardCollumn == 6){
                    keyboardCollumn = 9;
                } else if(keyboardCollumn == 3){
                    keyboardCollumn = 4;
                }
            }
            if(keyboardRow > 0){
                keyboardRow--;
            } else {
                keyboardRow = 4;
            }
            if(keyboardRow == 0){
                if(keyboardCollumn == 6){
                    keyboardCollumn = 4;
                } else if(keyboardCollumn == 7){
                    keyboardCollumn = 5;
                } else if(keyboardCollumn > 7){
                    keyboardCollumn = 6;
                } else if(keyboardCollumn == 3 || keyboardCollumn == 4 || keyboardCollumn == 5){
                    keyboardCollumn = 3;
                }
            }
            keyboardChanged = true;
        }
        if(value == osKeyboardSelect){
            if(keyboardRow == 1 && keyboardCollumn == 0){    //Caps Lock
                if(keyCase == 0){
                    keyCase = 1;
                } else {
                    keyCase = 0;
                }
            } else {
                if(keyCase == 0){ //Lower Case
                    serialPrintf(controllerSerial, "%c", osKeyboardLowercase[keyboardRow][keyboardCollumn]);
                } else { //Upper Case
                    serialPrintf(controllerSerial, "%c", osKeyboardUppercase[keyboardRow][keyboardCollumn]);
                }
            }
            keyboardChanged = true;
        }

        if(volBrightPressed){
            if(brightness == 0){
                strncpy(bar_Icon, "./brightnessIcons/brightness_0.png", 60);
            } else if(brightness == 10){
                strncpy(bar_Icon, "./brightnessIcons/brightness_10.png", 60);
            } else if(brightness == 20){
                strncpy(bar_Icon, "./brightnessIcons/brightness_20.png", 60);
            } else if(brightness == 30){
                strncpy(bar_Icon, "./brightnessIcons/brightness_30.png", 60);
            } else if(brightness == 40){
                strncpy(bar_Icon, "./brightnessIcons/brightness_40.png", 60);
            } else if(brightness == 50){
                strncpy(bar_Icon, "./brightnessIcons/brightness_50.png", 60);
            } else if(brightness == 60){
                strncpy(bar_Icon, "./brightnessIcons/brightness_60.png", 60);
            } else if(brightness == 70){
                strncpy(bar_Icon, "./brightnessIcons/brightness_70.png", 60);
            } else if(brightness == 80){
                strncpy(bar_Icon, "./brightnessIcons/brightness_80.png", 60);
            } else if(brightness == 90){
                strncpy(bar_Icon, "./brightnessIcons/brightness_90.png", 60);
            } else if(brightness == 100){
                strncpy(bar_Icon, "./brightnessIcons/brightness_100.png", 60);
            }
        }
    }
}

void loadValues(){
    FILE *fp;
    char buff[255];
    int parsingFailed = false;
    int temp;

    fp = fopen("config", "r");
    if(fp == NULL){
        printf("Failed to load config file, using defaults\n");
        return;
    }
    fgets(buff, 255, (FILE*)fp);
    sscanf(buff, "%d %f %d %d %d %d", &batteryCapacity, &lowVoltageThreshold, &cpuFanThreshold, &brightness, &volume, &darkMode);
    fclose(fp);

    //Input Parsing
    if(brightness > 101 || brightness < 1){
        brightness = 101;
        parsingFailed = true;
    } else {
        temp = brightness / 10;
        brightness = temp * 10;
        parsingFailed = true;
    }
    if(volume > 100 || volume < 0){
        volume = 100;
        parsingFailed = true;
    } else {
        temp = volume / 10;
        volume = temp * 10;
        parsingFailed = true;
    }
    if(lowVoltageThreshold < 2.8){
        lowVoltageThreshold = 3.0;
        parsingFailed = true;
    }
    if(parsingFailed){
        saveValues();
    }
    printf("batteryCapacity: %d\n", batteryCapacity);
    printf("lowVoltageThreshold: %f\n", lowVoltageThreshold);
    printf("cpuFanThreshold: %d\n", cpuFanThreshold);
    printf("brightness: %d\n", brightness);
    printf("volume: %d\n", volume);
    printf("DarkMode: %d\n", darkMode);
}

void saveValues(){
    FILE *fptr;
    fptr = fopen("config", "w");
    if(fptr == NULL){
        printf("Error Logging Values! \n");
        return;
    } else {
        fprintf(fptr, "%d %f %d %d %d %d", batteryCapacity, lowVoltageThreshold, cpuFanThreshold, brightness, volume, darkMode);
    }
    fclose(fptr);
}

void updateBatteryImage(){
    char currentIcon[60];
    strncpy(currentIcon, level_Icon, 60);
    if(max17055_Status != -1){ //Check if Max17055 is connected
        currentSOC = getSOC();
        if(currentSOC > 90){
            strncpy(level_Icon, "./batteryIcons/battery_full_white_18dp.png", 60);
        } else if(currentSOC > 80){
            strncpy(level_Icon, "./batteryIcons/battery_90_white_18dp.png", 60);
        } else if(currentSOC > 70){
            strncpy(level_Icon, "./batteryIcons/battery_80_white_18dp.png", 60);
        } else if(currentSOC > 60){
            strncpy(level_Icon, "./batteryIcons/battery_70_white_18dp.png", 60);
        } else if(currentSOC > 50){
            strncpy(level_Icon, "./batteryIcons/battery_60_white_18dp.png", 60);
        } else if(currentSOC > 40){
            strncpy(level_Icon, "./batteryIcons/battery_50_white_18dp.png", 60);
        } else if(currentSOC > 30){
            strncpy(level_Icon, "./batteryIcons/battery_40_white_18dp.png", 60);
        } else if(currentSOC > 20){
            strncpy(level_Icon, "./batteryIcons/battery_30_white_18dp.png", 60);
        } else if(currentSOC > 10){
            strncpy(level_Icon, "./batteryIcons/battery_20_white_18dp.png", 60);
        } else if(currentSOC > 0){
            strncpy(level_Icon, "./batteryIcons/battery_10_white_18dp.png", 60);
        } else {
            strncpy(level_Icon, "./batteryIcons/battery_0_white_18dp.png", 60);
        }
    } else {
        strncpy(level_Icon, "./batteryIcons/battery_error_18d.png", 60);
    }
    if(strcmp(currentIcon,level_Icon) != 0){
        //printf("Level Icon Changed\n");
        batteryLevelChanged = true;
    }
}

void batteryChargingDisplay(){
    unsigned int time = millis();
    if(chargeIconTimer + 1000 < time){
        chargeIconTimer = time;
        if(chargeStep < 4){
            chargeStep += 1;
        } else {
            chargeStep = 0;
        }
    }
    if(chargeStep == 0){
        strncpy(level_Icon, "./batteryIcons/battery_charge_0_18dp.png", 60);
    } else if(chargeStep == 1){
        strncpy(level_Icon, "./batteryIcons/battery_charge_1_18dp.png", 60);
    } else if(chargeStep == 2){
        strncpy(level_Icon, "./batteryIcons/battery_charge_2_18dp.png", 60);
    } else if(chargeStep == 3){
        strncpy(level_Icon, "./batteryIcons/battery_charge_3_18dp.png", 60);
    } else if(chargeStep == 4){
        strncpy(level_Icon, "./batteryIcons/battery_charge_4_18dp.png", 60);
    }
}

void checkGPIO(){
    char buf[BUFSIZ];
    //bool volumeChange = false;
    if(digitalRead(volUp_button) == 0){
        if(volume <= 90){
            volume += 10;
            snprintf(buf, sizeof(buf), "amixer -M set Playback %d%%", volume); //Playback
            //printf("%s\n", buf); //Debug
            system(buf);
            //volumeChange = true;
        }
        iconTimer = millis();
        volBrightPressed = true;
    }
    if(digitalRead(volDn_button) == 0){
        if(volume >= 10){
            volume -= 10;
            snprintf(buf, sizeof(buf), "amixer -M set Playback %d%%", volume); //Playback
            //printf("%s\n", buf); //Debug
            system(buf);
            //volumeChange = true;
        }
        iconTimer = millis();
        volBrightPressed = true;
    }
    if(digitalRead(pwr_button) == 1){
        printf("Shutdown\n");
        system("sudo halt"); //Shut Down
    }
    if(volBrightPressed){
        saveValues();

        if(volume == 0){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_0.png", 60);
        } else if( volume == 10){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_10.png", 60);
        } else if(volume == 20){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_20.png", 60);
        } else if(volume == 30){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_30.png", 60);
        } else if(volume == 40){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_40.png", 60);
        } else if(volume == 50){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_50.png", 60);
        } else if(volume == 60){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_60.png", 60);
        } else if(volume == 70){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_70.png", 60);
        } else if(volume == 80){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_80.png", 60);
        } else if(volume == 90){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_90.png", 60);
        } else if(volume == 100){
            strncpy(bar_Icon, "./volumeIcons/volume_bar_100.png", 60);
        }
    }
    if(digitalRead(charge_detect) == 0){ //If charging
        charging = true;
    } else {
        charging = false;
    }
}

void fanControl(){
    float systemp, millideg;
    FILE *thermal;

    thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    fscanf(thermal, "%f", &millideg);
    fclose(thermal);
    systemp = (int)(millideg / 10.0f) / 100.0f;
    //printf("CPU: %.2fC \n", systemp);
    if(systemp > cpuFanThreshold){
        fanSpeed = 420;
        pwmWrite(fan_pwm, fanSpeed);
    } else {
        fanSpeed = 0;
        pwmWrite(fan_pwm, fanSpeed);
    }
}
