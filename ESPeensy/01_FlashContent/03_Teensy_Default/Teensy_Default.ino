#include <usb_private.h>
#include <SD.h>
#include <SPI.h>

const int led_pin = 11; 
int summe;

int ledkeys(void)       {return int(keyboard_leds);}
bool is_scroll_on(void) {return ((ledkeys() & 4) == 4) ? true : false;}
bool is_caps_on(void)   {return ((ledkeys() & 2) == 2) ? true : false;}
bool is_num_on(void)    {return ((ledkeys() & 1) == 1) ? true : false;}
bool flashing = false;

void unpress_key(void)
{
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.send_now();
  delay(500);
}
void taster()
  {
    int val;
    int s = analogRead(3);
    int t = analogRead(2);
    int u = analogRead(1);
    int v = analogRead(0);
    int tasten[4];

    if (s == 1){tasten[0] = 1;}else{tasten[0] = 0;}
    if (t == 1){tasten[1] = 1;}else{tasten[1] = 0;}
    if (u == 1){tasten[2] = 1;}else{tasten[2] = 0;}
    if (v == 1){tasten[3] = 1;}else{tasten[3] = 0;}
    summe = tasten[0]*1 + tasten[1]*2 + tasten[2]*4 + tasten[3]*8;
    // Serial.print (String(summe) + "\n");
  } 
void press_numlock(void)
{
  Keyboard.set_key1(KEY_NUM_LOCK);
  Keyboard.send_now();
  delay(200);
}
void CheckSerial (String line){
  // hole Zeilenweise Daten aus Serial1 (ESP8266-E1)
  // Zeile ?bertragen
  
  // Zeile auswerten
  if (line.startsWith("RAW") == true){
    Keyboard.print (line.substring(4));
  };
  if (line.startsWith("FIL") == true){
    StartFromSD (line.substring(4));
  };
  if (line.startsWith("DUC") == true){
    RunDuckyLine (line.substring(4));
  };
  if (line.startsWith("GET") == true){
    // Dateien der SD ausgeben
  };
  
}

void RunDuckyLine(String line) {

  if (line.startsWith("ALT")) {    
    Keyboard.set_modifier(MODIFIERKEY_ALT);
    keys(line.substring(4));
    Keyboard.send_now();
    unpress_key();
  }

  if (line.startsWith("CTRL")) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    keys(line.substring(5));
    Keyboard.send_now();
    unpress_key();
  }
  
  if (line.startsWith("CONTROL")) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    keys(line.substring(8));
    Keyboard.send_now();
    unpress_key();
  }
  
  if (line.startsWith("SHIFT")) {
    Keyboard.set_modifier(MODIFIERKEY_SHIFT);
    keys(line.substring(6));
    Keyboard.send_now();
    unpress_key();
  }
  
  if (line.startsWith("WINDOWS")) {
    Keyboard.set_modifier(MODIFIERKEY_GUI);
    keys(line.substring(8));
    Keyboard.send_now();
    unpress_key();
  }
  
    if (line.startsWith("MENU")) {
    Keyboard.set_modifier(MODIFIERKEY_RIGHT_GUI);
    keys(line.substring(5));
    Keyboard.send_now();
    unpress_key();
  }
  
    if (line.startsWith("ENTER")) {
    Keyboard.set_key1(KEY_ENTER);
    keys(line.substring(6));
    Keyboard.send_now();
    unpress_key();
  }
  
    if (line.startsWith("ALT-SHIFT")) {
    Keyboard.set_modifier(MODIFIERKEY_ALT | MODIFIERKEY_SHIFT);
    keys(line.substring(10));
    Keyboard.send_now();
    unpress_key();
  }
  
  if (line.startsWith("CTRL-ALT")) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL | MODIFIERKEY_ALT);
    keys(line.substring(9));
    Keyboard.send_now();
    unpress_key();
  }
  
  if (line.startsWith("CTRL-SHIFT")) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL | MODIFIERKEY_SHIFT);
    keys(line.substring(11));
    Keyboard.send_now();
    unpress_key();
  }
  
  if (line.startsWith("DEFAULT_DELAY")) {
    delay (500);
  }
  if (line.startsWith("DEFAULTDELAY")) {
    delay (500);
  }
  
  if (line.startsWith("DELAY")) {
    delay (line.substring(6).toInt());
  }
  
  if (line.startsWith("STRING")) {
    Keyboard.print (line.substring(7));
  }
  
  if (line.startsWith("STRING-ENTER")) {
    Keyboard.print (line.substring(13));
    Keyboard.set_key1(KEY_ENTER);
    Keyboard.send_now();
    unpress_key();
  }
  
  if (line.startsWith("DOWNARROW")) {
    Keyboard.set_key1(KEY_DOWN);
    Keyboard.send_now();
    delay (line.substring(10).toInt());
    unpress_key();
  }
} 
void keys(String key) {
  if(key=="A") {  Keyboard.set_key1 (KEY_A) ;}
  if(key=="B") {  Keyboard.set_key1 (KEY_B) ;}
  if(key=="C") {  Keyboard.set_key1 (KEY_C) ;}
  if(key=="D") {  Keyboard.set_key1 (KEY_D) ;}
  if(key=="E") {  Keyboard.set_key1 (KEY_E) ;}
  if(key=="F") {  Keyboard.set_key1 (KEY_F) ;}
  if(key=="G") {  Keyboard.set_key1 (KEY_G) ;}
  if(key=="H") {  Keyboard.set_key1 (KEY_H) ;}
  if(key=="I") {  Keyboard.set_key1 (KEY_I) ;} 
  if(key=="J") {  Keyboard.set_key1 (KEY_J) ;} 
  if(key=="K") {  Keyboard.set_key1 (KEY_K) ;} 
  if(key=="L") {  Keyboard.set_key1 (KEY_L) ;} 
  if(key=="M") {  Keyboard.set_key1 (KEY_M) ;} 
  if(key=="N") {  Keyboard.set_key1 (KEY_N) ;} 
  if(key=="O") {  Keyboard.set_key1 (KEY_O) ;} 
  if(key=="P") {  Keyboard.set_key1 (KEY_P) ;} 
  if(key=="Q") {  Keyboard.set_key1 (KEY_Q) ;} 
  if(key=="R") {  Keyboard.set_key1 (KEY_R) ;} 
  if(key=="S") {  Keyboard.set_key1 (KEY_S) ;} 
  if(key=="T") {  Keyboard.set_key1 (KEY_T) ;} 
  if(key=="U") {  Keyboard.set_key1 (KEY_U) ;} 
  if(key=="V") {  Keyboard.set_key1 (KEY_V) ;} 
  if(key=="W") {  Keyboard.set_key1 (KEY_W) ;} 
  if(key=="X") {  Keyboard.set_key1 (KEY_X) ;} 
  if(key=="Y") {  Keyboard.set_key1 (KEY_Y) ;} 
  if(key=="Z") {  Keyboard.set_key1 (KEY_Z) ;}
  if(key=="1") {  Keyboard.set_key1 (KEY_1) ;}  
  if(key=="2") {  Keyboard.set_key1 (KEY_2) ;} 
  if(key=="3") {  Keyboard.set_key1 (KEY_3) ;} 
  if(key=="4") {  Keyboard.set_key1 (KEY_4) ;} 
  if(key=="5") {  Keyboard.set_key1 (KEY_5) ;} 
  if(key=="6") {  Keyboard.set_key1 (KEY_6) ;} 
  if(key=="7") {  Keyboard.set_key1 (KEY_7) ;} 
  if(key=="8") {  Keyboard.set_key1 (KEY_8) ;} 
  if(key=="9") {  Keyboard.set_key1 (KEY_9) ;} 
  if(key=="0") {  Keyboard.set_key1 (KEY_0) ;} 
  if(key=="ENTER") {  Keyboard.set_key1 (KEY_ENTER) ;}  
  if(key=="SPACE") {  Keyboard.set_key1 (KEY_SPACE) ;} 
  if(key=="TAB") {  Keyboard.set_key1 (KEY_TAB) ;} 
} 

