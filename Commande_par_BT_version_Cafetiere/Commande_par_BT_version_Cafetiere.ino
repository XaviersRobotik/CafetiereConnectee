#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 4); // RX, TX

#define CAFETIERE 10

char myChar,Code1;
boolean Code2;
String Chaine;


// Fonction de decodage des parametre de l'application ArduDroid
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
}

void loop() // tout le temps
{
    while ( mySerial.available() ) // reception des commandes BT
    { 
        myChar = mySerial.read();
        if(myChar != '#') {
          Chaine = Chaine + myChar;
        } else {
          Serial.println("");
          if (Recherche(Chaine))
          {
              //Serial.print("Chaine trouvee : ");
              //Serial.println(Code2);
              ActionPin(Code2);
          }
          Chaine = ""; // drop de l'ancienne chaine
        }  
        Serial.print(myChar);
    }

}
