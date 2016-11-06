#include <SPI.h>
#include <Ethernet.h>
#include <String.h>
#define LED 7

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 0, 177};

EthernetServer server(80);

String request = "";

void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.begin(1200);
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void loop()
{
  EthernetClient client = server.available();
  
  if(client && client.connected())
  {
    if(client.available() > 0)
    {
      char c = (char)client.read();
      if(c != '\n')
      {
        request += c;
        digitalWrite(LED, LOW);
      }
      else
      {
        if(request == "BREW HTCPCP/1.0")
        {
          Serial.println(request);
          digitalWrite(LED, HIGH);
        }
          
        request = "";
      }
    }
  }
}
