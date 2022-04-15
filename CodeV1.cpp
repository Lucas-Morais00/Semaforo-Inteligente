/*Primeira versão do código para consulta, sem conteudo correto*/

/*ESP32 Wifi libs*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 

/*Pins usage & IBM Watson IoT Platform info*/
#define DHTPIN 4 
#define DHTTYPE DHT11
#define ORG "1xs2qq" 
#define DEVICE_TYPE "ESP32-V1.0" 
#define DEVICE_ID "Semaforo-Inteligente-V1.0" 
#define TOKEN "n+bOVh?*95HUfc?&7!"
#define ANALOG_PIN_0 36

int sensorValue = 0;
int analog_value = 0;

/*Wifi Info*/
const char* ssid = "RM_2G";
const char* password = "RM27111803";

/*IBM Watson IoT Platform connection*/
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/status1/fmt/json";
char authMethod[] = "use-token-auth";
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

}}}

long lastMsg = 0;

void loop() {
    client.loop();
    long now = millis();
    if (now - lastMsg > 3000) {
        lastMsg = now;
         float valor = 0.001611328* analogRead(ANALOG_PIN_0);
        String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload += ",\"valor\":";
              payload += valor;
              payload += "}}";

        Serial.print("Sending payload: ");
        Serial.println(payload);

        if (client.publish(pubTopic1, (char*) payload.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }

        }
    }
