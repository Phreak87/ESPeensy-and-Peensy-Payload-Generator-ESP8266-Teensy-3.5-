#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid     = "TnSy";
const char* password  = "Peensy3.5!";

IPAddress ip(192, 168, 0, 1);       // IP of the AP
IPAddress netmask(255, 255, 255, 0);  // Netmask

WiFiServer server(80);

void setup() {
  delay(500);
  Serial.begin(115200);
  WiFi.softAPConfig(ip, ip, netmask);   // configure ip address for softAP 
  WiFi.softAP(ssid, password);        // configure ssid and password for softAP
  server.begin();                   // start TCP server  
}

void loop() {

  // ## Connect Client
  WiFiClient client = server.available(); 
  if (!client){return;}
  //Serial.println("Client connected");

  // ## Warte auf Daten
  unsigned long ultimeout = millis()+250;
  while(!client.available() && (millis()<ultimeout)){delay(1);}
  if(millis()>ultimeout){Serial.println("client connection time-out!");return;}
  //Serial.println("Client received");

  // ## Hole Datenheader
  String sRequest = client.readStringUntil('\r'); client.flush();
  if(sRequest==""){client.stop();return;}
  //Serial.println(sRequest);
    
  // ## Hole Daten aus Header
  // 1) GET / HTTP/1.1
  // 2) GET /Index.html HTTP/1.1
  // 3) GET /Index.html?DCL=STRING-ENTER%20Hallo HTTP/1.1
  String sPath="",sParam="", sCmd="", sResponse="";
  int iGET = sRequest.indexOf("GET");       //Serial.println(iGET);  // 0
  int iQST = sRequest.indexOf("?",iGET+4);  //Serial.println(iQST);  // 16
  int iEQL = sRequest.indexOf("=",iGET+4);  //Serial.println(iEQL);  // 20
  int iSPE = sRequest.indexOf(" ",iGET+4);  //Serial.println(iSPE);  // 41
    
  if(iQST==-1){sPath  = sRequest.substring(iGET+4,iSPE);}   // 0-41   (2)=> /Index.html
  if(iQST>=0) {sPath  = sRequest.substring(iGET+4,iQST);    // 0-16   (3)=> /Index.html
                        sParam = sRequest.substring(iQST+1,iEQL);   // 16-20  (3)=> ?DCL
                        sCmd   = sRequest.substring(iEQL+1,iSPE);}  // 20-41  (3)=> =STRING-ENTER%20Hallo
    Serial.println(sPath);
    //Serial.println(sParam);
    //Serial.println(sCmd);

  String html1 = "<HTML><HEAD></HEAD><BODY><H1>Ducky remote payload executor</H1>";
  String html2 = "<form method='GET' action='/DCL'><select style='width:150px' name='PAR'>";
  String html3 = "<Option>STRING</option><Option>ENTER</option><Option>STRING-ENTER</option><Option>ALT</option><Option>CTRL</option><Option>SHIFT</option><Option>WINDOWS</option><Option>MENU</option>";
  String html4 = "<Option>ALT-SHIFT</option><Option>CTRL-ALT</option><Option>CTRL-SHIFT</option><Option>COMMAND</option><Option>GUI</option><Option>REPEAT</option><Option>DOWNARROW</option></Select><input  style='width:150px' name='STR' type='text' value='cmd'/>";
  String html5 = "<button style='width:150px' type='submit'>Senden (DuckyLine)</button></form><form method='GET' action='/DCF'> <select style='width:150px' name='PAR' size='1'><option>SD_FILE</option></Select>";
  String html6 = "<select style='width:150px' name='STR' size='1'><option>1.txt</option>";
  String html7 = "<option>2.txt</option><option>3.txt</option><option>4.txt</option><option>5.txt</option><option>6.txt</option><option>7.txt</option><option>8.txt</option><option>9.txt</option>";
  String html8 = "<option>10.txt</option><option>11.txt</option><option>12.txt</option><option>13.txt</option><option>14.txt</option><option>15.txt</option><option>16.txt</option></Select>";
  String html9 = "<button style='width:150px' type='submit'>Senden (DuckyFile)</button></form></BODY></HTML>";

  sResponse = html1 + html2 + html3 + html4 + html5 + html6 + html7 + html8 + html9;

  // if(sPath=="/DCL"){sResponse="<html><head><title>DCL OK</title></head><body><h1>DCL OK</h1><p>Sended to Teensy</p></body></html>";}
  // if(sPath=="/DCF"){sResponse="<html><head><title>DCF OK</title></head><body><h1>DCF OK</h1><p>Sended to Teensy</p></body></html>";}
  // if(sPath=="/RAW"){sResponse="<html><head><title>RAW OK</title></head><body><h1>RAW OK</h1><p>Sended to Teensy</p></body></html>";}
  String sHeader  = "HTTP/1.1 200 OK\r\n";
  sHeader += "Content-Length: ";
  sHeader += sResponse.length();
  sHeader += "\r\n";
  sHeader += "Content-Type: text/html\r\n";
  sHeader += "Connection: close\r\n";
  sHeader += "\r\n";

  client.print(sHeader);          // Send the response-Header to the client
  client.print(sResponse);        // Send the response to the client
  client.stop();

}
