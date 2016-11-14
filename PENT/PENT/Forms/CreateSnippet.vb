Public Class CreateSnippet
    Public SCR_Name As String
    Public SCR_Source As String
    Public SCR_Arch As String
    Public SCR_State As String

    Private Sub SpeichernToolStripButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SpeichernToolStripButton.Click
        ' ------------------------------------------------------------------
        ' Wenn die funktion bereits einen namen hat
        ' ------------------------------------------------------------------
        SCR_Name = ToolStripTextBox1.Text
        If CBO_Script.Text = "CSource" Then
            Dim CCont As String = RichTextBox1.Text
            Dim CContL0 As String = Split(CCont, vbLf)(0)
            Dim CFuncts As System.Text.RegularExpressions.Match = System.Text.RegularExpressions.Regex.Match(CContL0, "void ([a-zA-Z_]*)[ ]?\((.*?)\)")
            If CFuncts.Groups(1).Value <> "" Then SCR_Name = CFuncts.Groups(1).Value
        End If

        If SCR_Name = "" Then
            Dim INP As String = InputBox("Bitte geben Sie den Namen des Snippets ein", , SCR_Name)
            If INP = "" Then Exit Sub
            SCR_Name = INP
        End If

        SCR_Source = RichTextBox1.Text
        Dim FilePath As String = "Snippets\" & CBO_OS.Text & "\" & CBO_SourceState.Text & "\" & CBO_Script.Text & "\" & SCR_Name & "#" & Mid(CBO_TargetState.Text, 1, 1)
        My.Computer.FileSystem.WriteAllText(FilePath & ".txt", SCR_Source, False)
        Me.Close()

    End Sub

    Private Sub ToolStripButton1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton1.Click
        SplitContainer1.Panel2Collapsed = Not SplitContainer1.Panel2Collapsed
    End Sub

    Private Sub RichTextBox1_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RichTextBox1.TextChanged
        Select Case CBO_Script.Text
            Case "DuckyScript"
                RichTextBox2.Text = New DuckyDecoder(RichTextBox1.Text).Output
            Case "CSource"
                RichTextBox2.Text = RichTextBox1.Text
            Case "Batch"
                RichTextBox2.Text = New DuckyDecoder(New Batch(RichTextBox1.Text).Output).Output
            Case "WScript"
                RichTextBox2.Text = New DuckyDecoder(New WScript(RichTextBox1.Text, "Wscript").Output).Output
            Case "PSShell"
                RichTextBox2.Text = New DuckyDecoder(New WScript(RichTextBox1.Text, "PSscript").Output).Output
        End Select
    End Sub

    Private Sub CBO_Script_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CBO_Script.Click
        ShowHelp()
    End Sub
    Private Sub ToolStripButton2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton2.Click
        ShowHelp()
        SplitContainer1.Panel2Collapsed = False
    End Sub
    Sub ShowHelp()
        If My.Computer.FileSystem.FileExists("Help\" & CBO_Script.Text & ".txt") Then
            RichTextBox2.Text = My.Computer.FileSystem.ReadAllText("Help\" & CBO_Script.Text & ".txt")
        Else
            RichTextBox2.Text = "No Help present in Programm\Help\"
        End If
    End Sub

    Private Sub CreateSnippet_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        RichTextBox1_TextChanged(Nothing, Nothing)
    End Sub

    Private Sub StringEnterEachLineToolStripMenuItem_Click(sender As System.Object, e As System.EventArgs) Handles StringEnterEachLineToolStripMenuItem.Click
        Dim NewText As New System.Text.StringBuilder
        For Each line In Split(RichTextBox1.Text, vbLf)
            NewText.AppendLine("STRING-ENTER " & line)
        Next
        RichTextBox1.Text = NewText.ToString
    End Sub
End Class