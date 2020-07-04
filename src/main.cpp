/*
  Autor:   Makers Bierzo
  Fecha:     06/2020
  
  Version:  V0.0.1 

  Prueba versión v0.02 ( alarma)
 
  Descripcion:
  Lectura de humedad y temperatura con dht22 y wemos d1 mini. Consulta mediante bot de telegram
 
  Pinde de conexión del dht22:
  D4
 
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> //Incluimos la librería para  Telegram  - https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h> // Incluimos la librería Json para el uso de la API de Telegram ATENCIÓN : Utilizar la versión 5.x.x porque las siguientes no funcioncionan -  https://github.com/bblanchon/ArduinoJson
#include "DHT.h" // Librería para el sensor  V1.3.4
#include <Adafruit_Sensor.h> //Librería para la lectura del sensor V1.0.3

//------- DATOS PARA LA CONEXIÓN AL WIFI Y BOT DE TELEGRAM ------//

char ssid[] = "MOVISTAR_8002";              // el nombre de su red SSID
char password[] = "MhPXue3fuhQGxHQcuNcE";       // la contraseña de su red


#define TELEGRAM_BOT_TOKEN "1214719176:AAFpskFrYxA66OVn4gf4dpwDRyqK2jpCCS4"  // TOKEN proporcionado por BOTFATHER
#define CHAT_ID_PROPIO "644092158"

//------- ---------------------- ------//

WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);

int Bot_mtbs = 1000;    // tiempo medio entre escaneo de mensajes
long Bot_lasttime;      // la última vez que se realizó la exploración de mensajes
int dht_mtbs = 5000;    // tiempo entre lecturas 5 segundos
long hBdt_lasttime;     // la última vez que se realizó la exploración de mensajes
bool Start = false;


// ----------- Funciones de temperatura -----------//

#define DHTTYPE DHT22   // TIPO DE SENSOR
const int DHTPin = D4;  // Pin donde esta conectado ( por defecto en nuestro proyecto esta conectado en el pin 4)
DHT dht(DHTPin, DHTTYPE);

String str_tem = ""; // Declarar variables string globales para almacenar los valores de temperatura y humedad
String str_hum = "";
float h = dht.readHumidity(); // Variable para la lecutra de la humedad
float t = dht.readTemperature(); // Varíable para la lectura de la temperatura
bool Alar_Hum = ""; // Variables para la humedad actual
bool Alar_Tem = ""; // Variables para la temperatura actural

// Limites máximos y minimos de temperatura y humedad --// 
float HumMax = 50;
float HumMin = 45;
float TemMax = 20;
float TemMin = 19;




 
//------------Configurar estados y respuestas del bot de telegram-----------------//


 // -- Parte fija , no necesita modificaciones -- //
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

// --- comienza la parte que se puede personalizar -- //
// Respuesta a los mensajes del bot

    if (text == "/estado") { 
      String resultado = str_hum +"\n";
      resultado += str_tem +"\n";
      bot.sendMessage(chat_id, resultado, "");
    }

    if (text == "/temperatura") {
      bot.sendMessage(chat_id, str_tem , "");
    }

    if (text == "/humedad") {
      bot.sendMessage(chat_id, str_hum , "");
    }

    if (text == "/start") {
      String welcome = "Hola " + from_name + " esta es tu sala de control" ".\n";
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
  dht.begin();

  // Establezca WiFi en modo estación y desconéctese de un AP si era anteriormente estaba conectado
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Intenta conectarse a la red Wifi
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  // -- Valores que se muestran en el monitor serie --//
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Comunicación de manera no segura
  //client.setInsecure();
  //bot._debug = true; // para ver el funcionamiento del bot

  // Comunicación de manera segura
  bot._debug = true; // para ver el funcionamiento del bot
  const uint8_t fingerprint[20] = { 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 };
  client.setFingerprint(fingerprint);

}

void loop() {

//--funcion de alarma --//


if (Alar_Hum){

  if (h>HumMax) {
    String Alar_HumMax = "Superada la humedad MÁXIMA" "\n";
    bot.sendMessage(CHAT_ID_PROPIO, Alar_HumMax, "");
}

  else if (h<HumMin) { 
    String Alar_HumMin = "Superada la humedad MINIMA" "\n";
    bot.sendMessage(CHAT_ID_PROPIO, Alar_HumMin, "");
}
 Alar_Hum = false;
}

  // Cada tiempo definido en Bot_mtbs vemos si se recibe algún mensaje
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }

  // Cada tiempo definido en dht_mtbs leemos el sensor de temperatura/humedad
  if (millis() > hBdt_lasttime + dht_mtbs)  {
    
    

    if (isnan(h) || isnan(t)) {
        Serial.println("¡Error al leer del sensor DHT!");
        return;
    }

    str_tem = "Temperatura : " + String(t, 2);   
    Serial.println(str_tem);
    str_hum = "Humedad : " + String(h, 2);
    Serial.println(str_hum);
    
    hBdt_lasttime = millis();
  }

}


    