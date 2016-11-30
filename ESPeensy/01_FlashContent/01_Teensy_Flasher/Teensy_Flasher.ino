//**** Using a Teensy 3.1 to Connect an ESP8266 to PC USB Serial *******
//     Compiled with Arduino 1.60 and Teensyduino 1.21b6
//     ESP8266 Firmware: AT21SDK95-2015-01-24.bin


long LED_14_TimeOn=0;
long LED_15_TimeOn=0;

void setup() {
  
      pinMode(14, OUTPUT);  // YELLOW LED - SENDING
      pinMode(15, OUTPUT);  // RED LED - RECEIVING

    // Setup computer to Teensy serial
    Serial.begin(115200);

    // Setup Teensy to ESP8266 serial
    // Use baud rate 115200 during firmware update
    Serial1.begin(115200);

}

void loop() {

    // Send bytes from ESP8266 -> Teensy to Computer
    if ( Serial1.available() ) {
         digitalWriteFast(14, HIGH);   // set the LED on
         LED_14_TimeOn = millis();
        Serial.write( Serial1.read() );
    }

    // Send bytes from Computer -> Teensy back to ESP8266
    if ( Serial.available() ) {
        digitalWriteFast(15, HIGH);   // set the LED on
        LED_15_TimeOn = millis();
        Serial1.write( Serial.read() );
    }
	
		if (millis() - LED_14_TimeOn > 20) {
		digitalWriteFast(14, LOW);   // set the LED off
		}
		if (millis() - LED_15_TimeOn > 20) {
		digitalWriteFast(15, LOW);   // set the LED off
		}

}