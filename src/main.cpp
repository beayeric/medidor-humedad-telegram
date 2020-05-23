#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>

//Introducimos los datos de la red a la que se conectara
//We enter the data of the network to which you will connect

const char* ssid = "xxxx";
const char* password = "xxxx";

int contconexion = 0;

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321 // definimos el sensor de temperatura y humedad

//Configuramos la conexión con telegram
//We set up the telegram connection
const char BotToken[] = "xxxxxxxxxxxxxxxxx";

WiFiClientSecure client;
UniversalTelegramBot bot (BotToken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;



//----------------------------------------------------------------------------------
//Funciones de toma de temperatura


 

const int DHTPin = D4;     // el pin donde esta conectado el sensor
 
DHT dht(DHTPin, DHTTYPE);
 
void setup() {
   Serial.begin(9600);
   Serial.println("Test de sonda!");
 
   dht.begin();
}
 
void loop() {
   // Wait a few seconds between measurements.
   delay(4000);
 
   // Reading temperature or humidity takes about 250 milliseconds!
   float h = dht.readHumidity();
   float t = dht.readTemperature();
 
   if (isnan(h) || isnan(t)) {
      Serial.println("Error de conexión con la sonda!");
      return;
   }
 
 
   Serial.print("La humedad es de: ");
   Serial.print(h);
   Serial.print(" %\t");
   Serial.print("La temperatura es de: ");
   Serial.print(t);
   Serial.print(" *C ");
}

//----------------------------------------------------------------------------------
//Configuramos el bot de telegram --- We set up the telegram bot 


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/estado") {
      teclapower();
      bot.sendMessage(chat_id, "El estado es:");  

   
    if (text == "/options") {
      String keyboardJson = "[[\"/estado\"]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Elige una de estas opciones", "", keyboardJson, true);
    }
  }
}  
//-------------------------------------------------------
void setup() {
  ac.begin();
  #if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  #else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
  #endif 
  Serial.println("");
  Serial.println("");
 

//----------------------------------------------------------------------------------
void loop() {
  server.handleClient();  
  
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}