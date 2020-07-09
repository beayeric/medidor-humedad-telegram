# medidor-humedad-telegram

v 0.0.1 Medidor de humedad y temperatura mediante wemos d1 y dht22 con consulta por telegram.



Este proyecto nace de la necesidad de crear un sistema para el control de temperatura y humedad de una sala. Autonomo y activo.

Como premisas para la realización : 

1 - Precio razonable.
2 - Que sea autonomo y activo ( es decir que una vez conectado, no tengamos que hacer nada)
3 - Que pueda avisarnos de las "alarmas" mediante telegram. 


Hardware : 

Placa: Wemos D1 mini 
https://www.luisllamas.es/wemos-d1-mini-una-genial-placa-de-desarrollo-con-esp8266/

Sensor de temperatura y humedad : Shield DHT22 Sensor de temperatura y humedad


Informaciión sobre el dht22  y sus diferencias con el dht21 :  https://www.luisllamas.es/arduino-dht11-dht22/



Realizado con: Visual Studio Code

Crear bot de telegram: https://blogthinkbig.com/crear-bot-de-telegram-botfather

Espacio de trabajo : https://gist.github.com/beayeric/578148c643d78d334279f16d0b67d86d



 ////// ------------------ v0.0.2.  -------------------//////

    -- Configuración de una alarma al superar valores prefefinidos
    -- Aviso de la alarma mediante bot de telegram
    -- Consulta de los valores predefinidos mediante el bot de telegram. 
    -- Aviso del estado acutal de temperatura y humedad, cuando salta la alarma. 
    
    
    Notas : CHAT_ID_PROPIO : Tienes que poner tu ID de usuario de telegram, el del bot. 
    Lo puedes saber aquí --> https://telegram.me/getidsbot



////---- próximas versiones ---///

1. Actualizar el codigo via OTA . v0.0.3
2. Almacenar la humedad de aviso en Eeprom para que no se pierda al reiniciar. v0.0.4

