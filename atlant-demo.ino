#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <LiquidCrystal.h>
#include <LcdBarGraph.h>
/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

#define ledpin 10
#define window_size 15
#define button 9


byte lcdNumCols = 16;
String in_chars = "";
String Quality = "Good";
int index = 0;
float READ_c[window_size];
float C_avg = 0;
float C_sum = 0;
float C_base = 0.3;
float C_bar = 0;
float a = 0;
float cons = 100-(100/0.5);

//*******************LCD Initialisation**********************

const int rs = 11, en = 12, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LcdBarGraph lbg(&lcd, lcdNumCols,0,1);

//***********************************************************
void setup(void) {
  Serial.begin(9600);

  lcd.begin(16, 2);
  pinMode(ledpin,OUTPUT);
  pinMode(button, INPUT_PULLUP);
  
  digitalWrite(ledpin,HIGH);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  delay(100);
}



void loop(void) {
  uint16_t r, g, b, c, colorTemp, lux;
  
  //*******************RGB Value*****************************
  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  //*******************Serial Input**************************
  char in_char = ' ';

  while (Serial.available()){
    in_char = Serial.read();
    if (int(in_char)!=-1){
      in_chars+=in_char;
    }
  }
  if (in_char=='\n'){
    Serial.print(in_chars);
    String command = String(in_chars);
    command.trim();
    
    if (command == "on"){
      digitalWrite(ledpin,HIGH);
      delay(200);
    }
    if(command == "off"){
      digitalWrite(ledpin,LOW);
      delay(200);
    }
    in_chars = "";
    command = "";
  }


  if(digitalRead(button) == LOW){
    C_base = C_avg;
  }

  //********************RGB to GreyScale**********************
  float C_srgb = 0;
  float RED = float(r)/25000;
  float GREEN = float(g)/25000;
  float BLUE = float(b)/25000;

  float C_lin = 0.2126*RED+0.7152*GREEN+0.0722*BLUE;
  

  if (C_srgb <= 0.0031308){
    C_srgb = 12.92 * C_lin;
  }
  else {
    float C_pow = pow(C_lin,1/2.4);
    C_srgb = 1.055*C_pow - 0.055;
  }

  C_sum = C_sum - READ_c[index];
  READ_c[index]= C_srgb;
  C_sum = C_sum + C_srgb;
  index = (index+1) % window_size;
  
  C_avg = C_sum / window_size;


  a = 100/(0.5*C_base);

  C_bar = a*C_avg + cons;

  if(C_bar<0){
    C_bar = 0;
  }
  if(C_bar>100){
    C_bar = 100;
  }
 


  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("GreyScale: "); Serial.print(C_avg, DEC); Serial.print(" ");
  Serial.print("C_bar: "); Serial.print(C_bar, DEC); Serial.print(" ");
  Serial.print("a: "); Serial.print(a, DEC); Serial.print(" ");
    Serial.print("b: "); Serial.print(b, DEC); Serial.print(" ");

  Serial.println(" ");

  lcd.setCursor(0,0);
  lcd.print("Absorbtion:");lcd.print(C_avg,3);
  lcd.setCursor(0, 1);
  lbg.drawValue(int(C_bar),100);

  
  delay(100);
  
  //********************RGB to GreyScale**********************
  

  
}
