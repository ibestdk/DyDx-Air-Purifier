#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         D2
  #define TFT_RST        D3                                            
  #define TFT_DC         D4

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// For 1.14", 1.3", 1.54", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#include "dydxIcon.h"
#include <Fonts/roboto12t.h>
#include <Fonts/roboto14t.h>
#include <Fonts/roboto16t.h>
#include <Fonts/roboto20t.h>
#include <Fonts/roboto48t.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
WiFiManager wm;
char auth[] = "Z07tA5yefNJnnRK84SjVLVhzjyh83YEf";
//char auth[] = "m8JlQIu1twntzB1x1uDv4yDhEZkpRAiH"; //test
#define BLYNK_PRINT Serial
#define buzzerPin D8
#define menuBtn D10
#define wifiResetBtn D12
#define dhtPin D6
#include "DHT.h"
#define DHTTYPE DHT11
//#include <Wire.h> 
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(D8,D9); // RX, TX
//SoftwareSerial ControlSerial(D11,D12); // RX, TX
DHT dht(dhtPin, DHTTYPE);
int pm1 = 0;
unsigned int pm2_5 = 0;
int pm10 = 0;
float t = 0;
float h = 0;
bool isDHTRead = false;
bool isPMRead = false;
bool isOff = false;
bool isAuto = true;
int currMode = 1;
int pmMode = -1;
int lastpmMode = -1;
int fanSpeed = 0;
unsigned int DisplayTime = 2000;
unsigned int lastDisplayTime = 0;
unsigned int reConnectTime = 0;
unsigned int PMTime = 1500;
unsigned int lastPM = 0;
unsigned int TempTime = 5000;
unsigned int lastTemp = 0;
unsigned int DimTime = 5000;
unsigned int lastDim = 0;
unsigned int BtnTime = 15;
unsigned int lastBtn = 0;
unsigned int rapidTime = 80000;
unsigned int lastRapid = 0;
unsigned int currMillis = 0;
unsigned int lastReconnect = 0;
int laststate = 0;
//Btn
int buttonStatePrevious = LOW;                      // previousstate of the switch

unsigned long minButtonLongPressDuration = 2500;    // Time we wait before we see the press as a long press
unsigned long buttonLongPressMillis;                // Time in ms when we the button was pressed
bool buttonStateLongPress = false;                  // True if it is a long press

const int intervalButton = 50;                      // Time between two readings of the button state
unsigned long previousButtonMillis;                 // Timestamp of the latest reading

unsigned long buttonPressDuration;                  // Time the button is pressed in ms

//// GENERAL ////

unsigned long currentMillis;          // Variabele to store the number of milleseconds since the Arduino has started

