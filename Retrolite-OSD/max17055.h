float getSOC();
int getCapacity();
int getRemainingCapacity();
float getInstantaneousCurrent();
float getInstantaneousVoltage();
float getTimeToEmpty();
void max17055Init();

int vr_mv = 3880;

/*
 * This macro converts empty voltage target (VE) and recovery voltage (VR)
 * in mV to max17055 0x3a reg value. max17055 declares 0% (empty battery) at
 * VE. max17055 reenables empty detection when the cell voltage rises above VR.
 * VE ranges from 0 to 5110mV, and VR ranges from 0 to 5080mV.
 */

#define MAX17055_VEMPTY_REG(ve_mv, vr_mv)\
  ((((ve_mv * 1000) / 10) << 7) | (vr_mv / 40))

float getSOC() {
  int SOC_raw = wiringPiI2CReadReg16(I2CDevice, RepSOC);
  return SOC_raw * percentage_multiplier;
}

int getCapacity() {
  int capacity_raw = wiringPiI2CReadReg16(I2CDevice, DesignCap);
  capacity_raw = capacity_raw * capacity_multiplier_mAH;
  return (capacity_raw);
}

int getRemainingCapacity() {
  int remainingCapacity_raw = wiringPiI2CReadReg16(I2CDevice, RepCap);
  remainingCapacity_raw = remainingCapacity_raw * capacity_multiplier_mAH;
  return (remainingCapacity_raw);
}

float getInstantaneousCurrent() {
  int16_t current_raw = wiringPiI2CReadReg16(I2CDevice, Current);
  return current_raw * current_multiplier_mV;
}

float getInstantaneousVoltage() {
  int voltage_raw = wiringPiI2CReadReg16(I2CDevice, VCell);
  return voltage_raw * voltage_multiplier_V;
}

float getTimeToEmpty() {
  int TTE_raw = wiringPiI2CReadReg16(I2CDevice, TimeToEmpty);
  return TTE_raw * time_multiplier_Hours;
}

float getTimeToFull() { 
  int TTF_raw = wiringPiI2CReadReg16(I2CDevice, TimeToFull);
  return TTF_raw * time_multiplier_Hours;
}

uint16_t getVEmpty() {
  uint16_t VEmpty_raw = wiringPiI2CReadReg16(I2CDevice, 0x3A);
  return (((VEmpty_raw >> 7) & 0x1FF) * 10);
}

void max17055Init() {
  int StatusPOR = wiringPiI2CReadReg16(I2CDevice, 0x00) & 0x0002;
  int currentCapacity = getCapacity();
  if (currentCapacity != batteryCapacity) {
    printf("Updating battery Capacity from %dmah to %dmah\n", currentCapacity, batteryCapacity);
    StatusPOR = 1; //Force re-config
  }
  if (StatusPOR == 0) { //If no re-config neccessary
    getCapacity();
    getSOC();
  } else { //Initial Config
    while (wiringPiI2CReadReg16(I2CDevice, 0x3D) & 1) {
      delay(10);
    }
    wiringPiI2CWriteReg16(I2CDevice, 0x18, (batteryCapacity / capacity_multiplier_mAH)); //Write DesignCap
    wiringPiI2CWriteReg16(I2CDevice, 0x45, (int)(batteryCapacity / capacity_multiplier_mAH) / 32); //Write dQAcc
    wiringPiI2CWriteReg16(I2CDevice, 0x1E, 0x666); //256mA); //Write IchgTerm
    wiringPiI2CWriteReg16(I2CDevice, 0x3A, MAX17055_VEMPTY_REG((int)lowVoltageThreshold, 3880)); //3.1V //Write VEmpty
    int HibCFG = wiringPiI2CReadReg16(I2CDevice, 0xBA); //Store original HibCFG value
    wiringPiI2CWriteReg16(I2CDevice, 0x60, 0x90); //Exit Hibernate Mode Step 1
    wiringPiI2CWriteReg16(I2CDevice, 0xBA, 0x0); //Exit Hibernate Mode Step 2
    wiringPiI2CWriteReg16(I2CDevice, 0x60, 0x0); //Exit Hibernate Mode Step 3

    if (ChargeVoltage > 4.275) {
      wiringPiI2CWriteReg16(I2CDevice, 0x46, (int)((batteryCapacity / capacity_multiplier_mAH) / 32) * 51200 / (batteryCapacity / capacity_multiplier_mAH));
      wiringPiI2CWriteReg16(I2CDevice, 0xDB, 0x8400);
    } else {
      wiringPiI2CWriteReg16(I2CDevice, 0x46, (int)((batteryCapacity / capacity_multiplier_mAH) / 32) * 44138 / (batteryCapacity / capacity_multiplier_mAH));
      wiringPiI2CWriteReg16(I2CDevice, 0xDB, 0x8000);
    }

    while (wiringPiI2CReadReg16(I2CDevice, 0xDB) & 0x8000) {
      delay(10);
    }
    wiringPiI2CWriteReg16(I2CDevice, 0xBA, HibCFG);

    max17055_Status = wiringPiI2CReadReg16(I2CDevice, 0x00);
    wiringPiI2CWriteReg16(I2CDevice, 0x00, Status & 0xFFFD);
  }
}
