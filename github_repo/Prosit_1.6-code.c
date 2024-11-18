#define LEDPIN 7
#define PUSHPIN 8

int randNumber = 0;
int data;

const int led = 9;
const int inter = 2;

volatile bool bascule = false;

void acquisition(){
  randNumber = random(3, 7); // Je tire un nombre au hasard entre 3 et 6
  Serial.println(randNumber); // Délai du nombre de secondes simulant l'acquisition de données
  digitalWrite(led, LOW); // Eteindre la LED après acquisition
}

void basculement(){
  bascule = !bascule;
  Serial.println("Interruption");
  if (bascule) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}

void setup(){
  randomSeed(analogRead()); // Initialisation générateur de nombres aléatoires
  pinMode(LEDPIN, OUTPUT); // Initialisation LED
  pinMode(PUSHPIN, INPUT); // Initialisation bouton
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Démarrage");
  pinMode(led, OUTPUT);
  pinMode(inter, INPUT_PULLUP); // Activer l'interruption sur la broche inter
  attachInterrupt(digitalPinToInterrupt(inter), basculement, FALLING);
}

void loop(){
  // Le code principal est géré dans l'interruption
  delay(randNumber * 1000);
  acquisition();
}