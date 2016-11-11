Public Class DuckyDecoder
    Dim STRDucky As String = ""
    Dim LastLine As String = ""
    Public Output As String = ""

    Sub New(DuckyCode As String)
        Dim Out As New System.Text.StringBuilder
        STRDucky = DuckyCode
        For Each Line In Split(DuckyCode, vbLf)
            Out.Append(ParseLine(Line.Trim(vbCr), False))
        Next
        Output = Out.ToString
    End Sub

    Private Function ParseLine(ByVal Line As String, ByVal Debug As Boolean) As String
        If Line.Trim = "" Then Return ""
        Dim VALRES As New System.Text.StringBuilder

        Dim RGX As System.Text.RegularExpressions.Match = System.Text.RegularExpressions.Regex.Match(Line, "^([A-Za-z\-]*)( .*)?$")
        If Debug = True Then VALRES.AppendLine("// From Duckysource " & Line)
        Dim ACT As String = RGX.Groups(1).Value
        Dim VAL As String = Mid(RGX.Groups(2).Value, 2)

        Select Case ACT.ToUpper
            ' -----------------------------------------------------------------------------------
            ' Control Single
            ' -----------------------------------------------------------------------------------
            Case "ALT"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_ALT);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "CTRL"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_CTRL);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "CONTROL"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_CTRL);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "SHIFT"
                VALRES.AppendLine("Keyboard.set_key1(MODIFIERKEY_SHIFT);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "WINDOWS"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_GUI);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "MENU"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_RIGHT_GUI);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "ENTER"
                VALRES.AppendLine("Keyboard.set_key1(KEY_ENTER);")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")

                ' -----------------------------------------------------------------------------------
                ' Control Multiple
                ' -----------------------------------------------------------------------------------
            Case "ALT-SHIFT"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_ALT | MODIFIERKEY_SHIFT);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "CTRL-ALT"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_CTRL | MODIFIERKEY_ALT);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")
            Case "CTRL-SHIFT"
                VALRES.AppendLine("Keyboard.set_modifier(MODIFIERKEY_CTRL | MODIFIERKEY_SHIFT);")
                If VAL.Trim <> "" Then VALRES.AppendLine("Keyboard.set_key1(" & Keys(VAL) & ");")
                VALRES.AppendLine("Keyboard.send_now();unpress_key();")

                ' -----------------------------------------------------------------------------------
                ' Timing
                ' -----------------------------------------------------------------------------------
            Case "DEFAULT_DELAY"
                VALRES.AppendLine("delay (500);")
            Case "DEFAULTDELAY"
                VALRES.AppendLine("delay (500);")
            Case "DELAY"
                VALRES.AppendLine("delay (" & VAL & ");")

            Case "COMMAND"
            Case "GUI"
            Case "REM"
                VALRES.AppendLine("// " & VAL)
            Case "HOLDFOR"

            Case "REPEAT"
                If IsNumeric(VAL) = False Then Return ""
                VALRES.AppendLine("for (int i=0; i<" & CInt(VAL) & "; i++){")
                VALRES.AppendLine(LastLine)
                VALRES.AppendLine("}")
            Case "STRING"
                VALRES.AppendLine("Keyboard.print (" & Chr(34) & Modify(VAL) & Chr(34) & ");")
            Case "STRING-ENTER"
                VALRES.AppendLine("Keyboard.print (" & Chr(34) & Modify(VAL) & Chr(34) & ");")
                VALRES.AppendLine("Keyboard.set_key1(KEY_ENTER);Keyboard.send_now();unpress_key();")
            Case "DOWNARROW"
                VALRES.AppendLine("Keyboard.set_key1(KEY_DOWN);")
                VALRES.AppendLine("Keyboard.send_now();")
                If VAL <> "" Then VALRES.AppendLine("delay (" & VAL & ");")
                VALRES.AppendLine("unpress_key();")
            Case Else
                ' MsgBox("Undefined")
        End Select

        LastLine = VALRES.ToString
        Return VALRES.ToString


    End Function
    Private Function Keys(ByVal Key As String) As String
        'KEY_A	KEY_B	KEY_C	KEY_D
        'KEY_E	KEY_F	KEY_G	KEY_H
        'KEY_I	KEY_J	KEY_K	KEY_L
        'KEY_M	KEY_N	KEY_O	KEY_P
        'KEY_Q	KEY_R	KEY_S	KEY_T
        'KEY_U	KEY_V	KEY_W	KEY_X
        'KEY_Y	KEY_Z	KEY_1	KEY_2
        'KEY_3	KEY_4	KEY_5	KEY_6
        'KEY_7	KEY_8	KEY_9	KEY_0
        'KEY_ENTER	KEY_ESC	KEY_BACKSPACE	KEY_TAB
        'KEY_SPACE	KEY_MINUS	KEY_EQUAL	KEY_LEFT_BRACE
        'KEY_RIGHT_BRACE	KEY_BACKSLASH	KEY_NUMBER	KEY_SEMICOLON
        'KEY_QUOTE	KEY_TILDE	KEY_COMMA	KEY_PERIOD
        'KEY_SLASH	KEY_CAPS_LOCK	KEY_F1	KEY_F2
        'KEY_F3	KEY_F4	KEY_F5	KEY_F6
        'KEY_F7	KEY_F8	KEY_F9	KEY_F10
        'KEY_F11	KEY_F12	KEY_PRINTSCREEN	KEY_SCROLL_LOCK
        'KEY_PAUSE	KEY_INSERT	KEY_HOME	KEY_PAGE_UP
        'KEY_DELETE	KEY_END	KEY_PAGE_DOWN	KEY_RIGHT
        'KEY_LEFT	KEY_DOWN	KEY_UP	KEY_NUM_LOCK
        'KEYPAD_SLASH	KEYPAD_ASTERIX	KEYPAD_MINUS	KEYPAD_PLUS
        'KEYPAD_ENTER	KEYPAD_1	KEYPAD_2	KEYPAD_3
        'KEYPAD_4	KEYPAD_5	KEYPAD_6	KEYPAD_7
        'KEYPAD_8	KEYPAD_9	KEYPAD_0	KEYPAD_PERIOD

        Select Case Key.ToUpper
            Case "1" : Return "KEY_1"
            Case "2" : Return "KEY_2"
            Case "3" : Return "KEY_3"
            Case "4" : Return "KEY_4"
            Case "5" : Return "KEY_5"
            Case "6" : Return "KEY_6"
            Case "7" : Return "KEY_7"
            Case "8" : Return "KEY_8"
            Case "9" : Return "KEY_9"
            Case "0" : Return "KEY_0"

            Case "A" : Return "KEY_A"
            Case "B" : Return "KEY_B"
            Case "C" : Return "KEY_C"
            Case "D" : Return "KEY_D"
            Case "E" : Return "KEY_E"
            Case "F" : Return "KEY_F"
            Case "G" : Return "KEY_G"
            Case "H" : Return "KEY_H"
            Case "I" : Return "KEY_I"
            Case "J" : Return "KEY_J"
            Case "K" : Return "KEY_K"
            Case "L" : Return "KEY_L"
            Case "M" : Return "KEY_M"
            Case "N" : Return "KEY_N"
            Case "O" : Return "KEY_O"
            Case "P" : Return "KEY_P"
            Case "Q" : Return "KEY_Q"
            Case "R" : Return "KEY_R"
            Case "S" : Return "KEY_S"
            Case "T" : Return "KEY_T"
            Case "U" : Return "KEY_U"
            Case "V" : Return "KEY_V"
            Case "W" : Return "KEY_W"
            Case "X" : Return "KEY_X"
            Case "Y" : Return "KEY_Y"
            Case "Z" : Return "KEY_Z"

            Case "ENTER" : Return "KEY_ENTER"
            Case "TAB" : Return "KEY_TAB"
            Case "SPACE" : Return "KEY_SPACE"

        End Select
        Return ""
    End Function
    Private Function Modify(Text As String) As String
        Dim n As String = Text
        n = n.Replace("\", "\\")
        n = n.Replace(Chr(34), "\" & Chr(34))
        Return n
    End Function

End Class
