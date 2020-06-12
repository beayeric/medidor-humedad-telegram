/**
 * Primeras pruebas de intento de lectura de la temperatura y humedad desde WeMos D1 Mini 
 * desde un bot de telegram.
 * */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> //Incluimos la librería para  Telegram  - https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h> // Incluimos la librería Json para el uso de la API de Telegram ATENCIÓN : Utilizar la versión 5.x.x porque las siguientes no funcioncionan -  https://github.com/bblanchon/ArduinoJson
#include "DHT.h" // Librería para el sensor  V1.3.4
#include <Adafruit_Sensor.h> //Librería para la lectura del sensor V1.0.3



//------- DATOS PARA LA CONEXIÓN AL WIFI Y BOT DE TELEGRAM ------//

char ssid[] = "nombre de la red";         		  // el nombre de su red SSID
char password[] = "contraseña de la red";       // la contraseña de su red

#define TELEGRAM_BOT_TOKEN "token del bot de telegram"  // TOKEN proporcionado por BOTFATHER

//------- ---------------------- ------//

WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);

int Bot_mtbs = 1000; // tiempo medio entre escaneo de mensajes
long Bot_lasttime;   // la última vez que se realizó la exploración de mensajes
bool Start = false;

// ----------- Funciones de temperatura -----------//

#define DHTTYPE DHT22   // TIPO DE SENSOR
const int DHTPin = D4;  // Pin donde esta conectado ( por defecto en nuestro proyecto esta conectado en el pin 4)
DHT dht(DHTPin, DHTTYPE);


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";


//--------------Configurar estados y respuestas del bot de telegram-----------------------//


    if (text == "/estado") {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      bot.sendMessage(chat_id, "El estado de le temperatura y la humedad es:", "");
    }

    if (text == "/temperatura") {
      float t = dht.readTemperature();
      bot.sendMessage(chat_id, "La temperatura en sala es de:", "");
    }

    if (text == "/humedad") {
      dht.readHumidity();
      bot.sendMessage(chat_id, "La temperatura en sala es de:", "");
    }

    }

    if (text == "/inicio") {
      String welcome = "Hola" + from_name + "esta es tu sale de control" ".\n";
      welcome += "Esta es una prueba de medidor de humedad y temperatura con bot de telegram.\n\n";
      welcome += "/humedad : para saber el nivel de humedad relativa\n";
      welcome += "/temperatura : para saber el estado de la temperatura\n";
      welcome += "/estado : para saber estado de la humedad y la temperatura al mismo tiempo\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  // Establezca WiFi en modo estación y desconéctese de un AP si era anteriormente estaba conectado
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Intente conectarse a la red Wifi
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);
  delay(10);
  digitalWrite(LED_BUILTIN, HIGH); // el led OFF al inicio

  client.setInsecure();

}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }

// ----- funciones para medir con el sensor ---//
  delay(4000); // tiempo de espera entre lecturas
 
  
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

}