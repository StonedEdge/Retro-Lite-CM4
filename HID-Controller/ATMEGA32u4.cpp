/* 
 * Retro Lite QM-C64 
 * Simplified ATMEGA32u4 USB Gamepad + Mouse Firmware
 *
 * Features
 * --------
 * - Dual analog sticks
 * - 13 buttons
 * - POV Hat Dpad
 * - Mouse emulation
 * - EEPROM joystick calibration
 *
 * Calibration
 * -----------
 * Hold SELECT + R3 to enter calibration mode
 *
 * STEP 1:
 * Centre both sticks
 *
 * STEP 2:
 * Press A to capture centre point
 *
 * STEP 3:
 * Rotate sticks fully in all directions
 *
 * STEP 4:
 * Press A again to save calibration
 */

#include <Joystick.h>
#include <Mouse.h>
#include <EEPROM.h>

// ======================================================
// BUTTON DEFINITIONS
// ======================================================

const byte buttonCount = 13;

byte buttonPins[buttonCount] = {
  0,1,2,3,4,5,6,7,12,13,14,15,16
};

byte dpadPins[4] = {
  8,11,9,10
};

// ======================================================
// BUTTON STATES
// ======================================================

byte dpadPinsState[4];
byte lastButtonState[buttonCount];
byte currentButtonState[buttonCount];

// ======================================================
// ANALOG PINS
// ======================================================

const int leftJoyX  = A3;
const int leftJoyY  = A2;
const int rightJoyX = A1;
const int rightJoyY = A0;

// ======================================================
// JOYSTICK SETTINGS
// ======================================================

const bool invertLeftY  = false;
const bool invertLeftX  = false;

const bool invertRightY = true;
const bool invertRightX = true;

const int deadBandLeft  = 10;
const int deadBandRight = 10;

const int earlyLeftX  = 30;
const int earlyLeftY  = 30;

const int earlyRightX = 30;
const int earlyRightY = 30;

// ======================================================
// MOUSE SETTINGS
// ======================================================

boolean mouseEnabled = false;

int mouseDivider = 8;

unsigned long mouseTimer = 0;
int mouseInterval = 10;

unsigned long mouseModeTimer = 0;
boolean mouseModeTimerStarted = false;

// ======================================================
// CALIBRATION
// ======================================================

boolean calibrationMode = false;
int calibrationStep = 1;

// ======================================================
// USB GAMEPAD
// ======================================================

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_GAMEPAD,
  buttonCount,
  1,
  true, true, true,
  true, true, true,
  false, false,
  false, false, false
);

// ======================================================
// DEFAULT CALIBRATION VALUES
// ======================================================

int minLeftX  = 330;
int maxLeftX  = 830;
int midLeftX  = 512;

int minLeftY  = 205;
int maxLeftY  = 635;
int midLeftY  = 477;

int minRightY = 215;
int maxRightY = 730;
int midRightY = 529;

int minRightX = 300;
int maxRightX = 780;
int midRightX = 525;

// ======================================================
// LOOKUP TABLES
// ======================================================

byte leftXLUT[350];
byte leftYLUT[350];

byte rightXLUT[350];
byte rightYLUT[350];

// ======================================================
// SETUP
// ======================================================