//Buzzer
#include "pitches.h"
int turnOnmelody[] = {

  NOTE_E7, NOTE_F7, NOTE_G7
};
int turnOffmelody[] = {

  NOTE_G7, NOTE_F7, NOTE_C7
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {

  4, 4, 4
};

//Brightness
int brightState = 1;

void setup(){
  pinMode(menuBtn,INPUT);
  pinMode(buzzerPin,OUTPUT);
  pinMode(wifiResetBtn,INPUT);
  digitalWrite(buzzerPin,LOW);
  dht.begin();
  Serial.begin(9600);
  Blynk.config(auth, IPAddress(185,78,164,48), 8080);
  //Serial.println(F("Hello! ST77xx TFT Test"));
  manFanMode(0);
  WelcomeSound();
  // Use this initializer if using a 1.8" TFT screen:
  //tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  // OR use this initializer if using a 1.8" TFT screen with offset such as WaveShare:
  // tft.initR(INITR_GREENTAB);      // Init ST7735S chip, green tab

  // OR use this initializer (uncomment) if using a 1.44" TFT:
  //tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab

  // OR use this initializer (uncomment) if using a 0.96" 160x80 TFT:
  //tft.initR(INITR_MINI160x80);  // Init ST7735S mini display

  // OR use this initializer (uncomment) if using a 1.3" or 1.54" 240x240 TFT:
  //tft.init(240, 240);           // Init ST7789 240x240

  // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240

  // OR use this initializer (uncomment) if using a 1.14" 240x135 TFT:
  //tft.init(135, 240);           // Init ST7789 240x135
  
  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  tft.setSPISpeed(79999999);
  
  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);
  
  Brightness(6);
  tft.setRotation(3);
  // large block of text
  /*
  tft.fillScreen(ST77XX_BLACK);

  WelcomeSplash(ST77XX_WHITE);
  delay(3000);
  APSetup(ST77XX_WHITE);
  delay(3000);
  WifiConnected(".Pvpeez13");
  delay(3000);*/
  
  //WelcomeSplash();
  ShowLogo();
  WiFiManager wm;

  //wm.resetSettings();
  int btnState = digitalRead(wifiResetBtn);
    if(btnState == 0){
      //wm.resetSettings();
    }
  
  //pinMode(fanPin,OUTPUT);
  //digitalWrite(fanPin,HIGH);
  //reset settings - wipe credentials for testing
    //wm.resetSettings();
     
    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result
    
    bool res;
    //res = wm.startConfigPortal("Dy|Dx Air AP"); // anonymous ap
    // res = wm.autoConnect(); // auto generated AP name from chipid
    //Serial.println(WiFi.SSID());
     
    //res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  if(WiFi.SSID() == "" && WiFi.status() != WL_CONNECTED){
      //ShowAPSetup();
      ShowQR_APSetup();
      res = wm.startConfigPortal("Dy|Dx Air AP");
      //res = wm.autoconnect("Dy|Dx Air AP");
          //delay(3000);
      if(res == false){
        ESP.reset();
        delay(5000);
      }

    }
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  //if you get here you have connected to the WiFi
  tft.invertDisplay(true);
  tft.fillScreen(ST77XX_BLACK);
  delay(1000);
  if(WiFi.status() == WL_CONNECTED){
      WifiConnected(WiFi.SSID());
      WiFi.setAutoReconnect(true);
      Blynk.connect();
      //Serial.println("connected...yeey :)");
      Blynk.virtualWrite(V4,1);
      Blynk.virtualWrite(V9,"---Power On---");
  }
  delay(1000);
  ShowAutoMode();
  delay(1000);
  //ShowNightMode();
  tft.fillRect(21,21,279,199,ST77XX_BLACK);
  Serial.println("done");
  //MainUI(pm2_5,t,h);
  
  //ControlSerial.begin(9600);
}
BLYNK_WRITE(V4){
  int state = param.asInt();
            if(state == 1){
          //digitalWrite(rstBoard,HIGH);
          Serial.println("111");
          PowerOn();
        }
        if (state == 0 && isOff == false){
          Serial.println("222");
          isOff == true;
          PowerOff();
        }
}
BLYNK_WRITE(V6){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();     
        currMode = 1;
        DisplayMode(currMode);
      }
    }
}
BLYNK_WRITE(V7){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis(); 
        currMode = 2;
        DisplayMode(currMode);
      } 
    }
}
BLYNK_WRITE(V8){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();      
        currMode = 3;
        DisplayMode(currMode);
      }
    }
}
BLYNK_WRITE(V10){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();      
        currMode = 4;
        DisplayMode(currMode);
        currMillis = millis();
      }
    }
}
BLYNK_WRITE(V11){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();      
        currMode = 5;
        DisplayMode(currMode);
      }
    }
}
BLYNK_WRITE(V12)
{
  //reads the slider value when it changes in the app
  if(currMode == 5){
      int SliderValue = param.asInt();
      
      manFanMode(SliderValue/20);
  }
  else{
    Blynk.virtualWrite(V12,0);
  }
}
BLYNK_WRITE(V15)
{
  //reads the slider value when it changes in the app
  //SliderValue = param.asInt();
}
BLYNK_WRITE(V13){
    if (param.asInt()){
      if(millis() - lastBtn > BtnTime){
        lastBtn = millis();
        BrightnessAdjust();
      }
    }
}
void loop() {
  //MenuBtn
  currentMillis = millis();    
  readButtonState();
  if(isOff != true){
  //PMS3003
    if(millis() - lastPM > PMTime){
    lastPM = millis();

    currentMillis = millis();    
    readButtonState();
    PMMeasure();
    if(currMode == 1){
    pmToFanMode();
    }
  }
  else{
    currentMillis = millis();    
    readButtonState();
  }

    //TFT
    if(millis() - lastDisplayTime > DisplayTime){
    lastDisplayTime = millis();

    currentMillis = millis();    
    readButtonState();
    MainUI(pm2_5,t,h);
  }
    else{
    currentMillis = millis();    
    readButtonState();
  }
  //DHT11
    if(millis() - lastTemp > TempTime){
    lastTemp = millis();
    
    currentMillis = millis();    
    readButtonState();
    TempandHumu();
  }
    else{
    currentMillis = millis();    
    readButtonState();
  }
  if(currMode == 4){
      currMillis = millis();
      if(currMillis - lastRapid > rapidTime){
      lastRapid = millis();
      currMode = 1;
      DisplayMode(currMode);
    }
  }
 }

}
// UI
void ShowLogo(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(70, 25, dydxLogo, 180, 180, ST77XX_WHITE);
  delay(1500);
  tft.fillScreen(ST77XX_BLACK);
}
void WelcomeSplash(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n\n\n");
  tft.setTextSize(1);
  tft.print("                ");
  tft.setTextSize(4);
  tft.println("Dy|Dx");
  tft.setTextSize(3);
  tft.println("\n   Air Purifier");
}
void ShowQR_APSetup(){
  tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.drawBitmap(60, 20, wifiQR, 200, 200, ST77XX_WHITE);

}
void WifiConnected (String ssid){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(3, 3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n\n\n");
  tft.setFont(&RobotoThin16pt7b);
  tft.setTextSize(0);
  tft.println("         Connected to");
  tft.setFont();
  tft.setTextSize(4);
  tft.print("\n  ");
  tft.setFont(&RobotoThin20pt7b);
  tft.setTextSize(0);
  tft.println(ssid);
  tft.setTextWrap(false);
}
void PowerOff (){
  tft.fillScreen(ST77XX_BLACK);
  ShutdownSound();
  tft.setCursor(90, 120);
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&RobotoThin20pt7b);
  tft.setTextSize(0);
  tft.println("See you!");
  tft.setFont();
  delay(1000);
  Brightness(0);
  Blynk.virtualWrite(V9,"---Power Off---");
  delay(500);
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8,0);
  Blynk.virtualWrite(V10,0);
  Blynk.virtualWrite(V11,0);
  Blynk.virtualWrite(V13,0);
  Blynk.virtualWrite(V14,0);
  tft.enableDisplay(false);
}
void PowerOn (){
  ESP.restart();
}
void MainUI_Border(uint16_t color){
  
  tft.fillRoundRect(100,190,120,15,7,color);
  //tft.fillRect(21,21,280,200,ST77XX_BLACK);
}
void MainUI(int pm2_5, int temp, int humi){
  DisplayPMColor();
  //tft.fillRect(21,21,279,199,ST77XX_BLACK);
  tft.fillRect(25,25,275,80,ST77XX_BLACK);
  tft.fillRect(60, 130,135,35,ST77XX_BLACK);
  //tft.fillRect(135,130,60,35,ST77XX_BLUE);
  

  
  tft.setCursor(21, 40);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(3);
  tft.print("\n");
  //tft.setTextSize(2);
  //tft.println("                 ");
  tft.setTextSize(2);
  tft.print("\n\n");
  if(isPMRead == false){
    tft.setTextSize(4);
    tft.print("    ");
    tft.setTextSize(1);
    tft.print("    ");
    tft.setTextSize(0);
    tft.setFont(&RobotoThin48pt7b);
    tft.println("---");
  }
  else{
    if(pm2_5 < 10){
    tft.setTextSize(4);
    tft.print("   ");
    tft.setTextSize(1);
    tft.print("  ");
    tft.setTextSize(0);
    tft.setFont(&RobotoThin48pt7b);
    tft.println("00" + String(pm2_5));
  }
  else if(pm2_5 >= 10 && pm2_5 < 100){
    tft.setTextSize(4);
    tft.print("   ");
    tft.setTextSize(1);
    tft.print("  ");
    tft.setTextSize(0);
    tft.setFont(&RobotoThin48pt7b);
    tft.println("0" + String(pm2_5));
  }
  else if(pm2_5 >= 100 && pm2_5 < 999){
    tft.setTextSize(4);
    tft.print("   ");
    tft.setTextSize(1);
    tft.print("  ");
    tft.setTextSize(0);
    tft.setFont(&RobotoThin48pt7b);
    tft.println(pm2_5);
  }
  else if (pm2_5 >= 1000){
    tft.setTextSize(4);
    tft.print("  ");
    tft.setTextSize(1);
    tft.print("  ");
    tft.setTextSize(0);
    tft.setFont(&RobotoThin48pt7b);
    tft.println(pm2_5);
  }
  }
  tft.setFont();
  tft.fillRoundRect(60, 117, 200, 2, 1, ST77XX_WHITE);
  tft.setCursor(53, 154);
  tft.setFont(&RobotoThin20pt7b);
  tft.setTextSize(0);
  if(isDHTRead == false){
    tft.setFont();
    tft.setTextSize(1);
    tft.print("   ");
    tft.setFont(&RobotoThin20pt7b);
    tft.setTextSize(0);
    tft.print("--");
  }
  else{
    tft.setFont();
    tft.setTextSize(1);
    tft.print(" ");
    tft.setFont(&RobotoThin20pt7b);
    tft.setTextSize(0);
    tft.print(temp);
  }
  tft.setFont(&RobotoThin14pt7b);
  tft.setTextSize(0);
  tft.print("C");


  if(isDHTRead == false){
    tft.setFont();
    tft.setTextSize(1);
    tft.print("     ");
    tft.setFont(&RobotoThin20pt7b);
    tft.setTextSize(0);
    tft.print("--");
  }
  else{
    tft.setTextSize(1);
    tft.print("  ");
    tft.setFont(&RobotoThin20pt7b);
    tft.setTextSize(0);
    tft.print(humi);
  }
  tft.setFont(&RobotoThin14pt7b);
  tft.print("%");
  tft.setFont();
  showModeIcon(currMode);
  if(WiFi.status()== WL_CONNECTED){
    ShowWifiIcon();
  }
  else{
    tft.fillRect(197,134,24,24,ST77XX_BLACK);
    Blynk.connect();
  }
}
void PMMeasure(){
  String input = Serial.readStringUntil('\n');
  if(input.startsWith("PM")){
    input.remove(0,2); //Remove word "PM"
    if(input == "Cannot find the data header."){
      isPMRead = false;
    }
    else{
      int con_pm2_5 = input.toInt();
      if(con_pm2_5 == -1){
        isPMRead = false;
      }
      else{
        pm2_5 = con_pm2_5;
        isPMRead = true;
      Blynk.virtualWrite(V1,pm2_5);
      }
    }
    //Serial.println(pm2_5);
    delay(250);
  }
    //isPMRead = false;
}
void TempandHumu(){
  delay(250);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    isDHTRead = false;
    return;
  }
  if((t >= -55 && t <= 55) && (h >= 0 && h <= 100)){
    isDHTRead = true;
    Blynk.virtualWrite(V2,t);
    Blynk.virtualWrite(V3,h);
  }
  else{
    isDHTRead = false;
  }
  /*
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.println(F("°F "));*/
}
void DisplayMode(int _mode){
  if(_mode == 1){
    isAuto == true;
    ShowAutoMode();
    pmToFanMode();
  }
  if(_mode == 2){
    isAuto == false;  
    ShowTurboMode();
    manFanMode(10); 
  }
  if(_mode == 3){
    isAuto == false;  
    ShowNightMode();
    manFanMode(0); 
  }
  if(_mode == 4){
    isAuto == false;   
    ShowRapidMode();
    manFanMode(11);
  }
  if(_mode == 5){ 
    isAuto == false;
    ShowManualMode();
  }
}
void DisplayPMColor(){
  if(pm2_5 <= 50){
    MainUI_Border(ST77XX_GREEN);
    Blynk.virtualWrite(V9,"Good");
  }
  else if(pm2_5 > 50 && pm2_5 <= 100){
    MainUI_Border(ST77XX_YELLOW);
    Blynk.virtualWrite(V9,"Moderate");
  }
  else if(pm2_5 > 100 && pm2_5 <= 150){
    MainUI_Border(ST77XX_ORANGE);
    Blynk.virtualWrite(V9,"Unhealthy");
  }
  else if(pm2_5 > 150 && pm2_5 <= 200){
    MainUI_Border(ST77XX_RED);
    Blynk.virtualWrite(V9,"Unhealthy");
  }
  else if(pm2_5 > 200 && pm2_5 <= 300){
    MainUI_Border(0xA819);
    Blynk.virtualWrite(V9,"Very Unhealthy");
  }
  else if(pm2_5 > 300){
    MainUI_Border(0x6803);
    Blynk.virtualWrite(V9,"Hazardous");
  }
}
void ShowAutoMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(91, 51, auto128px, 128, 128, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  Blynk.virtualWrite(V6,1);
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8,0);
  Blynk.virtualWrite(V10,0);
  Blynk.virtualWrite(V11,0);
  Brightness(6);
}
void ShowTurboMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(91, 51, turbo128px, 128, 128, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,1);
  Blynk.virtualWrite(V8,0);
  Blynk.virtualWrite(V10,0);
  Blynk.virtualWrite(V11,0);
  Brightness(6);
}
void ShowNightMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(91, 51, night128px, 128, 128, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8,1);
  Blynk.virtualWrite(V10,0);
  Blynk.virtualWrite(V11,0);
  Brightness(1);
}
void ShowRapidMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(91, 51, rapid128px, 128, 128, ST77XX_CYAN);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8,0);
  Blynk.virtualWrite(V10,1);
  Blynk.virtualWrite(V11,0);
  Brightness(6);
}
void ShowManualMode(){
  tft.fillScreen(ST77XX_BLACK);
  tft.drawBitmap(91, 51, manual128px, 128, 128, ST77XX_CYAN);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  Blynk.virtualWrite(V6,0);
  Blynk.virtualWrite(V7,0);
  Blynk.virtualWrite(V8,0);
  Blynk.virtualWrite(V10,0);
  Blynk.virtualWrite(V11,1);
  Brightness(6);
}
void showModeIcon(int _mode){
  if(_mode == 1){
    //tft.drawBitmap(206, 161, autoMode, 24, 24, ST77XX_WHITE);
    tft.drawBitmap(228, 130, auto32px, 32, 32, ST77XX_WHITE);
  }
  else if(_mode == 2){
    //tft.drawBitmap(207, 159, turboMode, 24, 24, ST77XX_WHITE);
    tft.drawBitmap(228, 130, turbo32px, 32, 32, ST77XX_WHITE);
  }
  else if(_mode == 3){
    //tft.drawBitmap(208, 161, nightMode, 20, 20, ST77XX_WHITE);
    tft.drawBitmap(228, 130, night32px, 32, 32, ST77XX_WHITE);
  }
  else if(_mode == 4){
    tft.drawBitmap(228, 130, rapid32px, 32, 32, ST77XX_CYAN);
  }
  else if(_mode == 5){
    tft.drawBitmap(228, 130, manual32px, 32, 32, ST77XX_WHITE);
  }
}
void ShowWifiIcon(){
  tft.drawBitmap(197, 134, wifi24px, 24, 24, ST77XX_WHITE);
  //tft.drawBitmap(235, 161, wifiOn, 22, 22, ST77XX_WHITE);
}
//End UI
//Back-end
void pmToFanMode(){
  if((pmMode != lastpmMode) && isAuto == true){
    for(int i = 0; i < 2; i++){
    Serial.print("FANA");
    if(pm2_5 <= 20){
      fanSpeed = 10; pmMode = 1; Serial.println(101); delay(250);
    }
    else if(pm2_5 > 20 && pm2_5 <= 40){
      fanSpeed = 20; pmMode = 2; Serial.println(102); delay(250);
    }
    else if(pm2_5 > 40 && pm2_5 <= 60){
      fanSpeed = 30; pmMode = 3; Serial.println(103); delay(250);
    }
    else if(pm2_5 > 60 && pm2_5 <= 80){
      fanSpeed = 40; pmMode = 4; Serial.println(104); delay(250);
    }
    else if(pm2_5 > 80 && pm2_5 <= 100){
      fanSpeed = 50; pmMode = 5; Serial.println(105); delay(250);
    }
    else if(pm2_5 > 100 && pm2_5 <= 120){
      fanSpeed = 60; pmMode = 6; Serial.println(106); delay(250);
    }
    else if(pm2_5 > 120 && pm2_5 <= 140){
      fanSpeed = 70; pmMode = 7; Serial.println(107); delay(250);
    }
    else if(pm2_5 > 140 && pm2_5 <= 160){
      fanSpeed = 80; pmMode = 8; Serial.println(108); delay(250);
    }
    else if(pm2_5 > 160 && pm2_5 <= 180){
      fanSpeed = 90; pmMode = 9; Serial.println(109); delay(250);
    }
    else if(pm2_5 > 180){
      fanSpeed = 100; pmMode = 10; Serial.println(110); delay(250);
    }
  lastpmMode = pmMode; 
  Blynk.virtualWrite(V5,fanSpeed);
  delay(50);
  }}

}
void manFanMode(int _mode){
  for(int i = 0; i < 2; i++){
  Serial.print("FAN");
  switch(_mode){
    case 0: fanSpeed = 5; lastpmMode = 0; Serial.println(100); delay(250);  break;
    case 1: fanSpeed = 10; lastpmMode = 1; Serial.println(101); delay(250);  break;
    case 2: fanSpeed = 20; lastpmMode = 2; Serial.println(102); delay(250); break;
    case 3: fanSpeed = 30; lastpmMode = 3; Serial.println(103); delay(250); break;
    case 4: fanSpeed = 40; lastpmMode = 4; Serial.println(104); delay(250); break;
    case 5: fanSpeed = 50; lastpmMode = 5; Serial.println(105); delay(250); break;
    case 6: fanSpeed = 60; lastpmMode = 6; Serial.println(106); delay(250); break;
    case 7: fanSpeed = 70; lastpmMode = 7; Serial.println(107); delay(250); break;
    case 8: fanSpeed = 80; lastpmMode = 8; Serial.println(108); delay(250); break;
    case 9: fanSpeed = 90; lastpmMode = 9; Serial.println(109); delay(250); break;
    case 10: fanSpeed = 100; lastpmMode = 10; Serial.println(110); delay(250); break;
    case 11: fanSpeed = 150; lastpmMode = 10; Serial.println(111); delay(250); break;
  }
  Blynk.virtualWrite(V5,fanSpeed);
  delay(50);
}}
void readButtonState() {

  // If the difference in time between the previous reading is larger than intervalButton
  if(currentMillis - previousButtonMillis > intervalButton) {
    
    // Read the digital value of the button (LOW/HIGH)
    int buttonState = digitalRead(menuBtn);    

    // If the button has been pushed AND
    // If the button wasn't pressed before AND
    // IF there was not already a measurement running to determine how long the button has been pressed
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
      //Serial.println("Button pressed");
    }

    // Calculate how long the button has been pressed
    buttonPressDuration = currentMillis - buttonLongPressMillis;

    // If the button is pressed AND
    // If there is no measurement running to determine how long the button is pressed AND
    // If the time the button has been pressed is larger or equal to the time needed for a long press
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
      buttonStateLongPress = true;
      if(isOff == false){
        isOff = true;
        PowerOff();
      }
      else if(isOff == true){
        PowerOn();
      }
      //Serial.println("Button long pressed");
    }
      
    // If the button is released AND
    // If the button was pressed before
    if (buttonState == LOW && buttonStatePrevious == HIGH) {
      buttonStatePrevious = LOW;
      buttonStateLongPress = false;
      //Serial.println("Button released");

      // If there is no measurement running to determine how long the button was pressed AND
      // If the time the button has been pressed is smaller than the minimal time needed for a long press
      // Note: The video shows:
      //       if (!buttonStateLongPress && buttonPressDuration < minButtonLongPressDuration) {
      //       since buttonStateLongPress is set to FALSE on line 75, !buttonStateLongPress is always TRUE
      //       and can be removed.
      if (buttonPressDuration < minButtonLongPressDuration && isOff == false) {
        if(currMode < 3){
          currMode++;
        }
        else{
          currMode = 1;
        }        
        digitalWrite(buzzerPin,HIGH);
        delay(250);
        digitalWrite(buzzerPin,LOW);
        DisplayMode(currMode);
        //Serial.println("Button pressed shortly");
      }
    }
    
    // store the current timestamp in previousButtonMillis
    previousButtonMillis = currentMillis;

  }

}
void Brightness(int _mode){
  for(int i = 0; i < 2; i++){
    Serial.print("BRI");
  switch(_mode){
    case 0: Serial.println(0); delay(250); break;
    case 1: Serial.println(1); delay(250); break;
    case 2: Serial.println(2); delay(250); break;
    case 3: Serial.println(3); delay(250); break;
    case 4: Serial.println(4); delay(250); break;
    case 5: Serial.println(5); delay(250); break;
    case 6: Serial.println(6); delay(250); break;
  }
  delay(250);
  }
}
void BrightnessAdjust(){
  if(brightState == 1){
    brightState++;
    Brightness(3);
  }
  else if(brightState == 2){
    brightState++;
    Brightness(1);
  }
  else if(brightState == 3){
    Brightness(6);
  }
  if(brightState == 3){
    brightState = 1;
  }
}
void WelcomeSound() {

  // iterate over the notes of the melody:

  for (int thisNote = 0; thisNote < 3; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.

    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

    int noteDuration = 800 / noteDurations[thisNote];

    tone(buzzerPin, turnOnmelody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.

    // the note's duration + 30% seems to work well:

    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);

    // stop the tone playing:

    noTone(buzzerPin);

  }
}
void ShutdownSound() {

  // iterate over the notes of the melody:

  for (int thisNote = 0; thisNote < 3; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.

    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

    int noteDuration = 700 / noteDurations[thisNote];

    tone(buzzerPin, turnOffmelody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.

    // the note's duration + 30% seems to work well:

    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);

    // stop the tone playing:

    noTone(buzzerPin);

  }
}
//End Back-end
