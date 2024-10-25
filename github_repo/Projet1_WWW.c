///////////////// PROJET WORLDWIDE WEATHER WATCHER //////////////////////

///////////////////////// LIBRAIRIES/////////////////////////////////////

#include <stdio.h>//gestion du programme (Print)
#include <string.h>//gestion des variables (Char, Listes...)
#include <stdlib.h>//gestion du programme
#include <Wire.h>//gestion du programme (Connection  filaire avec des capteurs)
#include <SD.h>//carte SD
#include <SPI.h>//carte SD
#include <RTClib.h>//horloge
#include <Adafruit_Sensor.h>//capteur de température, humidité, pression
#include <Adafruit_BME280.h>//capteur de température, humidité, pression
#include <SoftwareSerial.h>//capteur gps
#include <TinyGPS++.h>//capteur gps

//////////// DECLARATION DES Capteurs ////////////

// Définir la broche où le capteur est connecté
RTC_DS1307 rtc;
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C pour le capteur BME280
TinyGPSPlus gps;            
SoftwareSerial ss(4, 3);

//////DECLARATION DES VARIABLES, ENTREES ET SORTIES, CONSTANTES ////////

// Définition des constantes
const int led_verte = 5;
const int led_jaune = 6;
const int led_rouge = 7;
const int led_bleue = 9;
const int bouton_A = 3;
const int bouton_B = 2;
const int carteSD = 4; 
// Définition des variables
bool etatbouton_A = false;
bool etatbouton_B = false;
// Variables des boucles
int i = 0;
bool newMode = false;
unsigned long lastInterrupt = 0;    //Temps de la denrière Interruption
unsigned long lastArchiveTime = 0;  // Temps de la dernière archive sur la carte SD
unsigned long previousMillis = 0;  // Temps de la dernière mesure de capteur  
// Version du système
const char* version = "Clouduino";
// Intervalle entre les mesures
int inter = 3000;
// Temps avant archivage
int TeAA = 30000;
// Taille avant archivage 
int TaAA = 6;
// Déclaration des capteurs
float capteur_1;
float capteur_2;
float capteur_3;
float capteur_4;

///// Tableau de stockage des mesures sous forme de listes chaînées /////

// Mesures du capteur GPS
typedef struct Liste_C1 {
    float gps;
    struct Liste_C1 *suivant1;
} Liste_C1;
// Mesures du capteur de température
typedef struct Liste_C2 {
    float temp;
    struct Liste_C2 *suivant2;
} Liste_C2;
// Mesures du capteur d’humidité
typedef struct Liste_C3 {
    float humi;
    struct Liste_C3 *suivant3;
} Liste_C3;
// Mesures du capteur de pression
typedef struct Liste_C4 {
    float pres;
    struct Liste_C4 *suivant4;
} Liste_C4;
//Pointeurs sur les listes
Liste_C1* tete1 = NULL;
Liste_C2* tete2 = NULL;
Liste_C3* tete3 = NULL;
Liste_C4* tete4 = NULL;

