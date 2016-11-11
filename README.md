# Teensy-Payload-Generator

Generates Teensy (Peensy) Payloads simply from reusable snippets.
Reusable means: create a function once (like open admin-cmd) and
use this code for each other payload you generate. so you can build
flexible penetration tests very easy without programming knowledge.

![alt text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 1")
![alt text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 1")

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