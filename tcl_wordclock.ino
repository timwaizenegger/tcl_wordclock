#include <Arduino.h>

/*
   WORDCLOCK v2
   Tim Waizenegger (c) 2015

   last change: 11.10.2018
     - re-enable effects
     - full KSD

   last change: 08.05.2020
     - automatic summer/winter time switch
     - disable timeadjust
*/



#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "Sodaq_DS3231.h"


/*
  Buttons bei Anjas Uhr: a,3 b,2
  Buttons bei nr 3 Uhr: a,3 b,4
*/
#define PIN_BUTTON_A 3
#define PIN_BUTTON_B 2
volatile bool interruptEventAHappened = false;
volatile bool interruptEventBHappened = false;
/////////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////////

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(11, 11, 6,
                            NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  0, matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 100, 255), matrix.Color(100, 255, 0), matrix.Color(100, 0, 255), 0
};



/////////////////////////////////////////////////////////////////////////////////////
// Colors and Effects
/////////////////////////////////////////////////////////////////////////////////////

#define NUMBEROFCLOCKFACECOLORS 7
uint16_t allClockFaceColors[][4] = {
  {matrix.Color(200, 100, 0), matrix.Color(200, 150, 0), matrix.Color(150, 200, 0), matrix.Color(100, 200, 0)},
  {matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255)},
  {matrix.Color(100, 255, 200), matrix.Color(100, 255, 200), matrix.Color(100, 255, 200), matrix.Color(100, 255, 200)},
  {matrix.Color(0, 100, 200), matrix.Color(0, 150, 200), matrix.Color(0, 200, 150), matrix.Color(0, 200, 100)},
  {matrix.Color(255, 100, 255), matrix.Color(200, 100, 255), matrix.Color(200, 100, 255), matrix.Color(255, 100, 255)},
  {matrix.Color(255, 100, 255), matrix.Color(180, 255, 100), matrix.Color(200, 100, 255), matrix.Color(180, 255, 100)},
  {matrix.Color(255, 255, 255), matrix.Color(255, 255, 255), matrix.Color(255, 255, 255), matrix.Color(255, 255, 255)}


};

char currentClockFaceColorsPos = 0;
uint16_t* currentClockFaceColors = allClockFaceColors[currentClockFaceColorsPos];

void changeClockFaceColors() {
  if (++currentClockFaceColorsPos >= NUMBEROFCLOCKFACECOLORS) currentClockFaceColorsPos = 0;
  currentClockFaceColors = allClockFaceColors[currentClockFaceColorsPos];
}





void drawFlasher1() {
  matrix.fillScreen(colors[1]);
  matrix.show();
  delay(100);
  matrix.fillScreen(colors[2]);
  matrix.show();
  delay(100);
  matrix.fillScreen(colors[3]);
  matrix.show();
  delay(100);
  matrix.fillScreen(colors[0]);
  matrix.show();
  delay(10);
}

void drawFlasher2() {
  matrix.fillScreen(Wheel(random(0, 255)));
  matrix.show();
  delay(100);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return matrix.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return matrix.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return matrix.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
  uint16_t j, x, y;

  for (j = 0; j < 256; j++) {
    for (x = 0; x < matrix.width(); x++) {
      if (interruptEventAHappened || interruptEventBHappened) return;
      for (y = 0; y < matrix.height(); y++) {
        matrix.drawPixel(x, y, Wheel((j + x + y) & 255));
      }
    }
    matrix.show();
    delay(wait);
  }
}

void drawPatt1() {
  matrix.fillScreen(0);
  matrix.fillRect(0, 0, 4, 10, matrix.Color(255, 0, 0));
  matrix.fillRect(4, 0, 3, 10, matrix.Color(255, 255, 255));
  matrix.fillRect(7, 0, 4, 10, matrix.Color(0, 255, 0));
  matrix.show();
}
void drawPatt2() {
  matrix.fillScreen(0);
  matrix.fillRect(0, 0, 11, 3, matrix.Color(0, 0, 0));
  matrix.fillRect(0, 3, 11, 4, matrix.Color(0, 255, 0));
  matrix.fillRect(0, 7, 11, 3, matrix.Color(200, 255, 0));
  matrix.show();
}


