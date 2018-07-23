#include <UIPEthernet.h>
#include <LiquidCrystal_I2C.h>

// PINs
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int triggerPin = 5;
const int echoPin = 4;

// Hier die eigene MAC-Adresse eintragen
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xEE  };  
// Change the IP below to your subnet if you have any issues
//    IPAddress ip(192, 168, 0, 115);                                
IPAddress ip(192, 168, 188, 213);
// UDP Port zum Datenempfang Lox MS -> Arduino
unsigned int ARDUPORT = 7013;

// IP Loxone Miniserver
IPAddress MSIP(192, 168, 188, 121);
// UDP Port zum Datenversand Arduino -> Lox MS
unsigned int MSPORT = 7014;

// UDP Instanz erzeugen
EthernetUDP Udp;

// VARs
const int cm_min = 20;
const int cm_max = 110;
const int liter_max = 4180;
int loopCount, liter;
float cm, dauer, letzteDauer;

char cm_als_string[10];
const float laufzeit_schall_x2 = 29.1 * 2;
bool debug = true;


void setup() {

  Serial.begin(9600);

  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};

  Ethernet.begin(mac,IPAddress(192,168,188,6));

  int success = Udp.begin(5000);

  Serial.print("initialize: ");
  Serial.println(success ? "success" : "failed");
  // netzwerkInit();
  // PIN-Modes
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  Serial.println(Ethernet.localIP());
}

void loop() {
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Erfassung - Dauer in Mikrosekunden
  dauer = pulseIn(echoPin, HIGH);

  // Berechnung
  letzteDauer = 0.95 * letzteDauer + 0.05 * dauer;

  if (loopCount++ % 10 == 0) {
    // Berechne Laufzeit in Mikrosekunden zu Wegstrecke in cm
    cm = letzteDauer / laufzeit_schall_x2;

    // Berechne Liter
    liter = CmZuLiter(cm);

     // an Loxone senden
    dtostrf(cm, 4, 0, cm_als_string); //Distanz fuer UDP Versand umwandeln
    if (debug) {
      Serial.println(cm_als_string);
      Serial.println(liter);
    }
    sendUDP(cm_als_string); //Ergebnis an MiniServer senden

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
  return (1 - ((x - cm_min) / (cm_max - cm_min))) * liter_max;
}

//UDP-Befehl senden
void sendUDP(String text) {
    Serial.println("trying to send string to loxone");
    Udp.beginPacket(MSIP, MSPORT);
    Udp.print(text);
    Serial.println("DONE SENDING\n");
    Udp.endPacket();
    Serial.println("PACKET ENDED\n");
  delay(10);
}

void netzwerkInit() {
  Serial.println("Setup network");
  Ethernet.begin(mac, ip); 
 // if (!Ethernet.begin(mac)) Serial.println("DHCP Fehler");
 // else {
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
  //}
  Serial.println(" network Done");
  Udp.begin(ARDUPORT);

}

