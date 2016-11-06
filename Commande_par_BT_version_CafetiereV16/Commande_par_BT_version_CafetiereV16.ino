/////////////////////////////////
//   Cafetiere connectée en BT   /
//   Version 1.7                 /
//   Du 12/08/2015               /
//   Auteur Xavier DUVERT        / 
//////////////////////////////////


#include <SoftwareSerial.h>

SoftwareSerial BTModule(2, 4); // RX, TX

#define CAFETIERE 10 // Broche du SSR
#define LED 13 // Broche de la led

#define ECHO 7 // Broche US Echo
#define TRIGGER 8 // Broche US Trigger 


char myChar,Code1;
boolean Code2,Flag_Arret;
String Chaine;
int Volume;
int Reservoir_fond = 17; // Hauteur du reservoir en CM
int Reservoir_couvercle = 2; // Hauteur d'eau Maxi

boolean Niveau_Eau_OK()
{
  long duration, distance; // Duration used to calculate distance
  
  digitalWrite(TRIGGER, LOW); 
  delayMicroseconds(2); 

 digitalWrite(TRIGGER, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(TRIGGER, LOW);
 duration = pulseIn(ECHO, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 Serial.print("Distance : ");
 Serial.println(distance);
 
 if( distance >= Reservoir_fond) // test si il reste de l'eau
  { 
    BTModule.print("Vide "); // il n'y a plus d'eau 
    Serial.print("Vide");
    Serial.println(" ");
    delay(2000);
    return false;
  }
  else
  {
    Volume = map(distance,Reservoir_fond, Reservoir_couvercle,0,10); // Calcul Niveau d'eau
    //BTModule.print(" "); // Effacemnt de la case
    BTModule.print("Nb Tasses ");
    BTModule.print(Volume); // Envoi du niveau d'eau
    Serial.print("Nb Tasses ");
    Serial.println(Volume);
    delay(2000);
    return true;
  }
    
 
}

// Fonction de decodage des parametres de l'application 
boolean Recherche(String ADecoder)
{ 
  boolean Flag = true;
  
  Serial.print("Valeur A decoder : ");  
  Serial.println(ADecoder);
  for (int i = 0; i <= (ADecoder.length()); i++)
  {
    Serial.print("Valeur A decoder i : ");  
    Serial.print(ADecoder[i]);
    Serial.print("Position :");
    Serial.println(i);
  }
  
  
  Code1 = ADecoder[0];
  if (Code1 != 'C') Flag= false;
  Serial.print("Code1 : ");
  Serial.println(Code1);
        
  Code2 = ADecoder[1] - 48;
  if (Code2 > 1) Flag = false; // Mauvais decodage des valeurs
  Serial.print("Code2 : ");
  Serial.println(Code2);
  
  return Flag;
}

// Fonction de modification de l'atat des broches
void ActionPin(int Valeur)
{  
       pinMode(CAFETIERE,OUTPUT);
       digitalWrite(CAFETIERE,Valeur);
       digitalWrite(LED,Valeur);
       Serial.print("Passage de la broche ");
       Serial.print(CAFETIERE); 
       Serial.print(" a la valeur ");  
       Serial.println(Valeur);
}

void setup()
{
    Serial.begin(9600);

    //BTModule.begin(38400);
    BTModule.begin(9600);
    delay(1000);
    BTModule.print("Cafetiére a vos ordres !");
    
    pinMode(CAFETIERE,OUTPUT);
    digitalWrite(CAFETIERE,LOW); // Pour etre sur qu'au démarrage de l'arduino la cafetiere est arretée.
    digitalWrite(LED,LOW); // Pour controler l'etat de la cafetiere.   
   
    pinMode(TRIGGER, OUTPUT); // Configuration des broches du capteur US
    pinMode(ECHO, INPUT);
    
    Niveau_Eau_OK(); // montre le niveau d'eau
    
}

void loop() // tout le temps
{
    Niveau_Eau_OK(); // montre le niveau d'eau  
  
    while ( BTModule.available() ) // reception des commandes BT
    { 
        myChar = BTModule.read(); // reception des caractere du BT
        if(myChar != '#') {        // si caractere special de debut
          Chaine = Chaine + myChar; // concatenation de carateres recus
        } else {
          Serial.println("");
          if (Recherche(Chaine) )
          {
              //Serial.print("Chaine trouvee : ");
              //Serial.println(Code2);
              if(Code2 == 0) // pour l'arret on n'a pas besoin de vérifier le niveau d'eau
              {
                ActionPin(0);
                Flag_Arret = true;
                if(Flag_Arret == true)
                {
                   BTModule.print("Cafetière arrêtée");
                }
              }
              else if(Code2 == 1 && Niveau_Eau_OK() )
               { // on peut mettre en marche si le niveau d'eau est bon
                  ActionPin(1);
                  Flag_Arret = false;
                  if(Flag_Arret == false)
                  {
                   BTModule.print("Cafetière allumée");
                  }

               }
               if(Code2 == 1 && !Niveau_Eau_OK())
               {
                 BTModule.print("Mettez de l'eau elle est ");
               }
          }
          Chaine = ""; // drop de l'ancienne chaine
        }  
        Serial.print(myChar);
        
        
        
        
    } // fin du while

    if(Niveau_Eau_OK() == false && Flag_Arret == false) // Si plus d'eau, arret de la cafetiere.
        {
          BTModule.print("Niveau d'eau faible, Arret");
          Serial.println("Niveau d'eau faible, Arret");
          Code2 = 0;
          ActionPin(0);
          delay(2000);
          Flag_Arret = true;
        }
   
    
}
