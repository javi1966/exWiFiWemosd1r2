#include <ESP8266WiFi.h>
#include <SPI.h>
#include "MCP3002.h"

#define  LED  5  //D1 es GPIO5

MCP3002 adc(SS);


WiFiServer server(80);
WiFiClient client;

//char buf[100];
String buf ;
String cabecJSON = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n";
float fValorI = 5.2;
String estadoI = "OK";
float fValorU = 225.6;
String estadoU = "NOK";

bool bActualiza = true;
const int timerUpdate = 30; //1/2 minuto
String writeAPIKey = "F81DR9CCLURUGK87";
const char* host = "api.thingspeak.com";
int valor;
float amp;
float tension = 224;

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

  WiFi.mode(WIFI_STA);
  //WiFi.softAP("MOVISTAR_47E8","ndfBakCEvtHwj8jSSEMJ");
  // WiFi.softAP("Wireless-N","z123456z");
  //WiFi.softAP("Hello_IoT", "12345678");
  // WiFi.softAP("AI-THINKER_C0E300");
  WiFi.begin("Wireless-N", "z123456z");
  WiFi.config(IPAddress(192, 168, 1, 50), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));



  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)  //10 segundos
  {
    Serial.write('.');
    //Serial.print(WiFi.status());
    delay(500);
  }
  Serial.println();

  // Check connection
  if (WiFi.status() == WL_CONNECTED)
  {
    // ... print IP Address
    Serial.print("IP address STATION: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Can not connect to WiFi station. Go into AP mode.");

    // Go into software AP mode.
    WiFi.mode(WIFI_AP);

    delay(10);

    WiFi.softAP("AI-THINKER_C0E300");

    Serial.print("IP address Access Point: ");
    Serial.println(WiFi.softAPIP());
  }




  IPAddress http_server_ip = WiFi.localIP();

  server.begin();

  Serial.print("nuestra server IP:");
  Serial.print(http_server_ip);
  Serial.print("\r\n");


  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(TimingISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();


}

void loop() {
  // put your main code here, to run repeatedly:
  char temp[10];


  if (bActualiza) {

    valor = adc.analogRead(0);
    Serial.println(String(valor));
    amp = (15.5 * (valor - 508) / 233); // 355 valor adc 867 - valor vcc/2 512
    /*
      sprintf(temp, "%2f", amp);
      Serial.println(String(V));

    */
    if (client.connect(host, 80)) {

      // Construct API request body
      String body = "field1=";
      body +=  String(amp);
      body += "&field2=";
      body += String(tension);




      Serial.println(body);

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(body.length());
      client.print("\n\n");
      client.print(body);
      client.print("\n\n");

    }

    client.stop();

    digitalWrite(LED, HIGH); //flashing led
    delay(500);
    digitalWrite(LED, LOW);

    bActualiza = false;

  }


  WiFiClient client = server.available();

  if (client) {
    Serial.print("NUEVO CLIENTE\r\n");

    while (client.connected()) {

      String req = client.readStringUntil('\r');
      Serial.print(req);
      Serial.print("\r\n");

      if (req.indexOf("MonitorEnergia") > 0)
      {
        /*client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();  */
        if (req.indexOf("corriente") > 0) {

          buf += "{\"Corriente\":{\"Valor\":";

          buf += String(amp);

          buf += ",\"status\":";
          buf += "\"";
          buf += String(estadoI);
          buf += "\"";
          buf += "}}";

          Serial.print("buf Corriente\r\n");
        }
        else if (req.indexOf("voltaj") > 0) {

          buf += "{\"Tension\":{\"Valor\":";

          buf += String(tension);

          buf += ",\"status\":";
          buf += "\"";
          buf += String(estadoU);
          buf += "\"";
          buf += "}}";

          Serial.print("buf Tension\r\n");
        }

      }//if (req.indexOf("/MonitorEnergia"))

      Serial.print(cabecJSON);
      Serial.print(buf);
      Serial.print("\r\n");

      client.println(cabecJSON);
      client.println(buf);
      buf = "";
      break;
    }
    delay(1);

    client.stop();
    Serial.print("client disconnected\r\n");

  }

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
