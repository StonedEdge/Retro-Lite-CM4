#define SDA_PIN 6 
#define SDA_PORT PORTA
#define SCL_PIN 4 
#define SCL_PORT PORTA
#include <avr/io.h>
#include <SoftI2CMaster.h>

byte power_btn = 8; //Power button connected to this pin. Low Active
byte sys_on = 1; //Regulator power. Active High
byte sht_dwn = 2; //Connected to GPIO25. Signal to start Pi Shutdown. Active High
byte low_volt_shutdown = 0; //Connected to GPIO16 on pi. Used for low voltage shut down

byte BQ24292i_ADDRESS = 0xD6; 
byte powerBtnState;
byte systemState = 0; //Low Power Off
bool shutdownInit = false;

unsigned long powerBtnTimer;
unsigned long shutDown;
long powerOnDelay = 1000;
long powerOffDelay = 3000;
long shutDownDelay = 10000;
bool btnTimerStarted = false;
bool shutDownTimerStarted = false;

void setup() {
  //Serial.begin(9600);
  i2c_init();
  BQ_INIT(); 
  pinMode(power_btn, INPUT_PULLUP);
  pinMode(sys_on, OUTPUT);
  pinMode(sht_dwn, OUTPUT);
  pinMode(low_volt_shutdown, INPUT);
}

void loop() {
  powerButtonDebounce(); //Constantly read button and assign powerBtnState its correct value/debounce button 
  if (!shutdownInit) { //Initailized to 0 on first boot, run on first passthrough initially. Else, set btnTimerStarted=false. 
    if (powerBtnState) { //If button has been pressed, then run powerTimerCheck function, else set btnTimerStarted=false.
      powerTimerCheck(); 
    } else {
      btnTimerStarted = false;
    }
  } else {
    shutdownTimer();
  }

  if (digitalRead(low_volt_shutdown)) { 
    shutdownTimer();
  }

}

void powerTimerCheck(){
  if(!btnTimerStarted){
    btnTimerStarted = true;
    powerBtnTimer = millis();
  } else {
    if(systemState == 0){
      if(powerBtnTimer + powerOnDelay < millis()){
        systemState = 1;
        digitalWrite(sys_on, HIGH);
        btnTimerStarted = false;
      }
    } else {
      if(powerBtnTimer + powerOffDelay < millis()){
        systemState = 0;
        digitalWrite(sht_dwn, HIGH);
        btnTimerStarted = false;
        shutdownInit = true;
      }
    }
  }
}

void shutdownTimer(){
  if(!shutDownTimerStarted){
    shutDown = millis();
    shutDownTimerStarted = true;
    digitalWrite(sht_dwn, HIGH);//Tell Pi to Shut down
  } else {
    if(shutDown + shutDownDelay < millis()){
      digitalWrite(sys_on, LOW);
      digitalWrite(sht_dwn, LOW);
      shutDownTimerStarted = false;
      shutdownInit = false;
    }
  }
}

void powerButtonDebounce(){
  int input = !digitalRead(power_btn); //Set input to 1 if button is pressed, 0 if released/depressed
  if (input != powerBtnState){ // powerBtnState initialized to 0 on first pass through 
    powerBtnState = input; //Set powerBtnState to 1 if button is pressed, 0 if released/depressed
  }
}

void BQ_Write(uint8_t address, uint8_t reg, const uint8_t message) {
  i2c_start_wait(address | I2C_WRITE);
  i2c_write(reg);
  i2c_write(message);
  i2c_stop();
}

 uint8_t BQ_Read(uint8_t address, uint8_t reg){
  i2c_start_wait(address | I2C_WRITE);
  i2c_write(reg);
  i2c_rep_start(address | I2C_READ);
  uint8_t data = i2c_read(true);
  i2c_stop();
  return(data);
 }

// BQ24292i Register Configuration. To be cleaned. 
//
//// REG00
//const uint8_t EN_HIZ = 0;
//const uint8_t EN_ILIM = 1;
//const uint8_t INILIM = 0b111;
//const uint8_t INILIM = 0b111;   // 3A
//
//// REG01
//const uint8_t REG_RST = 0;
//const uint8_t WD_RST = 0;
//const uint8_t CHG_CONFIG = 0b01;
//const uint8_t SYS_MIN = 0b011;   // 3.3V
//const uint8_t BOOST_LIM = 1;
//
//// REG02
//const uint8_t ICHG = 0b011000; // 1.5A
//const uint8_t FORCE_20PCT = 0;
//
//// REG03 default values are OK
//
//// REG04
//const uint8_t VREG = 0b101100; // 4.208V
//const uint8_t BATLOWV = 0;
//const uint8_t VRECHG = 0;
//
//// REG05
//const uint8_t EN_TERM = 1;
//const uint8_t TERM_STAT = 0;
//const uint8_t WATCHDOG = 0b00; // Disable I2C WD
//const uint8_t EN_TIMER = 1;
//const uint8_t CHG_TIMER = 0b01;
//
////REG06
//const uint8_t BAT_COMP = 0;
//const uint8_t VCLAMP = 0;
//const uint8_t TREG = 0b01;   // 80C

uint8_t REG00_config = 0b01111111;
uint8_t REG01_config = 0b00010001;
uint8_t REG02_config = 0b01000000; //2A Charge Current. Do not alter for Retro Lite CM4 cell. 
uint8_t REG04_config = 0b10110000;
uint8_t REG05_config = 0b10001010;
uint8_t REG06_config = 0b00000001;
uint8_t REG07_config = 0b01001011;

void BQ_INIT() {
    BQ_Write(BQ24292i_ADDRESS, 0x00, REG00_config); 
    BQ_Write(BQ24292i_ADDRESS, 0x01, REG01_config); 
    BQ_Write(BQ24292i_ADDRESS, 0x02, REG02_config); 
    BQ_Write(BQ24292i_ADDRESS, 0x04, REG04_config);
    BQ_Write(BQ24292i_ADDRESS, 0x05, REG05_config); 
    BQ_Write(BQ24292i_ADDRESS, 0x06, REG06_config);
    BQ_Write(BQ24292i_ADDRESS, 0x07, REG07_config);
}
