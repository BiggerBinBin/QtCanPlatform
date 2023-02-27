namespace PLIN_API_Example
{
    partial class Frame_Dlg
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
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem(new string[] {
            "0x01",
            "2",
            "SubscriberAutoLength",
            "8",
            "Enhanced"}, -1);
            System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem("");
            System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem("");
            this.lvGFT = new System.Windows.Forms.ListView();
            this.ColGFTID = new System.Windows.Forms.ColumnHeader();
            this.ColGFTPID = new System.Windows.Forms.ColumnHeader();
            this.ColGFTDirect = new System.Windows.Forms.ColumnHeader();
            this.ColGFTLen = new System.Windows.Forms.ColumnHeader();
            this.ColGFTChecksum = new System.Windows.Forms.ColumnHeader();
            this.label1 = new System.Windows.Forms.Label();
            this.pgGFTDef = new System.Windows.Forms.PropertyGrid();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // lvGFT
            // 
            this.lvGFT.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.lvGFT.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ColGFTID,
            this.ColGFTPID,
            this.ColGFTDirect,
            this.ColGFTLen,
            this.ColGFTChecksum});
            this.lvGFT.FullRowSelect = true;
            this.lvGFT.HideSelection = false;
            this.lvGFT.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem4,
            listViewItem5,
            listViewItem6});
            this.lvGFT.Location = new System.Drawing.Point(12, 25);
            this.lvGFT.Name = "lvGFT";
            this.lvGFT.ShowItemToolTips = true;
            this.lvGFT.Size = new System.Drawing.Size(338, 219);
            this.lvGFT.TabIndex = 1;
            this.lvGFT.UseCompatibleStateImageBehavior = false;
            this.lvGFT.View = System.Windows.Forms.View.Details;
            this.lvGFT.SelectedIndexChanged += new System.EventHandler(this.lvGFT_SelectedIndexChanged);
            this.lvGFT.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvGFT_KeyDown);
            // 
            // ColGFTID
            // 
            this.ColGFTID.Text = "ID";
            this.ColGFTID.Width = 41;
            // 
            // ColGFTPID
            // 
            this.ColGFTPID.Text = "PID";
            this.ColGFTPID.Width = 38;
            // 
            // ColGFTDirect
            // 
            this.ColGFTDirect.Text = "Direction";
            this.ColGFTDirect.Width = 124;
            // 
            // ColGFTLen
            // 
            this.ColGFTLen.Text = "Length";
            this.ColGFTLen.Width = 50;
            // 
            // ColGFTChecksum
            // 
            this.ColGFTChecksum.Text = "CST";
            this.ColGFTChecksum.Width = 61;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(102, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Global Frame Table:";
            // 
            // pgGFTDef
            // 
            this.pgGFTDef.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pgGFTDef.Location = new System.Drawing.Point(354, 25);
            this.pgGFTDef.Name = "pgGFTDef";
            this.pgGFTDef.Size = new System.Drawing.Size(256, 219);
            this.pgGFTDef.TabIndex = 3;
            this.pgGFTDef.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.pgGFTDef_PropertyValueChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(356, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(57, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Properties:";
            // 
            // Frame_Dlg
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(622, 256);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.pgGFTDef);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lvGFT);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(638, 295);
            this.Name = "Frame_Dlg";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Global Frame Table";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView lvGFT;
        private System.Windows.Forms.ColumnHeader ColGFTID;
        private System.Windows.Forms.ColumnHeader ColGFTPID;
        private System.Windows.Forms.ColumnHeader ColGFTDirect;
        private System.Windows.Forms.ColumnHeader ColGFTLen;
        private System.Windows.Forms.ColumnHeader ColGFTChecksum;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PropertyGrid pgGFTDef;
        private System.Windows.Forms.Label label2;
    }
}