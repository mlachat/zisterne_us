 #include <Wire.h>
#include <LiquidCrystal_I2C.h>

// PINs
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int triggerPin = 7;
const int echoPin = 8;

// VARs
const int cm_min = 26;
const int cm_max = 136;
const int liter_max = 4181;
int loopCount, liter, balken;
float cm, dauer, letzteDauer;

char cm_als_string[10];
const float laufzeit_schall_x2 = 29.1 * 2;
bool debug = true;


void setup() {
  Serial.begin(9600);
  // PIN-Modes
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);

   // LCD
  displayInit();
}

void loop() {
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Erfassung - Dauer in Mikrosekunden
  dauer = pulseIn(echoPin, HIGH);

  // Berechnung
  letzteDauer = 0.90 * letzteDauer + 0.1 * dauer;

  if (loopCount++ % 10 == 0) {
    // Berechne Laufzeit in Mikrosekunden zu Wegstrecke in cm
    cm = letzteDauer / laufzeit_schall_x2;

    // Berechne Liter
    liter = CmZuLiter(cm);

    // Berechne Balken
    balken = round((16.0 * liter) / liter_max);

    if (debug) {
      Serial.print(cm);
      Serial.print(" cm\n");
    }

    // LCD-Ausgabe
    lcd.clear();

    // Zeile 1
    lcd.setCursor(4, 0);
    lcd.print(liter);
    lcd.print(" Liter");

    // Zeile 2
    for (int i = 0; i < balken; i++) {
      lcd.setCursor(i, 1);
      lcd.write((unsigned char)1023);
    }
  }
  delay(250);
}

int CmZuLiter(float x) {
  if (x < cm_min ) {
    x = cm_min ;
  }
  if (x > cm_max) {
    x = cm_max ;
  }

  // 30 CM = 100%, 140cm = 0%
  return (1 - ((x - cm_min) / (cm_max - cm_min))) * liter_max;
}

void displayInit() {
  lcd.begin();
  lcd.setCursor(3, 0);
  lcd.print("Willkommen");
}

