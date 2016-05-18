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
  WiFi.mode(WIFI_AP);
  //WiFi.softAP("MOVISTAR_47E8","ndfBakCEvtHwj8jSSEMJ");
  // WiFi.softAP("WLAN_BC00","693e0a0635eff7934b87");
  WiFi.softAP("Hello_IoT", "12345678");
  server.begin();
  Serial.begin(115200);
  IPAddress http_server_ip = WiFi.softAPIP();
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
