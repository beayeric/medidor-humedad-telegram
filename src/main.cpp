/*
  Autor:     Makers Bierzo
  Fecha:     09/07/2020
  
  Version:    V0.0.1 
  Version:    V0.0.2 
    -- Configuración de una alarma al alcanzar valores predefinidos
    -- Aviso de la alarma mediante bot de telegram
    -- Consulta de los valores predefinidos mediante el bot de telegram. 
    -- Aviso del estado acutal de temperatura y humedad, cuando salta la alarma. 
 
  Descripcion HARDWARE :
  Lectura de humedad y temperatura con dht22 y wemos d1 mini. Consulta mediante bot de telegram
 
  Pin de de conexión del dht22:
  D4
 
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> //Incluimos la librería para  Telegram  - https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>          // Incluimos la librería Json para el uso de la API de Telegram ATENCIÓN : Utilizar la versión 5.x.x porque las siguientes no funcioncionan -  https://github.com/bblanchon/ArduinoJson
#include "DHT.h"                  // Librería para el sensor  V1.3.4
#include <Adafruit_Sensor.h>      //Librería para la lectura del sensor V1.0.3
#include "Configuration.h"        //incorpora los datos de configuración de la wifi y del bot de telegram que creemos

#define ONLINE_TIME 86400000      //24h para hacer un reset del ESP y evitar cuelgues

//------- ---------------------- ------//

WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);

int Bot_mtbs = 1000;    // tiempo medio entre escaneo de mensajes
long Bot_lasttime;      // la última vez que se realizó la exploración de mensajes
int dht_mtbs = 5000;    // tiempo entre lecturas 5 segundos
long hBdt_lasttime;     // la última vez que se realizó la exploración de mensajes
bool Start = false;

// ----------- Variables globales de temperatura y humedad -----------//

#define DHTTYPE DHT22   // TIPO DE SENSOR
const int DHTPin = D4;  // Pin donde esta conectado ( por defecto en nuestro proyecto esta conectado en el pin 4)
DHT dht(DHTPin, DHTTYPE);

// Strings globales para almacenar los valores de temperatura y humedad
String str_tem = "";               
String str_hum = "";

// Strings globales para almacenar los valores de temperatura y humedad actuales 
String Hum_Actual = "La humedad actual es: ";  
String Temp_Actual = "La temperatura actual es: ";

// Strings globales para almacenar los valores máximos y mínimos de humedad y la temperatura
String Alarma_Hum_Maxima = "La humedad máxima es: "; 
String Alarma_Hum_Minima = "La humedad mínima es: ";
String Alarma_Tem_Maxima = "La temperatura máxima es: ";
String Alarma_Tem_Minima = "La temperatura mínima es: ";


float h=0.0;  // Variable para la lecutra de la humedad del dht
float t=0.0;  // Variable para la lectura de la temperatura del dht

// Variables para temporizar el restablecimiento de los avisos automáticamente
int num_rest = 120;     // Restablecer alaramas 120*5/60 = 10 minutos seguidos
int8 tempo_RestAlHumMax = 0;
int8 tempo_RestAlHumMin = 0;  
int8 tempo_RestAlTemMax = 0;  
int8 tempo_RestAlTemMin = 0;

// Variables bool para activar o desactivar los avisos
bool Alar_Hum_Max = true; 
bool Alar_Hum_Min = true;
bool Alar_Tem_Max = true; 
bool Alar_Tem_Min = true;

// Variables para los Limites máximos y minimos de temperatura y humedad --// 
float HumMax = 60.0;
float HumMin = 40.0;
float TemMax = 30.0;
float TemMin = 20.0;
 



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

    // --- Comienza la parte que se puede personalizar -- //
    // Respuesta a los mensajes del bot

    if (text == "/control") { 
      String resultado = str_hum +" %"+"\n";
      resultado += str_tem +" ºC"+"\n";
      bot.sendMessage(chat_id, resultado, "");
    }

    if (text == "/temperatura") {
      bot.sendMessage(chat_id, str_tem  + " ºC" , "");
    }

    if (text == "/humedad") {
      bot.sendMessage(chat_id, str_hum  + " %", "");
    
    }

    if (text == "/estado") { 
      String alarma = Alarma_Hum_Maxima + String(HumMax, 2) + "\n";
      alarma += Alarma_Hum_Minima + String(HumMin, 2) + "\n";
      alarma += String ("--------------") + "\n";
      alarma += Alarma_Tem_Maxima + String(TemMax, 2) + "\n";
      alarma += Alarma_Tem_Minima + String(TemMin, 2) + "\n";
      alarma += String ("--------------") + "\n";
      alarma += Hum_Actual +  String (h,2)+ "\n";
      alarma += Temp_Actual + String(t,2)+ "\n";
      alarma += String ("--------------") + "\n";
      alarma += String ("Estado alarma") + "\n";
      alarma += String ("Estado Alarma Hum Max: ") + String(Alar_Hum_Max)+ "\n";
      alarma += String ("Estado Alarma Hum Min: ") + String(Alar_Hum_Min)+ "\n";
      alarma += String ("") + "\n";
      alarma += String ("Estado Alarma Tem Max: ")+  String(Alar_Tem_Max)+ "\n";
      alarma += String ("Estado Alarma Tem Min: ")+  String(Alar_Tem_Min)+ "\n";
      alarma += String ("--------------") + "\n";
      alarma += String ("Contador reset") + "\n";
      alarma += String ("Temporizador Restablecer Hum Max : " )+ String(tempo_RestAlHumMax)+ "\n";
      alarma += String ("Temporizador Restablecer Hum Min : " )+ String(tempo_RestAlHumMin)+ "\n";
      alarma += String ("") + "\n";
      alarma += String ("Temporizador Restablecer Temp Max : " )+ String(tempo_RestAlTemMax)+ "\n";
      alarma += String ("Temporizador Restablecer Temp Min : " )+ String(tempo_RestAlTemMin)+ "\n";
      bot.sendMessage(chat_id, alarma, "");
    }
    
    /// ---- Parte donde puedes añadir las descripciones de las funciones del bot --- //
    if (text == "/start") {
      String welcome = "Hola " + from_name + " esta es tu sala de control" ".\n";
      welcome += "Esta es una prueba de medidor de humedad y temperatura con bot de telegram.\n\n";
      welcome += "/humedad : para saber el nivel de humedad relativa\n";
      welcome += "/temperatura : para saber el estado de la temperatura\n";
      welcome += "/control : para saber estado de la humedad y la temperatura al mismo tiempo\n";
      welcome += "/estado : para saber el estado actual de todos los parametros.\n";
      welcome += "/options : muestra una botonera con las opciones arriba indicadas.\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
    if (text == "/options") {
      String keyboardJson = "[[\"/start\",\"/estado\",\"/control\"],[\"/temperatura\"],[\"/humedad\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Elige una de estas opciones", "", keyboardJson, true);
    }
  }
}

//------------Mis Funciones-----------------//

void avisoPorHumMax() {

  if (Alar_Hum_Max){
      if (h >= HumMax) {
        String Alar_HumMax = "Aviso por HUMEDAD MÁXIMA \n";
        // Al saltar la alarma, tambíen mostrara la humedad actual.
        Alar_HumMax += Hum_Actual + String(h,2);  
        bot.sendMessage(CHAT_ID_PROPIO, Alar_HumMax, "");
        Alar_Hum_Max = false;
      }
    } else {
      if (h >= HumMax) {
        // Para que la temporización sea con lecturas consecutivas
        tempo_RestAlHumMax = 0; 
      }
    }

}

void avisoPorHumMin() {

  if (Alar_Hum_Min){
      if (h <= HumMin) { 
        String Alar_HumMin = "Aviso por HUMEDAD MíNIMA \n";
        // Al saltar la alarma, tambíen mostrara la humedad actual.
        Alar_HumMin += Hum_Actual + String(h,2);
        bot.sendMessage(CHAT_ID_PROPIO, Alar_HumMin, "");
        Alar_Hum_Min = false;
      } 
    } else {
      if (h <= HumMin){
        // Para que la temporización sea con lecturas consecutivas
        tempo_RestAlHumMin = 0; 
      }
    }

}

void restablecerAviPorHumMax(){

    // Si el aviso está desactivado y la humedad es inferior al valor máximo 
    // podemos restablecer el aviso si esto sucede num_rest veces seguidas 
    if (Alar_Hum_Max == false && h < HumMax) {
      tempo_RestAlHumMax++;
      if (tempo_RestAlHumMax == num_rest){
        String Alar_Hum_Max_Reset = "Restablecido el aviso HUMEDAD MÁXIMA \n";
        Alar_Hum_Max_Reset += Hum_Actual + String (h,2);
        bot.sendMessage(CHAT_ID_PROPIO, Alar_Hum_Max_Reset, "");
        Alar_Hum_Max = true;
        tempo_RestAlHumMax = 0;
      }
    }

}

void restablecerAviPorHumMin(){
    
    // Si el aviso está desactivado y la humedad es superior al valor mínimo
    // podemos restablecer el aviso si esto sucede num_rest veces seguidas 
    if (Alar_Hum_Min == false && h > HumMin) {
      tempo_RestAlHumMin++;
      if (tempo_RestAlHumMin == num_rest){
        String Alar_Hum_Min_Reset = "Restablecido el aviso HUMEDAD MíNIMA \n";
        Alar_Hum_Min_Reset += Hum_Actual + String (h,2);
        bot.sendMessage(CHAT_ID_PROPIO, Alar_Hum_Min_Reset, "");
        Alar_Hum_Min = true;
        tempo_RestAlHumMin =0;
      }        
    }

}

void avisoPorTemMax(){

  if (Alar_Tem_Max){
      if (t >= TemMax) {
        String Alar_TemMax = "Aviso por TEMPERATURA MÁXIMA \n";
        // Al saltar la alarma, tambíen mostrara la temperatura actual.
        Alar_TemMax += Temp_Actual + String(t,2);
        bot.sendMessage(CHAT_ID_PROPIO, Alar_TemMax, "");
        Alar_Tem_Max = false;
      }    
    } else {
      if (t >= TemMax) {
        // Para que la temporización sea con lecturas consecutivas
        tempo_RestAlTemMax  = 0; 
      }
    }

}

void avisoPorTemMin(){

   if (Alar_Tem_Min){
      if (t <= TemMin) { 
       String Alar_TemMin = "Aviso por TEMPERATURA MíNIMA \n";
       // Al saltar la alarma, tambíen mostrara la temperatura actual.
       Alar_TemMin += Temp_Actual + String(t,2);
       bot.sendMessage(CHAT_ID_PROPIO, Alar_TemMin, "");
       Alar_Tem_Min = false;     
      }
    } else {
      if (t <= TemMin){
        // Para que la temporización sea con lecturas consecutivas
        tempo_RestAlTemMin = 0; 
      }
    }

}

void restablecerAviPorTemMax(){

    // Si el aviso está desactivado y la temperatura es inferior al valor máximo 
    // podemos restablecer el aviso si esto sucede num_rest veces seguidas 
    if (Alar_Tem_Max == false && t < TemMax) {
      tempo_RestAlTemMax++;
      if (tempo_RestAlTemMax == num_rest) {
       String Alar_Tem_Max_Reset = "Restablecido el aviso TEMPERATURA MÁXIMA \n";
       Alar_Tem_Max_Reset += Temp_Actual + String (t,2);
       bot.sendMessage(CHAT_ID_PROPIO, Alar_Tem_Max_Reset, "");
       Alar_Tem_Max = true;
       tempo_RestAlTemMax  = 0;

      }
    }

}

void restablecerAviPorTemMin(){

    // Si el aviso está desactivado y la temperatura es superior al valor mínimo
    // podemos restablecer el aviso si esto sucede num_rest veces seguidas 
    if (Alar_Tem_Min == false && t > TemMin) {
     tempo_RestAlTemMin++;
      if (tempo_RestAlTemMin == num_rest) {
       String Alar_Tem_Min_Reset = "Restablecido el aviso TEMPERATURA MíNIMA \n";
       Alar_Tem_Min_Reset += Temp_Actual + String (t,2); 
       bot.sendMessage(CHAT_ID_PROPIO, Alar_Tem_Min_Reset, "");
       Alar_Tem_Min = true;
       tempo_RestAlTemMin = 0;
      }
    }

}

void saludoBot(){
  bot.sendMessage(CHAT_ID_PROPIO, "Saludos, volvemos a empezar ;)", "");
}


//------- SETUP ------//

void setup() {
  
  // Inicializamos el puerto serie y el sensor dht
  Serial.begin(115200);
  dht.begin();

  // Establecer WiFi en modo estación y desconectarse de un AP, si estaba conectado.
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
  //bot._debug = true; // para ver el funcionamiento del bot
  const uint8_t fingerprint[20] = { 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 };
  client.setFingerprint(fingerprint);

  saludoBot();

}

void loop() {

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
    
    h = dht.readHumidity();     // lecutra de la humedad
    t = dht.readTemperature();  // lectura de la temperatura

    if (isnan(h) || isnan(t)) {
        Serial.println("¡Error al leer del sensor DHT!");
        return;
    }
    
    // -- Conversión de valores t y h a String para poder mostrarlos con el Bot -- //
    str_tem = "Temperatura : " + String(t, 2);   
    Serial.println(str_tem);      // Mostrar Temperatura por el puerto serie
    str_hum = "Humedad : " + String(h, 2);
    Serial.println(str_hum);      // Mostrar Humedad por el puerto serie 
    
    //-- FUNCIONES DE AVISOS --//
    
    // HUMEDAD //
    // AVISO cuando la humedad es igual o superior al valor límite máximo
    avisoPorHumMax();
    // AVISO cuando la humedad es igual o inferior al valor límite mínimo
    avisoPorHumMin();

    // Reactivación de avisos de humedad temporizados,
    restablecerAviPorHumMax(); 
    restablecerAviPorHumMin();
  
  
    // TEMPERATURA // 
    // AVISO cuando la temperatura es igual o superior al valor límite máximo
    avisoPorTemMax();
    
    // AVISO cuando la temperatura es igual o inferior al valor límite mínimo
    avisoPorTemMin();

    // Reactivación de avisos de temperatura temporizados,
    restablecerAviPorTemMax();
    restablecerAviPorTemMin();
  
    
    // --- Mostrar el estado de la activación o no de las alarmas --//

    // 1 activada 0 desactivada 
    // Mostramos por el puerto serie el estado de las alarmas
    Serial.println("Estado Alarma Hum Max: "+ String(Alar_Hum_Max));
    Serial.println("Estado Alarma Hum Min: "+ String(Alar_Hum_Min));

    Serial.println("Estado Alarma Tem Max: "+ String(Alar_Tem_Max));
    Serial.println("Estado Alarma Tem Min: "+ String(Alar_Tem_Min));

    Serial.println("Temporizador Restablecer Hum Max : "+ String(tempo_RestAlHumMax));
    Serial.println("Temporizador Restablecer Hum Min : "+ String(tempo_RestAlHumMin));
    Serial.println("Temporizador Restablecer Temp Max : "+ String(tempo_RestAlTemMax));
    Serial.println("Temporizador Restablecer Temp Min : "+ String(tempo_RestAlTemMin));

    hBdt_lasttime = millis();

  }
  // condicional para reiniciar el ESP el tiempo definido (en este caso 24h) y evitar cuelgues
  
  if (millis() > ONLINE_TIME)
    ESP.restart();

} 