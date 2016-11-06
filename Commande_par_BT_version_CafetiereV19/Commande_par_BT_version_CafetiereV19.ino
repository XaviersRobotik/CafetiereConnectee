/////////////////////////////////
//   Cafetiere connectée en BT   /
//   Version 1.9                /
//   Du 19/05/2016               /
//   Auteur Xavier DUVERT        / 
//////////////////////////////////

#include <SoftwareSerial.h>

//#define DEBUG OUI

#define CAFETIERE 10 // Broche du SSR
#define LED 13 // Broche de la led

#define ECHO 7 // Broche US Echo
#define TRIGGER 8 // Broche US Trigger 

#define Reservoir_fond 17 // Hauteur du reservoir en CM
#define Reservoir_couvercle  2 // Hauteur d'eau Maxi

char myChar,Code_Recu;
boolean Code2,Etat_Cafetiere,ParNombredeTasse;
String Chaine, Temp;
unsigned int NbTasse,NbTasseDemande = 10;
unsigned long Temps;

SoftwareSerial BTModule(2, 4); // RX, TX

int Niveau_Eau()
{
  unsigned long duration, distance; // Duration used to calculate distance
  
  digitalWrite(TRIGGER, LOW); // init US 
  delayMicroseconds(2); 

  digitalWrite(TRIGGER, HIGH); // init US
  delayMicroseconds(10); 
 
  digitalWrite(TRIGGER, LOW); // recuperation distance
  duration = pulseIn(ECHO, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 
 #ifdef DEBUG
 Serial.print("Distance : ");
 Serial.println(distance);
 #endif
 
 NbTasse = map(distance,Reservoir_fond, Reservoir_couvercle,0,10); // Calcul Niveau d'eau

#ifdef DEBUG
Serial.print("Nb Tasses ");
Serial.println(NbTasse);
delay(2000);
#endif
return NbTasse;

}



// Fonction de decodage des parametres de l'application 
boolean Recherche(String ADecoder)
{ 
  boolean Flag = true;
  ParNombredeTasse = false;
  
  #ifdef DEBUG
  Serial.print("Valeur A decoder : ");  
  Serial.println(ADecoder);
  #endif
   
  if(ADecoder.indexOf("On") >0) { // si code CafetiereOn reçu
    // Cafetiere On a été reçu;
    Flag = true;
    Code_Recu = 'M';
    }
    
   if(ADecoder.indexOf("Off") >0) { // si code CafetiereOff reçu
    // Cafetiere Off a été reçu;
    Flag = true;
    Code_Recu = 'A';
    }
  
  if(ADecoder.indexOf("Tasses") >0) { // si code Tasses reçu
    // La commande Tasses a été reçu;
    Flag = true;
    int taille = ADecoder.length();
    Temp = ADecoder.substring(taille - 1); // recuperation des chiffres apres #Tasses
    NbTasseDemande = Temp.toInt(); // convertion en chiffre
    if (NbTasseDemande == 0 )
      NbTasseDemande = 10;
    Code_Recu = 'T';
    }
 
  #ifdef DEBUG
  Serial.print("Code_Recu : ");
  Serial.println(Code_Recu);
   
  Serial.print("Nb de tasse demandées : ");
  Serial.println(NbTasseDemande);
 
  #endif
  
  return Flag;
}

// Fonction de modification de l'atat des broches
void ActionPin(boolean Valeur)
{  
     digitalWrite(CAFETIERE,Valeur); // Broche du SSR
     digitalWrite(LED,Valeur); // Pour controler l'etat de la cafetiere.
     #ifdef DEBUG 
     Serial.print("Passage de la broche ");
     Serial.print(CAFETIERE); 
     Serial.print(" a la valeur ");  
     Serial.println(Valeur);
     #endif
}

void CafePourNTasses(int NbTasserequises)
{
  int NbTassealafin;
  Niveau_Eau(); // mise a jour du niveau d'eau
  
  #ifdef DEBUG
  Serial.println("CafePourNTasses");
  #endif 
  
  //Calcule Nb de tasse restante apres requete
  NbTassealafin = NbTasse -  NbTasserequises;
 
  if (NbTassealafin > 0) //si il y a assez d'eau
  {
      AllumeCafetiere(); // allume la cafetiere
      while(NbTasse > NbTassealafin) // tant que le nombre de tasse n'est pas atteint
      {
          BTModule.print("Café en cours pour ");
          BTModule.print(NbTasserequises);
          BTModule.print(" tasse(s)");
          Niveau_Eau(); // mise a jour du niveau d'eau
          delay(2000);
      }
      EteintCafetiere(); // Eteint la cafetiere
      
  }
  else // si il n'y a pas assez d'eau
  {
    BTModule.print("Pas assez d'eau pour ");
    BTModule.print(NbTasserequises);
    BTModule.print(" tasse(s)");
  }
}

void AllumeCafetiere()
{
  if(Niveau_Eau() >= 1 && Etat_Cafetiere == true)
  { // on peut mettre en marche si le niveau d'eau est bon
      ActionPin(HIGH);
      Etat_Cafetiere = false;
  }
  #ifdef DEBUG
  Serial.println("AllumeCafetiere");
  #endif  
}

void EteintCafetiere()
{
  // pour l'arret on n'a pas besoin de vérifier le niveau d'eau
  ActionPin(LOW); // Arret de la cafetiere
  Etat_Cafetiere = true;
  #ifdef DEBUG
  Serial.println("EteintCafetiere");
  #endif
}
  
  
void MiseAJourDesTasses()
{
  BTModule.print(NbTasseDemande);
  BTModule.print(" tasses demandées");
  #ifdef DEBUG
  Serial.println("MiseAJourDesTasses");
  #endif
}

void AfficheNbdeTasseDispo()
{
  BTModule.print(" "); // Effacemnt de la case
  BTModule.print("Nb Tasses ");
  BTModule.print(NbTasse); // Envoi du niveau d'eau
}


void setup()
{
    #ifdef DEBUG
    Serial.begin(9600);
    #endif
    
    pinMode(CAFETIERE,OUTPUT); // pin du relay de la cafetiere
    pinMode(LED,OUTPUT); // LED de vérification
        
    pinMode(TRIGGER, OUTPUT); // Configuration des broches du capteur US
    pinMode(ECHO, INPUT);
    
    EteintCafetiere(); // Pour etre sur qu'au démarrage de l'arduino la cafetiere est arretée.
    
    //BTModule.begin(38400);
    BTModule.begin(9600);
    delay(1000);
    
    Temps =  millis() + 2000; // calcul de la pause
    while (Temps < millis() ) // pause
    {
        BTModule.print("Cafetiére a vos ordres !");
        delay(100);   
    }
    
    
    Niveau_Eau(); // calcul le niveau d'eau  
    AfficheNbdeTasseDispo(); // affiche sur le téléphone le nombre de tasses dans le reservoir
    
}

void loop() // tout le temps
{
    Niveau_Eau(); // montre le niveau d'eau  
    
    while ( BTModule.available() ) // reception des commandes BT
    { 
        myChar = (char)BTModule.read(); // reception des caractere du BT
        Chaine.concat(myChar); // concatenation de carateres recus
        #ifdef DEBUG
        Serial.print("caractere recu : ");
        Serial.println(myChar);
        #endif
        
    }
    // tous les caracteres ont été reçu
    #ifdef DEBUG
    Serial.println("");
    #endif
          if ( Recherche(Chaine) )
          {
              #ifdef DEBUG
              Serial.print("Chaine trouvee : ");
              Serial.println(Code_Recu);
              #endif
              
              switch (Code_Recu) // en fonction de la commande reçcue
              {
                    case 'M' : CafePourNTasses(NbTasseDemande); // Marche
                    break;
                    case 'A' : EteintCafetiere(); // arret
                    break;
                    case 'T' : MiseAJourDesTasses(); // Mise a jour du nb de tasse a l'ecran du telephone
                    break;
                    default:
                    break;
              }
          }
          Chaine = ""; // drop de l'ancienne chaine
          Code_Recu = ' '; // reset du code une fois traité
        
    #ifdef DEBUG
    Serial.print(Code_Recu);
    #endif

  if(Etat_Cafetiere == false)
  {
     BTModule.print("Cafetière allumée");
     delay(1000);
     BTModule.print("On peut faire ");
     BTModule.print(NbTasse);
     BTModule.print(" tasses");
     delay(1000);
   }
    
  if(Etat_Cafetiere == true)
  {
     Temps =  millis() + 2000; // calcul de la pause
     while (Temps < millis() ) // pause
     {
        BTModule.print("Cafetière arrêtée");
       delay(1000);
     }        
  }

  if(Niveau_Eau() < 1 && Etat_Cafetiere == false) // Si plus d'eau, arret de la cafetiere.
    {
      EteintCafetiere(); // eteindre la cafetiere
      Temps =  millis() + 2000; // calcul de la pause
      while (Temps < millis() ) // pause
      {
           BTModule.print("Niveau d'eau faible, Arret");
           delay(100);
      }
     
      #ifdef DEBUG
      Serial.println("Niveau d'eau faible, Arret");
      #endif
      
      delay(4000);
      Etat_Cafetiere = true;
    }
} // fin Loop
