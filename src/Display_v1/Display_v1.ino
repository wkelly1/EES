
// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// This demo code returns raw readings, public domain

//including library
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <stdint.h>

#include <TouchScreen.h>
//#include <Elegoo_GFX.h>
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
int cycleTime = 10; //in seconds
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
boolean frequencyUpdate = false;
boolean cycleTimeUpdate = false;

void loop() {

  // a point object holds x y and z coordinates
  //TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
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

    //Plus button touch settings top
    if (p.x > 170 & p.x < 220 & p.y > 180 & p.y < 200) {
      Serial.println("Top + button activated");
      frequency = frequency + 1;
      // frequencyUpdate = true;
      updateReady = true;
    }

    //Minus button touch settings top
    if (p.x > 5 & p.x < 75 & p.y > 169 & p.y < 220) {
      Serial.println("Top - button activated");
      //cant have less that 1 frequency
      if (frequency > 1) {
        frequency = frequency - 1;
        //frequencyUpdate = true;
        updateReady = true;
      }
      else {
        Serial.println("Can't have a frequency less that one");
      }

    }

    //Plus button touch settings bottom (cycle time increase)
    if (p.x > 170 & p.x < 230 & p.y > 100 & p.y < 160) {
      Serial.println("Bottom + button activated");
      //cant have less than 1 cycle time
      if (cycleTime > 1) {
        cycleTime = cycleTime + 1;
        // cycleTimeUpdate = true;
        updateReady = true;
      }
      else {
        Serial.println("Can't have a cycle time less that one");
      }

    }


    //Minus button touch settings bottom (cycle time increase)
    if (p.x > 15 & p.x < 80 & p.y > 80 & p.y < 163) {
      Serial.println("Bottom - button activated");
      //cant have less than 1 cycle time
      if (cycleTime > 1) {
        cycleTime = cycleTime - 1;
        updateReady = true;
      }
      else {
        Serial.println("Can't have a cycle time less that one");
      }

    }




    //Go Button
    if (p.x > 130 & p.x < 230 & p.y > 40 & p.y < 60) {
      Serial.println("Go activated");
      runCycle(frequency, cycleTime);


    }
    returnTFTpins(); //Fixes updating issue with shared touch and TFT pin
  }


  //Start of screen updating
  updateScreen(frequencyUpdate, cycleTimeUpdate);

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
  tft.println("Frequency (Hz)");
  //defining box
  tft.drawRect(10, position1, tft.width() - 20, sectionHeight, RED);
  tft.fillRect(10, position1, tft.width() - 20, sectionHeight, BLUE);

  //Buttons to
  tft.drawRect(10, position1, 60, sectionHeight, GREEN);//Green inner box left
  tft.fillRect(10, position1, 60, sectionHeight, GREEN);//Green inner box left
  tft.drawRect(tft.width() - 70, position1, 60, sectionHeight, GREEN);
  tft.fillRect(tft.width() - 70, position1, 60, sectionHeight, GREEN);

  tft.setCursor(10, position2 - buttonSpacing + 10);
  tft.println("Cycle Time (s)");

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

  tft.setTextSize(3);
  tft.setCursor((tft.width() / 2) - 10, 110 );
  tft.println(frequency);

  tft.setCursor((tft.width() / 2) - 20, 185);
  tft.println(cycleTime);


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

void runCycle(int frequency, int cycleTime) {
  unsigned long startTime = 0;
  int endTime = 0;
  int currentTime = 0;
  unsigned long measureStartTime = 0;
  unsigned long measureEndTime = 0;
  int emergancyStop = 0;
  startTime = millis();
  endTime = startTime + (cycleTime * 1000);
  Serial.println("Running Cycle");
  Serial.println("Frequency of cycle = ");
  Serial.println(frequency);
  Serial.println("Cycle time of cycle = ");
  Serial.println(cycleTime);

  while (currentTime < endTime && emergancyStop == 0 ) {
    //Get touch screen point
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    Serial.println(p.x);
    digitalWrite(13, LOW);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE || emergancyStop == 0) {

      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
      Serial.println("Looped");
      // measureStartTime = millis();
      digitalWrite(relay1, HIGH);
      digitalWrite(relay2, LOW);
      digitalWrite(relay3, HIGH);
      digitalWrite(relay4, LOW);
      //1hz or 1 cycle a second requires 0.5 second of delay after each switch
      delay(500 / (frequency));
      digitalWrite(relay1, LOW);
      digitalWrite(relay2, HIGH);
      digitalWrite(relay3, LOW);
      digitalWrite(relay4, HIGH);
      delay(500 / (frequency));

      if (p.x > 6 & p.x < 105 & p.y > 25 & p.y < 75) {
        emergancyStop = 1;
        returnTFTpins();
        Serial.println("Emergancy Stop Activated");
        tft.setTextColor(RED);
        tft.setCursor(20, 150);
        tft.fillRect(0, 150 - 20, tft.width(), 100, WHITE);
        tft.println("Emergancy Stop");
        tft.println("Activated");
        delay(3000);
        drawHomeScreen();

      }
    }
    currentTime = millis();

  }
  Serial.println("Completed cycle");
}

void returnTFTpins() {

  pinMode(XM, OUTPUT);
  digitalWrite(XM, LOW);
  pinMode(YP, OUTPUT);
  digitalWrite(YP, LOW);
  pinMode(YM, OUTPUT);
  digitalWrite(YM, LOW);
  pinMode(XP, OUTPUT);
  digitalWrite(XP, HIGH);

}


void updateScreen(boolean frequencyUpdate, boolean cycleTimeUpdate) {
  if (updateReady == true) {
    tft.setTextColor(WHITE);
    tft.setTextSize(3);

    if (frequency < 10) {
      tft.setCursor((tft.width() / 2) - 10, 110 );
    }
    else {
      tft.setCursor((tft.width() / 2) - 20, 110);
    }
    tft.drawRect(tft.width() / 2 - 40, position1 + 10, 80, 30 , BLUE);
    tft.fillRect(tft.width() / 2 - 40, position1 + 10, 80, 30 , BLUE);
    tft.println(frequency);
    Serial.println(frequency);

    //Slows down update
    delay(50);

    if (cycleTime < 10) {
      tft.setCursor((tft.width() / 2) - 10, 185 );
    }
    else {
      tft.setCursor((tft.width() / 2) - 20, 185);
    }
    tft.drawRect(tft.width() / 2 - 40, position2 + 10, 80, 30 , BLUE);
    tft.fillRect(tft.width() / 2 - 40, position2 + 10, 80, 30 , BLUE);
    tft.println(cycleTime);
    Serial.println(cycleTime);

    //Slows down update
    delay(50);

  }
  updateReady = false;
}
















