#include <Wire.h>

#define   PIN_RED 5
#define   PIN_BLUE 11
#define   PIN_GREEN  9
#define   PIN_FAN 3

#define FAN_THRESHOLD 100
#define FAN_ONTIME 100
#define AVERAGE_SAMPLES 10

bool tickMessage = false;
bool tickFan = false;
unsigned long LastMessage = 0;
unsigned long previousMillisFan = 0;
unsigned long previousMillisLED = 0;
byte  b_red = 0;
byte  b_green = 0;
byte  b_blue = 255;

int AverageBrightness[AVERAGE_SAMPLES];
int Av_index = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(4);
  Wire.onReceive(receiveEvent);

  pinMode(PIN_RED , OUTPUT);
  pinMode(PIN_BLUE , OUTPUT);
  pinMode(PIN_GREEN , OUTPUT);
  pinMode(PIN_FAN , OUTPUT);

  digitalWrite(PIN_FAN, HIGH);
  analogWrite(PIN_RED, 2);
  analogWrite(PIN_BLUE, 2);
  analogWrite(PIN_GREEN, 2);
  delay(3000);
  analogWrite(PIN_RED, 0);
  analogWrite(PIN_GREEN, 0);
  analogWrite(PIN_BLUE, 0);
  digitalWrite(PIN_FAN, LOW);
}

void receiveEvent(int a)
{
  b_red = Wire.read();
  b_green = Wire.read();
  b_blue = Wire.read();
  tickMessage = true;
}

bool Connected() {
  if (tickMessage) {
    tickMessage = false;
    LastMessage = millis();
  }
  if (millis() - LastMessage > 5000) {
    return false;
  }
  else {
    return true;
  }
}


void showLEDs() {
  analogWrite(PIN_RED, b_red);
  analogWrite(PIN_GREEN, b_green);
  analogWrite(PIN_BLUE, b_blue);
}

void setFan() {
  if (millis() - previousMillisFan > FAN_ONTIME) {
    previousMillisFan = millis();

    if (Av_index == AVERAGE_SAMPLES) {
      Av_index = 0;
    }
    else {
      Av_index++;
    }
    AverageBrightness[Av_index]=b_red+b_green+b_blue;
    
    int Average = 0;
    for (int i = 0; i < AVERAGE_SAMPLES; i++) {
      Average += AverageBrightness[i];
          }
    Serial.println(Average);
    Average = Average / AVERAGE_SAMPLES;
    
    if ((Average) > FAN_THRESHOLD) {
      digitalWrite(PIN_FAN, HIGH);
    }
    else {
      digitalWrite(PIN_FAN, LOW);
    }
  }
}

void rainbow(int t) {
  if (millis() - previousMillisLED > t) {
    previousMillisLED = millis();
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
      return;
    }
  }

}

void loop() {
  setFan();

  if (Connected()) {
    showLEDs();
  }
  else {
    rainbow(50);
  }
}
