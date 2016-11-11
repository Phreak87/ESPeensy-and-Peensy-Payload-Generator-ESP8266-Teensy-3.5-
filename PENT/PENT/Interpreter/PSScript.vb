Public Class PSScript
    Public Output As String
    Sub New(ByVal Script As String, ByVal Name As String)
        Dim Out As New System.Text.StringBuilder
        Dim ILine As Integer = 1
        For Each line In Split(Script, vbLf)
            If line.Trim = "" Then Continue For
            If ILine = 1 Then Out.AppendLine("STRING-ENTER echo " & line.Replace("&", "^&") & " > " & Name & ".ps1") : ILine = ILine + 1 : Continue For
            If ILine > 1 Then Out.AppendLine("STRING-ENTER echo " & line.Replace("&", "^&") & " >> " & Name & ".ps1") : ILine = ILine + 1
        Next
        ' Out.AppendLine("STRING-ENTER Cscript " & Name & ".vbs")
        Output = Out.ToString
    End Sub
End Class
