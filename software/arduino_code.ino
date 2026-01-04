// RO Filtration System with Joystick-Controlled Menu and I2C LCD Display
// Features: TDS input, DHT sensor, flow sensor, float sensors, pump control via joystick and auto mode, warning threshold, EEPROM saving

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <DHT.h>

// I2C LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Address 0x27 for most I2C modules

// Joystick pins
const int xPin = A1;
const int yPin = A2;
const int buttonPin = 9;

// Buzzer pin
const int buzzer = 8;

// Pump control pin (relay active LOW)
const int pumpPin = 7;
bool pumpState = false;
bool autoMode = true;

// Flow sensor pin
const int flowSensorPin = 2;
volatile int flowPulseCount = 0;
float flowRate = 0;
unsigned long lastFlowCalc = 0;

// DHT Sensor
#define DHTPIN A3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0;
float humidity = 0;
float evapRatio = 0;

// Float sensors (logic 0 = water, logic 1 = empty)
const int floatOverheadPin = 4;
const int floatOutputPin = 5;
bool overheadNoWater = false;
bool outputNoWater = false;

// TDS and Threshold
int tdsValue = 0;
int tdsWarningThreshold = 1000;
bool tdsWarningActive = false;

// Menu navigation
int menuIndex = 0;
bool inMenu = true;
unsigned long lastJoyNav = 0;

const char* menuItems[] = {
  "View Status",
  "Set TDS",
  "Set TDS Warn",
  "Pump: Auto/Man",
  "Save Settings",
  "Exit"
};
const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(pumpPin, HIGH);  // relay off (active LOW)

  pinMode(flowSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), countFlow, RISING);

  pinMode(floatOverheadPin, INPUT_PULLUP);
  pinMode(floatOutputPin, INPUT_PULLUP);

  dht.begin();

  // Load from EEPROM
  tdsValue = EEPROM.read(0) * 10;
  tdsWarningThreshold = EEPROM.read(1) * 10;

  lcd.setCursor(0, 0);
  lcd.print(" Solar Powered ");
  lcd.setCursor(0, 1);
  lcd.print("Smart RO System");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mineral Water");
  lcd.setCursor(0, 1);
  lcd.print("Filter Control");
  delay(2000);

  lcd.clear();
  displayMenu();
}

void loop() {
  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  unsigned long now = millis();

  // Read float sensors
  overheadNoWater = digitalRead(floatOverheadPin);  // HIGH = empty
  outputNoWater = digitalRead(floatOutputPin);      // HIGH = empty

  // Auto pump control
  if (autoMode) {
    if (overheadNoWater) {
      pumpState = false;
    } else if (outputNoWater) {
      pumpState = true;
    } else {
      pumpState = false;
    }
    digitalWrite(pumpPin, pumpState ? LOW : HIGH);
  }

  // Menu navigation
  if (inMenu && now - lastJoyNav > 300) {
    if (yVal < 400) {
      menuIndex = (menuIndex + 1) % menuSize;
      lastJoyNav = now;
      displayMenu();
    } else if (yVal > 600) {
      menuIndex = (menuIndex - 1 + menuSize) % menuSize;
      lastJoyNav = now;
      displayMenu();
    }
  }

  if (inMenu && digitalRead(buttonPin) == LOW) {
    unsigned long pressStart = millis();
    while (digitalRead(buttonPin) == LOW);
    if (millis() - pressStart > 100) {
      switch (menuIndex) {

        case 0:
          inMenu = false;
          break;

        case 1:
          adjustTDS();
          displayMenu();
          break;

        case 2:
          adjustThreshold();
          displayMenu();
          break;

        case 3:
          autoMode = !autoMode;
          if (autoMode) {
            if (overheadNoWater) pumpState = false;
            else if (outputNoWater) pumpState = true;
            else pumpState = false;

            digitalWrite(pumpPin, pumpState ? LOW : HIGH);
          }
          lcd.clear();
          lcd.print(autoMode ? "Mode: AUTO" : "Mode: MAN");
          delay(1500);
          displayMenu();
          break;

        case 4:
          EEPROM.write(0, tdsValue / 10);
          EEPROM.write(1, tdsWarningThreshold / 10);
          lcd.clear();
          lcd.print("Settings Saved");
          delay(1000);
          displayMenu();
          break;

        case 5:
          inMenu = false;
          break;
      }
    }
  }

  if (!inMenu) {
    updateSensors();
    displayData();

    if (!autoMode && digitalRead(buttonPin) == LOW) {
      unsigned long pressStart = millis();
      while (digitalRead(buttonPin) == LOW);
      if (millis() - pressStart > 100) {
        if (!overheadNoWater && !outputNoWater) {
          pumpState = !pumpState;
          digitalWrite(pumpPin, pumpState ? LOW : HIGH);
        }
      }
    }

    if (digitalRead(buttonPin) == LOW) {
      unsigned long pressStart = millis();
      while (digitalRead(buttonPin) == LOW);
      if (millis() - pressStart > 100) {
        inMenu = true;
        displayMenu();
      }
    }
  }

  tdsWarningActive = (tdsValue > tdsWarningThreshold);
  digitalWrite(buzzer, tdsWarningActive ? HIGH : LOW);

  if (millis() - lastFlowCalc >= 1000) {
    detachInterrupt(digitalPinToInterrupt(flowSensorPin));
    flowRate = (flowPulseCount / 7.5);
    flowPulseCount = 0;
    lastFlowCalc = millis();
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), countFlow, RISING);
  }
}

