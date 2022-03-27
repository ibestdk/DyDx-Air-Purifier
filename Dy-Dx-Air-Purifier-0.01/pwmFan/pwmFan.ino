#include <SoftwareSerial.h>
#define brightnessPin 6
//SoftwareSerial mySerial2(10,11); // RX, TX
SoftwareSerial mySerial(2,3); // RX, TX
//SoftwareSerial mySerial3(2,3); // RX, TX
int pm1 = 0;
int pm2_5 = 0;
int pm10 = 0;
int last_Update = 0;
int UpdateTime = 1000;
bool isDHTRead = false;
bool isPMRead = false;
int last_pm_25 = -1;

const byte OC1A_PIN = 9;
const byte OC1B_PIN = 10;

const word PWM_FREQ_HZ = 25000; //Adjust this value to adjust the frequency
const word TCNT1_TOP = 16000000/(2*PWM_FREQ_HZ);

void setup() {
  Serial.begin(9600);
  while (!Serial) ;
  mySerial.begin(9600);
  //ControlSerial.begin(9600);
  pinMode(OC1A_PIN, OUTPUT);
  pinMode(brightnessPin, OUTPUT);
  analogWrite(brightnessPin, 255);
  // Clear Timer1 control and count registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // Set Timer1 configuration
  // COM1A(1:0) = 0b10   (Output A clear rising/set falling)
  // COM1B(1:0) = 0b00   (Output B normal operation)
  // WGM(13:10) = 0b1010 (Phase correct PWM)
  // ICNC1      = 0b0    (Input capture noise canceler disabled)
  // ICES1      = 0b0    (Input capture edge select disabled)
  // CS(12:10)  = 0b001  (Input clock select = clock/1)
  
  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << CS10);
  ICR1 = TCNT1_TOP;
  setPwmDuty(0);
}

void loop() {
  
  Brightness();
  PMMeasure();
  FanMode();
  if(millis() - last_Update > UpdateTime){
      last_Update = millis();
      sentPM2port(pm2_5);
  }
}
void setPwmDuty(byte duty) {
  OCR1A = (word) (duty*TCNT1_TOP)/100;
}
void SetFan(int fanPwm){
  switch(fanPwm){
    case 100: setPwmDuty(5); break;
    case 101: setPwmDuty(10); break;
    case 102: setPwmDuty(20); break;
    case 103: setPwmDuty(20); break;
    case 104: setPwmDuty(30); break;
    case 105: setPwmDuty(30); break;
    case 106: setPwmDuty(40); break;
    case 107: setPwmDuty(40); break;
    case 108: setPwmDuty(50); break;
    case 109: setPwmDuty(50); break;
    case 110: setPwmDuty(60); break;
    case 111: setPwmDuty(80); break;
  }
}
void FanMode(){
  int fanPwm = 0;
  String input = Serial.readStringUntil('\n');
  if(input.startsWith("FANA")){
    input.remove(0,4); //Remove word "FAN"
    fanPwm = input.toInt();
    Serial.println(fanPwm);
    SetFan(fanPwm);
  }
  if(input.startsWith("FAN")){
    input.remove(0,3); //Remove word "FAN"
    fanPwm = input.toInt();
    Serial.println(fanPwm);
    SetFan(fanPwm);
  }
}
void PMMeasure(){
  mySerial.listen();
  pm2_5 = -1;
  int index = 0;
  char value;
  char previousValue;
 
  while (mySerial.available()) {
    value = mySerial.read();
    if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)){
      Serial.println("Cannot find the data header.");
      isPMRead = false;
      break;
    }
    isPMRead = true;
    if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
      previousValue = value;
    }
    else if (index == 5) {
      pm1 = 256 * previousValue + value;
      //Serial.print("{ ");
      //Serial.print("\"pm1\": ");
      //Serial.print(pm1);
      //Serial.print(" ug/m3");
      //Serial.print(", ");
    }
    else if (index == 7) {
      pm2_5 = 256 * previousValue + value;
      //Serial.print("\"pm2_5\": ");
      //Serial.print(pm2_5);
      //Serial.print(" ug/m3");
      //Serial.print(", ");
    }
    else if (index == 9) {
      pm10 = 256 * previousValue + value;
      //Serial.print("\"pm10\": ");
      //Serial.print(pm10);
      //Serial.print(" ug/m3");
    } 
    else if (index > 15) {
      break;
    }
  index++;
  }
  while(mySerial.available()) mySerial.read();
  //Serial.println(" }");

  //Blynk.virtualWrite(V1,pm2_5);
  delay(500);
}
void sentPM2port(int input){
  if(input != last_pm_25 && input > -1){
    Serial.print("PM");
    Serial.println(input);
    last_pm_25 = input;
    delay(100);
    
  }
}
void Brightness(){
  int bri = -1;
  String input = Serial.readStringUntil('\n');
  if(input.startsWith("BRI")){
    input.remove(0,3); //Remove word "BRI"
    bri = input.toInt();
    switch(bri){
    case 0: analogWrite(brightnessPin, 0); delay(250); break;
    case 1: analogWrite(brightnessPin, 42); delay(250); break;
    case 2: analogWrite(brightnessPin, 86); delay(250); break;
    case 3: analogWrite(brightnessPin, 127); delay(250); break;
    case 4: analogWrite(brightnessPin, 171); delay(250); break;
    case 5: analogWrite(brightnessPin, 213); delay(250); break;
    case 6: analogWrite(brightnessPin, 255); delay(250); break;
  }
}
  delay(250);
}
