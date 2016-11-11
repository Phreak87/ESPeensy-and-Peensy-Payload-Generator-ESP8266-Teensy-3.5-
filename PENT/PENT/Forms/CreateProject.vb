Public Class CreatePRJ

    Dim PROJName As String = ""

    Private Sub Project_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        LoadTree()
        If PROJName = "" Then LoadFile("Projects\Default.txt")
    End Sub

    Sub LoadFile(ByVal Filename As String)
        ListView1.Items.Clear()
        For Each line In Split(My.Computer.FileSystem.ReadAllText(Filename), vbCrLf)
            Dim Splitter As String() = Split(line, vbTab)
            If Splitter.Count = 1 Then Exit Sub
            Dim LVIT As New ListViewItem(Splitter(0))
            LVIT.SubItems.Add(Splitter(1))
            LVIT.SubItems.Add(Splitter(2))
            LVIT.SubItems.Add(Splitter(3))
            LVIT.SubItems.Add(Splitter(4))
            LVIT.SubItems.Add(Splitter(5))
            ListView1.Items.Add(LVIT)
            UpdateState(Splitter(4))
        Next
    End Sub
    Sub LoadTree()
        TreeView1.Nodes.Clear()
        Dim RNode As TreeNode = TreeView1.Nodes.Add(ToolStripComboBox1.Text)

        ' Root -> OS -> Level -> Type
        Dim OS As String = "Snippets\" & ToolStripComboBox1.Text
        Dim LEVEL As String = OS & "\" & CBO_STATE.Text

        For Each LEVEL In My.Computer.FileSystem.GetDirectories(OS)
            If LastBackSlash(LEVEL) = CBO_STATE.Text Then
                Dim LEVELNode As TreeNode = RNode.Nodes.Add(LastBackSlash(LEVEL))
                For Each LANG In My.Computer.FileSystem.GetDirectories(LEVEL)
                    Dim LANGNode As TreeNode = LEVELNode.Nodes.Add(LastBackSlash(LANG))
                    For Each FILE In My.Computer.FileSystem.GetFiles(LANG)
                        LANGNode.Nodes.Add(LastBackSlash(FILE))
                    Next
                Next
                LEVELNode.ExpandAll()
            End If
        Next
        RNode.Expand()
    End Sub

    Function LastBackSlash(ByVal File As String) As String
        Return Mid(File, InStrRev(File, "\") + 1)
    End Function

    Sub AddTreeNode()
        TreeView1.PathSeparator = "\"
        If TreeView1.SelectedNode.Level < 3 Then Exit Sub
        Dim TSPLIT As String() = Split(TreeView1.SelectedNode.FullPath, "\")
        Dim matchRes As System.Text.RegularExpressions.Match = System.Text.RegularExpressions.Regex.Match(TreeView1.SelectedNode.FullPath, ".*#([0-9])\.(txt|vbs|c|ps1)")

        ' ------------------------------------------------------------------
        ' Wenn der C-Code eine Funktion mit Parametern ist
        ' ------------------------------------------------------------------
        Dim Parm As String = ""
        If TSPLIT(2) = "CSource" Then
            Dim CCont As String = My.Computer.FileSystem.ReadAllText("Snippets\" & TreeView1.SelectedNode.FullPath)
            Dim CContL0 As String = Split(CCont, vbLf)(0)
            Dim CFuncts As System.Text.RegularExpressions.Match = System.Text.RegularExpressions.Regex.Match(CContL0, "void ([a-zA-Z_]*)\((.*?)\)")
            If CFuncts.Groups(2).Value <> "" Then
                For Each eintrag In Split(CFuncts.Groups(2).Value, ",")
                    Parm = Parm & InputBox(eintrag, "Bitte Parameter eingeben:") & ","
                Next
                Parm = Parm.Remove(Parm.Length - 1)
            End If
        End If

        Dim NewITEM As New ListViewItem(TSPLIT(0))
        NewITEM.SubItems.Add(TSPLIT(1))
        NewITEM.SubItems.Add(TSPLIT(2))
        NewITEM.SubItems.Add(TSPLIT(3))
        NewITEM.SubItems.Add(matchRes.Groups(1).Value)
        NewITEM.SubItems.Add(Parm)
        ListView1.Items.Add(NewITEM)

        UpdateState(matchRes.Groups(1).Value)


    End Sub

    Sub UpdateState(ByVal State As String)
        If State = "" Then Exit Sub
        Select Case (State)
            Case 0 : CBO_STATE.Text = "0_INIT" : LoadTree()
            Case 1 : CBO_STATE.Text = "1_GUI" : LoadTree()
            Case 2 : CBO_STATE.Text = "2_CMD" : LoadTree()
            Case 3 : CBO_STATE.Text = "3_POS" : LoadTree()
            Case ""
            Case Else : MsgBox(State)
        End Select
    End Sub

    Function GenFunction(ByVal Name As String, ByVal Content As String) As String
        Dim Out As New System.Text.StringBuilder
        If Content.StartsWith("void ") Then Return Content
        Out.AppendLine("void " & ModifyName(Name) & "(void){")
        Out.AppendLine(Content)
        Out.AppendLine("}")
        Return Out.ToString
    End Function

    Function ModifyName(ByVal Name As String) As String
        If Name.Trim.Contains("#") Then   Name = Mid(Name, 1, InStrRev(Name, "#") - 1)
        If Name.Trim.Contains(".") Then Name = Mid(Name, 1, InStrRev(Name, ".") - 1)
        Name = Name.Replace(" ", "_")
        Name = Name.Replace("#", "")
        Return Name
    End Function


    Private Sub ToolStripButton5_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton5.Click
        Dim n As New CreateSnippet
        Dim m As New TreePath(TreeView1.SelectedNode, CBO_STATE.Text)

        n.CBO_OS.Text = m.OperatingSYS : n.CBO_OS.Enabled = False
        n.CBO_Script.Text = m.ScriptType : n.CBO_Script.Enabled = False
        n.CBO_SourceState.Text = m.SourceState : n.CBO_SourceState.Enabled = False
        n.CBO_TargetState.Text = m.TargetState

        n.ShowDialog()
        LoadTree()
    End Sub

#Region "RefreshNode"
    Private Sub ToolStripComboBox2_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles CBO_STATE.TextChanged
        LoadTree()
    End Sub
    Private Sub ToolStripComboBox1_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles ToolStripComboBox1.TextChanged
        LoadTree()
    End Sub
    Private Sub ToolStripButton1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        AddTreeNode()
    End Sub
    Private Sub TreeView1_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles TreeView1.DoubleClick
        AddTreeNode()
    End Sub
#End Region

    Private Sub ListView1_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ListView1.SelectedIndexChanged
        If ListView1.SelectedItems.Count = 0 Then Exit Sub
        CBO_STATE.Text = ListView1.SelectedItems(0).SubItems(1).Text
        Dim OS As String = ListView1.SelectedItems(0).SubItems(0).Text
        Dim State As String = ListView1.SelectedItems(0).SubItems(1).Text
        Dim SType As String = ListView1.SelectedItems(0).SubItems(2).Text
        Dim SName As String = ListView1.SelectedItems(0).SubItems(3).Text
        Dim FilePath As String = "Snippets\" & OS & "\" & State & "\" & SType & "\" & SName
        Dim FileCont As String = ""

            If SType = "RAW" Then
            FileCont = SName
            Else
                If My.Computer.FileSystem.FileExists(FilePath) = False Then Exit Sub
                FileCont = My.Computer.FileSystem.ReadAllText(FilePath)
            End If

            RichTextBox1.Text = FileCont
            LoadTree()
    End Sub


    Private Sub ToolStripButton2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton2.Click
        If TreeView1.SelectedNode.Level <> 3 Then Exit Sub
        Dim Eintrag As String = TreeView1.SelectedNode.FullPath
        My.Computer.FileSystem.DeleteFile("Snippets" & "\" & Eintrag)
        LoadTree()
    End Sub

    Private Sub TreeView1_AfterSelect(ByVal sender As System.Object, ByVal e As System.Windows.Forms.TreeViewEventArgs) Handles TreeView1.AfterSelect
        If TreeView1.SelectedNode.Level <> 3 Then Exit Sub
        TreeView1.PathSeparator = "\"
        Dim Eintrag As String = TreeView1.SelectedNode.FullPath
        RichTextBox1.Text = My.Computer.FileSystem.ReadAllText("Snippets" & "\" & Eintrag)
    End Sub

    Private Sub ToolStripButton3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton3.Click
        Dim Out As New System.Text.StringBuilder
        Dim VAR As New Dictionary(Of String, String)

        ' ----------------------------------------------------------
        ' Default-Funktionen anhängen
        ' ----------------------------------------------------------
        For Each File In My.Computer.FileSystem.GetFiles("Defaults")
            Out.AppendLine(My.Computer.FileSystem.ReadAllText(File))
        Next

        ' ----------------------------------------------------------
        ' Funktionsaufrufe
        ' ----------------------------------------------------------
        Out.AppendLine("void setup (void){")
        For Each eintrag As ListViewItem In ListView1.Items
            Dim SName As String = eintrag.SubItems(3).Text
            Dim SType As String = eintrag.SubItems(2).Text
            Dim Parm As String = eintrag.SubItems(5).Text
            Select Case SType
                Case "RAW" : Out.AppendLine(vbTab & SName)
                Case Else : Out.AppendLine(vbTab & ModifyName(SName) & "(" & IIf(Parm = "", "", Parm) & ");")
            End Select
        Next
        Out.AppendLine("}")

        ' ----------------------------------------------------------
        ' Funktionen des Benutzers
        ' ----------------------------------------------------------
        Dim FList As New List(Of String)
        For Each eintrag As ListViewItem In ListView1.Items
            Dim OS As String = eintrag.SubItems(0).Text
            Dim State As String = eintrag.SubItems(1).Text
            Dim SType As String = eintrag.SubItems(2).Text
            Dim SName As String = eintrag.SubItems(3).Text

            If FList.Contains(SName) = False Then
                Dim FileCont As String = ""
                Dim FilePath As String = "Snippets\" & OS & "\" & State & "\" & SType & "\" & SName
                If SType <> "RAW" Then
                    If My.Computer.FileSystem.FileExists(FilePath) = False Then MsgBox("Die Datei " & FilePath & " wurde nicht gefunden") : Continue For
                    FileCont = My.Computer.FileSystem.ReadAllText(FilePath)
                End If

                Select Case SType
                    Case "CSource"
                        Out.AppendLine(GenFunction(SName, FileCont))
                    Case "DuckyScript"
                        Dim CCode As String = New DuckyDecoder(FileCont).Output
                        Out.AppendLine(GenFunction(SName, CCode))
                    Case "Batch"
                        Dim DuckyCode As String = New Batch(FileCont).Output
                        Dim CCode As String = New DuckyDecoder(DuckyCode).Output
                        Out.AppendLine(GenFunction(SName, CCode))
                    Case "WScript"
                        Dim DuckyCode As String = New WScript(FileCont, ModifyName(SName)).Output
                        Dim CCode As String = New DuckyDecoder(DuckyCode).Output
                        Out.AppendLine(GenFunction(SName, CCode))
                    Case "PSShell"
                        Dim DuckyCode As String = New PSScript(FileCont, ModifyName(SName)).Output
                        Dim CCode As String = New DuckyDecoder(DuckyCode).Output
                        Out.AppendLine(GenFunction(SName, CCode))
                    Case "RAW"
                        ' -----------------------------------------------------------------------
                        ' Keine Funktion, wird in Setuproutine direkt verarbeitet
                        ' -----------------------------------------------------------------------
                    Case Else
                        MsgBox("Unbekannter Typ " & SType)
                End Select
                If SType <> "RAW" Then FList.Add(SName)
            End If
        Next

        ' ------------------------------------------------------------------
        ' Variablen ersetzen
        ' ------------------------------------------------------------------
        Dim RegVars As System.Text.RegularExpressions.MatchCollection = System.Text.RegularExpressions.Regex.Matches(Out.ToString, "(\[\[[a-zA-Z0-9]*\]\])")
        For Each eintrag In RegVars
            If VAR.ContainsKey(eintrag.value) = False Then
                VAR.Add(eintrag.value, InputBox("Parameterwert", eintrag.value))
            End If
        Next
        For Each eintrag In VAR : Out.Replace(eintrag.Key, eintrag.Value) : Next

        Out.AppendLine("void loop (){}")
        My.Computer.Clipboard.SetText(Out.ToString) : RichTextBox1.Text = Out.ToString
    End Sub

    Private Sub ToolStripButton4_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton4.Click
        My.Computer.Clipboard.SetText(RichTextBox1.Text)
    End Sub

    Private Sub ToolStripButton6_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton6.Click
        Dim LVIT As ListViewItem = New ListViewItem(ToolStripComboBox1.Text)
        LVIT.SubItems.Add(CBO_STATE.Text)
        LVIT.SubItems.Add("RAW")
        LVIT.SubItems.Add(InputBox("Bitte Ruhdaten für die Setuproutine eingeben"))
        LVIT.SubItems.Add("")
        LVIT.SubItems.Add("")
        ListView1.Items.Add(LVIT)
    End Sub

#Region "Laden - Speichern - Neu"
    Private Sub SpeichernToolStripButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SpeichernToolStripButton.Click
        If PROJName = "" Then PROJName = InputBox("Projektname eingeben", , "Default")
        If PROJName = "" Then Exit Sub
        Dim Struct As New System.Text.StringBuilder
        For Each eintrag In ListView1.Items
            Struct.AppendLine(eintrag.subitems(0).text & vbTab & _
                              eintrag.subitems(1).text & vbTab & _
                              eintrag.subitems(2).text & vbTab & _
                              eintrag.subitems(3).text & vbTab &
                              eintrag.subitems(4).text & vbTab & _
                              eintrag.subitems(5).text)
        Next
        My.Computer.FileSystem.WriteAllText("Projects\" & PROJName & ".txt", Struct.ToString, False)
    End Sub

    Private Sub NeuToolStripButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NeuToolStripButton.Click
        LoadFile("Projects\Default.txt")
        PROJName = ""
    End Sub

    Sub OpenFile()
        Dim OFD As New OpenFileDialog
        OFD.InitialDirectory = Application.StartupPath & "\Projects"
        OFD.ShowDialog() : LoadFile(OFD.FileName)
        PROJName = Mid(OFD.FileName, InStrRev(OFD.FileName, "\") + 1).Replace(".txt", "")
    End Sub

    Private Sub ÖffnenToolStripButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ÖffnenToolStripButton.Click
        OpenFile()
    End Sub
#End Region

#Region "Move and Delete from Listview"
    Private Sub ToolStripButton8_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton8.Click
        Dim Sel As ListViewItem = ListView1.SelectedItems(0)
        Dim SelIDX As Integer = Sel.Index
        If SelIDX = 0 Then Exit Sub
        Dim Top As ListViewItem = ListView1.Items(ListView1.SelectedItems(0).Index - 1)
        Dim TopIDX As Integer = Top.Index

        ListView1.Items.RemoveAt(TopIDX)
        ListView1.Items.RemoveAt(TopIDX)

        ListView1.Items.Insert(TopIDX, Sel)
        ListView1.Items.Insert(SelIDX, Top)
    End Sub
    Private Sub ToolStripButton9_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton9.Click
        Dim Sel As ListViewItem = ListView1.SelectedItems(0)
        Dim SelIDX As Integer = Sel.Index
        If ListView1.Items.Count - 1 = SelIDX Then Exit Sub
        Dim Bot As ListViewItem = ListView1.Items(ListView1.SelectedItems(0).Index + 1)
        Dim BotIDX As Integer = Bot.Index

        ListView1.Items.Remove(Sel)
        ListView1.Items.Remove(Bot)

        ListView1.Items.Insert(SelIDX, Bot)
        ListView1.Items.Insert(BotIDX, Sel)
    End Sub
    Private Sub ToolStripButton1_Click_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton1.Click
        For Each eintrag As ListViewItem In ListView1.SelectedItems
            eintrag.Remove()
        Next
    End Sub
#End Region

    Private Sub ToolStripButton7_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton7.Click
        If IsNothing(TreeView1.SelectedNode) Then Exit Sub
        Dim n As New CreateSnippet
        Dim m As New TreePath(TreeView1.SelectedNode, CBO_STATE.Text)
        If My.Computer.FileSystem.FileExists(m.FilePath) = False Then Exit Sub
        Dim FileCont As String = My.Computer.FileSystem.ReadAllText(m.FilePath)

        n.ToolStripTextBox1.Text = m.ScriptName
        n.CBO_OS.Text = m.OperatingSYS : n.CBO_OS.Enabled = False
        n.CBO_Script.Text = m.ScriptType : n.CBO_Script.Enabled = False
        n.CBO_SourceState.Text = m.SourceState : n.CBO_SourceState.Enabled = False
        n.CBO_TargetState.Text = m.TargetState '  : n.CBO_TargetState.Enabled = False

        n.RichTextBox1.Text = FileCont
        n.ShowDialog()
        LoadTree()
    End Sub

    Class TreePath
        Public OperatingSYS As String = ""
        Public SourceState As String = ""
        Public TargetState As String = ""
        Public TargetStateI As Integer = 0
        Public ScriptType As String = ""
        Public ScriptName As String = ""
        Public ScriptNameOrg As String = ""
        Public FilePath As String = ""

        Sub New(ByVal Node As TreeNode, ByVal ActState As String)
            If IsNothing(Node) Then Exit Sub
            SourceState = ActState : Node.TreeView.PathSeparator = "\"
            Dim TSPLIT As String() = Split(Node.FullPath, "\")
            If TSPLIT.Count >= 0 Then OperatingSYS = TSPLIT(0)
            If TSPLIT.Count >= 1 Then TargetState = TSPLIT(1)
            If TSPLIT.Count >= 1 Then TargetStateI = Mid(TSPLIT(1), 1, 1)
            If TSPLIT.Count >= 3 Then ScriptType = TSPLIT(2)
            If TSPLIT.Count >= 4 Then ScriptNameOrg = TSPLIT(3)
            If ScriptNameOrg <> "" Then
                If ScriptNameOrg.Contains("#") Then
                    ScriptName = Mid(ScriptNameOrg, 1, InStrRev(ScriptNameOrg, "#") - 1)
                    TargetStateI = Mid(ScriptNameOrg, InStrRev(ScriptNameOrg, "#") + 1, 1)
                    Select Case (TargetStateI)
                        Case 0 : TargetState = "0_INIT"
                        Case 1 : TargetState = "1_GUI"
                        Case 2 : TargetState = "2_CMD"
                        Case 3 : TargetState = "3_POS"
                    End Select
                Else
                    ScriptName = Mid(ScriptNameOrg, 1, InStrRev(ScriptNameOrg, ".") - 1)
                End If
                FilePath = ("Snippets\" & OperatingSYS & "\" & SourceState & "\" & ScriptType & "\" & ScriptNameOrg)
            End If
        End Sub
    End Class

    Private Sub ToolStripButton10_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton10.Click
        Dim m As New TreePath(TreeView1.SelectedNode, CBO_STATE.Text)
        Shell("Explorer.exe " & Mid(m.FilePath, 1, InStrRev(m.FilePath, "\")))
    End Sub

    Private Sub ToolStripButton11_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ToolStripButton11.Click
        If ListView1.SelectedItems.Count = 0 Then Exit Sub
        ListView1.SelectedItems(0).SubItems(5).Text = InputBox("Parameter", , ListView1.SelectedItems(0).SubItems(5).Text)
    End Sub
End Class