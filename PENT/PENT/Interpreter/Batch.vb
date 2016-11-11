Public Class Batch
    Public Output As String
    Sub New(ByVal Script As String)
        Dim Out As New System.Text.StringBuilder
        For Each line In Split(Script, vbLf)
            If line.Trim = "" Then Continue For
            If line.Trim.StartsWith("REM ") Then
                Out.AppendLine(line)
            Else
                Out.AppendLine("STRING-ENTER " & line)
            End If
        Next
        Output = Out.ToString
    End Sub
End Class
