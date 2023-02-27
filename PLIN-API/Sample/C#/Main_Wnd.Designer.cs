namespace PLIN_API_Example
{
    partial class Main_Wnd
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnIdent = new System.Windows.Forms.Button();
            this.btnHwRefresh = new System.Windows.Forms.Button();
            this.cbbChannel = new System.Windows.Forms.ComboBox();
            this.cbbHwMode = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.cbbBaudrates = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.btnInit = new System.Windows.Forms.Button();
            this.btnRelease = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnConfigure = new System.Windows.Forms.Button();
            this.rdbFilterOpen = new System.Windows.Forms.RadioButton();
            this.rdbFilterCustom = new System.Windows.Forms.RadioButton();
            this.rdbFilterClose = new System.Windows.Forms.RadioButton();
            this.btnFilterQuery = new System.Windows.Forms.Button();
            this.nudIdTo = new System.Windows.Forms.NumericUpDown();
            this.nudIdFrom = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.btnFilterApply = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.btnReset = new System.Windows.Forms.Button();
            this.btnStatus = new System.Windows.Forms.Button();
            this.btnGetVersions = new System.Windows.Forms.Button();
            this.lbxInfo = new System.Windows.Forms.ListBox();
            this.btnInfoClear = new System.Windows.Forms.Button();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.chbShowPeriod = new System.Windows.Forms.CheckBox();
            this.rdbManual = new System.Windows.Forms.RadioButton();
            this.lstMessages = new System.Windows.Forms.ListView();
            this.clhID = new System.Windows.Forms.ColumnHeader();
            this.clhLength = new System.Windows.Forms.ColumnHeader();
            this.clhData = new System.Windows.Forms.ColumnHeader();
            this.clhCount = new System.Windows.Forms.ColumnHeader();
            this.clhRcvTime = new System.Windows.Forms.ColumnHeader();
            this.clhDirection = new System.Windows.Forms.ColumnHeader();
            this.clhErrors = new System.Windows.Forms.ColumnHeader();
            this.clhCST = new System.Windows.Forms.ColumnHeader();
            this.clhCRC = new System.Windows.Forms.ColumnHeader();
            this.btnMsgClear = new System.Windows.Forms.Button();
            this.rdbTimer = new System.Windows.Forms.RadioButton();
            this.btnRead = new System.Windows.Forms.Button();
            this.tmrRead = new System.Windows.Forms.Timer(this.components);
            this.tmrDisplay = new System.Windows.Forms.Timer(this.components);
            this.btnWrite = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.cbbID = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.cbbCST = new System.Windows.Forms.ComboBox();
            this.label9 = new System.Windows.Forms.Label();
            this.cbbDirection = new System.Windows.Forms.ComboBox();
            this.nudLength = new System.Windows.Forms.NumericUpDown();
            this.txtData7 = new System.Windows.Forms.TextBox();
            this.txtData6 = new System.Windows.Forms.TextBox();
            this.txtData5 = new System.Windows.Forms.TextBox();
            this.txtData4 = new System.Windows.Forms.TextBox();
            this.txtData3 = new System.Windows.Forms.TextBox();
            this.txtData2 = new System.Windows.Forms.TextBox();
            this.txtData1 = new System.Windows.Forms.TextBox();
            this.txtData0 = new System.Windows.Forms.TextBox();
            this.txtID = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudIdTo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudIdFrom)).BeginInit();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudLength)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.btnIdent);
            this.groupBox1.Controls.Add(this.btnHwRefresh);
            this.groupBox1.Controls.Add(this.cbbChannel);
            this.groupBox1.Controls.Add(this.cbbHwMode);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.cbbBaudrates);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.btnInit);
            this.groupBox1.Controls.Add(this.btnRelease);
            this.groupBox1.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.groupBox1.Location = new System.Drawing.Point(8, 8);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(734, 65);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = " Connection ";
            // 
            // btnIdent
            // 
            this.btnIdent.Location = new System.Drawing.Point(532, 30);
            this.btnIdent.Name = "btnIdent";
            this.btnIdent.Size = new System.Drawing.Size(54, 23);
            this.btnIdent.TabIndex = 9;
            this.btnIdent.Text = "Identify";
            this.btnIdent.UseVisualStyleBackColor = true;
            this.btnIdent.Click += new System.EventHandler(this.btnIdent_Click);
            // 
            // btnHwRefresh
            // 
            this.btnHwRefresh.Cursor = System.Windows.Forms.Cursors.Default;
            this.btnHwRefresh.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnHwRefresh.Location = new System.Drawing.Point(234, 31);
            this.btnHwRefresh.Name = "btnHwRefresh";
            this.btnHwRefresh.Size = new System.Drawing.Size(65, 23);
            this.btnHwRefresh.TabIndex = 4;
            this.btnHwRefresh.Text = "Refresh";
            this.btnHwRefresh.Click += new System.EventHandler(this.btnHwRefresh_Click);
            // 
            // cbbChannel
            // 
            this.cbbChannel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbChannel.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbbChannel.Items.AddRange(new object[] {
            "None",
            "USB-Channel 1",
            "USB-Channel 2",
            "USB-Channel 3",
            "USB-Channel 4",
            "USB-Channel 5",
            "USB-Channel 6",
            "USB-Channel 7",
            "USB-Channel 8"});
            this.cbbChannel.Location = new System.Drawing.Point(6, 31);
            this.cbbChannel.Name = "cbbChannel";
            this.cbbChannel.Size = new System.Drawing.Size(222, 21);
            this.cbbChannel.TabIndex = 3;
            this.cbbChannel.SelectedIndexChanged += new System.EventHandler(this.cbbChannel_SelectedIndexChanged);
            // 
            // cbbHwMode
            // 
            this.cbbHwMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbHwMode.Items.AddRange(new object[] {
            "Slave",
            "Master"});
            this.cbbHwMode.Location = new System.Drawing.Point(427, 32);
            this.cbbHwMode.Name = "cbbHwMode";
            this.cbbHwMode.Size = new System.Drawing.Size(88, 21);
            this.cbbHwMode.TabIndex = 8;
            this.cbbHwMode.SelectedIndexChanged += new System.EventHandler(this.cbbHwMode_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(428, 16);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(90, 23);
            this.label3.TabIndex = 7;
            this.label3.Text = "Mode:";
            // 
            // cbbBaudrates
            // 
            this.cbbBaudrates.Items.AddRange(new object[] {
            "2400",
            "9600",
            "10400",
            "19200"});
            this.cbbBaudrates.Location = new System.Drawing.Point(305, 32);
            this.cbbBaudrates.Name = "cbbBaudrates";
            this.cbbBaudrates.Size = new System.Drawing.Size(116, 21);
            this.cbbBaudrates.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(305, 16);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 23);
            this.label2.TabIndex = 5;
            this.label2.Text = "Baudrate:";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(7, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 23);
            this.label1.TabIndex = 2;
            this.label1.Text = "Hardware:";
            // 
            // btnInit
            // 
            this.btnInit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnInit.Cursor = System.Windows.Forms.Cursors.Default;
            this.btnInit.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnInit.Location = new System.Drawing.Point(592, 30);
            this.btnInit.Name = "btnInit";
            this.btnInit.Size = new System.Drawing.Size(65, 23);
            this.btnInit.TabIndex = 10;
            this.btnInit.Text = "Initialize";
            this.btnInit.Click += new System.EventHandler(this.btnInit_Click);
            // 
            // btnRelease
            // 
            this.btnRelease.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRelease.Cursor = System.Windows.Forms.Cursors.Default;
            this.btnRelease.Enabled = false;
            this.btnRelease.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnRelease.Location = new System.Drawing.Point(663, 30);
            this.btnRelease.Name = "btnRelease";
            this.btnRelease.Size = new System.Drawing.Size(65, 23);
            this.btnRelease.TabIndex = 11;
            this.btnRelease.Text = "Release";
            this.btnRelease.Click += new System.EventHandler(this.btnRelease_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.btnConfigure);
            this.groupBox3.Controls.Add(this.rdbFilterOpen);
            this.groupBox3.Controls.Add(this.rdbFilterCustom);
            this.groupBox3.Controls.Add(this.rdbFilterClose);
            this.groupBox3.Controls.Add(this.btnFilterQuery);
            this.groupBox3.Controls.Add(this.nudIdTo);
            this.groupBox3.Controls.Add(this.nudIdFrom);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.btnFilterApply);
            this.groupBox3.Location = new System.Drawing.Point(8, 79);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(734, 58);
            this.groupBox3.TabIndex = 12;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Configuration && Message Filtering ";
            // 
            // btnConfigure
            // 
            this.btnConfigure.Enabled = false;
            this.btnConfigure.Location = new System.Drawing.Point(6, 26);
            this.btnConfigure.Name = "btnConfigure";
            this.btnConfigure.Size = new System.Drawing.Size(152, 23);
            this.btnConfigure.TabIndex = 22;
            this.btnConfigure.Text = "Configure Global Frames";
            this.btnConfigure.UseVisualStyleBackColor = true;
            this.btnConfigure.Click += new System.EventHandler(this.btnConfigure_Click);
            // 
            // rdbFilterOpen
            // 
            this.rdbFilterOpen.Checked = true;
            this.rdbFilterOpen.Location = new System.Drawing.Point(196, 29);
            this.rdbFilterOpen.Name = "rdbFilterOpen";
            this.rdbFilterOpen.Size = new System.Drawing.Size(53, 17);
            this.rdbFilterOpen.TabIndex = 13;
            this.rdbFilterOpen.TabStop = true;
            this.rdbFilterOpen.Text = "Open";
            this.rdbFilterOpen.UseVisualStyleBackColor = true;
            this.rdbFilterOpen.CheckedChanged += new System.EventHandler(this.rdbFilter_CheckedChanged);
            // 
            // rdbFilterCustom
            // 
            this.rdbFilterCustom.Location = new System.Drawing.Point(314, 29);
            this.rdbFilterCustom.Name = "rdbFilterCustom";
            this.rdbFilterCustom.Size = new System.Drawing.Size(104, 17);
            this.rdbFilterCustom.TabIndex = 15;
            this.rdbFilterCustom.Text = "Custom (expand)";
            this.rdbFilterCustom.UseVisualStyleBackColor = true;
            this.rdbFilterCustom.CheckedChanged += new System.EventHandler(this.rdbFilter_CheckedChanged);
            // 
            // rdbFilterClose
            // 
            this.rdbFilterClose.Location = new System.Drawing.Point(253, 29);
            this.rdbFilterClose.Name = "rdbFilterClose";
            this.rdbFilterClose.Size = new System.Drawing.Size(58, 17);
            this.rdbFilterClose.TabIndex = 14;
            this.rdbFilterClose.Text = "Close";
            this.rdbFilterClose.UseVisualStyleBackColor = true;
            this.rdbFilterClose.CheckedChanged += new System.EventHandler(this.rdbFilter_CheckedChanged);
            // 
            // btnFilterQuery
            // 
            this.btnFilterQuery.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFilterQuery.Enabled = false;
            this.btnFilterQuery.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnFilterQuery.Location = new System.Drawing.Point(663, 26);
            this.btnFilterQuery.Name = "btnFilterQuery";
            this.btnFilterQuery.Size = new System.Drawing.Size(65, 23);
            this.btnFilterQuery.TabIndex = 21;
            this.btnFilterQuery.Text = "Query";
            this.btnFilterQuery.UseVisualStyleBackColor = true;
            this.btnFilterQuery.Click += new System.EventHandler(this.btnFilterQuery_Click);
            // 
            // nudIdTo
            // 
            this.nudIdTo.Location = new System.Drawing.Point(499, 29);
            this.nudIdTo.Maximum = new decimal(new int[] {
            63,
            0,
            0,
            0});
            this.nudIdTo.Name = "nudIdTo";
            this.nudIdTo.Size = new System.Drawing.Size(69, 20);
            this.nudIdTo.TabIndex = 19;
            this.nudIdTo.Value = new decimal(new int[] {
            63,
            0,
            0,
            0});
            // 
            // nudIdFrom
            // 
            this.nudIdFrom.Location = new System.Drawing.Point(424, 29);
            this.nudIdFrom.Maximum = new decimal(new int[] {
            63,
            0,
            0,
            0});
            this.nudIdFrom.Name = "nudIdFrom";
            this.nudIdFrom.Size = new System.Drawing.Size(69, 20);
            this.nudIdFrom.TabIndex = 17;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(497, 10);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(28, 23);
            this.label8.TabIndex = 18;
            this.label8.Text = "To:";
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(422, 12);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(38, 23);
            this.label7.TabIndex = 16;
            this.label7.Text = "From:";
            // 
            // btnFilterApply
            // 
            this.btnFilterApply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFilterApply.Enabled = false;
            this.btnFilterApply.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnFilterApply.Location = new System.Drawing.Point(592, 26);
            this.btnFilterApply.Name = "btnFilterApply";
            this.btnFilterApply.Size = new System.Drawing.Size(65, 23);
            this.btnFilterApply.TabIndex = 20;
            this.btnFilterApply.Text = "Apply";
            this.btnFilterApply.UseVisualStyleBackColor = true;
            this.btnFilterApply.Click += new System.EventHandler(this.btnFilterApply_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox4.Controls.Add(this.btnReset);
            this.groupBox4.Controls.Add(this.btnStatus);
            this.groupBox4.Controls.Add(this.btnGetVersions);
            this.groupBox4.Controls.Add(this.lbxInfo);
            this.groupBox4.Controls.Add(this.btnInfoClear);
            this.groupBox4.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.groupBox4.Location = new System.Drawing.Point(8, 397);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(734, 135);
            this.groupBox4.TabIndex = 51;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Information";
            // 
            // btnReset
            // 
            this.btnReset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnReset.Enabled = false;
            this.btnReset.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnReset.Location = new System.Drawing.Point(663, 48);
            this.btnReset.Name = "btnReset";
            this.btnReset.Size = new System.Drawing.Size(65, 23);
            this.btnReset.TabIndex = 56;
            this.btnReset.Text = "Reset";
            this.btnReset.UseVisualStyleBackColor = true;
            this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // btnStatus
            // 
            this.btnStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnStatus.Enabled = false;
            this.btnStatus.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnStatus.Location = new System.Drawing.Point(592, 48);
            this.btnStatus.Name = "btnStatus";
            this.btnStatus.Size = new System.Drawing.Size(65, 23);
            this.btnStatus.TabIndex = 55;
            this.btnStatus.Text = "Status";
            this.btnStatus.UseVisualStyleBackColor = true;
            this.btnStatus.Click += new System.EventHandler(this.btnStatus_Click);
            // 
            // btnGetVersions
            // 
            this.btnGetVersions.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnGetVersions.Enabled = false;
            this.btnGetVersions.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnGetVersions.Location = new System.Drawing.Point(592, 19);
            this.btnGetVersions.Name = "btnGetVersions";
            this.btnGetVersions.Size = new System.Drawing.Size(65, 23);
            this.btnGetVersions.TabIndex = 53;
            this.btnGetVersions.Text = "Versions";
            this.btnGetVersions.UseVisualStyleBackColor = true;
            this.btnGetVersions.Click += new System.EventHandler(this.btnGetVersions_Click);
            // 
            // lbxInfo
            // 
            this.lbxInfo.FormattingEnabled = true;
            this.lbxInfo.Items.AddRange(new object[] {
            "Select a Hardware and a configuration for it. Then click \"Initialize\" button"});
            this.lbxInfo.Location = new System.Drawing.Point(10, 19);
            this.lbxInfo.Name = "lbxInfo";
            this.lbxInfo.ScrollAlwaysVisible = true;
            this.lbxInfo.Size = new System.Drawing.Size(558, 108);
            this.lbxInfo.TabIndex = 52;
            // 
            // btnInfoClear
            // 
            this.btnInfoClear.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnInfoClear.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnInfoClear.Location = new System.Drawing.Point(663, 19);
            this.btnInfoClear.Name = "btnInfoClear";
            this.btnInfoClear.Size = new System.Drawing.Size(65, 23);
            this.btnInfoClear.TabIndex = 54;
            this.btnInfoClear.Text = "Clear";
            this.btnInfoClear.UseVisualStyleBackColor = true;
            this.btnInfoClear.Click += new System.EventHandler(this.btnInfoClear_Click);
            // 
            // groupBox5
            // 
            this.groupBox5.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox5.Controls.Add(this.chbShowPeriod);
            this.groupBox5.Controls.Add(this.rdbManual);
            this.groupBox5.Controls.Add(this.lstMessages);
            this.groupBox5.Controls.Add(this.btnMsgClear);
            this.groupBox5.Controls.Add(this.rdbTimer);
            this.groupBox5.Controls.Add(this.btnRead);
            this.groupBox5.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.groupBox5.Location = new System.Drawing.Point(8, 143);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(734, 140);
            this.groupBox5.TabIndex = 22;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = " Messages Reading ";
            // 
            // chbShowPeriod
            // 
            this.chbShowPeriod.AutoSize = true;
            this.chbShowPeriod.Checked = true;
            this.chbShowPeriod.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chbShowPeriod.Location = new System.Drawing.Point(227, 15);
            this.chbShowPeriod.Name = "chbShowPeriod";
            this.chbShowPeriod.Size = new System.Drawing.Size(123, 17);
            this.chbShowPeriod.TabIndex = 26;
            this.chbShowPeriod.Text = "Timestamp as period";
            this.chbShowPeriod.UseVisualStyleBackColor = true;
            this.chbShowPeriod.CheckedChanged += new System.EventHandler(this.chbShowPeriod_CheckedChanged);
            // 
            // rdbManual
            // 
            this.rdbManual.AutoSize = true;
            this.rdbManual.Location = new System.Drawing.Point(129, 14);
            this.rdbManual.Name = "rdbManual";
            this.rdbManual.Size = new System.Drawing.Size(89, 17);
            this.rdbManual.TabIndex = 25;
            this.rdbManual.Text = "Manual Read";
            this.rdbManual.UseVisualStyleBackColor = true;
            this.rdbManual.CheckedChanged += new System.EventHandler(this.rdbTimer_CheckedChanged);
            // 
            // lstMessages
            // 
            this.lstMessages.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clhID,
            this.clhLength,
            this.clhData,
            this.clhCount,
            this.clhRcvTime,
            this.clhDirection,
            this.clhErrors,
            this.clhCST,
            this.clhCRC});
            this.lstMessages.FullRowSelect = true;
            this.lstMessages.Location = new System.Drawing.Point(8, 37);
            this.lstMessages.MultiSelect = false;
            this.lstMessages.Name = "lstMessages";
            this.lstMessages.Size = new System.Drawing.Size(560, 96);
            this.lstMessages.TabIndex = 27;
            this.lstMessages.UseCompatibleStateImageBehavior = false;
            this.lstMessages.View = System.Windows.Forms.View.Details;
            // 
            // clhID
            // 
            this.clhID.Text = "ID";
            this.clhID.Width = 90;
            // 
            // clhLength
            // 
            this.clhLength.Text = "Length";
            this.clhLength.Width = 50;
            // 
            // clhData
            // 
            this.clhData.Text = "Data";
            this.clhData.Width = 160;
            // 
            // clhCount
            // 
            this.clhCount.Text = "Count";
            this.clhCount.Width = 49;
            // 
            // clhRcvTime
            // 
            this.clhRcvTime.Text = "Rcv Time";
            this.clhRcvTime.Width = 97;
            // 
            // clhDirection
            // 
            this.clhDirection.Text = "Direction";
            // 
            // clhErrors
            // 
            this.clhErrors.Text = "Errors";
            // 
            // clhCST
            // 
            this.clhCST.Text = "CST";
            // 
            // clhCRC
            // 
            this.clhCRC.Text = "Checksum";
            // 
            // btnMsgClear
            // 
            this.btnMsgClear.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnMsgClear.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnMsgClear.Location = new System.Drawing.Point(663, 37);
            this.btnMsgClear.Name = "btnMsgClear";
            this.btnMsgClear.Size = new System.Drawing.Size(65, 23);
            this.btnMsgClear.TabIndex = 29;
            this.btnMsgClear.Text = "Clear";
            this.btnMsgClear.UseVisualStyleBackColor = true;
            this.btnMsgClear.Click += new System.EventHandler(this.btnMsgClear_Click);
            // 
            // rdbTimer
            // 
            this.rdbTimer.AutoSize = true;
            this.rdbTimer.Checked = true;
            this.rdbTimer.Location = new System.Drawing.Point(6, 14);
            this.rdbTimer.Name = "rdbTimer";
            this.rdbTimer.Size = new System.Drawing.Size(117, 17);
            this.rdbTimer.TabIndex = 23;
            this.rdbTimer.TabStop = true;
            this.rdbTimer.Text = "Read using a Timer";
            this.rdbTimer.UseVisualStyleBackColor = true;
            this.rdbTimer.CheckedChanged += new System.EventHandler(this.rdbTimer_CheckedChanged);
            // 
            // btnRead
            // 
            this.btnRead.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRead.Enabled = false;
            this.btnRead.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnRead.Location = new System.Drawing.Point(592, 37);
            this.btnRead.Name = "btnRead";
            this.btnRead.Size = new System.Drawing.Size(65, 23);
            this.btnRead.TabIndex = 28;
            this.btnRead.Text = "Read";
            this.btnRead.UseVisualStyleBackColor = true;
            this.btnRead.Click += new System.EventHandler(this.btnRead_Click);
            // 
            // tmrRead
            // 
            this.tmrRead.Interval = 50;
            this.tmrRead.Tick += new System.EventHandler(this.tmrRead_Tick);
            // 
            // tmrDisplay
            // 
            this.tmrDisplay.Tick += new System.EventHandler(this.tmrDisplay_Tick);
            // 
            // btnWrite
            // 
            this.btnWrite.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnWrite.Cursor = System.Windows.Forms.Cursors.Default;
            this.btnWrite.Enabled = false;
            this.btnWrite.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnWrite.Location = new System.Drawing.Point(663, 29);
            this.btnWrite.Name = "btnWrite";
            this.btnWrite.Size = new System.Drawing.Size(65, 23);
            this.btnWrite.TabIndex = 50;
            this.btnWrite.Text = "Write";
            this.btnWrite.Click += new System.EventHandler(this.btnWrite_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.cbbID);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.cbbCST);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.cbbDirection);
            this.groupBox2.Controls.Add(this.nudLength);
            this.groupBox2.Controls.Add(this.txtData7);
            this.groupBox2.Controls.Add(this.txtData6);
            this.groupBox2.Controls.Add(this.txtData5);
            this.groupBox2.Controls.Add(this.txtData4);
            this.groupBox2.Controls.Add(this.txtData3);
            this.groupBox2.Controls.Add(this.txtData2);
            this.groupBox2.Controls.Add(this.txtData1);
            this.groupBox2.Controls.Add(this.txtData0);
            this.groupBox2.Controls.Add(this.txtID);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.btnWrite);
            this.groupBox2.Location = new System.Drawing.Point(8, 289);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(733, 102);
            this.groupBox2.TabIndex = 30;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Write Messages";
            // 
            // cbbID
            // 
            this.cbbID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbID.FormattingEnabled = true;
            this.cbbID.Location = new System.Drawing.Point(10, 32);
            this.cbbID.Name = "cbbID";
            this.cbbID.Size = new System.Drawing.Size(98, 21);
            this.cbbID.TabIndex = 32;
            this.cbbID.SelectedIndexChanged += new System.EventHandler(this.cbbID_SelectedIndexChanged);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(7, 16);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(49, 13);
            this.label11.TabIndex = 31;
            this.label11.Text = "ID (Hex):";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(396, 16);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(87, 13);
            this.label10.TabIndex = 37;
            this.label10.Text = "Checksum Type:";
            // 
            // cbbCST
            // 
            this.cbbCST.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbCST.Enabled = false;
            this.cbbCST.FormattingEnabled = true;
            this.cbbCST.Items.AddRange(new object[] {
            "Classic",
            "Enhanced",
            "Automatic"});
            this.cbbCST.Location = new System.Drawing.Point(394, 32);
            this.cbbCST.Name = "cbbCST";
            this.cbbCST.Size = new System.Drawing.Size(121, 21);
            this.cbbCST.TabIndex = 38;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(237, 16);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(52, 13);
            this.label9.TabIndex = 35;
            this.label9.Text = "Direction:";
            // 
            // cbbDirection
            // 
            this.cbbDirection.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbDirection.Enabled = false;
            this.cbbDirection.FormattingEnabled = true;
            this.cbbDirection.Items.AddRange(new object[] {
            "Publisher",
            "Subscriber"});
            this.cbbDirection.Location = new System.Drawing.Point(240, 31);
            this.cbbDirection.Name = "cbbDirection";
            this.cbbDirection.Size = new System.Drawing.Size(142, 21);
            this.cbbDirection.TabIndex = 36;
            // 
            // nudLength
            // 
            this.nudLength.Enabled = false;
            this.nudLength.Location = new System.Drawing.Point(10, 71);
            this.nudLength.Maximum = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.nudLength.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudLength.Name = "nudLength";
            this.nudLength.Size = new System.Drawing.Size(98, 20);
            this.nudLength.TabIndex = 40;
            this.nudLength.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudLength.ValueChanged += new System.EventHandler(this.nudLength_ValueChanged);
            // 
            // txtData7
            // 
            this.txtData7.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData7.Enabled = false;
            this.txtData7.Location = new System.Drawing.Point(392, 71);
            this.txtData7.MaxLength = 2;
            this.txtData7.Name = "txtData7";
            this.txtData7.Size = new System.Drawing.Size(32, 20);
            this.txtData7.TabIndex = 49;
            this.txtData7.Text = "00";
            this.txtData7.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData7.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData7.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtData6
            // 
            this.txtData6.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData6.Enabled = false;
            this.txtData6.Location = new System.Drawing.Point(354, 71);
            this.txtData6.MaxLength = 2;
            this.txtData6.Name = "txtData6";
            this.txtData6.Size = new System.Drawing.Size(32, 20);
            this.txtData6.TabIndex = 48;
            this.txtData6.Text = "00";
            this.txtData6.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData6.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData6.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtData5
            // 
            this.txtData5.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData5.Enabled = false;
            this.txtData5.Location = new System.Drawing.Point(316, 71);
            this.txtData5.MaxLength = 2;
            this.txtData5.Name = "txtData5";
            this.txtData5.Size = new System.Drawing.Size(32, 20);
            this.txtData5.TabIndex = 47;
            this.txtData5.Text = "00";
            this.txtData5.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData5.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData5.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtData4
            // 
            this.txtData4.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData4.Enabled = false;
            this.txtData4.Location = new System.Drawing.Point(278, 71);
            this.txtData4.MaxLength = 2;
            this.txtData4.Name = "txtData4";
            this.txtData4.Size = new System.Drawing.Size(32, 20);
            this.txtData4.TabIndex = 46;
            this.txtData4.Text = "00";
            this.txtData4.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData4.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData4.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtData3
            // 
            this.txtData3.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData3.Enabled = false;
            this.txtData3.Location = new System.Drawing.Point(240, 71);
            this.txtData3.MaxLength = 2;
            this.txtData3.Name = "txtData3";
            this.txtData3.Size = new System.Drawing.Size(32, 20);
            this.txtData3.TabIndex = 45;
            this.txtData3.Text = "00";
            this.txtData3.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData3.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtData2
            // 
            this.txtData2.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData2.Enabled = false;
            this.txtData2.Location = new System.Drawing.Point(202, 71);
            this.txtData2.MaxLength = 2;
            this.txtData2.Name = "txtData2";
            this.txtData2.Size = new System.Drawing.Size(32, 20);
            this.txtData2.TabIndex = 44;
            this.txtData2.Text = "00";
            this.txtData2.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData2.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtData1
            // 
            this.txtData1.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData1.Enabled = false;
            this.txtData1.Location = new System.Drawing.Point(164, 71);
            this.txtData1.MaxLength = 2;
            this.txtData1.Name = "txtData1";
            this.txtData1.Size = new System.Drawing.Size(32, 20);
            this.txtData1.TabIndex = 43;
            this.txtData1.Text = "00";
            this.txtData1.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData1.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtData0
            // 
            this.txtData0.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtData0.Enabled = false;
            this.txtData0.Location = new System.Drawing.Point(126, 71);
            this.txtData0.MaxLength = 2;
            this.txtData0.Name = "txtData0";
            this.txtData0.Size = new System.Drawing.Size(32, 20);
            this.txtData0.TabIndex = 42;
            this.txtData0.Text = "00";
            this.txtData0.TextChanged += new System.EventHandler(this.txtData0_TextChanged);
            this.txtData0.Leave += new System.EventHandler(this.txtData0_Leave);
            this.txtData0.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // txtID
            // 
            this.txtID.Enabled = false;
            this.txtID.Location = new System.Drawing.Point(126, 32);
            this.txtID.MaxLength = 2;
            this.txtID.Name = "txtID";
            this.txtID.Size = new System.Drawing.Size(100, 20);
            this.txtID.TabIndex = 34;
            this.txtID.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtHexFormat_KeyPress);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(123, 55);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(60, 13);
            this.label6.TabIndex = 41;
            this.label6.Text = "Data (1..8):";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 55);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(43, 13);
            this.label5.TabIndex = 39;
            this.label5.Text = "Length:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(123, 16);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(56, 13);
            this.label4.TabIndex = 33;
            this.label4.Text = "PID (Hex):";
            // 
            // Main_Wnd
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(754, 543);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Main_Wnd";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "PLIN-API Example";
            this.Shown += new System.EventHandler(this.Form1_Shown);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nudIdTo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudIdFrom)).EndInit();
            this.groupBox4.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudLength)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnHwRefresh;
        private System.Windows.Forms.ComboBox cbbChannel;
        private System.Windows.Forms.ComboBox cbbHwMode;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cbbBaudrates;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnInit;
        private System.Windows.Forms.Button btnRelease;
        private System.Windows.Forms.Button btnIdent;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnFilterQuery;
        private System.Windows.Forms.NumericUpDown nudIdTo;
        private System.Windows.Forms.NumericUpDown nudIdFrom;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button btnFilterApply;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button btnReset;
        private System.Windows.Forms.Button btnStatus;
        private System.Windows.Forms.Button btnGetVersions;
        private System.Windows.Forms.ListBox lbxInfo;
        private System.Windows.Forms.Button btnInfoClear;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.CheckBox chbShowPeriod;
        private System.Windows.Forms.RadioButton rdbManual;
        private System.Windows.Forms.ListView lstMessages;
        private System.Windows.Forms.ColumnHeader clhID;
        private System.Windows.Forms.ColumnHeader clhLength;
        private System.Windows.Forms.ColumnHeader clhData;
        private System.Windows.Forms.ColumnHeader clhCount;
        private System.Windows.Forms.ColumnHeader clhRcvTime;
        private System.Windows.Forms.Button btnMsgClear;
        private System.Windows.Forms.RadioButton rdbTimer;
        private System.Windows.Forms.Button btnRead;
        private System.Windows.Forms.Timer tmrRead;
        private System.Windows.Forms.ColumnHeader clhDirection;
        private System.Windows.Forms.ColumnHeader clhErrors;
        private System.Windows.Forms.ColumnHeader clhCST;
        private System.Windows.Forms.ColumnHeader clhCRC;
        private System.Windows.Forms.Timer tmrDisplay;
        private System.Windows.Forms.RadioButton rdbFilterOpen;
        private System.Windows.Forms.RadioButton rdbFilterCustom;
        private System.Windows.Forms.RadioButton rdbFilterClose;
        private System.Windows.Forms.Button btnWrite;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.NumericUpDown nudLength;
        private System.Windows.Forms.TextBox txtData7;
        private System.Windows.Forms.TextBox txtData6;
        private System.Windows.Forms.TextBox txtData5;
        private System.Windows.Forms.TextBox txtData4;
        private System.Windows.Forms.TextBox txtData3;
        private System.Windows.Forms.TextBox txtData2;
        private System.Windows.Forms.TextBox txtData1;
        private System.Windows.Forms.TextBox txtData0;
        private System.Windows.Forms.TextBox txtID;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ComboBox cbbDirection;
        private System.Windows.Forms.ComboBox cbbCST;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox cbbID;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Button btnConfigure;
    }
}

