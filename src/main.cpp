#include <Arduino.h>

#include <LiquidCrystal_I2C.h> // bibliotheque liquidCrystal I2C
#include <Wire.h>

#include <DHT.h>
#include <DHT_U.h> // bibliotheque DHT22
// #include <Adafruit_Sensor.h>

#include <Adafruit_ESP8266.h>

// configuration du capteur DHT22
#define DHTPIN A2     // initialise l'entre analogique
#define DHTTYPE DHT22 // defini le type de capteur

DHT dht(DHTPIN, DHTTYPE); //defini l'objet DHT
LiquidCrystal_I2C lcd(0x27, 16, 2);

//bouton arrosage manuelle
int buttonWatering = 2;

// definition des led
int red = 9;
int blue = 11;
int green = 10;

// definition de la pompe
int pompe = 13;

void manualWateringInterrupt()
{
  // temoin lumineux arrosage en cours
  digitalWrite(green, HIGH);
}

float soilData() // retourne l'humidité du sol
{
  float soilMoisture; // donné traitée
  float soilRead = analogRead(A1);
  return soilMoisture = 100 - ((soilRead * 100) / 642);
}

void viewData(float tempExt, float humExt, float humSoil)
{
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("temperature :");
  lcd.setCursor(0, 1);
  lcd.print(tempExt);
  lcd.print(" deg C");
  delay(1000);

  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("hum ambiant :");
  lcd.setCursor(0, 1);
  lcd.print(humExt);
  lcd.print(" %");
  delay(1000);

  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("hum sol :");
  lcd.setCursor(0, 1);
  lcd.print(humSoil);
  lcd.print(" %");
  delay(1000);
}

void automaticWatering(float soilMoisture)
{
  while (soilMoisture <= 20) // humidité du sol minimal en %
  {
    digitalWrite(green, LOW); // humidité basse
    digitalWrite(red, HIGH);  // terre seche
    digitalWrite(pompe, LOW); // pompe allumé
    digitalWrite(blue, HIGH); // arrosage en cour
    soilMoisture = soilData();
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.print("hum sol :");
    lcd.setCursor(0, 1);
    lcd.print(soilMoisture);
    lcd.print(" %");
  }
  digitalWrite(blue, LOW);   // eteint led bleu
  digitalWrite(red, LOW);    // eteint led rouge
  digitalWrite(pompe, HIGH); // pompe eteinte humiditée ok
}

void setup()
{
  // initialisation bouton poussoir
  pinMode(buttonWatering, INPUT_PULLUP);
  //mise en route du capteur DHT22
  dht.begin();

  pinMode(blue, OUTPUT);  // LED arrosage auto en cours (bleu)
  pinMode(green, OUTPUT); // LED arrosage manuelle en cours (vert)
  pinMode(red, OUTPUT);   // LED terre seche (rouge)

  // initialisation de l'affiche LCD
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Arrosage Auto");

  // interruption pour arrosage manuel
  attachInterrupt(0, manualWateringInterrupt, CHANGE);
  // initialisation de la sortie du relais
  pinMode(pompe, OUTPUT);

  // mise a l'etat initiale de toutes les pins numerique
  digitalWrite(pompe, HIGH);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);

  //setup ESP8266
}

void manualWatering()
{
  digitalWrite(green, HIGH);
  // declenchement du relais de la pompe
  digitalWrite(pompe, LOW);
  // arrosage 10 s
  delay(10000);
  digitalWrite(pompe, HIGH);
  digitalWrite(green, LOW);
}

void loop()
{
  if (digitalRead(green) == HIGH)
  {
    manualWatering();
  }

  // on recupere la temperature de l'air
  float dtempExt = dht.readTemperature();

  // on recupere l'humidite de l'air
  float dhumidExt = dht.readHumidity();

  // on recupere l'humiditée du sol
  float soilDataMoisture = soilData();

  // affichage des données
  viewData(dtempExt, dhumidExt, soilDataMoisture);
  automaticWatering(soilDataMoisture);
}
