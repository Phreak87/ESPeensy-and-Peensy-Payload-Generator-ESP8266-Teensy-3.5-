Public Class SwitchSelect
    Public Ret As String = ""

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        If ABS.Checked = True Then ResetAll()
        If Button1.BackColor.ToKnownColor = 5 Then Button1.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button1.BackColor = Color.GreenYellow Then Button1.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        If ABS.Checked = True Then ResetAll()
        If Button2.BackColor.ToKnownColor = 5 Then Button2.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button2.BackColor = Color.GreenYellow Then Button2.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Private Sub Button3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button3.Click
        If ABS.Checked = True Then ResetAll()
        If Button3.BackColor.ToKnownColor = 5 Then Button3.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button3.BackColor = Color.GreenYellow Then Button3.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Private Sub Button4_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button4.Click
        If ABS.Checked = True Then ResetAll()
        If Button4.BackColor.ToKnownColor = 5 Then Button4.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button4.BackColor = Color.GreenYellow Then Button4.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Sub ResetAll()
        Button1.BackColor = Color.FromKnownColor(5)
        Button2.BackColor = Color.FromKnownColor(5)
        Button3.BackColor = Color.FromKnownColor(5)
        Button4.BackColor = Color.FromKnownColor(5)
    End Sub

    Function ReCalc() As String
        Dim T1 As Integer = 0
        Dim T2 As Integer = 0
        Dim T3 As Integer = 0
        Dim T4 As Integer = 0
        If Button1.BackColor = Color.GreenYellow Then T1 = 1
        If Button2.BackColor = Color.GreenYellow Then T2 = 1
        If Button3.BackColor = Color.GreenYellow Then T3 = 1
        If Button4.BackColor = Color.GreenYellow Then T4 = 1
        If ABS.Checked = False Then Ret = "B" & 1 * T1 + 2 * T2 + 4 * T3 + 8 * T4
        If ABS.Checked = True Then Ret = "A" & 1 * T1 + 2 * T2 + 3 * T3 + 4 * T4
        If ABS.Checked = True And Ret = "A0" Then Button1.BackColor = Color.GreenYellow : ReCalc()
        Return Ret
    End Function

    Private Sub CheckBox1_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CheckBox1.CheckedChanged
        Panel1.Enabled = Not CheckBox1.Checked
        If Panel1.Enabled = False Then Ret = "" : ResetAll()
        If Panel1.Enabled = True Then Label2.Text = ReCalc()
    End Sub

    Sub New(ByVal Number As String)

        ' Dieser Aufruf ist für den Designer erforderlich.
        InitializeComponent()

        '------------------
        ' Always
        '------------------
        If Number = "" Then CheckBox1.Checked = True : ResetAll() : Exit Sub

        Dim Key As String = "B" : If IsNumeric(Mid(Number, 1, 1)) = False Then Key = Mid(Number, 1, 1)
        Dim Num As Integer = 1 : If IsNumeric(Mid(Number, 2)) = True Then Num = Mid(Number, 2)

        If Key = "B" Then
            If Num - 8 >= 0 Then Button4.BackColor = Color.GreenYellow : Num = Num - 8
            If Num - 4 >= 0 Then Button3.BackColor = Color.GreenYellow : Num = Num - 4
            If Num - 2 >= 0 Then Button2.BackColor = Color.GreenYellow : Num = Num - 2
            If Num - 1 >= 0 Then Button1.BackColor = Color.GreenYellow : Num = Num - 1
            CheckBox1.Checked = False : BIN.Checked = True : Label2.Text = ReCalc()
        End If

        If Key = "A" Then
            ResetAll()
            If Num = 4 Then Button4.BackColor = Color.GreenYellow : Num = Num - 8
            If Num = 3 Then Button3.BackColor = Color.GreenYellow : Num = Num - 4
            If Num = 2 Then Button2.BackColor = Color.GreenYellow : Num = Num - 2
            If Num = 1 Then Button1.BackColor = Color.GreenYellow : Num = Num - 1
            CheckBox1.Checked = False : ABS.Checked = True : Label2.Text = ReCalc()
        End If

    End Sub

    Private Sub RadioButton1_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ABS.CheckedChanged
        If ABS.Checked = True Then ResetAll()
        Label2.Text = ReCalc()
    End Sub

End Class