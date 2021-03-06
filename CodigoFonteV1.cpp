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

/*Pins usage ESP32*/
const int trigPin = 5;
const int echoPin = 18;

/*IBM Watson IoT Platform info*/
#define ORG "9e8of7" 
#define DEVICE_TYPE "ESP32-V1.0" 
#define DEVICE_ID "Semaforo-Inteligente-V1.0" 
#define TOKEN "n+bOVh?*95HUfc?&7!"

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;
int contador;
int contadorVermelho;
int estadoSemaforo;

/*Wifi Info*/
const char* ssid = "RM_2G";
const char* password = "RM27111803";

/*IBM Watson IoT Platform connection*/
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/teste1/fmt/json";
char pubTopic2[] = "iot-2/evt/status2/fmt/json";
char pubTopic3[] = "iot-2/evt/status3/fmt/json";
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
            delay(100);

    }
  }

    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input

}

long lastMsg = 0;

void loop() {

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;

  //Calculando contador
    if (distanceCm <10){
      contador = contador + 1;
      delay(1000);
  }

  //Calculando contadorVermelho
    if (distanceCm <10) && (estadoSemaforo == 2){
      contadorVermelho = contadorVermelho + 1;
      delay(1000);
  }

  //Calculando estado do semaforo
  // 2-Vermelho  3-Amarelo  4-Verde
  if (contador < 10){
    estadoSemaforo = 4;
    delay(500);
  } else if (contador == 10){
    estadoSemaforo = 3;
    delay(3000);
  } else {
    estadoSemaforo = 2;
    if(contadorVermelho == 5){
      contador = 0;
    }
    delay(2000);
  }

    client.loop();
    long now = millis();

    if (now - lastMsg > 1000) {
        lastMsg = now;
         float distancia = distanceCm;
        String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload += ",\"distancia\":";
              payload += distancia;
              payload += "}}";
       
        Serial.print("Sending payload: ");
        Serial.println(payload);

        if (client.publish(pubTopic1, (char*) payload.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }
        String payload1 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload1 += ",\"contador\":";
              payload1 += contador;
              payload1 += "}}";
       
         if (client.publish(pubTopic2, (char*) payload1.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }
        String payload2 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload2 += ",\"estadoSemaforo\":";
              payload2 += estadoSemaforo;
              payload2 += "}}";
       
         if (client.publish(pubTopic3, (char*) payload2.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
    }
}

/*Arrumar leitura estranha do sensor
Mudar visualiza????o estadoSemaforo
Decidir valores de distancia e contagem de carros*/
