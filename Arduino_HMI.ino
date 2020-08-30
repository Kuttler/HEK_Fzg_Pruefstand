//Bibliotheken
#include <Wire.h> //Bibliothek für I2C
#include "RTClib.h" //Bibliothek für Real Time Clock  (RTC)
#include <SD.h>   //SD Card Bibliothek
#include <SPI.h>  //Bibliothek zum zugriff auf die SPI Schnittstelle des Arduinos
#include <U8x8lib.h>

//Variablen Definition
int Timer=0;
//Fahrzeugdaten
const float Dichte_Umgebungsluft = 1.2;
const float Stroemungskoeffizient = 0.2;
float Steigungswinkel = 0;
const float Erdbeschleunigung = 9.81;
const float Anstroemflaeche = 0.4;
float Fahrzeugmasse = 130; //Normalgewicht des Fahrzeugs+Fahrer/in
float Reibungskoeffizient = 0.04; //Reibungskoeffizient der aktuellen Bereifung
float Fahrzeugwiderstand = Fahrzeugmasse * Erdbeschleunigung * cos(Steigungswinkel) * Reibungskoeffizient + 0.5 * Stroemungskoeffizient * Dichte_Umgebungsluft * Anstroemflaeche * 12.73 + Fahrzeugmasse * Erdbeschleunigung * sin(Steigungswinkel);
//I2CMutliplexer
#define TCAADDR 0x70
//Datenlogger
File outFile; // Die Datei welche geschrieben bzw. gelesen werden soll
RTC_DS1307 rtc; // Zugriff auf RTC
int pinCS = 10; // Der Chipselect Pin auf Arduino Mega
String filename = "Pruefstandsprotokoll.txt"; //Dateiname der zuschreibenden Datei

//Displays
float Nextion_Faktor = 0.26;
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

//Eingabe Tater
#define Pin_Betriebsmodus_1  49 //Pins des Arduinos bekommen Alias
#define Pin_Betriebsmodus_2  51
#define Pin_Betriebsmodus_3 53
#define Pin_Not_Bremse 54
#define Pin_LED_BM_1 47
#define Pin_LED_BM_2 45
#define Pin_LED_BM_3 43
int Betriebsmodus = 0;

//Eingabe Potentiomerter
#define Potendtiometer_1 A0 //Der Pin A0 wird als Pin für das Poteniometer 1 festgelegt
#define Potendtiometer_2 A2 //Der Pin A1 wird als Pin für das Poteniometer 2 
float Faktor_Masse = 0, Faktor_Reibung = 0; //Faktoren zur Berechnung der Parameter
float Faktor_Bremse = 0, Faktor_Steigung = 0;
int Bremseinstellung=0;

//Benötigte Funktionen
void Adresse(uint8_t i); //I2C Multiplexer, mach aus einer Adresse 3
void Faktor_Berechnug_Potentiometer();
void Faktor_Berechnug_Potentiometer_2();//Fertig nicht getestet
void Initalisierung_Datenlogger();
void Datenspeichern_Datenlogger();
void Initalisierung_Displays();
void Display_Amzeige_Zustand_Start ();
void Display_Amzeige_Zustand_0 ();
void Display_Anzeige_Zustand_1();
void Display_Anzeige_Zustand_2();
void Betriebsmoduseingabe();

int Zustand = 0;

