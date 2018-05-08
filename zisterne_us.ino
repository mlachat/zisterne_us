#include <EthernetClient.h>
#include <Ethernet.h>
#include <Dhcp.h>
#include <EthernetServer.h>
#include <Dns.h>
#include <EthernetUdp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// PINs
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int triggerPin = 7;
const int echoPin = 8;

// Hier die eigene MAC-Adresse eintragen
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEB };
// UDP Port zum Datenempfang Lox MS -> Arduino
unsigned int ARDUPORT = 7013;

// IP Loxone Miniserver
IPAddress MSIP(192, 168, 178, 100);
// UDP Port zum Datenversand Arduino -> Lox MS
unsigned int MSPORT = 7014;

// Daten-Puffer initalisieren
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// UDP Instanz erzeugen
EthernetUDP Udp;

// VARs
const int cm_min = 20;
const int cm_max = 110;
const int liter_max = 4180;
int loopCount, liter, balken;
float cm, dauer, letzteDauer;

char cm_als_string[10];
const float laufzeit_schall_x2 = 29.1*2;
bool debug = true;


void setup() {
  Serial.begin(9600);
  // PIN-Modes
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);

  // Netzwerk Setup
  netzwerkInit();
  
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
    letzteDauer = 0.95 * letzteDauer + 0.05 * dauer;

    if(loopCount++ % 100 == 0) {
      // Berechne Laufzeit in Mikrosekunden zu Wegstrecke in cm
      cm = letzteDauer/laufzeit_schall_x2;
    
      // Berechne Liter
      liter = CmZuLiter(cm);
     
      // Berechne Balken
      balken = round((16.0*liter)/liter_max);
  
      if (debug) {
        Serial.print(cm);
        Serial.print(" cm\n");
      }
  
      // an Loxone senden 
      dtostrf(cm, 4, 0, cm_als_string); //Distanz fuer UDP Versand umwandeln
      sendUDP(cm_als_string); //Ergebnis an MiniServer senden
    
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

  // 20 CM = 100%, 130cm = 0%
  return (1-((x - cm_min) / (cm_max - cm_min))) * liter_max;
}

//UDP-Befehl senden
void sendUDP(String text) {
  Udp.beginPacket(MSIP, MSPORT);
  Udp.print(text);
  Udp.endPacket();
  delay(10);
}

void displayInit() {
  lcd.begin();
  lcd.setCursor(3,0);
  lcd.print("Willkommen");
}

void netzwerkInit(){
  if (!Ethernet.begin(mac)) Serial.println("DHCP Fehler");
  else {
    Serial.println ("Netzwerkeinstellungen");
    Serial.println ("---------------");
    Serial.print("Arduino MAC Adresse: ");
    Serial.print(mac[0], HEX);
    Serial.print(":");
    Serial.print(mac[1], HEX);
    Serial.print(":");
    Serial.print(mac[2], HEX);
    Serial.print(":");
    Serial.print(mac[3], HEX);
    Serial.print(":");
    Serial.print(mac[4], HEX);
    Serial.print(":");
    Serial.println(mac[5], HEX);
    Serial.print("Arduino IP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("Arduino UDP Port: ");
    Serial.println(ARDUPORT);
    Serial.println ("---------------");
    Serial.print ("Miniserver IP: ");
    Serial.println(MSIP);
    Serial.print("Miniserver UDP Port: ");
    Serial.println(MSPORT);
    Serial.println ("---------------");
  }
  
  Udp.begin(ARDUPORT);

}

