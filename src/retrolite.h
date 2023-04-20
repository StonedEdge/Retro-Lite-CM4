//#ifndef RETROLITE_H_ 
//#define RETROLITE_H_

#define esc 27
#define backspace 8
#define enter 13
#define right 14
#define left 15
#define up 17
#define down 18

#define true 1
#define false 0

volatile bool run = true;

//I2C MAX17055 Registers
static int Status = 0x00;
static int VCell = 0x09;
//static int AvgVCell = 0x19;
static int Current = 0x0A;
//static int AvgCurrent = 0x0B;
static int RepSOC = 0x06;
static int RepCap = 0x05;
static int TimeToEmpty = 0x11;
static int TimeToFull = 0x20;
static int DesignCap = 0x18;

//GPIO Pins
static int volUp_button = 5; //Volume Up Button BCM = 19 wPi = 24
static int volDn_button = 16; //Volume Down Button BCM = 15 wPi = 16
static int pwr_button = 6;   //Software Power Button from ATTINY BCM = 25 wPi = 6
static int charge_detect = 7; //Pin goes low when charger is connected. BCM = 4 wPi = 7
static int pwm_Brightness = 26; //BCM = 12 wPi = 26
static int fan_pwm = 23; //BCM = 13, wPi = 23
static int low_voltage_shutdown = 27; // BCM = 16, wPi = 27
static int hpd_detect = 15; //BCM = 14, wPi = 15

//MAX17055
static int I2CAddress = 0x36; //0x36 Fuel Gauge, 0x48 ADS1115;
int I2CDevice;
int batteryCapacity = 4000; //0x1F40;

//Display
int displayX = 800;
int displayY = 480;
int displaySelect = 0; //Select HDMI. 0 for internal, 1 for External
static int statusIconSize = 18; //Size of status icon images
static int fontWidth = 8;
static int overlayImageX = 500; //Number of X pixels in overlay image
static int overlayImageY = 250; //Number of Y pixels in overlay image
int brightness = 100; //Default Full brightness
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
IMAGE_LAYER_T menuLayer;
int menuLayerEN = false;
IMAGE_LAYER_T menuTextLayer;
int menuTextLayerEN = false;
IMAGE_LAYER_T menuPointerLayer;
int menuPointerLayerEN = false;


//Controller Serial
int controllerSerial = -1;
int calibrationStep = 0;
int calibrationStepChanged = false;
int controllerDisconnected = false;
static int brightnessUp = 0x00;
static int brightnessDn = 0x01;
static int calibrationStepOne = 0x02;
static int calibrationStepTwo = 0x03;
static int calibrationComplete = 0x04;
static int osKeyboardEnabled = 0x05;    //No Longer Needed
static int osKeyboardLeft = 0x06;
static int osKeyboardRight = 0x07;
static int osKeyboardUp = 0x08;
static int osKeyboardDn = 0x09;
static int osKeyboardSelect = 0x10;
static int osKeyboardDisabled = 0x11;   //No Longer Needed
static int menuOpen = 0x12;
static int menuClose = 0x13;
static int controllerError = 0x127;
int osKeyboard = false;
int menuEnabled = false;
int menuUpdated = false;
int menuSelected = 1;
int menuDisplayed = 100;
int menuActive = 0; // 0 = Main Menu, 1 = Battery Info, 2 = Controller Calibration, 3 = OSK, 4 = Settings
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
static float current_multiplier_mV = 0.00015625;
static float voltage_multiplier_V = 7.8125e-5;
static float time_multiplier_Hours = 5.625/3600.0;
static float percentage_multiplier = 1.0/256.0;
int ioError = false;
float ChargeVoltage = 4.2;
int max17055_Status = 0;
float currentTime = 0;
float currentSOC = 0;
float systemp;
char level_Icon[60];// = "./batteryIcons/battery_full_white_18dp.png";
char overlayImage[60];// = "./overlays/calibration3.png";
char bar_Icon[60];// = "./volumeIcons/volume_bar_0.png";
char last_bar_Icon[60];
char keyboardHighlight[60];// = "./keyboard/00.png";
int controllerConnected = 0;
int volume = 100;
int chargeStep = 0; //Used for charging animation
int charging = false;
int cpuFanThreshold = 55; //Temperature fan starts running at. Default 65
int fanSpeed = 0;
float lowVoltageThreshold = 3.1; //If voltage drops below this value, engage auto shut down.
int batteryLevelChanged = false;
int volBrightPressed = false;
//char tempSTR[60];
char batteryText[5];
int hpd_change = false;

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
int getControllerData();
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
void displayText(char address[60], int xPos, int yPos, IMAGE_LAYER_T* imgLayer, int layerOffset, RGBA8_T colour);
void displaySetup();
float getSOC();
int getCapacity();
int getRemainingCapacity();
float getInstantaneousCurrent();
float getInstantaneousVoltage();
float getTimeToEmpty();
float getTimeToFull();
void max17055Init();
long map(long x, long in_min, long in_max, long out_min, long out_max);
void settingValueChanged(int value);


char osKeyboardLowercase[5][10] = {
    {esc, left, right, 32, 45, 47, enter, 0, 0, 0},
    {0, 122, 120, 99, 118, 98, 110, 109, 44, backspace},
    {97, 115, 100, 102, 103, 104, 106, 107, 108, 46},
    {113, 119, 101, 114, 116, 121, 117, 105, 111, 112},
    {49, 50, 51, 52, 53, 54, 55, 56, 57, 48}
};
char osKeyboardUppercase[5][10] = {
    {esc, up, down, 32, 95, 63, enter, 0, 0, 0},
    {0, 90, 88, 67, 86, 66, 78, 77, 60, backspace},
    {65, 83, 68, 70, 71, 72, 74, 75, 76, 62},
    {81, 87, 69, 82, 84, 89, 85, 73, 79, 80},
    {33, 64, 35, 36, 37, 94, 38, 42, 40, 41}
};

//#endif
