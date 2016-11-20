# Teensy-Payload-Generator

Generates Teensy (Peensy) Payloads simply from reusable snippets.
Reusable means: create a function once (like open admin-cmd) and
use this code for each other payload you generate. so you can build
flexible penetration tests very easy without programming knowledge.

![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Penteset.JPG "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/Snippet.JPG "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/front.JPG "Logo Title Text 1")
![alt text](https://github.com/Phreak87/Teensy-Payload-Generator/blob/master/back.JPG "Logo Title Text 1")

## DIP-Switches addition
if you got an DIP-switch connected to A0-A3 you can choose between 2 different functions:
> Binary:   Start a function if the 4 switches have the right number (0110 = 6 = [0*1] + [1*2] + [1*4] + [0*8])
> Absolute: Start a function if one defined DIP-switch is set to on (1 = function starts if DIP switch 1 is set)

## ReadFromSD 
if you got an SD-Card inserted, you can start a Duckyscript directly from the SDCard:
Simply add StartFromSD with the Filename as parameter and you done! 
You can add a switch-position too if you like to start from SDCard only if a specific switch postition is set.

## ReadFromSerial
connect a mobile device or computer via wifi or bluetooth to the ESP8266-12E / 1E or ESP32.
open the internet page of the ESP and select a command and text or file to execute.
the selected command will be send over serial to the teensy: the following commands are possible:
FIL: Executes a Ducky-file from the sd-card
DUC: Executes a single Ducky-command

## Keylogger (Future)
You can connect a Keyboard to Teensy and send the keypresses to the Target and save it on the SDCard too.
Waiting for instructions how to use USB-Host mode on Teensy 3.5.

## Teensy as SDCard (Future)
waiting for hardwaretype drive(SD) + keyboard in arduino studio.
in between use a micro usb hub with a usb flashdisk.

Script-Sources:

## DuckyScript:

copy and ducky-script payload to a new snippet and let it convert to c-code.
extended ducky-script by STRING-ENTER Command.
Example:
> STRING-ENTER @echo off & Color 01
returns:
> Keyboard.print ("@echo off & Color 01");
> Keyboard.set_key1(KEY_ENTER);Keyboard.send_now();unpress_key();

## Wscript / Powershell Script
copy any Wscript-content to a new snippet and let it convert to C-Code.
on the target it will generate an executable script-file with the content
you copied into the snippet. The snippet automatically masks critical characters too.
Example:
> Test.vbs Content:[Msgbox 'Test'] 
> echo Msgbox 'test' >> Test.vbs
> cscript Test.vbs

## Batch-Script
> copy any Batchfile-content to a new snippet and let it convert to C-Code.
> each line of the Batch will be executed as a single command. 
> The snippet automatically masks critical characters too.

## C-Code
> write down your own c-code.
> c-code can be:
> a complete function (with Parameters) 
> if you just enter commands the snippet will generate a function for these lines
> and call the function.