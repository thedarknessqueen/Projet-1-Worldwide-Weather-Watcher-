#include <SPI.h>
#include <SD.h>

const int chipSelect = 4; // Pin pour la carte SD

// Structure pour un élément d'une liste chaînée
struct Node {
  int data;
  Node* next;
};

// Déclaration de la tête de la liste
Node* head = NULL;
File dataFile;

void setup() {
  Serial.begin(9600);
}

void setup() {
  Serial.begin(9600);

  // Initialisation de la carte SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Erreur : carte SD non détectée.");
    while (1); // Boucle infinie si la carte SD n'est pas présente
  }
  Serial.println("Carte SD initialisée.");

  // Création du fichier
  dataFile = SD.open("LISTE.TXT", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Erreur : impossible de créer le fichier.");
    while (1); // Boucle infinie si la création du fichier échoue
  }

  // Création et initialisation de la liste chaînée
  int valeurs[5] = {10, 20, 30, 40, 50}; // Valeurs de la liste
  createLinkedList(valeurs, 5);

  // Écriture de la liste dans le fichier SD
  storeListInSD();

  // Fermeture du fichier
  dataFile.close();
  Serial.println("Liste enregistrée avec succès sur la carte SD.");
}

// Fonction pour créer une liste chaînée avec des valeurs spécifiques
void createLinkedList(int* valeurs, int n) {
  Node* temp;

  for (int i = 0; i < n; i++) {
    // Allocation dynamique de mémoire pour un nouveau nœud
    Node* newNode = new Node;
    newNode->data = valeurs[i]; // Initialisation avec les valeurs données
    newNode->next = NULL;

    if (head == NULL) {
      head = newNode; // Le premier nœud devient la tête de la liste
    } else {
      temp = head;
      // Parcourir la liste jusqu'à la fin pour ajouter le nœud
      while (temp->next != NULL) {
        temp = temp->next;
      }
      temp->next = newNode;
    }
  }
}

// Fonction pour stocker la liste chaînée sur la carte SD
void storeListInSD() {
  Node* temp = head;

  while (temp != NULL) {
    // Écriture de chaque élément de la liste dans le fichier
    dataFile.print("Element: ");
    dataFile.println(temp->data);

    // Affichage des éléments dans le moniteur série
    Serial.print("Element: ");
    Serial.println(temp->data);

    temp = temp->next;
  }

  // Forcer l'écriture dans le fichier
  dataFile.flush();
}