<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Frame_Dlg
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim ListViewItem4 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem(New String() {"0x01", "2", "SubscriberAutoLength", "8", "Enhanced"}, -1)
        Dim ListViewItem5 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("")
        Dim ListViewItem6 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("")
        Me.pgGFTDef = New System.Windows.Forms.PropertyGrid
        Me.ColGFTChecksum = New System.Windows.Forms.ColumnHeader
        Me.label1 = New System.Windows.Forms.Label
        Me.lvGFT = New System.Windows.Forms.ListView
        Me.ColGFTID = New System.Windows.Forms.ColumnHeader
        Me.ColGFTPID = New System.Windows.Forms.ColumnHeader
        Me.ColGFTDirect = New System.Windows.Forms.ColumnHeader
        Me.ColGFTLen = New System.Windows.Forms.ColumnHeader
        Me.label2 = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'pgGFTDef
        '
        Me.pgGFTDef.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pgGFTDef.Location = New System.Drawing.Point(354, 27)
        Me.pgGFTDef.Name = "pgGFTDef"
        Me.pgGFTDef.Size = New System.Drawing.Size(256, 219)
        Me.pgGFTDef.TabIndex = 7
        '
        'ColGFTChecksum
        '
        Me.ColGFTChecksum.Text = "CST"
        Me.ColGFTChecksum.Width = 61
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(12, 11)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(102, 13)
        Me.label1.TabIndex = 4
        Me.label1.Text = "Global Frame Table:"
        '
        'lvGFT
        '
        Me.lvGFT.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.lvGFT.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.ColGFTID, Me.ColGFTPID, Me.ColGFTDirect, Me.ColGFTLen, Me.ColGFTChecksum})
        Me.lvGFT.FullRowSelect = True
        Me.lvGFT.HideSelection = False
        Me.lvGFT.Items.AddRange(New System.Windows.Forms.ListViewItem() {ListViewItem4, ListViewItem5, ListViewItem6})
        Me.lvGFT.Location = New System.Drawing.Point(12, 27)
        Me.lvGFT.Name = "lvGFT"
        Me.lvGFT.ShowItemToolTips = True
        Me.lvGFT.Size = New System.Drawing.Size(338, 219)
        Me.lvGFT.TabIndex = 5
        Me.lvGFT.UseCompatibleStateImageBehavior = False
        Me.lvGFT.View = System.Windows.Forms.View.Details
        '
        'ColGFTID
        '
        Me.ColGFTID.Text = "ID"
        Me.ColGFTID.Width = 41
        '
        'ColGFTPID
        '
        Me.ColGFTPID.Text = "PID"
        Me.ColGFTPID.Width = 38
        '
        'ColGFTDirect
        '
        Me.ColGFTDirect.Text = "Direction"
        Me.ColGFTDirect.Width = 124
        '
        'ColGFTLen
        '
        Me.ColGFTLen.Text = "Length"
        Me.ColGFTLen.Width = 50
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(356, 11)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(57, 13)
        Me.label2.TabIndex = 6
        Me.label2.Text = "Properties:"
        '
        'Frame_Dlg
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(622, 256)
        Me.Controls.Add(Me.pgGFTDef)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.lvGFT)
        Me.Controls.Add(Me.label2)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.MinimumSize = New System.Drawing.Size(638, 295)
        Me.Name = "Frame_Dlg"
        Me.ShowIcon = False
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Global Frame Table"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents pgGFTDef As System.Windows.Forms.PropertyGrid
    Private WithEvents ColGFTChecksum As System.Windows.Forms.ColumnHeader
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents lvGFT As System.Windows.Forms.ListView
    Private WithEvents ColGFTID As System.Windows.Forms.ColumnHeader
    Private WithEvents ColGFTPID As System.Windows.Forms.ColumnHeader
    Private WithEvents ColGFTDirect As System.Windows.Forms.ColumnHeader
    Private WithEvents ColGFTLen As System.Windows.Forms.ColumnHeader
    Private WithEvents label2 As System.Windows.Forms.Label
End Class
