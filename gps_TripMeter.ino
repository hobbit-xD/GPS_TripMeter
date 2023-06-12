/*

DISPLAY WIRING:
LED - 3.3v
SCK - D13
SDA - D11
DC/A0 - D9
Reset - D8
CS - D10
GND - GND
VCC - 5v

NEO-GPS WIRING
TX - D6
RX - D7
5V - 5V
GND - GND

BUTTON WIRING
D2

*/


#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>

// pin definition for the Uno
#define TFT_CS 10
#define TFT_RST 8  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 9

SoftwareSerial ss(6, 7);  // RX, TX

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// The TinyGPSPlus object
TinyGPSPlus gps;

double PreviousLAT;
double PreviousLONG;
double Accumulator = 0.0;
bool PreviousLocationHasBeenSet = false;
double TotalDistanceTraveled = 0.0;

const int buttonPin = 2;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);
  ss.begin(9600);
  tft.initR(INITR_18GREENTAB);  // Init ST7735R chip, green tab
  tft.setRotation(1);

  // clear the screen with a black background
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  do {
    Serial.println("Setup");
    smartDelay(1000);

    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);

    if (gps.satellites.isValid()) {
      Serial.println(gps.satellites.value());
      tft.print(gps.satellites.value());
    }

  } while ((gps.satellites.value() < 5));
}

void loop() {

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // reset distance:
    TotalDistanceTraveled = 0;
  }

  // put your main code here, to run repeatedly:
  tft.fillScreen(ST77XX_BLACK);

  if (gps.satellites.isValid()) {
    Serial.println(gps.satellites.value());
    drawCentreString(String(gps.satellites.value()), 0, 10, 1, ST7735_WHITE);
  }

  if (gps.location.isValid() == 1 && gps.location.isUpdated() == 1 && int(gps.speed.kmph()) > 5) {

    double currentLAT = gps.location.lat();
    double currentLONG = gps.location.lng();

    if (!PreviousLocationHasBeenSet) {
      PreviousLAT = currentLAT;
      PreviousLONG = currentLONG;
      PreviousLocationHasBeenSet = true;
    }

    double distanceMeters = TinyGPSPlus::distanceBetween(
      PreviousLAT, PreviousLONG,
      currentLAT, currentLONG);

    Serial.print("Distance (m): ");
    Serial.println(distanceMeters);
    Serial.print("Speed: ");
    Serial.println(int(gps.speed.kmph()));
    Serial.print("Age: ");
    Serial.println(gps.location.age());

    PreviousLAT = currentLAT;
    PreviousLONG = currentLONG;

    if (distanceMeters >= 1)
      TotalDistanceTraveled += distanceMeters;
  }

  Serial.println(TotalDistanceTraveled, 3);

  if (TotalDistanceTraveled <= 1000) {
    drawCentreString("Distance(m): ", 0, 32, 2, ST7735_WHITE);
    drawCentreString(String(TotalDistanceTraveled, 0), 0, 64, 4, ST7735_WHITE);
  } else {
    drawCentreString("Distance(Km): ", 0, 32, 2, ST7735_WHITE);
    drawCentreString(String(TotalDistanceTraveled / 1000, 2), 0, 64, 4, ST7735_WHITE);
  }

  drawCentreString("Speed(Km/h): ", 0, 96, 2, ST7735_WHITE);
  drawCentreString(String(gps.speed.kmph(), 2), 0, 118, 2, ST7735_WHITE);
  smartDelay(1000);
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void drawCentreString(const String &buf, int x, int y, uint8_t fontSize, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;

  tft.setTextWrap(false);
  tft.setTextSize(fontSize);
  tft.setTextColor(color);
  tft.getTextBounds(buf, x, y, &x1, &y1, &w, &h);  //calc width of new string
  tft.setCursor(x, (y - h / 2));
  tft.print(buf);
}
