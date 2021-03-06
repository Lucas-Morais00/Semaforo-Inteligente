/* Semaforo Inteligente
Grupo de Pi 5 Centro Universitario SENAC
Rodrigo Fernandes
Isabela Miranda
Lucas Morais
Gustavo dos Santos */

/*ESP32 Wifi bibliotecas*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 

/*Pinos usados HCSR04 e ESP32*/
const int trigPin = 5;
const int echoPin = 18;

const int trigPin2 = 19;
const int echoPin2 = 23;

/*Pinos usados e variaveis LEDs e ESP32*/
int pinVermelho = 25;
int pinAmarelo = 27;
int pinVerde = 26;

int pinVermelho2 = 32;
int pinAmarelo2 = 12;
int pinVerde2 = 21;

int pinVermelhoPedestre = 4;
int pinAmareloPedestre = 13;
int pinVerdePedestre = 33;

int botao = 34;

int totalContador;

int contador = 0;
int contador2 = 0;
unsigned long delay1 = 0;
unsigned long delay2 = 0;

/*Função para mudar a velocidade do semaforo*/
void tempoSemaforo (int verde, int amarelo, int vermelho);

/*IBM Watson IoT Platform informações*/
#define ORG "9e8of7" 
#define DEVICE_TYPE "ESP32-V1.0" 
#define DEVICE_ID "Semaforo-Inteligente-V1.0" 
#define TOKEN "n+bOVh?*95HUfc?&7!"

/*Velocidade do som em cm/uS*/
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

/*Variaveis HCSR04*/
long duration;
float distanceCm;

long duration2;
float distanceCm2;

/*Wifi login*/
const char* ssid = "MORAIS_2G";
const char* password = "@25070400#";

/*IBM Watson IoT Platform conexão*/
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/teste1/fmt/json";
char pubTopic2[] = "iot-2/evt/status2/fmt/json";
char pubTopic3[] = "iot-2/evt/status3/fmt/json";
char pubTopic4[] = "iot-2/evt/status4/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void setup() {
  /*Wifi Connection ESP32 (61-74)*/
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("");
  }
  
  if (!client.connected()){
    Serial.println(server);
    while (!client.connect(clientId, authMethod, TOKEN)){
      Serial.print(".");
      delay(100);
    }
  }

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(trigPin2, OUTPUT); // Sets the trigPin2 as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin2 as an Input

  pinMode(pinVerde, OUTPUT); //Pinos de LED como Output
  pinMode(pinAmarelo, OUTPUT);
  pinMode(pinVermelho, OUTPUT);

  pinMode(pinVerde2, OUTPUT); //Pinos do semaforo 2 de LED como Output
  pinMode(pinAmarelo2, OUTPUT);
  pinMode(pinVermelho2, OUTPUT);

  pinMode(pinVerdePedestre, OUTPUT); //Pinos do semaforo 2 de LED como Output
  pinMode(pinAmareloPedestre, OUTPUT);
  pinMode(pinVermelhoPedestre, OUTPUT);

  pinMode(botao, INPUT);
}

long lastMsg = 0;

void loop(){
  //Calculando sensor 1 -------------------------------------------------------------------------------------
  //Limpo o trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  //Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  //Calcula a distância
  distanceCm = duration * SOUND_SPEED/2;
  //Calculando sensor 1 -------------------------------------------------------------------------------------

  //Calculando sensor 2 -------------------------------------------------------------------------------------
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  
  //Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  //Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin2, HIGH);
  
  //Calcula a distância
  distanceCm2 = duration2 * SOUND_SPEED/2;
  //Calculando sensor 2 -------------------------------------------------------------------------------------

  if(digitalRead(botao) == HIGH){
    digitalWrite(pinVerdePedestre, HIGH);
  } else{
    digitalWrite(pinVerdePedestre, LOW);
  }

  //Calculando contador
  if (distanceCm < 20){
    contador = contador + 1;
    delay(1000);
  }

  if (distanceCm2 < 20){
    contador2 = contador2 + 1;
    delay(1000);
  }
  
  Serial.println("Contador 1: ");
  Serial.println(contador);
  Serial.println("Contador 2: ");
  Serial.println(contador2);
	Serial.println(digitalRead(botao));
  
  //Calculando o tipo de fluxo do semáforo
  totalContador = contador - contador2;
  
  if (totalContador >= -2 && totalContador <= 2){
    //Semáforos com tempos iguais
    tempoSemaforo(6000, 2000, 8000);
  }
  else if (totalContador > 2 && totalContador <= 4){
    //Semáforo 1 mais tempo no verde
    tempoSemaforo(8000, 2000, 6000);
  }
  else if (totalContador > 4){
    //Semáforo 1 mais tempo no verde
    tempoSemaforo(10000, 2000, 4000);
  }
  else if (totalContador >= -4 && totalContador < -2){
    //Semáforo 2 mais tempo no verde
    tempoSemaforo(4000, 2000, 10000);
  }
  else if (totalContador < -4){
    //Semáforo 2 mais tempo no verde
    tempoSemaforo (2000, 2000, 12000);
  }

  //Zerando o contador quando acaba um ciclo do semaforo
  if ((millis() - delay2) >= 16000){
    contador = 0;
    contador2 = 0;
    delay2 = millis();
  }

  client.loop();
  long now = millis();

  //Enviando as informações para a plataforma IBM (131-158)
  if (now - lastMsg > 1000){
    lastMsg = now;
    float distancia = distanceCm;
    String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload += ",\"distancia\":";
           payload += distancia;
           payload += "}}";
    
    Serial.print("Sending payload: ");
    Serial.println(payload);

    if (client.publish(pubTopic1, (char*) payload.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }
    
    String payload1 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload1 += ",\"contador\":";
           payload1 += contador;
           payload1 += "}}";
       
    if (client.publish(pubTopic2, (char*) payload1.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }

		String payload2 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload2 += ",\"contador2\":";
           payload2 += contador2;
           payload2 += "}}";
       
    if (client.publish(pubTopic3, (char*) payload2.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }
		
		String payload3 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload3 += ",\"distancia2\":";
           payload3 += distanceCm2;
           payload3 += "}}";
    
    Serial.print("Sending payload: ");
    Serial.println(payload);

    if (client.publish(pubTopic4, (char*) payload3.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }
  }
}

//Função que define o tipo de fluxo do semáforo
void tempoSemaforo (int verde, int amarelo, int vermelho) {
  if ((millis() - delay1) <= verde){
    digitalWrite(pinVerde, HIGH);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, LOW);

    digitalWrite(pinVerde2, LOW);
    digitalWrite(pinAmarelo2, LOW);
    digitalWrite(pinVermelho2, HIGH);
  }

  if ((millis() - delay1) > verde && (millis() - delay1) <= (verde + amarelo)){
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, HIGH);
    digitalWrite(pinVermelho, LOW);

    digitalWrite(pinVerde2, LOW);
    digitalWrite(pinAmarelo2, LOW);
    digitalWrite(pinVermelho2, HIGH);
  }

  if ((millis() - delay1) > (verde + amarelo) && (millis() - delay1) <= (verde + amarelo + vermelho)){
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, HIGH);
    
    digitalWrite(pinVerde2, HIGH);
    digitalWrite(pinAmarelo2, LOW);
    digitalWrite(pinVermelho2, LOW);
  }

  else{
    delay1 = millis();
  }
}

/*
 Codigo funcionando perfeitamente para um semaforo - Mais atualizado
 */
