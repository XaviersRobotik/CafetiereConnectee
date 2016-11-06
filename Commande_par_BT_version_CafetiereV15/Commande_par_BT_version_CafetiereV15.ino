/////////////////////////////////
//   Cafetiere connectée en BT   /
//   Version 1.5                 /
//   Du 23/07/2015               /
//   Auteur Xavier DUVERT        / 
//////////////////////////////////


#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 4); // RX, TX

#define CAFETIERE 10 // Broche du SSR

//#define CAPTEUR_US A0 // broche du capteur US
#define ECHO 7 // Broche US Echo
#define TRIGGER 8 // Broche US Trigger 


char myChar,Code1;
boolean Code2;
String Chaine;
int Volume;
int Reservoir_fond = 50; // Hauteur du reservoir en CM
int Reservoir_couvercle = 5; // Hauteur d'eau Maxi

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
    mySerial.print("Vide"); // il n'y a plus d'eau 
    Serial.print("Vide");
    Serial.println(" ");
    return false;
  }
  else
  {
    Volume = distance * 3,14 * 10; // Calcul Niveau d'eau
    //mySerial.print(" "); // Effacemnt de la case
    mySerial.print(Volume); // Envoi du niveau d'eau
    Serial.print("volume ");
    Serial.println(Volume);
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
       Serial.print("Passage de la broche ");
       Serial.print(CAFETIERE); 
       Serial.print(" a la valeur ");  
       Serial.println(Valeur);
}

void setup()
{
    Serial.begin(9600);

    //mySerial.begin(38400);
    mySerial.begin(9600);

    mySerial.print("AT");
    delay(1000);
    pinMode(CAFETIERE,OUTPUT);
    digitalWrite(CAFETIERE,LOW); // Pour etre sur qu'au démarrage de l'arduino la cafetiere est arretée.
   
    pinMode(TRIGGER, OUTPUT); // Configuration des broches du capteur US
    pinMode(ECHO, INPUT);
    
    Niveau_Eau_OK(); // montre le niveau d'eau
    
}

void loop() // tout le temps
{
    while ( mySerial.available() ) // reception des commandes BT
    { 
        myChar = mySerial.read(); // reception des caractere du BT
        if(myChar != '#') {        // si caractere special de debut
          Chaine = Chaine + myChar; // concatenation de carateres recus
        } else {
          Serial.println("");
          if (Recherche(Chaine) )
          {
              //Serial.print("Chaine trouvee : ");
              //Serial.println(Code2);
              if(Code2 == 0) // pour l'arret on n'a pas besoin de vérifier le niveau d'eau
                ActionPin(0);
                else if(Code2 == 1 && Niveau_Eau_OK() ) // on peut mettre en marche si le niveau d'eau est bon
                  ActionPin(1);
              
          }
          Chaine = ""; // drop de l'ancienne chaine
        }  
        Serial.print(myChar);
    }

   
    
}