void drawKSD() {

  const char alength = 17;
  const char a[][2] = {
    {2, 0},
    {6, 0},
    {0, 3},
    {1, 3},
    {9, 3},
    {10, 3},
    {4, 4},
    {10, 4},
    {0, 6},
    {5, 6},
    {6, 6},
    {0, 8},
    {9, 8},
    {10, 8},
    {0, 9},
    {6, 9},
    {7, 9} // this char only for "full text clocks"
  };


  matrix.fillScreen(0);


  uint16_t color = matrix.Color(255, 255, 255);
  for (int i = 0; i < alength; i++) {

    matrix.drawPixel(a[i][0], a[i][1], color);
  }
  matrix.show();
  delay(300);
}


void drawNoise() {

  matrix.fillScreen(0);

  //uint16_t color = Wheel(random(0, 255));
  uint16_t color = matrix.Color(255, 255, 255);
  for (int i = 0; i < random(5, 100); i++) {

    matrix.drawPixel(random(0, 11), random(0, 11), color);
  }
  matrix.show();
  delay(random(5, 400));
}
void drawNoise2() {

  matrix.fillScreen(0);

  //uint16_t color = Wheel(random(0, 255));
  uint16_t color = matrix.Color(255, 255, 255);
  for (int i = 0; i < 20; i++) {

    matrix.drawPixel(random(0, 11), random(0, 11), color);
  }
  matrix.show();
  delay(200);
}
void drawNoise3() {

  matrix.fillScreen(0);

  //uint16_t color = Wheel(random(0, 255));
  uint16_t color = matrix.Color(255, 255, 255);
  for (int i = 0; i < 80; i++) {

    matrix.drawPixel(random(0, 11), random(0, 11), color);
  }
  matrix.show();
  delay(100);
}

/////////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE and state
/////////////////////////////////////////////////////////////////////////////////////
enum State {
  START,
  RUN_CLOCK,
  RUN_GFX,
  SET_MIN,
  SET_5MIN,
  SET_HOUR

};

State state;

uint8_t time_now_h;
uint8_t time_now_m;

char currentGfx = 0;

bool blinkMin = false;
bool blink5Min = false;
bool blinkHour = false;

/////////////////////////////////////////////////////////////////////////////////////
// Menu etc.
/////////////////////////////////////////////////////////////////////////////////////
void buttonInterruptA() {
  interruptEventAHappened = true;
  delayMicroseconds(300000);
}
void buttonInterruptB() {
  interruptEventBHappened = true;
  delayMicroseconds(300000);
}



/////////////////////////////////////////////////////////////////////////////////////
// clockface LINES
/////////////////////////////////////////////////////////////////////////////////////
const uint8_t l_meta_es[]     = {0, 0, 2};
const uint8_t l_meta_ist[]    = {3, 0, 3};
const uint8_t l_meta_uhr[]    = {8, 9, 3};

const uint8_t l_min_funf[]    = {7, 0, 4};
const uint8_t l_min_zehn[]    = {0, 1, 4};
const uint8_t l_min_zwanzig[] = {4, 1, 7};
const uint8_t l_min_drei[]    = {0, 2, 4};
const uint8_t l_min_viertel[] = {4, 2, 7};
const uint8_t l_min_nach[]    = {2, 3, 4};
const uint8_t l_min_vor[]     = {6, 3, 3};
const uint8_t l_min_halb[]    = {0, 4, 4};

const uint8_t l_h_zwolf[]     = {5, 4, 5};
const uint8_t l_h_zwei[]      = {0, 5, 4};
const uint8_t l_h_eins[]      = {2, 5, 4};
const uint8_t l_h_ein[]       = {2, 5, 3};
const uint8_t l_h_sieben[]    = {5, 5, 6};
const uint8_t l_h_drei[]      = {1, 6, 4};
const uint8_t l_h_funf[]      = {7, 6, 4};
const uint8_t l_h_elf[]       = {0, 7, 3};
const uint8_t l_h_neun[]      = {3, 7, 4};
const uint8_t l_h_vier[]      = {7, 7, 4};
const uint8_t l_h_acht[]      = {1, 8, 4};
const uint8_t l_h_zehn[]      = {5, 8, 4};
const uint8_t l_h_sechs[]     = {1, 9, 5};