void countFlow() {
  flowPulseCount++;
}

void updateSensors() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (!isnan(humidity) && !isnan(temperature)) {
    evapRatio = temperature / (humidity + 1);
  }
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("> ");
  lcd.print(menuItems[menuIndex]);
  lcd.setCursor(0, 1);
  int next = (menuIndex + 1) % menuSize;
  lcd.print("  ");
  lcd.print(menuItems[next]);
}

void adjustTDS() {
  lcd.clear();
  lcd.print("Adjust TDS:");
  while (digitalRead(buttonPin) == HIGH) {
    int xVal = analogRead(xPin);
    if (xVal < 400) tdsValue = max(tdsValue - 10, 0);
    else if (xVal > 600) tdsValue = min(tdsValue + 10, 1200);
    lcd.setCursor(0, 1);
    lcd.print("TDS: ");
    lcd.print(tdsValue);
    lcd.print("   ");
    delay(300);
  }
  delay(300);
}

void adjustThreshold() {
  lcd.clear();
  lcd.print("Set Warn Thresh:");
  while (digitalRead(buttonPin) == HIGH) {
    int xVal = analogRead(xPin);
    if (xVal < 400) tdsWarningThreshold = max(tdsWarningThreshold - 10, 0);
    else if (xVal > 600) tdsWarningThreshold = min(tdsWarningThreshold + 10, 1000);
    lcd.setCursor(0, 1);
    lcd.print("Limit: ");
    lcd.print(tdsWarningThreshold);
    lcd.print("   ");
    delay(300);
  }
  delay(300);
}

void displayData() {
  static int animFrame = 0;
  static int screen = 0;
  const char anim[4] = {'|', '/', '-', '\\'};

  switch (screen) {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("TDS:");
      lcd.print(tdsValue);
      lcd.print("ppm");

      lcd.setCursor(0, 1);
      lcd.print(tdsWarningActive ? "WARN! " : "OK    ");
      lcd.print("F:");
      lcd.print(flowRate, 1);
      delay(2000);
      break;

    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(temperature, 1);
      lcd.print(" H:");
      lcd.print(humidity, 0);

      lcd.setCursor(0, 1);
      lcd.print("E.Rat:");
      lcd.print(evapRatio, 1);
      delay(2000);
      break;

    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pump Status:");
      lcd.setCursor(0, 1);
      lcd.print(pumpState ? " ON  " : " OFF ");
      lcd.print(" ");
      lcd.print(anim[animFrame % 4]);
      animFrame++;
      delay(1000);
      break;

    case 3:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Overhead: ");
      lcd.print(overheadNoWater ? "Empty" : "Water");

      lcd.setCursor(0, 1);
      lcd.print("Output:   ");
      lcd.print(outputNoWater ? "Empty" : "Water");
      delay(3000);
      break;
  }

  screen = (screen + 1) % 4;
}