////////////////////////////// FONCTION //////////////////////////////
void interrupt() {
    if (millis() - lastInterrupt > 200) {
        lastInterrupt = millis();
        newMode = true;
    }
}
void choix() {
    if (newMode) {
        bool etatbouton_A = digitalRead(bouton_A) == LOW;
        bool etatbouton_B = digitalRead(bouton_B) == LOW;
        if (i==0){
          if (etatbouton_A && etatbouton_B) {
            i = 3; // Mode économique
            Serial.println(F("Vous êtes en mode économique."));
          }
          else if (etatbouton_A) {
            i = 1; // Mode configuration
            Serial.println(F("Vous êtes en mode configuration."));
            Serial.println(F("Entrez 1 pour modifier l’heure de la station météo."));
            Serial.println(F("Entrez 2 pour modifier l’intervalle."));
            Serial.println(F("Entrez 3 pour reset au mode par défaut."));
            Serial.println(F("Entrez 4 pour modifier l'archivage."));
            Serial.println(F("Entrez 5 pour afficher la version."));
          }
          else if (etatbouton_B) {
            i = 2; // Mode maintenance
            Serial.println(F("Vous êtes en mode maintenance."));
          } 
        }
        else {
            i = 0; // Mode standard
            Serial.println(F("Vous êtes de retour en mode standard."));
        }
    newMode = false;
    }
}
void recup_donnees() {
    unsigned long current2Millis = millis();
    int interbis;
    if (i == 3) {  // Ajuster l'intervalle en fonction du mode
        interbis = inter * 4;
    } else {
        interbis = inter;
    }
    if (current2Millis - previousMillis >= interbis) {  // Si l'intervalle est atteint, effectuer la mesure du capteur
        previousMillis = current2Millis;  // Mettre à jour l'heure de la dernière mesure
        capteur_2 = bme.readTemperature();
        capteur_3 = bme.readHumidity();
        capteur_4 = bme.readPressure();
        while (ss.available() > 0) {
            gps.encode(ss.read());
            if (gps.location.isUpdated()) {
                Serial.print(F("Latitude: "));
                capteur_1 = gps.location.lat();
                Serial.println(capteur_1, 6);    // Affiche la latitude 
                Liste_C1* elt1 = (Liste_C1*)malloc(sizeof(Liste_C1));  // Alloue de la mémoire pour un nouvel élément
                elt1->gps = capteur_1;
                elt1->suivant1 = NULL;

                if (tete1 == NULL) {
                    tete1 = elt1;  // Si la liste est vide, le nouvel élément devient la tête
                } else {
                    Liste_C1* current1 = tete1;
                    while (current1->suivant1 != NULL) {  // Parcourt la liste jusqu'au dernier élément
                        current1 = current1->suivant1;
                    }
                    current1->suivant1 = elt1;  // Ajoute le nouvel élément à la fin
                }
                Serial.print(F("Longitude: "));
                capteur_1 = gps.location.lng();
                Serial.println(capteur_1, 6);    // Affiche la longitude
            }
        }
        // Optionnel: Si tu veux afficher un message lorsque la position GPS n'est pas encore disponible
        if (!gps.location.isValid()) {
            capteur_1 = 0;
            Serial.println(F("Coordonnées GPS : Introuvables. Archivage NA. ")); 
        }
        // Afficher la température actuelle sur le moniteur série
        Serial.print(F("Température actuelle : "));
        Serial.print(capteur_2);
        Serial.println(F("°C"));
        Serial.print(F("Humidité actuelle : "));
        Serial.print(capteur_3);
        Serial.println(F("%"));
        Serial.print(F("Pression actuelle : "));
        Serial.print(capteur_4);
        Serial.println(F("Pa"));
        // Ajoute la nouvelle longitude dans la liste chaînée
        Liste_C1* elt1 = (Liste_C1*)malloc(sizeof(Liste_C1));  // Alloue de la mémoire pour un nouvel élément
        elt1->gps = capteur_1;
        elt1->suivant1 = NULL;

        if (tete1 == NULL) {
          tete1 = elt1;  // Si la liste est vide, le nouvel élément devient la tête
        } else {
          Liste_C1* current1 = tete1;
          while (current1->suivant1 != NULL) {  // Parcourir la liste jusqu'au dernier élément
            current1 = current1->suivant1;
          }
        current1->suivant1 = elt1;  // Ajoute le nouvel élément à la fin
        }
        // Ajoute la nouvelle température dans la liste chaînée
        Liste_C2* elt2 = (Liste_C2*)malloc(sizeof(Liste_C2));  // Alloue de la mémoire pour un nouvel élément
        elt2->temp = capteur_2;
        elt2->suivant2 = NULL;
        if (tete2 == NULL) {
          tete2 = elt2;  // Si la liste est vide, le nouvel élément devient la tête
        } else {
          Liste_C2* current2 = tete2;
          while (current2->suivant2 != NULL) {  // Parcourir la liste jusqu'au dernier élément
            current2 = current2->suivant2;
          }
        current2->suivant2 = elt2;  // Ajoute le nouvel élément à la fin
        }
      // Ajoute la nouvelle humidité dans la liste chaînée
        Liste_C3* elt3 = (Liste_C3*)malloc(sizeof(Liste_C3));  // Alloue de la mémoire pour un nouvel élément
        elt3->humi = capteur_3;
        elt3->suivant3 = NULL;

        if (tete3 == NULL) {
          tete3 = elt3;  // Si la liste est vide, le nouvel élément devient la tête
        } else {
          Liste_C3* current3 = tete3;
          while (current3->suivant3 != NULL) {  // Parcourir la liste jusqu'au dernier élément
            current3 = current3->suivant3;
          }
        current3->suivant3 = elt3;  // Ajoute le nouvel élément à la fin
        }
        // Ajoute la nouvelle pression dans la liste chaînée
        Liste_C4* elt4 = (Liste_C4*)malloc(sizeof(Liste_C4));  // Alloue de la mémoire pour un nouvel élément
        elt4->pres = capteur_4;
        elt4->suivant4 = NULL;

        if (tete4 == NULL) {
          tete4 = elt4;  // Si la liste est vide, le nouvel élément devient la tête
        } else {
          Liste_C4* current4 = tete4;
          while (current4->suivant4 != NULL) {  // Parcourir la liste jusqu'au dernier élément
            current4 = current4->suivant4;
          }
        current4->suivant4 = elt4;  // Ajoute le nouvel élément à la fin
        }
    }
}
void stock_donnes() {
    unsigned long currentMillis = millis();
    if (checkSize(tete1) >= TaAA ||checkSize(tete2) >= TaAA || checkSize(tete3) >= TaAA || checkSize(tete4) >= TaAA ||
        (currentMillis - lastArchiveTime) >= TeAA) {  // Vérifier si la taille d'une des listes dépasse TaAA ou si TeAA temps est écoulé
        File dataFile = SD.open("datalog.txt", FILE_WRITE);  // Ouvre le fichier sur la carte SD pour y écrire
        DateTime now = rtc.now();
        if (dataFile) {
            // Écrire les données gps
            Liste_C1* current1 = tete1;
            while (current1 != NULL) {
                dataFile.print("Date/Heure: ");
                dataFile.print(now.year(), DEC);
                dataFile.print('/');
                dataFile.print(now.month(), DEC);
                dataFile.print('/');
                dataFile.print(now.day(), DEC);
                dataFile.print(" - ");
                dataFile.print(now.hour(), DEC);
                dataFile.print(':');
                dataFile.print(now.minute(), DEC);
                dataFile.print(':');
                dataFile.print(now.second(), DEC);
                dataFile.print(" | GPS lat, long : ");
                dataFile.println(current1->gps);
                current1 = current1->suivant1;
            }
            // Écrire les données de température
            Liste_C2* current2 = tete2;
            while (current2 != NULL) {
                dataFile.print("Date/Heure: ");
                dataFile.print(now.year(), DEC);
                dataFile.print('/');
                dataFile.print(now.month(), DEC);
                dataFile.print('/');
                dataFile.print(now.day(), DEC);
                dataFile.print(" - ");
                dataFile.print(now.hour(), DEC);
                dataFile.print(':');
                dataFile.print(now.minute(), DEC);
                dataFile.print(':');
                dataFile.print(now.second(), DEC);
                dataFile.print(" | Température : ");
                dataFile.println(current2->temp);
                current2 = current2->suivant2;
            }
            // Écrire les données d'humidité
            Liste_C3* current3 = tete3;
            while (current3 != NULL) {
                dataFile.print("Date/Heure: ");
                dataFile.print(now.year(), DEC);
                dataFile.print('/');
                dataFile.print(now.month(), DEC);
                dataFile.print('/');
                dataFile.print(now.day(), DEC);
                dataFile.print(" - ");
                dataFile.print(now.hour(), DEC);
                dataFile.print(':');
                dataFile.print(now.minute(), DEC);
                dataFile.print(':');
                dataFile.print(now.second(), DEC);
                dataFile.print(" | Humidité : ");
                dataFile.println(current3->humi);
                current3 = current3->suivant3;
            }
            // Écrire les données de pression
            Liste_C4* current4 = tete4;
            while (current4 != NULL) {
                dataFile.print("Date/Heure: ");
                dataFile.print(now.year(), DEC);
                dataFile.print('/');
                dataFile.print(now.month(), DEC);
                dataFile.print('/');
                dataFile.print(now.day(), DEC);
                dataFile.print(" - ");
                dataFile.print(now.hour(), DEC);
                dataFile.print(':');
                dataFile.print(now.minute(), DEC);
                dataFile.print(':');
                dataFile.print(now.second(), DEC);
                dataFile.print(" | Pression : ");
                dataFile.println(current4->pres);
                current4 = current4->suivant4;
            }
            dataFile.close();
            Serial.println(F("Données écrites sur la carte SD."));
        } else {
            Serial.println(F("Erreur d'ouverture du fichier sur la carte SD."));
        }
        clearList1(&tete1);
        clearList2(&tete2);
        clearList3(&tete3);
        clearList4(&tete4);
        lastArchiveTime = currentMillis;
    }
}
int checkSize(void* head) {  // Ccalcule la taille d'une liste chaînée
    int size = 0;
    Liste_C1* current = (Liste_C1*)head;
    while (current != NULL) {
        size += sizeof(Liste_C1);  // Ajoute la taille de chaque élément
        current = current->suivant1;
    }
    return size;
}
void clearList1(Liste_C1** head) {  // Vide une liste chaînée
    Liste_C1* current = *head;
    while (current != NULL) {
        Liste_C1* next = current->suivant1;
        free(current);  // Libére la mémoire de chaque élément
        current = next;
    }
    *head = NULL;  // Réinitialise la tête de la liste
}
void clearList2(Liste_C2** head) {  // Vide une liste chaînée
    Liste_C2* current = *head;
    while (current != NULL) {
        Liste_C2* next = current->suivant2;
        free(current);  // Libére la mémoire de chaque élément
        current = next;
    }
    *head = NULL;  // Réinitialise la tête de la liste
}
void clearList3(Liste_C3** head) {  // Vide une liste chaînée
    Liste_C3* current = *head;
    while (current != NULL) {
        Liste_C3* next = current->suivant3;
        free(current);  // Libére la mémoire de chaque élément
        current = next;
    }
    *head = NULL;  // Réinitialise la tête de la liste
}
void clearList4(Liste_C4** head) {  // Vide une liste chaînée
    Liste_C4* current = *head;
    while (current != NULL) {
        Liste_C4* next = current->suivant4;
        free(current);  // Libére la mémoire de chaque élément
        current = next;
    }
    *head = NULL;  // Réinitialise la tête de la liste
}
int modif_inter() {
    Serial.println(F("Quelle intervalle de temps entre laquisition des données souhaitez vous ?"));
    inter = Serial.read() - '0';
}
int reset() {  // Réinitialisation des paramètres par défaut
    inter = 3000;
    TeAA = 10000;
    TaAA = 5;
    return 0;
}
int modifi_heure(int annee,int mois,int jour,int heure, int min,int sec){  // Modification de l'heure
    rtc.adjust(DateTime(annee, mois, jour, heure, min, sec)); 
}
int temps_AV_archi() {  // Modification du temps avant archivage
    Serial.println(F("Quelle intervalle de temps entre l'archivage des données souhaitez vous ?"));
    TeAA = Serial.read() - '0';
}
int taille_AV_archi() {  // Modification de la taille avant archivage
    Serial.println(F("Quelle intervalle de taille entre l'archivage des données souhaitez vous ?"));
    TaAA = Serial.read() - '0';
}