// somehow this works, but it's really ugly.
const uint8_t* l_hours[]      = {l_h_eins, l_h_zwei, l_h_drei, l_h_vier, l_h_funf, l_h_sechs, l_h_sieben, l_h_acht, l_h_neun, l_h_zehn, l_h_elf, l_h_zwolf, l_h_eins};

void drawLineFromArray(const uint8_t a[3], const uint8_t c) {
  matrix.drawFastHLine(a[0], a[1], a[2], currentClockFaceColors[c]);

}

boolean isFullHour() {
  return (time_now_m < 5);
}

boolean isMinuteBetween(const uint8_t a, const uint8_t b) {
  return (a < time_now_m) && (time_now_m < b);
}

uint8_t correctedHour() {
  if (isMinuteBetween(24, 60)) return time_now_h + 1;
  return time_now_h;
}

void drawFourMinuteDots() {
  const uint8_t m = time_now_m % 5;
  if (m > 0) matrix.drawFastHLine(0, 10, m, currentClockFaceColors[3]);
}



void drawTime() {

  //Serial.println("drawing time: m");
  //Serial.println(time_now_m);
  //Serial.println(time_now_h);
  //Serial.println(correctedHour() - 1);
  uint16_t bg = 0;
  if (blinkMin) {
    bg = matrix.Color(50, 0, 0);
  }
  else if (blinkHour) {
    bg = matrix.Color(0, 50, 0);
  }

  matrix.fillScreen(bg);

  drawLineFromArray(l_meta_es, 0);
  drawLineFromArray(l_meta_ist, 0);

  if (isFullHour()) {
    drawLineFromArray(l_meta_uhr, 0);
  }

  if ((1 == correctedHour()) && isFullHour()) {
    drawLineFromArray(l_h_ein, 2);
  } else {
    drawLineFromArray(l_hours[correctedHour() - 1], 2);
  }




  if (isMinuteBetween(4, 25) | isMinuteBetween(34, 40)) {
    drawLineFromArray(l_min_nach, 1);
  }

  if (isMinuteBetween(24, 30) | isMinuteBetween(39, 60)) {
    drawLineFromArray(l_min_vor, 1);
  }

  if (isMinuteBetween(4, 10) | isMinuteBetween(24, 30) | isMinuteBetween(34, 40) | isMinuteBetween(54, 60)) {
    drawLineFromArray(l_min_funf, 1);
  }

  if (isMinuteBetween(9, 15) | isMinuteBetween(49, 55)) {
    drawLineFromArray(l_min_zehn, 1);
  }

  if (isMinuteBetween(14, 20) | isMinuteBetween(44, 50)) {
    drawLineFromArray(l_min_viertel, 1);
  }

  if (isMinuteBetween(19, 25) | isMinuteBetween(39, 45)) {
    drawLineFromArray(l_min_zwanzig, 1);
  }

  if (isMinuteBetween(24, 40)) {
    drawLineFromArray(l_min_halb, 1);
  }

  //  Bedingung fuer "dreiviertel", im Moment nicht benutzt
  //  if (isMinuteBetween(44, 50)) {
  //    drawLineFromArray(l_min_drei, 1);
  //  }



  drawFourMinuteDots();

  matrix.show();



}



/////////////////////////////////////////////////////////////////////////////////////
// TIME
/////////////////////////////////////////////////////////////////////////////////////
bool lastSundayInMonthPassed(int numDays, int dow, int dayNum) {
  // dow == 0 -> sunday
  int daysRemain = numDays - dayNum;
  int daysTilNextSunday = 7 - dow;
  return daysRemain <= daysTilNextSunday;
}


