//#include <ESP8266HTTPClient.h>


//Bibliotecas
#include "ThingSpeak.h" //biblioteca nuvem (thingspeak)
#include "ESP8266WiFi.h"  //biblioteca nodeMCU para wifi (praticamente wifi.h)
#include "ESP8266HTTPClient.h" //biblioteca para recepção de dados do ThingSpeak
//definições wi-fi
#define SSID_REDE     "GVT-DB37"  // Jonathan //coloque aqui o nome da rede que se deseja conectar - implementar inserção pela serial
#define SENHA_REDE    "S1E9013861"  // 981152592 //coloque aqui a senha da rede que se deseja conectar - implementar inserção pela serial
//#define PRINT_DEBUG_MESSAGES //testar func de debug no thingspeak.h

//para trasmitir para nuvem
unsigned long myChannelNumber = 261758;
const char * myWriteAPIKey = "HXEVMBS2K2KNA63U";
//para receber da nuvem
String thingSpeakAPI = "api.thingspeak.com";
String talkBackAPIKey = "M1M6FIGWD4KRS3A0"; 
String talkBackID = "15153"; 
const int checkTalkBackInterval = 15 * 1000;    // Time interval in milliseconds to check TalkBack (number of seconds * 1000 = int
//objeto tipo WifiClient - utilizado para conexão wifi da node mcu
WiFiClient  client;

int t1 = 0;
int t2 = 0;
String resp;

void setup()
{
  
  Serial.begin(9600);  //serial monitor
  //wifi
  FazConexaoWiFi(); //de acordo com o nome
  Serial.println("IoT aplicado à Agricultura de Precisão: Sistema de controle em Ambiente Agrícola");
  //ThingSpeak - transmissão
  ThingSpeak.begin(client); //entender. tem possibilidade de conflito com a recepção
  wl_status_t st = WiFi.status();
  Serial.println(st); 
  //ThingSpeak - recepção: O que é necessario para?

  Serial.print("\n t1: "); Serial.print(t1);
  Serial.print("\n t2: "); Serial.print(t2);
}

void loop() {
  if (t2 == 0){
    t1=t1+1;
    Serial.print("\n incremento t1: "); Serial.print(t1);
  }
  if (t2 == 1){
    t1=t1-1;
    Serial.print("\n Decremento t1: "); Serial.print(t1);
  }
  if (t1==0){
    t2=0;
    Serial.print("\n t2 (t2=0): "); Serial.print(t2);
    Serial.print("\n t1 (t2=0): "); Serial.print(t1);
  }
  if (t1==10){
    t2=1;
    Serial.print("\n t2 (t2=10): "); Serial.print(t2);
    Serial.print("\n t1 (t2=10): "); Serial.print(t1);
  }
  ThingSpeak.setField(1,t1); //set campos 
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); //send campos montados + para onde (channel)
  Serial.print("\n t1:"); Serial.println(t1);

  // Check ThingSpeak for TalkBack Commands 
  resp = checkTalkBack();

  if (resp == "TURN_ON") Serial.print("\n\n DEMAIS: FUNFO if TURN ON\n");
  if (resp == "TURN_OFF")Serial.print("\n\n DEMENOS: FUNFO if TURN oFF\n");
  if (WiFi.status() != WL_CONNECTED) FazConexaoWiFi(); 
  
  delay(checkTalkBackInterval); 
}

String checkTalkBack() 
{ 
   HTTPClient http;
  Serial.print("[HTTP] begin...\n"); //eu
      
  //montando a URL
  String talkBackCommand; 
  char charIn; 
  String talkBackURL =  "http://" + thingSpeakAPI + "/talkbacks/" + talkBackID + "/commands/execute?api_key=" + talkBackAPIKey; 
    
  http.begin(talkBackURL);
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
      if(httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
         talkBackCommand = payload;
         Serial.print("entrou! (getString()): ");
         Serial.println(payload);
      }
    } else Serial.printf("[HTTP] GET... failed, ERROR: %s\n", http.errorToString(httpCode).c_str());
  http.end();
  delay(1000); 
  return talkBackCommand;
}

/************************************************************
 Função: FazConexaoWiFi -> conecta ao WiFi - com debug na serial;
 Author: Grenan;
 Recebe: NULL;
 Retorna: NULL;
 Created on:    18/04/2017
 Modified on:   18/07/2017
************************************************************/
void FazConexaoWiFi(void){
    client.stop();
    Serial.println("Conectando-se à rede WiFi...");
    Serial.println();  
    delay(1000);
    WiFi.begin(SSID_REDE, SENHA_REDE); 
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connectado com sucesso!");  
    Serial.print("IP obtido: ");
    Serial.println(WiFi.localIP());
     //delay(1000);
}
