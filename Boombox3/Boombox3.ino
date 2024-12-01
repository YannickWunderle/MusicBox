#include <Wire.h>
#include "FastLED.h"

#define NUM_LEDS 41
#define FLASH_LENGTH 25

#define MOSFET_PIN 9
#define RELAIS_PIN 12
#define DATA_PIN 10 //For ws2812 LEDS

#define FLOODLIGHT_ON A0
#define VOLTAGE_READ_PIN A2
#define OFF_BUTTON A3
#define BUTTON_1_PIN 3
#define BUTTON_2_PIN 2

bool NEW_PRESS = false;

int b_red = 0;
int b_green = 0;
int b_blue = 255;
int maximum_state = 10;

CRGB leds[NUM_LEDS];
float LOW_VOLTAGE = 17.5;
float lastVoltage = LOW_VOLTAGE;
unsigned long previousMillisLED = 0;
unsigned long previousMillisStripe = 0;
unsigned long previousMillisButtonLong = 0;
unsigned long previousMillisButtonPress = 0;
unsigned long previousMillisVoltage = 0;
unsigned long previousMillisRGB_LED = 0;
bool RGB_LED_State = false;
bool LED_STATE = false;
bool Button_state = true;
int stateStripe = 0;
int stateLED = 0;
int LEDindex = 0;
int stateColorStrobo = 0;

void setup() {
  Wire.begin();
  //Serial.begin(9600);

  pinMode(FLOODLIGHT_ON, INPUT);
  pinMode(VOLTAGE_READ_PIN, INPUT);
  pinMode(OFF_BUTTON, INPUT_PULLUP);
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), isr, FALLING);

  pinMode(MOSFET_PIN, OUTPUT);
  pinMode(RELAIS_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);

}

void isr(){
   if (millis() - previousMillisButtonPress > 250) {
    previousMillisButtonPress = millis();
    NEW_PRESS = true;
  }
}

void state_change() {
  if (millis() - previousMillisButtonLong > 800)  {
    previousMillisButtonLong = millis();
    NEW_PRESS = true;
  }

  if (NEW_PRESS)  {
  NEW_PRESS = false;
  if (digitalRead(BUTTON_2_PIN) == false) {
      if (stateLED < maximum_state) {
        stateLED++;
      }
      else {
        stateLED = 0;
      }
    }

    if (digitalRead(BUTTON_1_PIN) == false) {
      if (stateStripe < maximum_state) {
        stateStripe++;
      }
      else {
        stateStripe = 0;
      }
    }
  }
}


void checkVoltage() {

  if ((getVoltage() > LOW_VOLTAGE) || (getVoltage() < 13)) {
    digitalWrite(RELAIS_PIN, HIGH);
  }
  else {
    digitalWrite(RELAIS_PIN, LOW);
  }
}

//void checkButton() {
//  Serial.println("ButtonProg");
//  if (Button_state and !digitalRead(BUTTON_1_PIN)) {
//    Button_state = false;
//    Serial.println("stateChanged");
//    state_change();
//  }
//  if (!Button_state and digitalRead(BUTTON_1_PIN)) {
//    Button_state = true;
//  }
//}



void SendLED() {
  Wire.beginTransmission(4);
  Wire.write(char(b_red));
  Wire.write(char(b_green));
  Wire.write(char(b_blue));
  Serial.println(Wire.endTransmission());

}

float getVoltage() {
  float voltage;
  if (millis() - previousMillisVoltage > 100) {
    previousMillisVoltage = millis();
    voltage = 0.2 * analogRead(VOLTAGE_READ_PIN) / 27.2 + 0.8 * lastVoltage;
    lastVoltage = voltage;
    return voltage;
  }
  else {
    return lastVoltage;
  }
}

void showLEDs() {
  for (int ID = 0; ID < NUM_LEDS; ID++) {
    leds[ID].r = b_green;
    leds[ID].b = b_blue;
    leds[ID].g = b_red;
  }
  FastLED.show();
}

void StroboRGBLED(int t) {//doesnt work
  if (millis() - previousMillisRGB_LED > t) {
    previousMillisRGB_LED = millis();
    if (RGB_LED_State) {
      int b_red = 0;
      int b_green = 0;
      int b_blue = 0;
    }
    else {
      int b_red = 255;
      int b_green = 255;
      int b_blue = 255;
    }
    SendLED();
    RGB_LED_State = !RGB_LED_State;
  }
}