void updateTime() {

  DateTime nowUTC = rtc.now();

  //Serial.println("utcTime time");
  //Serial.println(nowUTC.month());
  //Serial.println(nowUTC.date());
  //Serial.println(nowUTC.hour());
  //Serial.println(nowUTC.minute());


  // correct for CET/CEST
  // last sunday of march / last sunday of october
  const int offsetWinter = 1;
  const int offsetSummer = 2;

  int month = nowUTC.month();
  int offset = offsetWinter;
  if ((month > 3 && month < 10)
    | (month == 3 && lastSundayInMonthPassed(31, nowUTC.dayOfWeek(), nowUTC.date()))
    | (month == 10 && !lastSundayInMonthPassed(31, nowUTC.dayOfWeek(), nowUTC.date())))
  {
    offset = offsetSummer;
  }


  //Serial.println("adding offset hours");
  //Serial.println(offset);

  int h_tzcorrect = nowUTC.hour() + offset;


  // correct hour to 12h format
  int h;
  h = h_tzcorrect % 12;
  if (0 == h) h = 12;

  time_now_h = h;
  time_now_m = nowUTC.minute();



  // test the clock...
  //  if (++time_now_m > 59) {
  //    time_now_m = 0;
  //    if (++time_now_h > 11) {
  //      time_now_h = 1;
  //    }
  //  }
}

void plusMinsInRtc(int mins) {
  DateTime now = rtc.now();
  uint32_t ts_now = now.getEpoch();
  uint32_t secs = mins * 60;
  rtc.setEpoch(ts_now + secs);
}


/////////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////////
void setup() {
  time_now_h = 1;
  time_now_m = 0;

  state = START;
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(90);
  matrix.setTextColor(0);

  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_A), buttonInterruptA, LOW);
  pinMode(PIN_BUTTON_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_B), buttonInterruptB, LOW);

  Serial.begin(57600);

  rtc.begin();
  randomSeed(42);

}

void loop() {

  //delay(100); // debounce button




  switch (state) {
    case START:
      //drawSplashScreen();
      //rainbow(3);
      drawKSD();
      delay(500);
      state = RUN_CLOCK;
      break;
    case RUN_CLOCK:
      if (interruptEventBHappened) {
        interruptEventBHappened = false;
        changeClockFaceColors();
      }
      if (interruptEventAHappened) {
        interruptEventAHappened = false;
        state = RUN_GFX;
        break;
      }

      updateTime();
      drawTime();
      delay(100);
      break;

    case RUN_GFX:
      if (interruptEventAHappened) {
        interruptEventAHappened = false;
        //state = SET_MIN;
        state = RUN_CLOCK;
        break;
      }

      if (interruptEventBHappened) {
        interruptEventBHappened = false;
        if (++currentGfx > 9) currentGfx = 0;
      }
      switch (currentGfx) {
        case 0:
          rainbow(30);
          break;
        case 1:
          rainbow(500);
          break;
        case 2:
          drawFlasher1();
          break;
        case 3:
          drawFlasher2();
          break;
        case 4:
          drawKSD();
          break;
        case 5:
          drawNoise();
          break;
        case 6:
          drawNoise2();
          break;
        case 7:
          drawNoise3();
          break;
        case 8:
          drawPatt1();
          break;
        case 9:
          drawPatt2();
          break;
      }

      break;
      //
      //    case SET_MIN:
      //      blinkMin = ! blinkMin;
      //      if (interruptEventAHappened) {
      //        interruptEventAHappened = false;
      //        blinkMin = false;
      //        state = SET_HOUR;
      //        break;
      //      }
      //      if (interruptEventBHappened) {
      //        interruptEventBHappened = false;
      //        plusMinsInRtc(1);
      //      }
      //      updateTime();
      //      drawTime();
      //      delay(50);
      //      break;

      //    case SET_5MIN:
      //      blink5Min = ! blink5Min;
      //      if (interruptEventAHappened) {
      //        interruptEventAHappened = false;
      //        blink5Min = false;
      //        state = SET_HOUR;
      //        break;
      //      }
      //      drawTime();
      //      delay(100);
      //      break;

      //    case SET_HOUR:
      //      blinkHour = ! blinkHour;
      //      if (interruptEventAHappened) {
      //        interruptEventAHappened = false;
      //        blinkHour = false;
      //        state = RUN_CLOCK;
      //        break;
      //      }
      //
      //      if (interruptEventBHappened) {
      //        interruptEventBHappened = false;
      //        plusMinsInRtc(60);
      //      }
      //      updateTime();
      //      drawTime();
      //      delay(50);
      //      break;

  }


}
