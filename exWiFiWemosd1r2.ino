#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include "MCP3002.h"

#define  LED  5  //D1 es GPIO5

MCP3002 adc(SS);



WiFiClient client;

//char buf[100];
String buf ;
String cabecJSON = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n";
float fValorI = 5.2;
String estadoI = "OK";
float fValorU = 225.6;
String estadoU; //pendiente 

bool bActualiza = true;
const int timerUpdate = 15;//60 * 2 ; //2 minutos
String writeAPIKey = "U8CFXKG9VLCAE1TD";
const char* host = "api.thingspeak.com";

float amp;
float tension;
//******************************************************************************************************************
double offsetI;
double filteredI;
double sqI,sumI;
int16_t sampleI;
double Irms;

double squareRoot(double fg)  
{
  double n = fg / 2.0;
  double lstX = 0.0;
  while (n != lstX)
  {
    lstX = n;
    n = (n + fg / n) / 2.0;
  }
  return n;
}

double calcIrms(unsigned int Number_of_Samples)
{
  /* Be sure to update this value based on the IC and the gain settings! */
  float multiplier = 0.039F;    /* ADS1115 @ +/- 4.096V gain (16-bit results) */
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI = adc.analogRead(0);//ads.readADC_Differential_0_1();

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset, 
  //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;
    //filteredI = sampleI * multiplier;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum 
    sumI += sqI;
  }
  
  Irms = squareRoot(sumI / Number_of_Samples)*multiplier; 

  //Reset accumulators
  sumI = 0;
    
 
  return Irms;
}

//*******************************************************************************************************************

void lecturaValores(){

  int valor;

  //valor = adc.analogRead(0);
  Serial.print("Corriente: ");
  //Serial.println(String(valor));
  //amp = (15.5 * (valor - 508) / 233); // 355 valor adc 867 - valor vcc/2 512

  double corriente = calcIrms(2048);
  Serial.println(String(corriente));

  
  if(  valor > 755)
    estadoI="NOK";
  else
    estadoI="OK";  

  valor = adc.analogRead(1);
  Serial.print("Tension: ");
  Serial.println(String(valor));

  if( valor < 870 || valor > 891)
    estadoU="NOK";
  else
    estadoU="OK";  

  tension = (245.0 * (valor - 511) / 388);

  if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;
      
      String body = "field1=";
      body +=  String(corriente);
      body += "&field2=";
      body +=  String(tension);

      Serial.println(body);
      
      http.begin(host, 80,"https://api.thingspeak.com/update?api_key="+writeAPIKey+"&"+body);

       int httpCode = http.GET();

       if (httpCode == HTTP_CODE_OK) {


          //String payload = http.getString();

         Serial.print("Resultado: ");
         Serial.println(http.getString());

       }   

        http.end();
     }//WL_CONNECTED

}

void setup() {
  // put your setup code here, to run once:

  pinMode(A0, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  adc.begin();

  delay(100);

  digitalWrite(LED, HIGH);


  Serial.println("\r\n");
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);

 
 // WiFi.begin("Wireless-N", "z123456z");
  WiFi.begin("MOVISTAR_B855", "AD2890A7F423CBF3BB79");
  
  int timeout = 0;
  //unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) // && millis() - startTime < 10000)  //10 segundos
  {
    Serial.write('*');
    //Serial.print(WiFi.status());
    delay(500);
    if (++timeout > 100)
    {
      Serial.println("Sin Conexion WIFI");
      while (1) {
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED, LOW);
        delay(100);
      }
    }
  }
  
  Serial.println();


  Serial.println(WiFi.localIP());
  Serial.printf("Chip ID = %08X", ESP.getChipId());
  Serial.println("");

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(TimingISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();
 

  delay(2000);
}


void loop() {
  // put your main code here, to run repeatedly:
  char temp[10];


  if (bActualiza) {

    lecturaValores();
  

    digitalWrite(LED, HIGH); //flashing led
    delay(500);
    digitalWrite(LED, LOW);

    bActualiza = false;

  }

  delay(20000); 

}


//****************************************************************************************************
void TimingISR() {

  static int cntTemp = 0;

  if (++cntTemp > timerUpdate) // 1 minuto
  {
    bActualiza = true;

    cntTemp = 0;
  }
  //bActualiza = true;

  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec

}