///////////////// VOID SETUP INITIALISATION DU SYSTÈME /////////////////

void setup() {
    Serial.begin(9600);
    ss.begin(9600); 
    pinMode(led_verte, OUTPUT);
    pinMode(led_jaune, OUTPUT);
    pinMode(led_rouge, OUTPUT);
    pinMode(led_bleue, OUTPUT);
    pinMode(bouton_A, INPUT_PULLUP);
    pinMode(bouton_B, INPUT_PULLUP);
    if (!rtc.begin()) {
    Serial.println(F("RTC non trouvée"));
    while (1);  // Boucle infinie en cas d'erreur pour éviter de continuer
    }
    if (!rtc.isrunning()) {  // Vérifiez que l'horloge est bien défini
      Serial.println(F("RTC non fonctionnelle. "));
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    if (!SD.begin(carteSD)) {  // Vérifiez que la carteSD est bien défini
        Serial.println(F("Erreur d'initialisation de la carte SD !"));
        while (1);  // Boucle infinie en cas d'erreur pour éviter de continuer
    } else {
        Serial.println(F("Carte SD initialisée avec succès."));
    }
    if (!bme.begin(0x76)) {  // Vérifiez que le capteur est bien défini
    Serial.println(F("Capteur BME280 invalide"));
    while (1);   // Boucle infinie en cas d'erreur pour éviter de continuer
    }
    Serial.println(F("Vous avez été log au mode Standard"));
    // Setup des interruptions 
    attachInterrupt(digitalPinToInterrupt(bouton_A), interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(bouton_B), interrupt, CHANGE);
}
////////////////////// VOID LOOP, CODE PRINCIPAL ///////////////////////
void loop() {
    choix(); 
        if(i == 0){   //standard
            digitalWrite(led_verte, HIGH); // Allumer la LED verte
            digitalWrite(led_jaune, LOW);
            digitalWrite(led_rouge, LOW);
            digitalWrite(led_bleue, LOW);
            recup_donnees(); // Appel de la fonction
            stock_donnes(); // Appel de la fonction
        }
        if(i==1){  //configuration
            digitalWrite(led_jaune, HIGH); // Allumer la LED jaune
            digitalWrite(led_rouge, LOW);
            digitalWrite(led_bleue, LOW);
            digitalWrite(led_verte, LOW);
            recup_donnees(); // Appel de la fonction
            stock_donnes(); // Appel de la fonction
            int confi = Serial.read() - '0'; 

            if (confi == 1) {
                modif_heure(); // Appel de la fonction
            }
            if (confi == 2) {
                modif_inter(); // Appel de la fonction
            }
            if (confi == 3) {
                reset(); // Appel de la fonction
            }
            if (confi == 4) {
                Serial.println(F("Entrez 1 pour modifier le temps avant l'archivage."));
                Serial.println(F("Entrez 2 pour modifier la taille avant l'archivage."));
                    int archi = Serial.read() - '0'; 

                    if (archi == 1) {
                        temps_AV_archi(); // Appel de la fonction
                    }
                    if (archi == 2) {
                        taille_AV_archi(); // Appel de la fonction
                    }
            }
            if (confi == 5) {
             Serial.println(version); // Affiche la version
            }
        }
        if(i == 2){ //maintenance
            digitalWrite(led_rouge, HIGH); // Allumer la LED rouge
            digitalWrite(led_bleue, LOW);
            digitalWrite(led_verte, LOW);
            digitalWrite(led_jaune, LOW);
            recup_donnees(); // Appel de la fonction
            clearList1(&tete1); // Appel de la fonction
            clearList2(&tete2); // Appel de la fonction
            clearList3(&tete3); // Appel de la fonction
            clearList4(&tete4); // Appel de la fonction
        }
        if(i==3){   //economique
            digitalWrite(led_bleue, HIGH); // Allumer la LED bleue
            digitalWrite(led_verte, LOW);
            digitalWrite(led_jaune, LOW);
            digitalWrite(led_rouge, LOW);
            recup_donnees(); // Appel de la fonction
            stock_donnes(); // Appel de la fonction
        }
}