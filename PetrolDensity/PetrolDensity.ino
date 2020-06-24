/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SENSOR_PIN_1 1
#define SENSOR_PIN_2 2
#define BUTTON_PIN 1

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



int ref = 0;
int pressure = 0;

float coeff = 0;
float density = 0;

float petrolDensityLower = 0.715;
float petrolDensityUpper = 0.78;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

   // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println("Density Measurement:");
  display.println("Calibration (Fill with deionised water)");

  display.display();

  ref = analogRead(SENSOR_PIN_1);

  //Density = PressureDelta * coefficient

  coeff = ref / 1; //Density of deionised water = 1g/ml
  
  display.clearDisplay();
  display.setCursor(0, 0);   
  display.println("Density Measurement:");
  display.println("Calibrated!");
  display.display();
  delay(2000);
}

void loop() {

  display.clearDisplay();
  display.setCursor(0, 0);  
  display.println("Density Measurement:");
  display.println("Press Button to begin measurement");
  display.display();

  while(digitalRead(BUTTON_PIN) == HIGH){}

  pressure = analogRead(SENSOR_PIN_1);

  density = pressure * coeff;
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Density Measurement:");
  display.print(density);
  display.println("g/ml");
  if(density >= petrolDensityLower && density <= petrolDensityUpper){
    display.println("Petrol Good");
  }
  else{
    display.println("Petrol Bad");
  }
  display.display();

  delay(5000);

  
}