void rainbow(int t) {
  if (millis() - previousMillisStripe > t) {
    previousMillisStripe = millis();
    if (b_green <= 0) {
      if (b_red < 255) {
        b_red = b_red + 1;
        b_blue = b_blue - 1;
      }
      else {
        b_green = 1;
        b_blue = 0;
      }
      showLEDs();
      SendLED();
      return;
    }

    if (b_blue <= 0) {
      if (b_green < 255) {
        b_green++;
        b_red--;
      }
      else {
        b_blue = 1;
        b_red = 0;
      }
      showLEDs();
      SendLED();
      return;
    }

    if (b_red <= 0) {
      if (b_blue < 255) {
        b_blue++;
        b_green--;
      }
      else {
        b_red = 1;
        b_green = 0;
      }
      showLEDs();
      SendLED();
      return;
    }
    else {
      b_red = 0;
      b_green = 0;
      b_blue = 255;
    }
  }

}



void randomStripe(int t) {
  if (millis() - previousMillisStripe > t) {
    previousMillisStripe = millis();
    leds[random(0, NUM_LEDS)].r = random(10, 255);
    leds[random(0, NUM_LEDS)].g = random(10, 255);
    leds[random(0, NUM_LEDS)].b = random(10, 255);
    FastLED.show();

  }
}

void StripeBlink(int t) {
  if (millis() - previousMillisStripe > t) {
    previousMillisStripe = millis();
    leds[random(0, NUM_LEDS)].r = random(10, 255);
    leds[random(0, NUM_LEDS)].g = random(10, 255);
    leds[random(0, NUM_LEDS)].b = random(10, 255);
    int i = random(0, NUM_LEDS);
    leds[i].r = 0;
    leds[i].g = 0;
    leds[i].b = 0;
    FastLED.show();
  }
}

void randomLEDSwipe(int t) {
  if (millis() - previousMillisStripe > t) {
    previousMillisStripe = millis();
    if (LEDindex < NUM_LEDS) {
      leds[LEDindex].r = random(10, 255);
      leds[LEDindex].g = random(10, 255);
      leds[LEDindex].b = random(10, 255);
      FastLED.show();
      LEDindex ++;
    }
    else {
      LEDindex = 0;
      b_red = random(10, 255);
      b_green = random(10, 255);
      b_blue = random(10, 255);
      SendLED();
    }

  }
}

void BlueMix(int t) {
  if (millis() - previousMillisStripe > t) {
    previousMillisStripe = millis();
    for (int i = 0; i < 13; i++) {
      leds[i].r = random(0, 15);
      leds[i].g = random(0, 5);
      leds[i].b = random(80, 255);

      b_red = random(0, 15);
      b_green = random(0, 5);
      b_blue = random(80, 255);
      SendLED();
    }
    for (int i = 14; i < 20; i++) {
      leds[i].r = random(80, 255);
      leds[i].g = 0;
      leds[i].b = random(80, 255);
    }
    for (int i = 21; i < 33; i++) {
      leds[i].r = random(0, 15);
      leds[i].g = random(0, 5);
      leds[i].b = random(80, 255);
    }
    for (int i = 34; i < NUM_LEDS; i++) {
      leds[i].r = random(80, 255);
      leds[i].g = 0;
      leds[i].b = random(80, 255);
    }
    FastLED.show();
  }
}

void ColorStrobo(int t) {
  if (millis() - previousMillisStripe > t) {
    previousMillisStripe = millis();
    if (stateColorStrobo < 11) {
      stateColorStrobo++;
    }
    else {
      stateColorStrobo = 0;
    }

    switch (stateColorStrobo) {
      case 1:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].r = 255;
          leds[i].g = 0;
          leds[i].b = 0;
        }
        b_red = 255;
        b_green = 0;
        b_blue = 0;
        break;
      case 3:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].r = 0;
          leds[i].g = 255 ;
          leds[i].b = 0;
        }
        b_red = 0;
        b_green = 255;
        b_blue = 0;
        break;
      case 5:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].r = 255;
          leds[i].g = 0;
          leds[i].b = 255;
        }
        b_red = 255;
        b_green = 0;
        b_blue = 255;
        break;
      case 7:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].r = 255;
          leds[i].g = 255;
          leds[i].b = 0;
        }
        b_red = 255;
        b_green = 255;
        b_blue = 0;
        break;
      case 9:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].r = 0;
          leds[i].g = 0;
          leds[i].b = 255 ;
        }
        b_red = 0;
        b_green = 0;
        b_blue = 255;
        break;

      case 11:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].r = 0;
          leds[i].g = 255;
          leds[i].b = 255 ;
        }
        b_red = 0;
        b_green = 255;
        b_blue = 255;
        break;

      default:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i].r = 0;
          leds[i].g = 0;
          leds[i].b = 0;

        }
        b_red = 0;
        b_green = 0;
        b_blue = 0;
        break;

    }

    FastLED.show();
    char a = b_green;
    b_green = b_red;
    b_red = a;
    SendLED();
  }
}