void setup() 
{/*
  Faktor_Berechnug_Potentiometer();
  Initalisierung_Datenlogger();
  Initalisierung_Displays();
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial.begin(9600);
  Wire.begin();
  Display_Amzeige_Zustand_Start ();
  Display_Amzeige_Zustand_0 ();
}*/

void loop() {
/*
  switch (Zustand) {
    case 0:
      Betriebsmoduseingabe();
      break;
    case 1:
      Display_Anzeige_Zustand_1();
      break;
    case 2:
      Display_Anzeige_Zustand_2();
    default:
      break;
  }
  */
  // Zustand 1
  Fahrzeugmasse = Faktor_Masse * analogRead(Potendtiometer_1); //Rechung für die Parameteränderung
  // bei Änderung der Potentiometer Stellung
  Reibungskoeffizient = Faktor_Reibung * analogRead(Potendtiometer_2); //Rechung für die Parameteränderung,
  // bei Änderung der Potentiometer Stellung
  Display_Anzeige_Zustand_1();


  //Zustand 2


  //Zustand 3
  Datenspeichern_Datenlogger();
}

void Adresse(uint8_t i) //I2C Multiplexer, mach aus einer Adresse 3 //Fertig
{
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR); //I2C wird an die Stelle 0x70 geschslten wo sich der Multiplexer befindet
  Wire.write(1 << i);              //Leitet der I2C an einen seiner AUsgänge
  Wire.endTransmission();
}
void Faktor_Berechnug_Potentiometer()//Fertig nicht getestet
{
  Faktor_Masse = Fahrzeugmasse / analogRead(Potendtiometer_1); //Der Faktor der Masse wird
  // berechnet mit der aktuellen Stellung des Poteniometers, dies ist somit der Nullpunkt
  Faktor_Reibung = Reibungskoeffizient / analogRead(Potendtiometer_2); //Der Faktor der Reibung wird
  // berechnet mit der aktuellen Stellung des Poteniometers, dies ist somit der Nullpunkt
}
/*void Faktor_Berechnug_Potentiometer_2()//Fertig nicht getestet
{
  Faktor_Bremse = Bremseinstellung / analogRead(Potendtiometer_1); //Der Faktor der Masse wird
  // berechnet mit der aktuellen Stellung des Poteniometers, dies ist somit der Nullpunkt
  Faktor_Steigung = Steigungswinkel / analogRead(Potendtiometer_2); //Der Faktor der Reibung wird
  // berechnet mit der aktuellen Stellung des Poteniometers, dies ist somit der Nullpunkt
}
void Initalisierung_Datenlogger()//Fertig nicht getestet
{
  if (! rtc.isrunning()) { // Überprüfen ob die Uhr schon initalisiert wurde
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Initlisieren des RTC mit der Zeit/Datum des Computers
  }
  pinMode(pinCS, OUTPUT);
}*/
void Datenspeichern_Datenlogger() //Fertig nicht getestet
{
  outFile = SD.open(filename, FILE_WRITE); /*Öffnet bzw. erzeugt die Datei wenn sie noch nicht vorhanden ist*/
  if (outFile) { //Wenn/Sobald die Datei exisitert
    DateTime jetzt = rtc.now();
    outFile.print(jetzt.year(), DEC);
    outFile.print('/');
    outFile.print(jetzt.month(), DEC);
    outFile.print('/');
    outFile.print(jetzt.day(), DEC);
    outFile.print(" ");
    outFile.print(jetzt.hour(), DEC);
    outFile.print(':');
    outFile.print(jetzt.minute(), DEC);
    outFile.print(':');
    outFile.print(jetzt.second(), DEC);
    outFile.print(" ");
    outFile.println("");//Drehzahl einfügen
  }
}
void Initalisierung_Displays()//Fertig
{
  u8x8.setPowerSave(0);         //Setzt PowerSave auf 0 schaltet das Display ein
  for (int i = 0; i < 6; i++)
  {
    Adresse(i);              //I läuft jede einzelne Adresse der Displays durch
    u8x8.begin();              //Schält alle Displays auf Betriebsbereit
  }
}

