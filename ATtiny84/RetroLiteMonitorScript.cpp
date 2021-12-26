byte power_btn = 8; //Power button connected to this pin. Low Active
byte sys_on = 1; //Regulator power. Active High
byte sht_dwn = 2; //Connected to GPIO25. Signal to start Pi Shutdown. Active High
byte low_volt_shutdown = 0; //Connected to GPIO16 on pi. Used for low voltage shut down

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
  pinMode(power_btn, INPUT_PULLUP);
  pinMode(sys_on, OUTPUT);
  pinMode(sht_dwn, OUTPUT);
  pinMode(low_volt_shutdown, INPUT);
}

void loop() {
  powerButtonDebounce(); //Constantly read button and assign powerBtnState its correct value/debounce button 
  if (!shutdownInit) { //Initailized to 0 on first boot, run on first passthrough initially. Else, set btnTimerStarted=false. 
    if (powerBtnState) { //If button has been pressed, then run powerTimerCheck function, else run shutdownTimer();
      powerTimerCheck(); 
    } else {
      btnTimerStarted = false;
    }
  } else {
    shutdownTimer();
  }

  if (low_volt_shutdown == HIGH) {
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
