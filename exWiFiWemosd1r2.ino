#include <ESP8266WiFi.h>

WiFiServer server(80);

//char buf[100];
String buf ;
String cabecJSON = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n";
float fValorI = 5.2;
String estadoI = "OK";
float fValorU = 225.6;
String estadoU = "NOK";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(100);

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
}

void loop() {
  // put your main code here, to run repeatedly:
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
          buf += "\"";
          buf += String(fValorI);
          buf += "\"";
          buf += ",\"status\":";
          buf += "\"";
          buf += String(estadoI);
          buf += "\"";
          buf += "}}";

          Serial.print("buf Corriente\r\n");
        }
        else if (req.indexOf("voltaj") > 0) {

          buf += "{\"Tension\":{\"Valor\":";
          buf += "\"";
          buf += String(fValorU);
          buf += "\"";
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