void Display_Amzeige_Zustand_Start ()//Fertig
{
  for (int x = 2; x < 5; x++)
  {
    Adresse(x);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0, 0, "Pruestands- ");
    u8x8.drawString(0, 2, "programm");
    u8x8.drawString(0, 4, "wird geladen");
    for (int y = 0; y < 17; y++)
    {
      Adresse(x);
      u8x8.setFont(u8x8_font_chroma48medium8_r);
      u8x8.drawString(y, 6, ".");
      Adresse(x - 1);
      u8x8.drawString(y, 6, " ");

      Serial1.print("tach.pic=");/*Hier wird dem Nextion Display mittgeteilt welches Objekt geändert werden soll (vor dem Komma) und welcher Wert an diesem Objekt geändert wird (nach dem Komma*/
      Serial1.print((y * 9)); //Die anzuzeigende Drehzahl
      Serial1.write(0xff);  // Nach jeder Nachricht an das Nextion Display
      Serial1.write(0xff);  // müssen diese drei Zeichen gesendet werden
      Serial1.write(0xff);

      Serial.print("tach.pic=");/*Hier wird dem Nextion Display mittgeteilt welches Objekt geändert werden soll (vor dem Komma) und welcher Wert an diesem Objekt geändert wird (nach dem Komma*/
      Serial.print((y * 9)); //Die anzuzeigende Drehzahl
      Serial.write(0xff);  // Nach jeder Nachricht an das Nextion Display
      Serial.write(0xff);  // müssen diese drei Zeichen gesendet werden
      Serial.write(0xff);
      delay(300);
    }
  }
  for (int x = 2; x < 5; x++)
  {
    Adresse(x);
    for (int y = 0; y < 7; y++)
    {
      u8x8.clearLine(y);
    }
  }
}
void Display_Amzeige_Zustand_0 () //Fertig
{
  Adresse(2);
  u8x8.setFont(u8x8_font_courB18_2x3_f );
  u8x8.drawString(0, 3, "Betriebs-");
  Adresse(3);
  u8x8.setFont(u8x8_font_courB18_2x3_f );
  u8x8.drawString(0, 3, "  Modus");
  Adresse(4);
  u8x8.setFont(u8x8_font_courB18_2x3_f );
  u8x8.drawString(0, 3, " waehlen");

  delay(1000);

  for (int x = 2; x < 5; x++)
  {
    Adresse(x);
    for (int y = 0; y < 7; y++)
    {
      u8x8.clearLine(y);
    }
  }
}

void Display_Anzeige_Zustand_1() //Fertig
{
  Adresse(2);
  u8x8.setFont(u8x8_font_8x13B_1x2_f );
  u8x8.drawString(0, 0, "  Fahrzeug-");
  u8x8.drawString(0, 3, "  parameter");
  u8x8.drawString(0, 6, "  einstellen");
  Adresse(3);
  u8x8.setFont(u8x8_font_chroma48medium8_r );
  u8x8.drawString(0, 0, "Fahrzeugmasse:");
  u8x8.setFont(u8x8_font_px437wyse700a_2x2_f );
  u8x8.drawString(0, 4, String(Fahrzeugmasse).c_str());
  u8x8.drawString(12, 4, "kg");
  Adresse(4);
  u8x8.setFont(u8x8_font_chroma48medium8_r );
  u8x8.drawString(0, 0 , "Reibungs-");
  u8x8.drawString(0, 1 , "koefizient:");
  u8x8.setFont(u8x8_font_px437wyse700a_2x2_f );
  u8x8.drawString(0, 4, String(Reibungskoeffizient).c_str());
  if((millis()-Timer)>=30000)
  {
    Zustand=3;
  }
}

void Betriebsmoduseingabe() {
  while (Betriebsmodus == 0)
    if (digitalRead(Pin_Betriebsmodus_1) == HIGH) {
      delay(10);
      if (digitalRead(Pin_Betriebsmodus_1) == HIGH) {
        Betriebsmodus = 1;
        digitalWrite(Pin_LED_BM_1, HIGH);
        digitalWrite(Pin_LED_BM_2, LOW);
        digitalWrite(Pin_LED_BM_3, LOW);
      }
    } else if (digitalRead(Pin_Betriebsmodus_2) == HIGH) {
      delay(10);
      if (digitalRead(Pin_Betriebsmodus_2) == HIGH) {
        digitalWrite(Pin_LED_BM_1, LOW);
        digitalWrite(Pin_LED_BM_2, HIGH);
        digitalWrite(Pin_LED_BM_3, LOW);
        Betriebsmodus = 2;
      }
    } else if (digitalRead(Pin_Betriebsmodus_3) == HIGH) {
      delay(10);
      if (digitalRead(Pin_Betriebsmodus_3) == HIGH) {
        digitalWrite(Pin_LED_BM_1, LOW);
        digitalWrite(Pin_LED_BM_2, LOW);
        digitalWrite(Pin_LED_BM_3, HIGH);
        Betriebsmodus = 3;
      }
    }
  Timer=millis();
  Zustand = 1;
}
