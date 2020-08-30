float rps;
float rpm;
volatile byte pulses;
unsigned long timeold;
const byte numReadings = 10;

unsigned long Anzeige=0.0;  
unsigned long MW_C=0.0;  
unsigned long MW=0.0;
unsigned long RPM; 
unsigned long Drehzahl;  


int Summe_Abweichung=0;
int Soll_Wert;
int Ist_Wert;
int Abweichung;
int Abweichung_alt;
int Kd=0.17;
int  Stellgroese;
float  Kp=0.72;
unsigned int  Ki=0.72;
float  Ta=0.1;
unsigned int Laufrollen_w;
float Verzoegerung=-0.095;
unsigned long delta_t;
unsigned long t;
unsigned int Drehzahl_Laufrollen;
unsigned int PWM_Motor;
int Drehzahl_Soll;
int Drehzahl_Ist;

void counter()
{
  pulses++; 
  if (pulses >= 10) { 
    detachInterrupt(0);
    rpm = 1000000.0 / (micros() - timeold) * 60;
    timeold = micros();
    pulses = 0;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("CLEARDATA"); 
/*Löscht alle Daten die sich gerade im seriellen Monitor befinden*/
  Serial.println("Label,Uhrzeit,Zeit,Drehzahl"); 
/*Setzt die Überschriften für Übertragung*/
  Serial.println("RESETTIMER"); 
  attachInterrupt(0, counter, RISING); //attaching the interrupt and declaring the variables, one of the interrupt pins on Nano is D2, and has to be declared as 0 here
  pulses = 0;
  rps = 0;
  rpm = 0;
  timeold = 0;
  delay(1000);  
}


void loop() 
{
    if (rpm > 1500) {
      rpm = RPM;
    }
    else
    {
      RPM = rpm;
    }
    
if(micros()>=(timeold+500000))
{
  rpm=0;
}
Drehzahl=rpm;
Drehzahl_Ist=Drehzahl;
attachInterrupt(0, counter, RISING);
Serial.print("DATA,TIME,TIMER,");  
 Serial.println(Drehzahl); //Übertragung der Drehzahl 
  
delta_t=micros()-t;
Drehzahl_Soll=Drehzahl_Ist+Verzoegerung*delta_t*0.000001;
t=micros();
Soll_Wert=1200;//Drehzahl_Soll;
Ist_Wert=Drehzahl_Ist;
Abweichung = Soll_Wert - Ist_Wert;     
Summe_Abweichung = Summe_Abweichung + Abweichung;
  if (Summe_Abweichung < -400) {
    Summe_Abweichung = -400; //Begrenzung I-Anteil
  }
  if (Summe_Abweichung > 400) {
    Summe_Abweichung = 400;
  }
Stellgroese = Kp*Abweichung+Ki*Ta*Summe_Abweichung+Kd+Ki/Ta*(Abweichung_alt-Abweichung);
Abweichung_alt=Abweichung;

  if (Stellgroese < 0) {
    Stellgroese = 0; //Begrenzung Stellgröße
  }
  if (Stellgroese > 255) {
    Stellgroese = 255;
  }

PWM_Motor=Stellgroese;
analogWrite(10,PWM_Motor);
 /* Serial.print("Ist_Wert: ");
  Serial.print(Drehzahl_Ist);
  Serial.print("\tSoll_Wert: ");
  Serial.print(Drehzahl_Soll);
  Serial.print("\tAbweichung: ");
  Serial.print(Abweichung);  
  Serial.print("\tSumme_Abweichung: ");
  Serial.print(Summe_Abweichung);  
  Serial.print("\tPWM: ");
  Serial.print(PWM_Motor);
  Serial.print("\t delta s: ");
  Serial.println(delta_t);
*/


}