void setup() {

  // USB axis ranges
  Joystick.setXAxisRange(0, 254);
  Joystick.setYAxisRange(0, 254);

  Joystick.setZAxisRange(0, 254);
  Joystick.setRxAxisRange(0, 254);

  // Start USB HID devices
  Joystick.begin(false);
  Mouse.begin();

  // Configure button inputs
  for(int i = 0; i < buttonCount; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Configure dpad inputs
  for(int i = 0; i < 4; i++) {
    pinMode(dpadPins[i], INPUT_PULLUP);
  }

  // Load calibration from EEPROM
  eepromLoad();

  // Build LUTs
  rebuildLUTs();
}

// ======================================================
// MAIN LOOP
// ======================================================

void loop() {

  buttonRead();

  // ====================================================
  // ENTER CALIBRATION MODE
  // SELECT + R3
  // ====================================================

  if(lastButtonState[8] && lastButtonState[12]) {

    calibrationMode = true;
    calibrationStep = 1;

    delay(1000);
  }

  // ====================================================
  // NORMAL MODE
  // ====================================================

  if(!calibrationMode) {

    joypadButtons();

    joystickInput();

    dPadInput();

    Joystick.sendState();

  }

  // ====================================================
  // CALIBRATION MODE
  // ====================================================

  else {

    joystickCalibration();
  }

  // ====================================================
  // MOUSE MODE TOGGLE
  // Hold R3 for 2 seconds
  // ====================================================

  if(lastButtonState[12]) {

    if(!mouseModeTimerStarted) {

      mouseModeTimerStarted = true;
      mouseModeTimer = millis();

    } else {

      if(mouseModeTimer + 2000 < millis()) {

        mouseEnabled = !mouseEnabled;

        mouseModeTimerStarted = false;
      }
    }

  } else {

    mouseModeTimerStarted = false;
  }

  // ====================================================
  // MOUSE UPDATE
  // ====================================================

  if(mouseEnabled) {

    if(mouseTimer + mouseInterval < millis()) {

      mouseControl();

      mouseTimer = millis();
    }
  }
}

// ======================================================
// BUTTON READ
// ======================================================

void buttonRead() {

  // Buttons
  for(int i = 0; i < buttonCount; i++) {

    int input = !digitalRead(buttonPins[i]);

    if(input != lastButtonState[i]) {
      lastButtonState[i] = input;
    }
  }

  // Dpad
  for(int i = 0; i < 4; i++) {

    int input = !digitalRead(dpadPins[i]);

    if(input != dpadPinsState[i]) {
      dpadPinsState[i] = input;
    }
  }
}

// ======================================================
// BUTTON OUTPUT
// ======================================================

void joypadButtons() {

  for(int i = 0; i < buttonCount; i++) {

    if(lastButtonState[i] != currentButtonState[i]) {

      Joystick.setButton(i, lastButtonState[i]);

      currentButtonState[i] = lastButtonState[i];
    }
  }
}

// ======================================================
// D-PAD
// ======================================================

void dPadInput() {

  int angle = -1;

  // UP
  if(dpadPinsState[0]) {

    if(dpadPinsState[1]) {
      angle = 45;
    }
    else if(dpadPinsState[3]) {
      angle = 315;
    }
    else {
      angle = 0;
    }
  }

  // DOWN
  else if(dpadPinsState[2]) {

    if(dpadPinsState[1]) {
      angle = 135;
    }
    else if(dpadPinsState[3]) {
      angle = 225;
    }
    else {
      angle = 180;
    }
  }

  // RIGHT
  else if(dpadPinsState[1]) {
    angle = 90;
  }

  // LEFT
  else if(dpadPinsState[3]) {
    angle = 270;
  }

  Joystick.setHatSwitch(0, angle);
}

// ======================================================
// JOYSTICK INPUT
// ======================================================

void joystickInput() {

  int var;

  // RIGHT Y
  var = readJoystick(rightJoyY, invertRightY);
  var = (var - minRightY) / 2;
  Joystick.setRxAxis(rightYLUT[var]);

  // RIGHT X
  var = readJoystick(rightJoyX, invertRightX);
  var = (var - minRightX) / 2;
  Joystick.setZAxis(rightXLUT[var]);

  // LEFT Y
  var = readJoystick(leftJoyY, invertLeftY);
  var = (var - minLeftY) / 2;
  Joystick.setYAxis(leftYLUT[var]);

  // LEFT X
  var = readJoystick(leftJoyX, invertLeftX);
  var = (var - minLeftX) / 2;
  Joystick.setXAxis(leftXLUT[var]);
}

// ======================================================
// MOUSE CONTROL
// ======================================================

void mouseControl() {

  int var;

  // Y AXIS
  var = readJoystick(leftJoyY, invertLeftY);
  var = (var - minLeftY) / 2;
  var = leftYLUT[var];
  var = var - 127;

  int yMove = var / mouseDivider;

  // X AXIS
  var = readJoystick(leftJoyX, invertLeftX);
  var = (var - minLeftX) / 2;
  var = leftXLUT[var];
  var = var - 127;

  int xMove = var / mouseDivider;

  // Move mouse
  Mouse.move(xMove, yMove, 0);

  // Left click
  if(lastButtonState[6]) {

    if(!Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.press(MOUSE_LEFT);
    }

  } else {

    if(Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.release(MOUSE_LEFT);
    }
  }

  // Right click
  if(lastButtonState[9]) {

    if(!Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.press(MOUSE_RIGHT);
    }

  } else {

    if(Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.release(MOUSE_RIGHT);
    }
  }
}

// ======================================================
// JOYSTICK CALIBRATION
// ======================================================

void joystickCalibration() {

  buttonRead();

  // ====================================================
  // STEP 1
  // WAIT FOR CENTERED STICKS
  // ====================================================

  if(calibrationStep == 1) {

    int lx = readJoystick(leftJoyX, invertLeftX);
    int ly = readJoystick(leftJoyY, invertLeftY);

    int rx = readJoystick(rightJoyX, invertRightX);
    int ry = readJoystick(rightJoyY, invertRightY);

    bool centered =
      abs(lx - 512) < 40 &&
      abs(ly - 512) < 40 &&
      abs(rx - 512) < 40 &&
      abs(ry - 512) < 40;

    // Press A to continue
    if(centered && lastButtonState[1]) {

      midLeftX  = lx;
      midLeftY  = ly;

      midRightX = rx;
      midRightY = ry;

      calibrationStep = 2;

      delay(500);
    }
  }

  // ====================================================
  // STEP 2
  // INITIALIZE MIN/MAX
  // ====================================================

  else if(calibrationStep == 2) {

    minLeftX  = midLeftX;
    minLeftY  = midLeftY;

    maxLeftX  = midLeftX;
    maxLeftY  = midLeftY;

    minRightX = midRightX;
    minRightY = midRightY;

    maxRightX = midRightX;
    maxRightY = midRightY;

    calibrationStep = 3;

    delay(500);
  }

  // ====================================================
  // STEP 3
  // RECORD FULL RANGE
  // ====================================================

  else if(calibrationStep == 3) {

    int var;

    // LEFT X
    var = readJoystick(leftJoyX, invertLeftX);

    if(var > maxLeftX) maxLeftX = var;
    if(var < minLeftX) minLeftX = var;

    // LEFT Y
    var = readJoystick(leftJoyY, invertLeftY);

    if(var > maxLeftY) maxLeftY = var;
    if(var < minLeftY) minLeftY = var;

    // RIGHT X
    var = readJoystick(rightJoyX, invertRightX);

    if(var > maxRightX) maxRightX = var;
    if(var < minRightX) minRightX = var;

    // RIGHT Y
    var = readJoystick(rightJoyY, invertRightY);

    if(var > maxRightY) maxRightY = var;
    if(var < minRightY) minRightY = var;

    // Press A to save
    if(lastButtonState[1]) {

      writeJoystickConfig();

      rebuildLUTs();

      calibrationMode = false;
      calibrationStep = 1;

      delay(1000);
    }
  }
}

// ======================================================
// JOYSTICK READ
// ======================================================

int readJoystick(int joystickPin, boolean invertOutput) {

  int var = analogRead(joystickPin);

  if(invertOutput) {
    var = 1023 - var;
  }

  return var;
}

// ======================================================
// LUT REBUILD
// ======================================================

void rebuildLUTs() {

  joystickBuildLUT(
    leftXLUT,
    minLeftX,
    midLeftX,
    maxLeftX,
    earlyLeftX,
    deadBandLeft
  );

  joystickBuildLUT(
    leftYLUT,
    minLeftY,
    midLeftY,
    maxLeftY,
    earlyLeftY,
    deadBandLeft
  );

  joystickBuildLUT(
    rightXLUT,
    minRightX,
    midRightX,
    maxRightX,
    earlyRightX,
    deadBandRight
  );

  joystickBuildLUT(
    rightYLUT,
    minRightY,
    midRightY,
    maxRightY,
    earlyRightY,
    deadBandRight
  );
}

// ======================================================
// LUT BUILD
// ======================================================

void joystickBuildLUT(
  byte output[350],
  int minIn,
  int midIn,
  int maxIn,
  int earlyStop,
  int deadBand
) {

  int shiftedMin = 0;

  int shiftedMid = (midIn - minIn) / 2;

  int shiftedMax = (maxIn - minIn) / 2;

  int temp;

  for(int i = 0; i < 350; i++) {

    // LOWER HALF
    if(i < shiftedMid) {

      if(i > shiftedMin + earlyStop) {

        temp = map(
          i,
          shiftedMin,
          shiftedMid - deadBand,
          0,
          127
        );

      } else {

        temp = 0;
      }

    }

    // UPPER HALF
    else {

      if(i < shiftedMax - earlyStop) {

        temp = map(
          i,
          shiftedMid + deadBand,
          shiftedMax,
          127,
          254
        );

      } else {

        temp = 254;
      }
    }

    // DEADZONE
    if(i < shiftedMid + deadBand &&
       i > shiftedMid - deadBand) {

      temp = 127;
    }

    output[i] = temp;
  }
}

// ======================================================
// EEPROM HELPERS
// ======================================================

void writeIntIntoEEPROM(int address, int number) {

  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;

  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}

int readIntFromEEPROM(int address) {

  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);

  return (byte1 << 8) + byte2;
}

