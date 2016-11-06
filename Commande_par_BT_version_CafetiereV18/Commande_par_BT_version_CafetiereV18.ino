/////////////////////////////////
//   Cafetiere connectée en BT   /
//   Version 1.8                 /
//   Du 24/09/2015               /
//   Auteur Xavier DUVERT        / 
//////////////////////////////////

#include <SoftwareSerial.h>

#define DEGUG

#define CAFETIERE 10 // Broche du SSR
#define LED 13 // Broche de la led

#define ECHO 7 // Broche US Echo
#define TRIGGER 8 // Broche US Trigger 

#define Reservoir_fond 17 // Hauteur du reservoir en CM
#define Reservoir_couvercle  2 // Hauteur d'eau Maxi

unsigned char myChar,Code1;
boolean Code2,Cafetiere_Arretee,ParNombredeTasse;
String Chaine;
unsigned int NbTasse,NbTasseDemande;
unsigned long Temps;

SoftwareSerial BTModule(2, 4); // RX, TX

boolean Niveau_Eau_OK()
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
 
 if( distance >= Reservoir_fond) // test si il reste de l'eau
  { 
    // BTModule.print("Vide "); // il n'y a plus d'eau 
    #ifdef DEBUG   
    Serial.print("Vide");
    Serial.println(" ");
    delay(2000);
    #endif
    return false;
  }
  else
  {
    NbTasse = map(distance,Reservoir_fond, Reservoir_couvercle,0,10); // Calcul Niveau d'eau
    //BTModule.print(" "); // Effacemnt de la case
    // BTModule.print("Nb Tasses ");
    // BTModule.print(NbTasse); // Envoi du niveau d'eau
    #ifdef DEBUG
    Serial.print("Nb Tasses ");
    Serial.println(NbTasse);
    delay(2000);
    #endif
    return true;
  }
    
 
}



// Fonction de decodage des parametres de l'application 
boolean Recherche(String ADecoder)
{ 
  boolean Flag = true;
  #ifdef DEBUG
  Serial.print("Valeur A decoder : ");  
  Serial.println(ADecoder);
  
  for (int i = 0; i <= (ADecoder.length()); i++)
  {
    Serial.print("Valeur A decoder i : ");  
    Serial.print(ADecoder[i]);
    Serial.print("Position :");
    Serial.println(i);
  }
  #endif
  
  Code1 = ADecoder[0];
  if (Code1 != 'C' || Code1 != 'N') Flag = false; // si le caratcere n'est ni C (Cafertiere) ni N (Nombre de tasse)
  
  Code2 = ADecoder[1] - 48; // pour convertir en numerique
  
  if (Code2 > 1 && Code1 == 'C')
    {
        Flag = false; // Mauvais decodage des valeurs
    }
    else  if (Code2 = 0 && Code1 == 'N')
      {    
          Flag = false; // manque le nb de tasse
      } else {
                NbTasseDemande = Code2;
                ParNombredeTasse = true;
              }

  #ifdef DEBUG
  Serial.print("Code1 : ");
  Serial.println(Code1);
   
  Serial.print("Code2 : ");
  Serial.println(Code2);
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
  //Calcule Nb de tasse restante apres requete
  NbTassealafin = NbTasse -  NbTasserequises;
  if (NbTassealafin > 0) //si il y a assez d'eau
  {
      ActionPin(HIGH);  // allume la cafetiere
      while(NbTasse > NbTassealafin) // tant que le nombre de tasse n'est pas atteint
      {
          BTModule.print("Café en cours pour ");
          BTModule.print(NbTasserequises);
          BTModule.print(" tasse(s)");
          
      }
      ActionPin(LOW);
      Cafetiere_Arretee = true;
  }
  else // si il n'y a pas assez d'eau
  {
    BTModule.print("Pas assez d'eau pour ");
    BTModule.print(NbTasserequises);
    BTModule.print(" tasse(s)");
  }
}
  

void setup()
{
    #ifdef DEBUG
    Serial.begin(9600);
    #endif
    
    ActionPin(LOW); // Pour etre sur qu'au démarrage de l'arduino la cafetiere est arretée.
    
    //BTModule.begin(38400);
    BTModule.begin(9600);
    delay(1000);
    
    Temps =  millis() + 2000; // calcul de la pause
    while (Temps < millis() ) // pause
    {
        BTModule.print("Cafetiére a vos ordres !");
        delay(100);   
    }
    pinMode(CAFETIERE,OUTPUT);
    pinMode(LED,OUTPUT);
    
    
    pinMode(TRIGGER, OUTPUT); // Configuration des broches du capteur US
    pinMode(ECHO, INPUT);
    
    // Niveau_Eau_OK(); // montre le niveau d'eau
    
}

void loop() // tout le temps
{
    Niveau_Eau_OK(); // montre le niveau d'eau  
  
    while ( BTModule.available() ) // reception des commandes BT
    { 
        myChar = BTModule.read(); // reception des caractere du BT
        if(myChar != '#') //si pas de caractere de fin
        {        
          Chaine = Chaine + myChar; // concatenation de carateres recus
        } else { // si caractere special de fin
          #ifdef DEBUG
          Serial.println("");
          #endif
          if ( Recherche(Chaine) )
          {
              #ifdef DEBUG
              Serial.print("Chaine trouvee : ");
              Serial.println(Code2);
              #endif
              
              if(Code2 == 0) // pour l'arret on n'a pas besoin de vérifier le niveau d'eau
              {
                ActionPin(LOW); // Arret de la cafetiere
                Cafetiere_Arretee = true;
              }
              else if(Code2 == 1 && Niveau_Eau_OK() && Cafetiere_Arretee == true)
              { // on peut mettre en marche si le niveau d'eau est bon
                  ActionPin(HIGH);
                  Cafetiere_Arretee = false;
              }
               if(Code2 == 1 && !Niveau_Eau_OK())
               {
                 Temps =  millis() + 2000; // calcul de la pause
                 while (Temps < millis() ) // pause
                 {
                     BTModule.print("Mettez de l'eau elle est vide");
                     delay(100);
                 }
               }               
          }
          Chaine = ""; // drop de l'ancienne chaine
        }  
    #ifdef DEBUG
    Serial.print(myChar);
    #endif
   } // fin du while

  if(Cafetiere_Arretee == false)
    {
     BTModule.print("Cafetière allumée");
     delay(1000);
     BTModule.print("On peut faire ");
     BTModule.print(NbTasse);
     BTModule.print(" tasses");
     delay(1000);
    }
    
  if(Cafetiere_Arretee == true)
  {
   Temps =  millis() + 2000; // calcul de la pause
   while (Temps < millis() ) // pause
    {
      BTModule.print("Cafetière arrêtée");
     delay(100);
    }        


  if(Niveau_Eau_OK() == false && Cafetiere_Arretee == false) // Si plus d'eau, arret de la cafetiere.
    {
      ActionPin(LOW);
      Temps =  millis() + 2000; // calcul de la pause
      while (Temps < millis() ) // pause
      {
           BTModule.print("Niveau d'eau faible, Arret");
           delay(100);
      }
     
      #ifdef DEBUG
      Serial.println("Niveau d'eau faible, Arret");
      #endif
      
      Code2 = 0;
      delay(4000);
      Cafetiere_Arretee = true;
    }
} // fin Loop
