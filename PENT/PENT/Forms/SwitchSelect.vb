Public Class SwitchSelect
    Public Ret As String = ""

    Private Sub Button1_Click(sender As System.Object, e As System.EventArgs) Handles Button1.Click
        If Button1.BackColor.ToKnownColor = 5 Then Button1.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button1.BackColor = Color.GreenYellow Then Button1.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Private Sub Button2_Click(sender As System.Object, e As System.EventArgs) Handles Button2.Click
        If Button2.BackColor.ToKnownColor = 5 Then Button2.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button2.BackColor = Color.GreenYellow Then Button2.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Private Sub Button3_Click(sender As System.Object, e As System.EventArgs) Handles Button3.Click
        If Button3.BackColor.ToKnownColor = 5 Then Button3.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button3.BackColor = Color.GreenYellow Then Button3.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Private Sub Button4_Click(sender As System.Object, e As System.EventArgs) Handles Button4.Click
        If Button4.BackColor.ToKnownColor = 5 Then Button4.BackColor = Color.GreenYellow : Label2.Text = ReCalc() : Exit Sub
        If Button4.BackColor = Color.GreenYellow Then Button4.BackColor = Color.FromKnownColor(5) : Label2.Text = ReCalc() : Exit Sub
    End Sub

    Function ReCalc() As Integer
        Dim T1 As Integer = 0
        Dim T2 As Integer = 0
        Dim T3 As Integer = 0
        Dim T4 As Integer = 0
        If Button1.BackColor = Color.GreenYellow Then T1 = 1
        If Button2.BackColor = Color.GreenYellow Then T2 = 1
        If Button3.BackColor = Color.GreenYellow Then T3 = 1
        If Button4.BackColor = Color.GreenYellow Then T4 = 1
        Ret = 1 * T1 + 2 * T2 + 4 * T3 + 8 * T4
        Return 1 * T1 + 2 * T2 + 4 * T3 + 8 * T4
    End Function

    Private Sub CheckBox1_CheckedChanged(sender As System.Object, e As System.EventArgs) Handles CheckBox1.CheckedChanged
        Panel1.Enabled = Not CheckBox1.Checked
        If Panel1.Enabled = False Then
            Ret = ""
        Else
            Ret = ReCalc()
        End If
    End Sub

    Sub New(Number As String)

        ' Dieser Aufruf ist für den Designer erforderlich.
        InitializeComponent()

        If IsNumeric(Number) = False Then Exit Sub
        If Number - 8 >= 0 Then Button4.BackColor = Color.GreenYellow : Number = Number - 8
        If Number - 4 >= 0 Then Button3.BackColor = Color.GreenYellow : Number = Number - 4
        If Number - 2 >= 0 Then Button2.BackColor = Color.GreenYellow : Number = Number - 2
        If Number - 1 >= 0 Then Button1.BackColor = Color.GreenYellow : Number = Number - 1
        CheckBox1.Checked = False : Label2.Text = ReCalc()
        ' Fügen Sie Initialisierungen nach dem InitializeComponent()-Aufruf hinzu.

    End Sub

    Private Sub SwitchSelect_Load(sender As System.Object, e As System.EventArgs) Handles MyBase.Load

    End Sub
End Class