// ======================================================
// EEPROM LOAD
// ======================================================

void eepromLoad() {

  if(readIntFromEEPROM(1) != -1) {
    readJoystickConfig();
  }
}

// ======================================================
// EEPROM READ
// ======================================================

void readJoystickConfig() {

  minLeftX  = readIntFromEEPROM(1);
  maxLeftX  = readIntFromEEPROM(3);
  midLeftX  = readIntFromEEPROM(5);

  minLeftY  = readIntFromEEPROM(7);
  maxLeftY  = readIntFromEEPROM(9);
  midLeftY  = readIntFromEEPROM(11);

  minRightY = readIntFromEEPROM(13);
  maxRightY = readIntFromEEPROM(15);
  midRightY = readIntFromEEPROM(17);

  minRightX = readIntFromEEPROM(19);
  maxRightX = readIntFromEEPROM(21);
  midRightX = readIntFromEEPROM(23);
}

// ======================================================
// EEPROM WRITE
// ======================================================

void writeJoystickConfig() {

  writeIntIntoEEPROM(1,  minLeftX);
  writeIntIntoEEPROM(3,  maxLeftX);
  writeIntIntoEEPROM(5,  midLeftX);

  writeIntIntoEEPROM(7,  minLeftY);
  writeIntIntoEEPROM(9,  maxLeftY);
  writeIntIntoEEPROM(11, midLeftY);

  writeIntIntoEEPROM(13, minRightY);
  writeIntIntoEEPROM(15, maxRightY);
  writeIntIntoEEPROM(17, midRightY);

  writeIntIntoEEPROM(19, minRightX);
  writeIntIntoEEPROM(21, maxRightX);
  writeIntIntoEEPROM(23, midRightX);
}