void setup (void){
  Serial.begin(115200);
  Serial1.begin(115200);
  taster();
  blink_fast(20,80);
  delay (3000);
  wait_for_drivers(2000);
  delay (8000);
  if (analogRead(0) == 1){StartFromSD("1.txt");}
  if (analogRead(1) == 1){StartFromSD("2.txt");}
  if (analogRead(2) == 1){StartFromSD("3.txt");}
  if (analogRead(3) == 1){StartFromSD("4.txt");}
}
void blink_fast(int blinkrate,int delaytime)
{
  int blinkcounter=0;
  for(blinkcounter=0; blinkcounter!=blinkrate; blinkcounter++)
  {
    digitalWrite(led_pin, HIGH);
    delay(delaytime);
    digitalWrite(led_pin, LOW);
    delay(delaytime);
  }
}
void wait_for_drivers(unsigned int speed)
{
  bool numLockTrap = is_num_on();
  while(numLockTrap == is_num_on())
  {
    blink_fast(5,80);
    press_numlock();
    unpress_key();
    delay(speed);
  }
  press_numlock();
  unpress_key();
  delay(speed);
}
void StartFromSD (String Filename){
    File myFile;
    String line;
    // Serial.println ("Init SD");
    if (SD.begin(BUILTIN_SDCARD)) {
      if (SD.exists("1.txt")) {
        myFile = SD.open("1.txt");
          if (myFile) {
            while (myFile.available()) {
              line = myFile.readStringUntil('\n');
              RunDuckyLine (line);
            }
            myFile.close();
          }
      }
  }
}

void loop (){
    // Send bytes from ESP8266 -> Teensy to Computer
    if ( Serial1.available() ) {
        Serial.write( Serial1.read() );
    }

    // Send bytes from Computer -> Teensy back to ESP8266
    if ( Serial.available() ) {
        Serial1.write( Serial.read() );
        flashing == true;
    }
	
    if (flashing == false){
      String line = Serial1.readStringUntil('\r');
      if (line.length() > 5){
        if (line.startsWith ("/DCL")){
          RunDuckyLine (line.replace("/DCL?PAR=","").replace("&STR="," "));
        }
        if (line.startsWith ("/DCF")){
          StartFromSD (line.replace("/DCF?PAR=SD_FILE","").replace("&STR=",""));
        }
        Keyboard.print (line);
      }
    }
}
