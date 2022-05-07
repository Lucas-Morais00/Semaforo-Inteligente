/*Semaforo Inteligente
Grupo de Pi 5 Centro Universitario SENAC

Rodrigo Fernandes
Isabela Miranda
Lucas Morais
Gustavo dos Santos*/

/*ESP32 Wifi libs*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 

/*Pins usage & IBM Watson IoT Platform info*/
#define ORG "9e8of7" 
#define DEVICE_TYPE "ESP32-V1.0" 
#define DEVICE_ID "Semaforo-Inteligente-V1.0" 
#define TOKEN "n+bOVh?*95HUfc?&7!"

/*Wifi Info*/
const char* ssid = "RM_2G";
const char* password = "RM27111803";

/*IBM Watson IoT Platform connection*/
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/teste1/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("");
    } 

    if (!client.connected()) {
 Serial.println(server);
        while (!client.connect(clientId, authMethod, TOKEN)) {
  Serial.print(".");
            delay(500);

    }
  }
}

void loop() {
  
}
