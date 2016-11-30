# Teensy-Payload-Generator

Generates Teensy (Peensy) Payloads simply from reusable snippets.
Reusable means: create a function once (like open admin-cmd) and
use this code for each other payload you generate. so you can build
flexible penetration tests very easy without programming knowledge.

![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Penteset.JPG "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Snippet.JPG "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/front.jpg "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/back.jpg "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Webinterface.png "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Wifi.png "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/FileUpload.png "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Firmware.png "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Verschaltung.jpg "Logo Title Text 1")

## Install procedure
1. Solder all needed DIP-switches and ESP8266-12E to the Teensy board like described in the last picture
2. Download Arduino Studio 
3. Add ESP8266 from Arduino librarys
4. Download latest Teensy librarys and run setup procedure.
5. Open Arduino Studio and select hardware: Teensy 3.5 / USB-Type: Serial. Then copy and paste the content of Teensy_as_Flasher.txt to the source-code section of Arduino Studio,
   plug in your teensy and press upload. Write down the shown Com-Port if driver finished installing. 
6. Now copy and paste ESP_V2.txt to Arduino Studio and Select Hardware: ESP8266 / Serial-Port (from driver installation above). Remove the Teensy and remember the USB Port. 
   Now *read my lips* press and HOLD the soldered flash button while plugging in Teensy into the SAME USB Port you used before. Release the flash button and click upload.
7. Back to Arduino Studio again and choose Board: Teensy 3.5 / USB-Type: KEYBOARD. Run Peensy-Payload-Generator to generate and export your own C-Code.
8. Copy and Paste C-Code from Teensy-Payload-Generator to Arduino Studio and Flash Teensy again.
9. Happy Pentesting ;)

## DIP-Switches addition
If you got an 4 DIP-switch connected to A0-A3 you can choose between 2 different functions:
> Binary:   You can choose between up to 16 different payloads or functions, which can be stored on the SDCard or the internal storage. (only one function per plug)
> Absolute: Assign a payload or function to one dip-switch. You can execute up to 4 payloads or functions in order.

## ReadFromSD 
If you got an SD-Card inserted, you can start a Duckyscript directly from the SDCard:
Simply add StartFromSD with the Filename as parameter and you´re done! 
You can add a switch-position too if you like to start from SDCard only if a specific switch postition is set.

## ReadFromSerial (requires an prepared ESP32/ESP8266 1E or 12E)
Connect a mobile device or computer via wifi or bluetooth to the ESP8266-12E / 1E or ESP32.
Open the internet page of the ESP and select "command and text" or "file" to execute.
The selected command will be send over serial to teensy: the following commands are possible:
FIL: Executes a Ducky-file from the SDCard
DUC: Executes a single Ducky-command for example STRING-ENTER cmd
To prepare your ESP you need to flash ESP_V2.txt to it. 
WiFi-AP: SSID = PnSy, PWD: Peensy3.5!, IP: 192.168.0.1

## Keylogger (Future)
You can connect a Keyboard to Teensy and send the keypresses to the target and save it on the SDCard too.
Waiting for instructions how to use USB-Host mode on Teensy 3.5.

## Teensy as SDCard (Future)
Waiting for hardwaretype drive(SD) + keyboard in arduino studio.
In between use a micro usb hub with a usb flashdisk.

Script-Sources:

## DuckyScript:

Copy a ducky-script payload to a new snippet and let it convert to c-code.
I extended ducky-script by the STRING-ENTER Command.
Example:
> STRING-ENTER @echo off & Color 01
returns:
> Keyboard.print ("@echo off & Color 01");
> Keyboard.set_key1(KEY_ENTER);Keyboard.send_now();unpress_key();

## Wscript / Powershell Script
Copy any Wscript-content to a new snippet and let it convert to C-Code.
On the target it will generate an executable script-file with the content
you copied into the snippet. The snippet automatically masks critical characters too.
Example:
> Test.vbs Content:[Msgbox 'Test'] 
> echo Msgbox 'test' >> Test.vbs
> cscript Test.vbs

## Batch-Script
Copy any Batchfile-content to a new snippet and let it convert to C-Code.
Each line of the Batch will be executed as a single command. 
The snippet automatically masks critical characters too.

## C-Code
Write down your own c-code.
C-code can be:
a complete function (with Parameters) 
or just commands. If you just enter commands the snippet will generate a function for these lines
and call the function.