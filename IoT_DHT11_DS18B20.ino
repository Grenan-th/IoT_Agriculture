
///////////////////////////////////////////////////////////////////////////////
// Sketch Target e Description: Leitura dos sensores 
// Author:        Grenan
// Created on:    18/04/2017 - dois DHT11 + temperatura DS18B20
// Modified on:   18/07/2017
///////////////////////////////////////////////////////////////////////////////
 
//Bibliotecas
#include "ThingSpeak.h" //biblioteca nuvem (thingspeak)
#include "ESP8266WiFi.h"  //biblioteca nodeMCU para wifi
#include "DHT.h" //biblioteca DHT
#include "OneWire.h" //bliblioteca onewire trabalha em conjunto com a Dallas
#include "DallasTemperature.h" //em conjunto com OneWire executa leitura do sensor de temperatura DS18B20
//setup conexões elétricas e demais definições
#define DHTPIN1 4       // pino conectado ao sensor DHT11_1
#define DHTPIN2 5       // pino conectado ao sensor DHT11_2
#define DHTTYPE DHT11   // DHT 11 -> poderia ser DHT22 etc...
#define DS18B20PIN 14  // pino conectado ao sensor de temperatura
//definições wi-fi
#define SSID_REDE     "GVT-DB37"  //coloque aqui o nome da rede que se deseja conectar - implementar inserção pela serial
#define SENHA_REDE    "S1E9013861"  //coloque aqui a senha da rede que se deseja conectar - implementar inserção pela serial
 
//Definições das estruturas dos sensores, wifi&cloud - necessário melhor compreensão da estruturas
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
OneWire TempOneWire(DS18B20PIN);
DallasTemperature sensors(&TempOneWire);  
DeviceAddress tempSensorDS;               

unsigned long myChannelNumber = 261758;
const char * myWriteAPIKey = "HXEVMBS2K2KNA63U";

//char ssid[] = "<GVT-DB37>";    //  your network SSID (name) 
//char pass[] = "<S1E9013861>";   // your network password
//int status = WL_IDLE_STATUS;
WiFiClient  client;
 
//variaves globais - implementar para todos os sensores, registros dos extremos - É NECESSÁRIO??? talvez seja para registrar picos não informados à nuvem, precisa de if, enconttou valor maior envia
/*float tempMin = 999;  //temperatura máxima - garante que qq valor lido será menor
float tempMax = 0;    //temperatura minima - garante que qq valor lido será maior*/
//A NUVEM DEVE CALCULAR!!! BIG DATA

///////////////////////////////////////////////
// Função: Setup - Parametrização inicial
// Author: Grenan
// Recebe: NULL
// Retorna: NULL
// Created on:    18/04/2017
// Modified on:   18/07/2017
///////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);  //serial monitor
  dht1.begin();
  dht2.begin();
  sensors.begin();
  //wifi
  FazConexaoWiFi();
  Serial.println("IoT aplicado à Agricultura de Precisão: Sistema de controle em Ambiente Agrícola");
  //ThingSpeak
  ThingSpeak.begin(client);
  
  //Pode ser retirado: Localiza e mostra enderecos dos sensores
  Serial.println("Sensores Teste!");
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  //check se erro - melhorar
  if (!sensors.getAddress(tempSensorDS, 0))
    Serial.println("Sensores nao encontrados !");
  Serial.println();
  wl_status_t st = WiFi.status();
  Serial.println(st); 
}
 
///////////////////////////////////////////////
// Função: loop - Principal - inserir descrição detalhada
// Author: Grenan
// Recebe: NULL
// Retorna: NULL
// Created on:    18/04/2017
// Modified on:   18/07/2017
///////////////////////////////////////////////
void loop() {
  
  /*******DHT********///melhorar a nome das variaveis
  float t1 = lerdht(dht1,0);
  float t2 = lerdht(dht2,0);
  float h1 = lerdht(dht1,1);
  float h2 = lerdht(dht2,1);
  // Checa a existencia de erro e retorno para uma nova tentativa se erro.
  if (isnan(h1) || isnan(t1)|| isnan(h2) || isnan(t2)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  /******DS18B20**********/
  float tempDS = lertempDS();
 
  /*********VALORES EXIBIDOS NA SERIAL***********/
  Serial.print("Humidity 01: ");
  Serial.print(h1);
  Serial.print(" %\t");
  Serial.print("Temperature 01: ");
  Serial.print(t1);
  Serial.println(" *C ");
   
  Serial.print("Humidity 02: ");
  Serial.print(h2);
  Serial.print(" %\t");
  Serial.print("Temperature 02: ");
  Serial.print(t2);
  Serial.println(" *C ");
 
  Serial.print("Temperatura Ambiente: ");
  Serial.print(tempDS);
  Serial.println(" *C ");

  ThingSpeak.setField(1,t1);
  ThingSpeak.setField(2,t2);
  ThingSpeak.setField(3,h1);
  ThingSpeak.setField(4,h2);
  ThingSpeak.setField(5,tempDS);
  //lógica do lastConnectionTime
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); 
  
  delay(20000); 
}

////////////////////////////////////////////////
// Função: lerdht -> através dos parametros dhtx e qual a grandeza lida;
// Author: Grenan;
// Recebe:  dhtx: identifica qual sensor utilizado dht1 ou dht2 ;
//          varth: variavel temperature(0) ou humidity (1);
// Retorna: valor da temperatura ou da umidade do sensor, conforme selecionado; - poderia passar dos dois switch case
// Created on:    18/04/2017
// Modified on:   18/07/2017
///////////////////////////////////////////////

float lerdht(DHT dhtx, int varth){
  //temperatura
  if (varth==0){   
    // Leitura da temperatura em Celsius () || em ºF seria: float f = dht.readTemperature(true);
    return dhtx.readTemperature();
  }
  if (varth==1){
    return dhtx.readHumidity();
  }
}

////////////////////////////////////////////////
// Função: lertempDS -> ler o valor de temperatura do DS18B20;
// Author: Grenan;
// Recebe: NULL
// Retorna: valor da temperatura em ºC;
// Created on:    18/04/2017
// Modified on:   18/07/2017
///////////////////////////////////////////////
float lertempDS(){
  sensors.requestTemperatures();
  return sensors.getTempC(tempSensorDS);
}

////////////////////////////////////////////////
// Função: FazConexaoWiFi -> conecta ao WiFi - com debug na serial;
// Author: Grenan;
// Recebe: NULL;
// Retorna: NULL;
// Created on:    18/04/2017
// Modified on:   18/07/2017
///////////////////////////////////////////////
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
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
 
    delay(1000);
}


 
