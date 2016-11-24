Module Module1

    Sub Main()
        If Environment.GetCommandLineArgs.Count = 1 Then Exit Sub
        Dim SRCParm As String = Environment.GetCommandLineArgs(1)
        Dim SRCFile As String = Mid(SRCParm, InStrRev(SRCParm, "\") + 1)
        Dim SRCFold As String = SRCParm.Replace(SRCFile, "")
        Dim DSTFile As String = Mid(SRCFile, 1, InStrRev(SRCFile, ".") - 1) & ".c"
        My.Computer.FileSystem.WriteAllText(SRCFold & DSTFile, HTMLToC(My.Computer.FileSystem.ReadAllText(SRCParm)), False)
    End Sub

    Function HTMLToC(ByVal HTML As String) As String
        Dim C As New System.Text.StringBuilder
        Dim P As New System.Text.StringBuilder
        C.AppendLine("String Content = """"")
        For Each Line In Split(HTML, vbLf)
            If Line.Trim = "" Then Continue For
            P.Append(Line.Replace("""", "'").Replace(vbTab, "").Trim)
            If P.Length > 120 Then C.AppendLine("Content += """ & P.ToString & """" & ";") : P.Clear()
        Next
        Return C.ToString
    End Function

End Module
