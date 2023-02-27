<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
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
        Me.components = New System.ComponentModel.Container
        Me.tmrRead = New System.Windows.Forms.Timer(Me.components)
        Me.tmrDisplay = New System.Windows.Forms.Timer(Me.components)
        Me.groupBox2 = New System.Windows.Forms.GroupBox
        Me.cbbID = New System.Windows.Forms.ComboBox
        Me.label11 = New System.Windows.Forms.Label
        Me.label10 = New System.Windows.Forms.Label
        Me.cbbCST = New System.Windows.Forms.ComboBox
        Me.label9 = New System.Windows.Forms.Label
        Me.cbbDirection = New System.Windows.Forms.ComboBox
        Me.nudLength = New System.Windows.Forms.NumericUpDown
        Me.txtData7 = New System.Windows.Forms.TextBox
        Me.txtData6 = New System.Windows.Forms.TextBox
        Me.txtData5 = New System.Windows.Forms.TextBox
        Me.txtData4 = New System.Windows.Forms.TextBox
        Me.txtData3 = New System.Windows.Forms.TextBox
        Me.txtData2 = New System.Windows.Forms.TextBox
        Me.txtData1 = New System.Windows.Forms.TextBox
        Me.txtData0 = New System.Windows.Forms.TextBox
        Me.txtID = New System.Windows.Forms.TextBox
        Me.label6 = New System.Windows.Forms.Label
        Me.label5 = New System.Windows.Forms.Label
        Me.label4 = New System.Windows.Forms.Label
        Me.btnWrite = New System.Windows.Forms.Button
        Me.btnMsgClear = New System.Windows.Forms.Button
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.btnIdent = New System.Windows.Forms.Button
        Me.btnHwRefresh = New System.Windows.Forms.Button
        Me.cbbChannel = New System.Windows.Forms.ComboBox
        Me.cbbHwMode = New System.Windows.Forms.ComboBox
        Me.label3 = New System.Windows.Forms.Label
        Me.cbbBaudrates = New System.Windows.Forms.ComboBox
        Me.label2 = New System.Windows.Forms.Label
        Me.label1 = New System.Windows.Forms.Label
        Me.btnInit = New System.Windows.Forms.Button
        Me.btnRelease = New System.Windows.Forms.Button
        Me.clhRcvTime = New System.Windows.Forms.ColumnHeader
        Me.clhCRC = New System.Windows.Forms.ColumnHeader
        Me.clhDirection = New System.Windows.Forms.ColumnHeader
        Me.clhCount = New System.Windows.Forms.ColumnHeader
        Me.clhErrors = New System.Windows.Forms.ColumnHeader
        Me.rdbTimer = New System.Windows.Forms.RadioButton
        Me.clhCST = New System.Windows.Forms.ColumnHeader
        Me.btnRead = New System.Windows.Forms.Button
        Me.clhData = New System.Windows.Forms.ColumnHeader
        Me.clhLength = New System.Windows.Forms.ColumnHeader
        Me.clhID = New System.Windows.Forms.ColumnHeader
        Me.btnConfigure = New System.Windows.Forms.Button
        Me.rdbFilterOpen = New System.Windows.Forms.RadioButton
        Me.rdbFilterCustom = New System.Windows.Forms.RadioButton
        Me.groupBox3 = New System.Windows.Forms.GroupBox
        Me.rdbFilterClose = New System.Windows.Forms.RadioButton
        Me.btnFilterQuery = New System.Windows.Forms.Button
        Me.nudIdTo = New System.Windows.Forms.NumericUpDown
        Me.nudIdFrom = New System.Windows.Forms.NumericUpDown
        Me.label8 = New System.Windows.Forms.Label
        Me.label7 = New System.Windows.Forms.Label
        Me.btnFilterApply = New System.Windows.Forms.Button
        Me.btnStatus = New System.Windows.Forms.Button
        Me.groupBox5 = New System.Windows.Forms.GroupBox
        Me.chbShowPeriod = New System.Windows.Forms.CheckBox
        Me.rdbManual = New System.Windows.Forms.RadioButton
        Me.lstMessages = New System.Windows.Forms.ListView
        Me.btnReset = New System.Windows.Forms.Button
        Me.groupBox4 = New System.Windows.Forms.GroupBox
        Me.btnGetVersions = New System.Windows.Forms.Button
        Me.lbxInfo = New System.Windows.Forms.ListBox
        Me.btnInfoClear = New System.Windows.Forms.Button
        Me.groupBox2.SuspendLayout()
        CType(Me.nudLength, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox1.SuspendLayout()
        Me.groupBox3.SuspendLayout()
        CType(Me.nudIdTo, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudIdFrom, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox5.SuspendLayout()
        Me.groupBox4.SuspendLayout()
        Me.SuspendLayout()
        '
        'tmrRead
        '
        Me.tmrRead.Interval = 50
        '
        'tmrDisplay
        '
        '
        'groupBox2
        '
        Me.groupBox2.Controls.Add(Me.cbbID)
        Me.groupBox2.Controls.Add(Me.label11)
        Me.groupBox2.Controls.Add(Me.label10)
        Me.groupBox2.Controls.Add(Me.cbbCST)
        Me.groupBox2.Controls.Add(Me.label9)
        Me.groupBox2.Controls.Add(Me.cbbDirection)
        Me.groupBox2.Controls.Add(Me.nudLength)
        Me.groupBox2.Controls.Add(Me.txtData7)
        Me.groupBox2.Controls.Add(Me.txtData6)
        Me.groupBox2.Controls.Add(Me.txtData5)
        Me.groupBox2.Controls.Add(Me.txtData4)
        Me.groupBox2.Controls.Add(Me.txtData3)
        Me.groupBox2.Controls.Add(Me.txtData2)
        Me.groupBox2.Controls.Add(Me.txtData1)
        Me.groupBox2.Controls.Add(Me.txtData0)
        Me.groupBox2.Controls.Add(Me.txtID)
        Me.groupBox2.Controls.Add(Me.label6)
        Me.groupBox2.Controls.Add(Me.label5)
        Me.groupBox2.Controls.Add(Me.label4)
        Me.groupBox2.Controls.Add(Me.btnWrite)
        Me.groupBox2.Location = New System.Drawing.Point(5, 285)
        Me.groupBox2.Name = "groupBox2"
        Me.groupBox2.Size = New System.Drawing.Size(733, 102)
        Me.groupBox2.TabIndex = 55
        Me.groupBox2.TabStop = False
        Me.groupBox2.Text = "Write Messages"
        '
        'cbbID
        '
        Me.cbbID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cbbID.FormattingEnabled = True
        Me.cbbID.Location = New System.Drawing.Point(10, 32)
        Me.cbbID.Name = "cbbID"
        Me.cbbID.Size = New System.Drawing.Size(98, 21)
        Me.cbbID.TabIndex = 32
        '
        'label11
        '
        Me.label11.AutoSize = True
        Me.label11.Location = New System.Drawing.Point(7, 16)
        Me.label11.Name = "label11"
        Me.label11.Size = New System.Drawing.Size(49, 13)
        Me.label11.TabIndex = 31
        Me.label11.Text = "ID (Hex):"
        '
        'label10
        '
        Me.label10.AutoSize = True
        Me.label10.Location = New System.Drawing.Point(396, 16)
        Me.label10.Name = "label10"
        Me.label10.Size = New System.Drawing.Size(87, 13)
        Me.label10.TabIndex = 37
        Me.label10.Text = "Checksum Type:"
        '
        'cbbCST
        '
        Me.cbbCST.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cbbCST.Enabled = False
        Me.cbbCST.FormattingEnabled = True
        Me.cbbCST.Items.AddRange(New Object() {"Classic", "Enhanced", "Automatic"})
        Me.cbbCST.Location = New System.Drawing.Point(394, 32)
        Me.cbbCST.Name = "cbbCST"
        Me.cbbCST.Size = New System.Drawing.Size(121, 21)
        Me.cbbCST.TabIndex = 38
        '
        'label9
        '
        Me.label9.AutoSize = True
        Me.label9.Location = New System.Drawing.Point(237, 16)
        Me.label9.Name = "label9"
        Me.label9.Size = New System.Drawing.Size(52, 13)
        Me.label9.TabIndex = 35
        Me.label9.Text = "Direction:"
        '
        'cbbDirection
        '
        Me.cbbDirection.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cbbDirection.Enabled = False
        Me.cbbDirection.FormattingEnabled = True
        Me.cbbDirection.Items.AddRange(New Object() {"Publisher", "Subscriber"})
        Me.cbbDirection.Location = New System.Drawing.Point(240, 31)
        Me.cbbDirection.Name = "cbbDirection"
        Me.cbbDirection.Size = New System.Drawing.Size(142, 21)
        Me.cbbDirection.TabIndex = 36
        '
        'nudLength
        '
        Me.nudLength.Enabled = False
        Me.nudLength.Location = New System.Drawing.Point(10, 71)
        Me.nudLength.Maximum = New Decimal(New Integer() {8, 0, 0, 0})
        Me.nudLength.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudLength.Name = "nudLength"
        Me.nudLength.Size = New System.Drawing.Size(98, 20)
        Me.nudLength.TabIndex = 40
        Me.nudLength.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'txtData7
        '
        Me.txtData7.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData7.Enabled = False
        Me.txtData7.Location = New System.Drawing.Point(392, 71)
        Me.txtData7.MaxLength = 2
        Me.txtData7.Name = "txtData7"
        Me.txtData7.Size = New System.Drawing.Size(32, 20)
        Me.txtData7.TabIndex = 49
        Me.txtData7.Text = "00"
        '
        'txtData6
        '
        Me.txtData6.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData6.Enabled = False
        Me.txtData6.Location = New System.Drawing.Point(354, 71)
        Me.txtData6.MaxLength = 2
        Me.txtData6.Name = "txtData6"
        Me.txtData6.Size = New System.Drawing.Size(32, 20)
        Me.txtData6.TabIndex = 48
        Me.txtData6.Text = "00"
        '
        'txtData5
        '
        Me.txtData5.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData5.Enabled = False
        Me.txtData5.Location = New System.Drawing.Point(316, 71)
        Me.txtData5.MaxLength = 2
        Me.txtData5.Name = "txtData5"
        Me.txtData5.Size = New System.Drawing.Size(32, 20)
        Me.txtData5.TabIndex = 47
        Me.txtData5.Text = "00"
        '
        'txtData4
        '
        Me.txtData4.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData4.Enabled = False
        Me.txtData4.Location = New System.Drawing.Point(278, 71)
        Me.txtData4.MaxLength = 2
        Me.txtData4.Name = "txtData4"
        Me.txtData4.Size = New System.Drawing.Size(32, 20)
        Me.txtData4.TabIndex = 46
        Me.txtData4.Text = "00"
        '
        'txtData3
        '
        Me.txtData3.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData3.Enabled = False
        Me.txtData3.Location = New System.Drawing.Point(240, 71)
        Me.txtData3.MaxLength = 2
        Me.txtData3.Name = "txtData3"
        Me.txtData3.Size = New System.Drawing.Size(32, 20)
        Me.txtData3.TabIndex = 45
        Me.txtData3.Text = "00"
        '
        'txtData2
        '
        Me.txtData2.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData2.Enabled = False
        Me.txtData2.Location = New System.Drawing.Point(202, 71)
        Me.txtData2.MaxLength = 2
        Me.txtData2.Name = "txtData2"
        Me.txtData2.Size = New System.Drawing.Size(32, 20)
        Me.txtData2.TabIndex = 44
        Me.txtData2.Text = "00"
        '
        'txtData1
        '
        Me.txtData1.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData1.Enabled = False
        Me.txtData1.Location = New System.Drawing.Point(164, 71)
        Me.txtData1.MaxLength = 2
        Me.txtData1.Name = "txtData1"
        Me.txtData1.Size = New System.Drawing.Size(32, 20)
        Me.txtData1.TabIndex = 43
        Me.txtData1.Text = "00"
        '
        'txtData0
        '
        Me.txtData0.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtData0.Enabled = False
        Me.txtData0.Location = New System.Drawing.Point(126, 71)
        Me.txtData0.MaxLength = 2
        Me.txtData0.Name = "txtData0"
        Me.txtData0.Size = New System.Drawing.Size(32, 20)
        Me.txtData0.TabIndex = 42
        Me.txtData0.Text = "00"
        '
        'txtID
        '
        Me.txtID.Enabled = False
        Me.txtID.Location = New System.Drawing.Point(126, 32)
        Me.txtID.MaxLength = 2
        Me.txtID.Name = "txtID"
        Me.txtID.Size = New System.Drawing.Size(100, 20)
        Me.txtID.TabIndex = 34
        '
        'label6
        '
        Me.label6.AutoSize = True
        Me.label6.Location = New System.Drawing.Point(123, 55)
        Me.label6.Name = "label6"
        Me.label6.Size = New System.Drawing.Size(60, 13)
        Me.label6.TabIndex = 41
        Me.label6.Text = "Data (1..8):"
        '
        'label5
        '
        Me.label5.AutoSize = True
        Me.label5.Location = New System.Drawing.Point(7, 55)
        Me.label5.Name = "label5"
        Me.label5.Size = New System.Drawing.Size(43, 13)
        Me.label5.TabIndex = 39
        Me.label5.Text = "Length:"
        '
        'label4
        '
        Me.label4.AutoSize = True
        Me.label4.Location = New System.Drawing.Point(123, 16)
        Me.label4.Name = "label4"
        Me.label4.Size = New System.Drawing.Size(56, 13)
        Me.label4.TabIndex = 33
        Me.label4.Text = "PID (Hex):"
        '
        'btnWrite
        '
        Me.btnWrite.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnWrite.Cursor = System.Windows.Forms.Cursors.Default
        Me.btnWrite.Enabled = False
        Me.btnWrite.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnWrite.Location = New System.Drawing.Point(663, 29)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(65, 23)
        Me.btnWrite.TabIndex = 50
        Me.btnWrite.Text = "Write"
        '
        'btnMsgClear
        '
        Me.btnMsgClear.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnMsgClear.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnMsgClear.Location = New System.Drawing.Point(663, 37)
        Me.btnMsgClear.Name = "btnMsgClear"
        Me.btnMsgClear.Size = New System.Drawing.Size(65, 23)
        Me.btnMsgClear.TabIndex = 29
        Me.btnMsgClear.Text = "Clear"
        Me.btnMsgClear.UseVisualStyleBackColor = True
        '
        'groupBox1
        '
        Me.groupBox1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.groupBox1.Controls.Add(Me.btnIdent)
        Me.groupBox1.Controls.Add(Me.btnHwRefresh)
        Me.groupBox1.Controls.Add(Me.cbbChannel)
        Me.groupBox1.Controls.Add(Me.cbbHwMode)
        Me.groupBox1.Controls.Add(Me.label3)
        Me.groupBox1.Controls.Add(Me.cbbBaudrates)
        Me.groupBox1.Controls.Add(Me.label2)
        Me.groupBox1.Controls.Add(Me.label1)
        Me.groupBox1.Controls.Add(Me.btnInit)
        Me.groupBox1.Controls.Add(Me.btnRelease)
        Me.groupBox1.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.groupBox1.Location = New System.Drawing.Point(5, 4)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(734, 65)
        Me.groupBox1.TabIndex = 52
        Me.groupBox1.TabStop = False
        Me.groupBox1.Text = " Connection "
        '
        'btnIdent
        '
        Me.btnIdent.Location = New System.Drawing.Point(532, 30)
        Me.btnIdent.Name = "btnIdent"
        Me.btnIdent.Size = New System.Drawing.Size(54, 23)
        Me.btnIdent.TabIndex = 9
        Me.btnIdent.Text = "Identify"
        Me.btnIdent.UseVisualStyleBackColor = True
        '
        'btnHwRefresh
        '
        Me.btnHwRefresh.Cursor = System.Windows.Forms.Cursors.Default
        Me.btnHwRefresh.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnHwRefresh.Location = New System.Drawing.Point(234, 31)
        Me.btnHwRefresh.Name = "btnHwRefresh"
        Me.btnHwRefresh.Size = New System.Drawing.Size(65, 23)
        Me.btnHwRefresh.TabIndex = 4
        Me.btnHwRefresh.Text = "Refresh"
        '
        'cbbChannel
        '
        Me.cbbChannel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cbbChannel.Font = New System.Drawing.Font("Consolas", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cbbChannel.Items.AddRange(New Object() {"None", "USB-Channel 1", "USB-Channel 2", "USB-Channel 3", "USB-Channel 4", "USB-Channel 5", "USB-Channel 6", "USB-Channel 7", "USB-Channel 8"})
        Me.cbbChannel.Location = New System.Drawing.Point(6, 31)
        Me.cbbChannel.Name = "cbbChannel"
        Me.cbbChannel.Size = New System.Drawing.Size(222, 21)
        Me.cbbChannel.TabIndex = 3
        '
        'cbbHwMode
        '
        Me.cbbHwMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cbbHwMode.Items.AddRange(New Object() {"Slave", "Master"})
        Me.cbbHwMode.Location = New System.Drawing.Point(427, 32)
        Me.cbbHwMode.Name = "cbbHwMode"
        Me.cbbHwMode.Size = New System.Drawing.Size(88, 21)
        Me.cbbHwMode.TabIndex = 8
        '
        'label3
        '
        Me.label3.Location = New System.Drawing.Point(428, 16)
        Me.label3.Name = "label3"
        Me.label3.Size = New System.Drawing.Size(90, 23)
        Me.label3.TabIndex = 7
        Me.label3.Text = "Mode:"
        '
        'cbbBaudrates
        '
        Me.cbbBaudrates.Items.AddRange(New Object() {"2400", "9600", "10400", "19200"})
        Me.cbbBaudrates.Location = New System.Drawing.Point(305, 32)
        Me.cbbBaudrates.Name = "cbbBaudrates"
        Me.cbbBaudrates.Size = New System.Drawing.Size(116, 21)
        Me.cbbBaudrates.TabIndex = 6
        '
        'label2
        '
        Me.label2.Location = New System.Drawing.Point(305, 16)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(56, 23)
        Me.label2.TabIndex = 5
        Me.label2.Text = "Baudrate:"
        '
        'label1
        '
        Me.label1.Location = New System.Drawing.Point(7, 16)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(56, 23)
        Me.label1.TabIndex = 2
        Me.label1.Text = "Hardware:"
        '
        'btnInit
        '
        Me.btnInit.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnInit.Cursor = System.Windows.Forms.Cursors.Default
        Me.btnInit.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnInit.Location = New System.Drawing.Point(592, 30)
        Me.btnInit.Name = "btnInit"
        Me.btnInit.Size = New System.Drawing.Size(65, 23)
        Me.btnInit.TabIndex = 10
        Me.btnInit.Text = "Initialize"
        '
        'btnRelease
        '
        Me.btnRelease.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRelease.Cursor = System.Windows.Forms.Cursors.Default
        Me.btnRelease.Enabled = False
        Me.btnRelease.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnRelease.Location = New System.Drawing.Point(663, 30)
        Me.btnRelease.Name = "btnRelease"
        Me.btnRelease.Size = New System.Drawing.Size(65, 23)
        Me.btnRelease.TabIndex = 11
        Me.btnRelease.Text = "Release"
        '
        'clhRcvTime
        '
        Me.clhRcvTime.Text = "Rcv Time"
        Me.clhRcvTime.Width = 97
        '
        'clhCRC
        '
        Me.clhCRC.Text = "Checksum"
        '
        'clhDirection
        '
        Me.clhDirection.Text = "Direction"
        '
        'clhCount
        '
        Me.clhCount.Text = "Count"
        Me.clhCount.Width = 49
        '
        'clhErrors
        '
        Me.clhErrors.Text = "Errors"
        '
        'rdbTimer
        '
        Me.rdbTimer.AutoSize = True
        Me.rdbTimer.Checked = True
        Me.rdbTimer.Location = New System.Drawing.Point(6, 14)
        Me.rdbTimer.Name = "rdbTimer"
        Me.rdbTimer.Size = New System.Drawing.Size(117, 17)
        Me.rdbTimer.TabIndex = 23
        Me.rdbTimer.TabStop = True
        Me.rdbTimer.Text = "Read using a Timer"
        Me.rdbTimer.UseVisualStyleBackColor = True
        '
        'clhCST
        '
        Me.clhCST.Text = "CST"
        '
        'btnRead
        '
        Me.btnRead.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRead.Enabled = False
        Me.btnRead.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnRead.Location = New System.Drawing.Point(592, 37)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(65, 23)
        Me.btnRead.TabIndex = 28
        Me.btnRead.Text = "Read"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'clhData
        '
        Me.clhData.Text = "Data"
        Me.clhData.Width = 160
        '
        'clhLength
        '
        Me.clhLength.Text = "Length"
        Me.clhLength.Width = 50
        '
        'clhID
        '
        Me.clhID.Text = "ID"
        Me.clhID.Width = 90
        '
        'btnConfigure
        '
        Me.btnConfigure.Enabled = False
        Me.btnConfigure.Location = New System.Drawing.Point(6, 26)
        Me.btnConfigure.Name = "btnConfigure"
        Me.btnConfigure.Size = New System.Drawing.Size(152, 23)
        Me.btnConfigure.TabIndex = 22
        Me.btnConfigure.Text = "Configure Global Frames"
        Me.btnConfigure.UseVisualStyleBackColor = True
        '
        'rdbFilterOpen
        '
        Me.rdbFilterOpen.Checked = True
        Me.rdbFilterOpen.Location = New System.Drawing.Point(196, 29)
        Me.rdbFilterOpen.Name = "rdbFilterOpen"
        Me.rdbFilterOpen.Size = New System.Drawing.Size(53, 17)
        Me.rdbFilterOpen.TabIndex = 13
        Me.rdbFilterOpen.TabStop = True
        Me.rdbFilterOpen.Text = "Open"
        Me.rdbFilterOpen.UseVisualStyleBackColor = True
        '
        'rdbFilterCustom
        '
        Me.rdbFilterCustom.Location = New System.Drawing.Point(314, 29)
        Me.rdbFilterCustom.Name = "rdbFilterCustom"
        Me.rdbFilterCustom.Size = New System.Drawing.Size(104, 17)
        Me.rdbFilterCustom.TabIndex = 15
        Me.rdbFilterCustom.Text = "Custom (expand)"
        Me.rdbFilterCustom.UseVisualStyleBackColor = True
        '
        'groupBox3
        '
        Me.groupBox3.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.groupBox3.Controls.Add(Me.btnConfigure)
        Me.groupBox3.Controls.Add(Me.rdbFilterOpen)
        Me.groupBox3.Controls.Add(Me.rdbFilterCustom)
        Me.groupBox3.Controls.Add(Me.rdbFilterClose)
        Me.groupBox3.Controls.Add(Me.btnFilterQuery)
        Me.groupBox3.Controls.Add(Me.nudIdTo)
        Me.groupBox3.Controls.Add(Me.nudIdFrom)
        Me.groupBox3.Controls.Add(Me.label8)
        Me.groupBox3.Controls.Add(Me.label7)
        Me.groupBox3.Controls.Add(Me.btnFilterApply)
        Me.groupBox3.Location = New System.Drawing.Point(5, 75)
        Me.groupBox3.Name = "groupBox3"
        Me.groupBox3.Size = New System.Drawing.Size(734, 58)
        Me.groupBox3.TabIndex = 53
        Me.groupBox3.TabStop = False
        Me.groupBox3.Text = "Configuration && Message Filtering "
        '
        'rdbFilterClose
        '
        Me.rdbFilterClose.Location = New System.Drawing.Point(253, 29)
        Me.rdbFilterClose.Name = "rdbFilterClose"
        Me.rdbFilterClose.Size = New System.Drawing.Size(58, 17)
        Me.rdbFilterClose.TabIndex = 14
        Me.rdbFilterClose.Text = "Close"
        Me.rdbFilterClose.UseVisualStyleBackColor = True
        '
        'btnFilterQuery
        '
        Me.btnFilterQuery.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnFilterQuery.Enabled = False
        Me.btnFilterQuery.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnFilterQuery.Location = New System.Drawing.Point(663, 26)
        Me.btnFilterQuery.Name = "btnFilterQuery"
        Me.btnFilterQuery.Size = New System.Drawing.Size(65, 23)
        Me.btnFilterQuery.TabIndex = 21
        Me.btnFilterQuery.Text = "Query"
        Me.btnFilterQuery.UseVisualStyleBackColor = True
        '
        'nudIdTo
        '
        Me.nudIdTo.Location = New System.Drawing.Point(499, 29)
        Me.nudIdTo.Maximum = New Decimal(New Integer() {63, 0, 0, 0})
        Me.nudIdTo.Name = "nudIdTo"
        Me.nudIdTo.Size = New System.Drawing.Size(69, 20)
        Me.nudIdTo.TabIndex = 19
        Me.nudIdTo.Value = New Decimal(New Integer() {63, 0, 0, 0})
        '
        'nudIdFrom
        '
        Me.nudIdFrom.Location = New System.Drawing.Point(424, 29)
        Me.nudIdFrom.Maximum = New Decimal(New Integer() {63, 0, 0, 0})
        Me.nudIdFrom.Name = "nudIdFrom"
        Me.nudIdFrom.Size = New System.Drawing.Size(69, 20)
        Me.nudIdFrom.TabIndex = 17
        '
        'label8
        '
        Me.label8.Location = New System.Drawing.Point(497, 10)
        Me.label8.Name = "label8"
        Me.label8.Size = New System.Drawing.Size(28, 23)
        Me.label8.TabIndex = 18
        Me.label8.Text = "To:"
        '
        'label7
        '
        Me.label7.Location = New System.Drawing.Point(422, 12)
        Me.label7.Name = "label7"
        Me.label7.Size = New System.Drawing.Size(38, 23)
        Me.label7.TabIndex = 16
        Me.label7.Text = "From:"
        '
        'btnFilterApply
        '
        Me.btnFilterApply.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnFilterApply.Enabled = False
        Me.btnFilterApply.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnFilterApply.Location = New System.Drawing.Point(592, 26)
        Me.btnFilterApply.Name = "btnFilterApply"
        Me.btnFilterApply.Size = New System.Drawing.Size(65, 23)
        Me.btnFilterApply.TabIndex = 20
        Me.btnFilterApply.Text = "Apply"
        Me.btnFilterApply.UseVisualStyleBackColor = True
        '
        'btnStatus
        '
        Me.btnStatus.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnStatus.Enabled = False
        Me.btnStatus.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnStatus.Location = New System.Drawing.Point(592, 48)
        Me.btnStatus.Name = "btnStatus"
        Me.btnStatus.Size = New System.Drawing.Size(65, 23)
        Me.btnStatus.TabIndex = 55
        Me.btnStatus.Text = "Status"
        Me.btnStatus.UseVisualStyleBackColor = True
        '
        'groupBox5
        '
        Me.groupBox5.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.groupBox5.Controls.Add(Me.chbShowPeriod)
        Me.groupBox5.Controls.Add(Me.rdbManual)
        Me.groupBox5.Controls.Add(Me.lstMessages)
        Me.groupBox5.Controls.Add(Me.btnMsgClear)
        Me.groupBox5.Controls.Add(Me.rdbTimer)
        Me.groupBox5.Controls.Add(Me.btnRead)
        Me.groupBox5.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.groupBox5.Location = New System.Drawing.Point(5, 139)
        Me.groupBox5.Name = "groupBox5"
        Me.groupBox5.Size = New System.Drawing.Size(734, 140)
        Me.groupBox5.TabIndex = 54
        Me.groupBox5.TabStop = False
        Me.groupBox5.Text = " Messages Reading "
        '
        'chbShowPeriod
        '
        Me.chbShowPeriod.AutoSize = True
        Me.chbShowPeriod.Checked = True
        Me.chbShowPeriod.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chbShowPeriod.Location = New System.Drawing.Point(227, 15)
        Me.chbShowPeriod.Name = "chbShowPeriod"
        Me.chbShowPeriod.Size = New System.Drawing.Size(123, 17)
        Me.chbShowPeriod.TabIndex = 26
        Me.chbShowPeriod.Text = "Timestamp as period"
        Me.chbShowPeriod.UseVisualStyleBackColor = True
        '
        'rdbManual
        '
        Me.rdbManual.AutoSize = True
        Me.rdbManual.Location = New System.Drawing.Point(129, 14)
        Me.rdbManual.Name = "rdbManual"
        Me.rdbManual.Size = New System.Drawing.Size(89, 17)
        Me.rdbManual.TabIndex = 25
        Me.rdbManual.Text = "Manual Read"
        Me.rdbManual.UseVisualStyleBackColor = True
        '
        'lstMessages
        '
        Me.lstMessages.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.clhID, Me.clhLength, Me.clhData, Me.clhCount, Me.clhRcvTime, Me.clhDirection, Me.clhErrors, Me.clhCST, Me.clhCRC})
        Me.lstMessages.FullRowSelect = True
        Me.lstMessages.Location = New System.Drawing.Point(8, 37)
        Me.lstMessages.MultiSelect = False
        Me.lstMessages.Name = "lstMessages"
        Me.lstMessages.Size = New System.Drawing.Size(560, 96)
        Me.lstMessages.TabIndex = 27
        Me.lstMessages.UseCompatibleStateImageBehavior = False
        Me.lstMessages.View = System.Windows.Forms.View.Details
        '
        'btnReset
        '
        Me.btnReset.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnReset.Enabled = False
        Me.btnReset.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnReset.Location = New System.Drawing.Point(663, 48)
        Me.btnReset.Name = "btnReset"
        Me.btnReset.Size = New System.Drawing.Size(65, 23)
        Me.btnReset.TabIndex = 56
        Me.btnReset.Text = "Reset"
        Me.btnReset.UseVisualStyleBackColor = True
        '
        'groupBox4
        '
        Me.groupBox4.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.groupBox4.Controls.Add(Me.btnReset)
        Me.groupBox4.Controls.Add(Me.btnStatus)
        Me.groupBox4.Controls.Add(Me.btnGetVersions)
        Me.groupBox4.Controls.Add(Me.lbxInfo)
        Me.groupBox4.Controls.Add(Me.btnInfoClear)
        Me.groupBox4.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.groupBox4.Location = New System.Drawing.Point(5, 393)
        Me.groupBox4.Name = "groupBox4"
        Me.groupBox4.Size = New System.Drawing.Size(734, 135)
        Me.groupBox4.TabIndex = 56
        Me.groupBox4.TabStop = False
        Me.groupBox4.Text = "Information"
        '
        'btnGetVersions
        '
        Me.btnGetVersions.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnGetVersions.Enabled = False
        Me.btnGetVersions.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnGetVersions.Location = New System.Drawing.Point(592, 19)
        Me.btnGetVersions.Name = "btnGetVersions"
        Me.btnGetVersions.Size = New System.Drawing.Size(65, 23)
        Me.btnGetVersions.TabIndex = 53
        Me.btnGetVersions.Text = "Versions"
        Me.btnGetVersions.UseVisualStyleBackColor = True
        '
        'lbxInfo
        '
        Me.lbxInfo.FormattingEnabled = True
        Me.lbxInfo.Items.AddRange(New Object() {"Select a Hardware and a configuration for it. Then click ""Initialize"" button"})
        Me.lbxInfo.Location = New System.Drawing.Point(10, 19)
        Me.lbxInfo.Name = "lbxInfo"
        Me.lbxInfo.ScrollAlwaysVisible = True
        Me.lbxInfo.Size = New System.Drawing.Size(558, 108)
        Me.lbxInfo.TabIndex = 52
        '
        'btnInfoClear
        '
        Me.btnInfoClear.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnInfoClear.FlatStyle = System.Windows.Forms.FlatStyle.System
        Me.btnInfoClear.Location = New System.Drawing.Point(663, 19)
        Me.btnInfoClear.Name = "btnInfoClear"
        Me.btnInfoClear.Size = New System.Drawing.Size(65, 23)
        Me.btnInfoClear.TabIndex = 54
        Me.btnInfoClear.Text = "Clear"
        Me.btnInfoClear.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(744, 533)
        Me.Controls.Add(Me.groupBox2)
        Me.Controls.Add(Me.groupBox1)
        Me.Controls.Add(Me.groupBox3)
        Me.Controls.Add(Me.groupBox5)
        Me.Controls.Add(Me.groupBox4)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "Form1"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "PLIN-API Example"
        Me.groupBox2.ResumeLayout(False)
        Me.groupBox2.PerformLayout()
        CType(Me.nudLength, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox1.ResumeLayout(False)
        Me.groupBox3.ResumeLayout(False)
        CType(Me.nudIdTo, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudIdFrom, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox5.ResumeLayout(False)
        Me.groupBox5.PerformLayout()
        Me.groupBox4.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents tmrRead As System.Windows.Forms.Timer
    Private WithEvents tmrDisplay As System.Windows.Forms.Timer
    Private WithEvents groupBox2 As System.Windows.Forms.GroupBox
    Private WithEvents cbbID As System.Windows.Forms.ComboBox
    Private WithEvents label11 As System.Windows.Forms.Label
    Private WithEvents label10 As System.Windows.Forms.Label
    Private WithEvents cbbCST As System.Windows.Forms.ComboBox
    Private WithEvents label9 As System.Windows.Forms.Label
    Private WithEvents cbbDirection As System.Windows.Forms.ComboBox
    Private WithEvents nudLength As System.Windows.Forms.NumericUpDown
    Private WithEvents txtData7 As System.Windows.Forms.TextBox
    Private WithEvents txtData6 As System.Windows.Forms.TextBox
    Private WithEvents txtData5 As System.Windows.Forms.TextBox
    Private WithEvents txtData4 As System.Windows.Forms.TextBox
    Private WithEvents txtData3 As System.Windows.Forms.TextBox
    Private WithEvents txtData2 As System.Windows.Forms.TextBox
    Private WithEvents txtData1 As System.Windows.Forms.TextBox
    Private WithEvents txtData0 As System.Windows.Forms.TextBox
    Private WithEvents txtID As System.Windows.Forms.TextBox
    Private WithEvents label6 As System.Windows.Forms.Label
    Private WithEvents label5 As System.Windows.Forms.Label
    Private WithEvents label4 As System.Windows.Forms.Label
    Private WithEvents btnWrite As System.Windows.Forms.Button
    Private WithEvents btnMsgClear As System.Windows.Forms.Button
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents btnIdent As System.Windows.Forms.Button
    Private WithEvents btnHwRefresh As System.Windows.Forms.Button
    Private WithEvents cbbChannel As System.Windows.Forms.ComboBox
    Private WithEvents cbbHwMode As System.Windows.Forms.ComboBox
    Private WithEvents label3 As System.Windows.Forms.Label
    Private WithEvents cbbBaudrates As System.Windows.Forms.ComboBox
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents btnInit As System.Windows.Forms.Button
    Private WithEvents btnRelease As System.Windows.Forms.Button
    Private WithEvents clhRcvTime As System.Windows.Forms.ColumnHeader
    Private WithEvents clhCRC As System.Windows.Forms.ColumnHeader
    Private WithEvents clhDirection As System.Windows.Forms.ColumnHeader
    Private WithEvents clhCount As System.Windows.Forms.ColumnHeader
    Private WithEvents clhErrors As System.Windows.Forms.ColumnHeader
    Private WithEvents rdbTimer As System.Windows.Forms.RadioButton
    Private WithEvents clhCST As System.Windows.Forms.ColumnHeader
    Private WithEvents btnRead As System.Windows.Forms.Button
    Private WithEvents clhData As System.Windows.Forms.ColumnHeader
    Private WithEvents clhLength As System.Windows.Forms.ColumnHeader
    Private WithEvents clhID As System.Windows.Forms.ColumnHeader
    Private WithEvents btnConfigure As System.Windows.Forms.Button
    Private WithEvents rdbFilterOpen As System.Windows.Forms.RadioButton
    Private WithEvents rdbFilterCustom As System.Windows.Forms.RadioButton
    Private WithEvents groupBox3 As System.Windows.Forms.GroupBox
    Private WithEvents rdbFilterClose As System.Windows.Forms.RadioButton
    Private WithEvents btnFilterQuery As System.Windows.Forms.Button
    Private WithEvents nudIdTo As System.Windows.Forms.NumericUpDown
    Private WithEvents nudIdFrom As System.Windows.Forms.NumericUpDown
    Private WithEvents label8 As System.Windows.Forms.Label
    Private WithEvents label7 As System.Windows.Forms.Label
    Private WithEvents btnFilterApply As System.Windows.Forms.Button
    Private WithEvents btnStatus As System.Windows.Forms.Button
    Private WithEvents groupBox5 As System.Windows.Forms.GroupBox
    Private WithEvents chbShowPeriod As System.Windows.Forms.CheckBox
    Private WithEvents rdbManual As System.Windows.Forms.RadioButton
    Private WithEvents lstMessages As System.Windows.Forms.ListView
    Private WithEvents btnReset As System.Windows.Forms.Button
    Private WithEvents groupBox4 As System.Windows.Forms.GroupBox
    Private WithEvents btnGetVersions As System.Windows.Forms.Button
    Private WithEvents lbxInfo As System.Windows.Forms.ListBox
    Private WithEvents btnInfoClear As System.Windows.Forms.Button

End Class
