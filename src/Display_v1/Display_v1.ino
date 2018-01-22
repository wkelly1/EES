// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// This demo code returns raw readings, public domain

#include <stdint.h>
#include "TouchScreen.h"
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>


// These are the pins for the shield!
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

//colour definitions for tft display
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

//pinout for relay
#define relay1 23
#define relay2 24
#define relay3 25
#define relay4 26



// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

int page = 0;
int drawnHomeScreen = 0;
int currentPosition = 0;
int frequency = 1; //in Hz
int time = 0;

int buttonSpacing = 30;
int numSections = 3;
int sectionHeight = (250 - ((numSections + 1) * buttonSpacing)) / numSections;
int position1 = 70 + buttonSpacing;
int position2 = position1 + sectionHeight + buttonSpacing;
int position3 = position2 + sectionHeight + buttonSpacing;

void setup() {

  Serial.begin(9600);
  tft.reset();

  /*---------Finds LCD driver--------------------*/
  uint16_t identifier = tft.readID();
  Serial.println(identifier);
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  }
  else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  }
  else if (identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }
  else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  }
  else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  }
  else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  }
  else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  }
  else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0x9341;

  }
  /*---------------------------------------------*/

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);


  tft.begin(identifier);
  tft.setRotation(2);
  Serial.println("READY!");
  drawHomeScreen();

  Serial.println(tft.width());
  Serial.println(tft.height());

}

boolean updateReady = false;

void loop() {

  // a point object holds x y and z coordinates
  
  TSPoint p = ts.getPoint();
  

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

    Serial.print("X = ");
    Serial.print(p.x);
    Serial.print("\tY = ");
    Serial.print(p.y);
    //Serial.print("\tPressure = "); Serial.println(p.z);
    Serial.println(".");


    if (p.x > 170 & p.x < 220 & p.y > 180 & p.y < 200) {
      Serial.println("Top + button activated");
      frequency = frequency + 1;
      updateReady = true;



      delay(500);
    }

    if (p.x > 130 & p.x < 230 & p.y > 40 & p.y < 60) {
      Serial.println("Go activated");
      runCycle(frequency);

      delay(500);

    }
  }

  Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
  if (updateReady == true) {
    tft.fillScreen(BLACK);
    tft.setTextColor(RED);
    tft.setTextSize(2);
    tft.setCursor(10, 10);

    tft.println("hello");
    Serial.println(frequency);
    delay(1000);
    updateReady = false;
  }
}

void drawHomeScreen() {

  tft.fillScreen(BLACK);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Jouls Thompson");
  tft.setCursor(10, 40);
  tft.println("Cryocooler Test");



  tft.setTextColor(WHITE);
  tft.setCursor(10, position1 - buttonSpacing + 10);
  tft.println("Frequency");
  //defining box
  tft.drawRect(10, position1, tft.width() - 20, sectionHeight, RED);
  tft.fillRect(10, position1, tft.width() - 20, sectionHeight, BLUE);

  //Buttons top
  tft.drawRect(10, position1, 60, sectionHeight, GREEN);//Green inner box left
  tft.fillRect(10, position1, 60, sectionHeight, GREEN);//Green inner box left
  tft.drawRect(tft.width() - 70, position1, 60, sectionHeight, GREEN);
  tft.fillRect(tft.width() - 70, position1, 60, sectionHeight, GREEN);

  tft.setCursor(10, position2 - buttonSpacing + 10);
  tft.println("Cycle Time");

  //defining box
  tft.drawRect(10, position2, tft.width() - 20, sectionHeight, BLUE);
  tft.fillRect(10, position2, tft.width() - 20, sectionHeight, BLUE);

  //Buttons 2nd top
  tft.drawRect(10, position2, 60, sectionHeight, GREEN);
  tft.fillRect(10, position2, 60, sectionHeight, GREEN);
  tft.drawRect(tft.width() - 70, position2, 60, sectionHeight, GREEN);
  tft.fillRect(tft.width() - 70, position2, 60, sectionHeight, GREEN);


  //Buttons 3rd top
  tft.drawRect(10, position3, 100, sectionHeight, RED);
  tft.fillRect(10, position3, 100, sectionHeight, RED);
  tft.drawRect(tft.width() - 110, position3, 100, sectionHeight, GREEN);
  tft.fillRect(tft.width() - 110, position3, 100, sectionHeight, GREEN);

  tft.setCursor((tft.width() / 2) - 10, position1 + ((sectionHeight / 2) ));
  tft.println(frequency);


  //Button text
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.setCursor(30, position1 + ((sectionHeight / 2) - 15));
  tft.println("-");

  tft.setCursor(30, position2 + ((sectionHeight / 2) - 15));
  tft.println("-");

  tft.setCursor(190, position1 + ((sectionHeight / 2) - 15));
  tft.println("+");

  tft.setCursor(190, position2 + ((sectionHeight / 2) - 15));
  tft.println("+");


  tft.setTextSize(2);
  tft.setCursor(30, position3 + ((sectionHeight / 2) - 8));
  tft.println("STOP");

  tft.setCursor(170, position3 + ((sectionHeight / 2) - 8));
  tft.println("GO");
}

void runCycle(int frequency) {
  int startTime = 0;
  int endTime = 0;
  int currentTime = 0;
  unsigned long measureStartTime = 0;
  unsigned long measureEndTime = 0;
  int emergancyStop = 0;
  startTime = millis();
  endTime = startTime + 5000;
  Serial.print("Running Cycle");
  while (currentTime < endTime || emergancyStop == 1 ) {

    measureStartTime = millis();

    //Get touch screen point
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));

      digitalWrite(relay1, HIGH);
      digitalWrite(relay2, LOW);
      digitalWrite(relay3, HIGH);
      digitalWrite(relay4, LOW);
      //1hz or 1 cycle a second requires 0.5 second of delay after each switch
      delay(frequency * 1000);
      digitalWrite(relay1, LOW);
      digitalWrite(relay2, HIGH);
      digitalWrite(relay3, LOW);
      digitalWrite(relay4, HIGH);
      measureEndTime = millis();
      delay(frequency * 1000);
      Serial.println(measureEndTime - measureStartTime);

      if (p.x > 6 & p.x < 105 & p.y > 25 & p.y < 75) {
        emergancyStop = 1;
        Serial.println("Emergancy Stop Activated");
        tft.setTextColor(RED);
        tft.println("Emergancy Stop Activated");

      }
    }
    currentTime = millis();

  }
}




















