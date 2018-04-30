#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// PINs
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int triggerPin = 7;
const int echoPin = 8;

// VARs
const int cm_min = 20;
const int cm_max = 151;
const int liter_max = 4500;
const int durchgaenge = 3;
int liter, balken;
float cm, dauer, letzteDauer;
int loopCount = 0;

void setup() {
  Serial.begin(9600);
  // PIN-Modes
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  
  // LCD
  lcd.begin();
  lcd.setCursor(3,0);
  lcd.print("Willkommen");

}

void loop() {
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    // Erfassung - Dauer in Mikrosekunden
    dauer = pulseIn(echoPin, HIGH);

    // Berechnung
    letzteDauer = 0.95 * letzteDauer + 0.05 * dauer;

  if(loopCount++ % 10 == 0)
  {
    // Berechne Mikrosekunden in cm
    cm = ((letzteDauer)/2)/29;
  
    // Berechne Liter
      liter = CmZuLiter(cm);
   
    // Berechne Balken
    balken = LiterZuBalken(liter);
  
     Serial.print(cm);
     Serial.print(" cm\n");
  
    // LCD-Ausgabe
    lcd.clear();
    
    // Zeile 1
    lcd.setCursor(4,0);
    lcd.print(liter);
    lcd.print(" Liter");
  
    // Zeile 2
    for (int i=0; i<balken; i++) {
      lcd.setCursor(i,1);
      lcd.write((unsigned char)1023);
    }
  }
  delay(100);
}

int CmZuLiter(float x) {
  if (x < cm_min ) {
      x = cm_min ;

  }
  if (x > cm_max) {
    x = cm_max ;
  }

     // Serial.print(x);
     // Serial.print('\n');
  // 20 CM = 100%, 151 = 0%
  return (1-((x - cm_min) / (cm_max - cm_min))) * liter_max;
}

int LiterZuBalken(int x) {
  int result;
  result = round((16.0*x)/liter_max);
  return result;
}
