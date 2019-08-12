/*
Machine à Thé 2019
Franckapik
*/

//LIBRAIRIES

#include <Servo.h> //servomoteurs
#include <LiquidCrystal.h> //affichage
#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS A0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//VARIABLES

//start
boolean commande=0;

//relais
int pint=8;
int pext=7;
int thermop=6;

//capteurs
int sondetemp=A0;
int niveaumin=10;
int niveaumax=11;

//interrupteurs;
int remplissage=A4;
int chauffage=A3;
int the=A2;
int onoff=A1;

//actionneurs
Servo cadran;
Servo roue;

//afficheur
LiquidCrystal lcd(4,12,0,1,2,3);
byte backlight_pwm = 13; //retroeclairage
enum {
  none,  /*!< Pas de bouton appuyé */
  up,
  down,
  left,
  right,
  bselect,
  chauf,
  remp,
  the1
};

//paramètres
int remplissagemax=10; //ms
int tmax=60; //°C
int cadranmin=15;
int cadranmax=100;
int dosagethe=500; //temps = dosage du thé (roue)
int volumetasse=5000; //ms


//fonctions

float getTemperature() //Capture de la température
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

int setCadran(int temp) //Affichage température sur le cadran
{
  int aiguille=0;
  aiguille=180-(temp*180/cadranmax);
  lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
  lcd.print(aiguille);
  cadran.write(aiguille);
}

versagethe(int the) { //Rotation de la roue en fonction du thé selectionné
  lcd.setCursor(0,1);
  lcd.print("Falling tea");
  delay(1000);
  lcd.clear();
  roue.attach(5);
  if (the) {
    roue.write(0);
    delay(dosagethe);
  } else {
    roue.write(180);
    delay(dosagethe);
  }
  roue.write(90);
}

versageeau(){
  lcd.setCursor(0,1);
  lcd.print("Tea preparation");
  delay(1000);
  lcd.clear();
  digitalWrite(pint, HIGH);
  lcd.setCursor(0,1);
  lcd.print("Check your cup!");
  delay(volumetasse);
  lcd.clear();
  digitalWrite(pint, LOW);
  lcd.setCursor(0,1);
  lcd.print("Tea is ready!");
}

byte getPressedButton() { //Pad du LCD

  int value = analogRead(A5);

  if (value < 50)
    return right;
  else if (value < 250)
    return up;
  else if (value < 450)
    return down;
  else if (value < 650)
    return left;
  else if (value < 850)
    return bselect;
  else
    return none;
}

void debug() {
  digitalWrite(pint, HIGH);
  delay(1000);
  digitalWrite(pint, LOW);
  digitalWrite(thermop, HIGH);
  delay(1000);
  digitalWrite(thermop, LOW);
  digitalWrite(pext, HIGH);
  delay(1000);
  digitalWrite(pext, LOW);

}

//SETUP

void setup() {
     // pin setup
     pinMode(pext, OUTPUT);
     pinMode(pint, OUTPUT);
     pinMode(thermop, OUTPUT);
     pinMode(sondetemp, INPUT);
     pinMode(niveaumin, INPUT_PULLUP);
     pinMode(niveaumax, INPUT_PULLUP);
     pinMode(chauffage,  INPUT_PULLUP);
     pinMode(remplissage,  INPUT_PULLUP);
     pinMode(the,  INPUT_PULLUP);
     pinMode(onoff,  INPUT_PULLUP);

     //lcd

     lcd.begin(16, 2);              // start the library

     //temperature
     sensors.begin();
}

void loop() {

//states
int chauf_int=digitalRead(chauffage);
int remp_int=digitalRead(remplissage);
int the_int=digitalRead(the);
boolean niveaumin_int=digitalRead(niveaumin);
boolean niveaumax_int=digitalRead(niveaumax);


//affichage des variables
lcd.setCursor(1,0);            // move cursor to second line "1" and 9 spaces over
lcd.print(digitalRead(niveaumin));
lcd.setCursor(3,0);            // move cursor to second line "1" and 9 spaces over
lcd.print(digitalRead(niveaumax));
lcd.setCursor(5,0);
lcd.print(digitalRead(chauffage));
lcd.setCursor(7,0);
lcd.print(digitalRead(remplissage));
lcd.setCursor(9,0);
lcd.print(digitalRead(the));
lcd.setCursor(11,0);
lcd.print(digitalRead(onoff));
lcd.setCursor(14,0);
lcd.print(getTemperature());


if(!digitalRead(onoff)) { //demande de thé
  if(digitalRead(remplissage)) { //interrupteur remplissage sur auto
    lcd.setCursor(0,1);
    lcd.print("Auto Fill");
    delay(2000);
    lcd.clear();

    if (!digitalRead(niveaumin)) {
      while(!digitalRead(niveaumax)){ //le niveau est pas au max
        digitalWrite(pext, HIGH); //allumage de la pompe jerrican
        lcd.setCursor(0,1);
        lcd.print("Filling");
      }
      digitalWrite(pext, LOW); //extinction de la pompe jerrican
    }
  } else {
    lcd.setCursor(0,1);
    lcd.print("No Fill");
    delay(2000);
    lcd.clear();
  }

  if(digitalRead(chauffage)) { //interrupteur chauffage sur auto
    lcd.setCursor(0,1);
    lcd.print("Auto Heat");
    delay(1000);
    lcd.clear();

      while(getTemperature() < tmax) { //température insuffisante
        digitalWrite(thermop, HIGH); //allumage du thermoplongeur
        lcd.setCursor(0,1);
        lcd.print(String(getTemperature())+String("-->") + String(tmax));
        delay(2000);
      }
      digitalWrite(thermop, LOW);
  } else {
    lcd.setCursor(0,1);
    lcd.print("No Heat");
    delay(1000);
    lcd.clear();
  }

  versagethe(the_int);
  versageeau();

  commande=0;
}

//Pas de thé en commande

  lcd.setCursor(0, 1);
  switch (getPressedButton()) {
  case none:
    lcd.print("");
    break;

  case up:
    ++tmax;
    lcd.print((String(tmax)));
    break;

  case down:
    --tmax;
    lcd.print((String(tmax)));
    break;

  case left:
  lcd.print("MORE");
  delay(500);
  lcd.clear();
  versagethe(digitalRead(the));
    break;

  case right:
    lcd.print("DEBUG");
    debug();
    break;

  case bselect:

    break;
}
}
