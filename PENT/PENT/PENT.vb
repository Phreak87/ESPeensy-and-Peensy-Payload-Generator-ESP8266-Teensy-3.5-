Imports System.Windows.Forms

Public Class PENT

    Private Sub ShowNewForm(ByVal sender As Object, ByVal e As EventArgs) Handles NewToolStripMenuItem.Click
        Dim ProjectForm As New CreatePRJ
        ProjectForm.MdiParent = Me
        ProjectForm.WindowState = FormWindowState.Maximized
        ProjectForm.Show()
    End Sub

    Private Sub SnippetErstellenToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim SnippetForm As New CreateSnippet
        SnippetForm.MdiParent = Me
        SnippetForm.WindowState = FormWindowState.Maximized
        SnippetForm.Show()
    End Sub

    Private Sub BeendenToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles BeendenToolStripMenuItem.Click
        Application.Exit()
    End Sub

    Private Sub OpenToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OpenToolStripMenuItem.Click
        Dim PEN As New CreatePRJ()
        PEN.OpenFile()
        PEN.MdiParent = Me
        PEN.WindowState = FormWindowState.Maximized
        PEN.Show()
    End Sub

    Private Sub PENT_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim ProjectForm As New CreatePRJ
        ProjectForm.MdiParent = Me
        ProjectForm.WindowState = FormWindowState.Maximized
        ProjectForm.Show()
    End Sub
End Class