void ColorStroboRGB_LED(int t) {
  if (millis() - previousMillisRGB_LED > t) {
    previousMillisRGB_LED = millis();
    if (stateColorStrobo < 11) {
      stateColorStrobo++;
    }
    else {
      stateColorStrobo = 0;
    }

    switch (stateColorStrobo) {
      case 1:
        b_red = 255;
        b_green = 0;
        b_blue = 0;
        break;
      case 3:
        b_red = 0;
        b_green = 255;
        b_blue = 0;
        break;
      case 5:
        b_red = 255;
        b_green = 0;
        b_blue = 255;
        break;
      case 7:
        b_red = 255;
        b_green = 255;
        b_blue = 0;
        break;
      case 9:
        b_red = 0;
        b_green = 0;
        b_blue = 255;
        break;
      case 11:
        b_red = 0;
        b_green = 255;
        b_blue = 255;
        break;

      default:
        b_red = 0;
        b_green = 0;
        b_blue = 0;
        break;
    }
    SendLED();
  }
}

void strobo(int stroboTime) {
  if (LED_STATE) {
    if (millis() - previousMillisLED > FLASH_LENGTH) {
      LED_STATE = false;
      previousMillisLED = millis();
    }
  }
  else {
    if (millis() - previousMillisLED > stroboTime) {
      LED_STATE = true;
      previousMillisLED = millis();
    }
  }
  digitalWrite(MOSFET_PIN, LED_STATE);
}

void checkOffButton() {
  int OffTimer = 0;
  while (digitalRead(OFF_BUTTON) == false) {
    OffTimer++;
    delay(20);
    if (OffTimer > 100) {
      digitalWrite(RELAIS_PIN, LOW);
    }

  }
}


void loop() {

  checkVoltage();
  checkOffButton();
  state_change();

  if (NEW_PRESS == true) {
    state_change();
  }

  switch (stateLED) {

    case 0:
      digitalWrite(MOSFET_PIN, LOW);
      break;
    case 1:
      analogWrite(MOSFET_PIN, 8);
      break;
    case 2:
      analogWrite(MOSFET_PIN, 16);
      break;
    case 3:
      analogWrite(MOSFET_PIN, 32);
      break;
    case 4:
      analogWrite(MOSFET_PIN, 64);
      break;
    case 5:
      analogWrite(MOSFET_PIN, 128);
      break;
    case 6:
      digitalWrite(MOSFET_PIN, HIGH);
      break;
    case 7:
      strobo(1023);
      break;
    case 8:
      strobo(512);
      break;
    case 9:
      strobo(255);
      break;
    case 10:
      strobo(128);
      break;
    default:
      digitalWrite(MOSFET_PIN, LOW);
      break;
  }


  switch (stateStripe) {

    case 0:
      b_red = 0;
      b_blue = 0;
      b_green = 0;
      showLEDs();
      SendLED();
      break;
    case 1:
      b_green = 255;
      b_blue = 255;
      b_red = 0;
      showLEDs();
      SendLED();
      break;
    case 2:
      b_red = 255;
      b_blue = 255;
      b_green = 0;
      showLEDs();
      SendLED();
      break;
    case 3:
      b_red = 255;
      b_blue = 0;
      b_green = 255;
      showLEDs();
      SendLED();
      break;
    case 4:
      rainbow(8);
      break;
    case 5:
      randomStripe(15);
      ColorStroboRGB_LED(40);
      break;
    case 6:
      rainbow(40);
      break;
    case 7:
      randomLEDSwipe(20);
      break;
    case 8:
      StripeBlink(15);
      ColorStroboRGB_LED(70);
      break;
    case 9:
      BlueMix(50);
      break;
    case 10:
      ColorStrobo(350);
      break;

    default:
      b_red = 0;
      b_blue = 0;
      b_green = 0;
      showLEDs();
      SendLED();
      break;

  }

}
