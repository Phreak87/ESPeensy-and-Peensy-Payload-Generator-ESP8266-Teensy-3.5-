// ---------------------------------
// Includes
// ---------------------------------
#include <FS.h>
#include <Time.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ---------------------------------
// Config-Variablen
// ---------------------------------
String 		  ESPeensyVersion 	= "1.0";
File 		    UploadFile;
String 		  ClientIP;
String 		  fileName;
int 		    FSTotal;
int 		    FSUsed;
// WLAN ----------------------------
const char* ssid     	  = "TnSy35";		  // AP-SSID
const char* password  	= "Peensy3.5!";	// AP-PWD
IPAddress 	ip(192, 168, 0, 1);			    // AP-IP
IPAddress 	netmask(255, 255, 255, 0);	// AP-NM
const char* DNSName     = "TnSy35";     // AP-DNS

String Head1 = "<HTML><HEAD><meta name='viewport' content='width=device-width, initial-scale=1'></meta><title>ESPeensy Control Panel</title><STYLE>* {font-family:verdana;font-size:14px;border-radius:3px;Margin:3px;Padding:2px;}div {Border:1px solid grey;Margin-Bottom:7px;background-color:#E6E6E6;}";
String Head2 = "h4 {Margin-Top:-2px;Margin-Left:-2px;Margin-Right:-2px;Padding:3px;background-color:#BDBDBD;}div>a{font-family:verdana;font-size:16px;Color:White;}div>a.Mark{background-color:#E6E6E6;color:black}Input,Select,Button{Border:1px solid lightgrey;Height:30px;Width:99%;Margin:0px;}";
String Head3 = "Input:Hover,Select:Hover,Button:Hover,tr:hover{box-shadow: 0 0 0 1px #0080FF inset}table {Margin-Top:8px;width:99%;border-collapse:collapse;}th {text-align:left;height:20px;}td{Padding-right:6px}tr:nth-child(even) {background-color: #D8D8D8;}tr:first-child {box-shadow: 0 0 0 0px #0080FF inset}";
String Head4 = "</STYLE></HEAD><BODY><DIV Style='background-color:#696969;Color:White;border-top: 2px solid blue;'><a Href='/'>Teensy Controls</a><a Href='wifi'>Wifi Settings</a><a Href='fupload'>File Manager</a><a Href='updatefwm'>Firmware Manager</a></DIV>";

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

  delay(1000);
  WiFi.mode(WIFI_AP);                    // Configure to be a Access-Point
  WiFi.softAPConfig(ip, ip, netmask);   // configure ip address for softAP 
  WiFi.softAP(ssid, password);

  server.on("/", web_root);
  server.on("/wifi", web_wifi);
  server.on("/fupload", web_fil);
  server.on("/updatefwm", web_fup);
 
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

//------------------------------------------------
//FSBrowser application - format bytes
//------------------------------------------------
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
  Content += "<DIV><H4>Wifi Attacks (Needs ESP8266 Reboot)</H4><Table><TR><TD><Button>Wifi-Deauth</Button></TD><TD><Button>Wifi-FakeAP</Button></TD>";
  Content += "</TR></Table></DIV><DIV><H4>Additional Information</H4><Table><TR><TH>Internal</TH><TH>External</TH></TR><TR><TD Width='50%'>";
  Content += "<i>IP-Adress: {IP}</i><BR><i>Subnetmask: {SUBNET}</i><BR><i>DNS-Server: {DNS}</i><BR><i>Gateway:    {GATE}</i></TD><TD Width='50%'>";
  Content += "<i>IP-Adress: {IP}</i><BR><i>API.IPIFY.ORG</i><BR></TD></TR></Table></DIV><DIV class='div1'><H4>Wifi-APs</H4><table><TH>SSID</TH>";
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

// LOOP
void loop(void){
  server.handleClient();
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
  server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  ESP.restart();
}
void handle_esp_restart() {
  ESP.restart();
}

