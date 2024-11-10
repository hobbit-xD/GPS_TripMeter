/*

DISPLAY WIRING:
CLK - 9
DIO - 10
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
#include <SPI.h>
#include <TM1637Display.h>

#define CLK 9
#define DIO 10

SoftwareSerial ss(6, 7);  // RX, TX

// The TinyGPSPlus object
TinyGPSPlus gps;

TM1637Display display(CLK, DIO);

double PreviousLAT;
double PreviousLONG;
double Accumulator = 0.0;
bool PreviousLocationHasBeenSet = false;
double TotalDistanceTraveled = 0.0;

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};

const int buttonPin = 2;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  // put your setup code here, to run once:
  Serial.begin(115200);
  ss.begin(9600);
  display.clear();
  display.setBrightness(7);  // set the brightness to 7 (0:dimmest, 7:brightest)

  do {
    Serial.println("Setup");
    smartDelay(1000);

    if (gps.satellites.isValid()) {
      Serial.println(gps.satellites.value());
      display.showNumberDec(gps.satellites.value(), false);
    }

  } while ((gps.satellites.value() < 5));

  display.setSegments(SEG_DONE);
  smartDelay(1000);
  display.clear();
}

void loop() {

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == LOW) {
    // reset distance:
    TotalDistanceTraveled = 0;
  }

  // put your main code here, to run repeatedly:

  if (gps.satellites.isValid()) {
    Serial.println(gps.satellites.value());
    //drawCentreString(String(gps.satellites.value()), 0, 10, 1, ST7735_WHITE);
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
    //drawCentreString("Distance(m): ", 0, 32, 2, ST7735_WHITE);
    //drawCentreString(String(TotalDistanceTraveled, 0), 0, 64, 4, ST7735_WHITE);
    display.showNumberDec(TotalDistanceTraveled, false);
  } else {
    //drawCentreString("Distance(Km): ", 0, 32, 2, ST7735_WHITE);
    //drawCentreString(String(TotalDistanceTraveled / 1000, 2), 0, 64, 4, ST7735_WHITE);

    display.showNumberDecEx(TotalDistanceTraveled / 10, 0b01000000, false);
  }

  // drawCentreString("Speed(Km/h): ", 0, 96, 2, ST7735_WHITE);
  // drawCentreString(String(gps.speed.kmph(), 2), 0, 118, 2, ST7735_WHITE);
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

// void drawCentreString(const String &buf, int x, int y, uint8_t fontSize, uint16_t color) {
//   int16_t x1, y1;
//   uint16_t w, h;

//   tft.setTextWrap(false);
//   tft.setTextSize(fontSize);
//   tft.setTextColor(color);
//   tft.getTextBounds(buf, x, y, &x1, &y1, &w, &h);  //calc width of new string
//   tft.setCursor(x, (y - h / 2));
//   tft.print(buf);
// }
