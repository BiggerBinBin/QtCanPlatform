using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using PLIN_API_Example.Properties;


namespace PLIN_API_Example
{
    public partial class Main_Wnd : Form
    {
        #region Structures

        /// <summary>
        /// Message Status structure used to show LIN Messages
        /// in a ListView
        /// </summary>
        private class MessageStatus
        {
            /// <summary>
            /// The received LIN message
            /// </summary>
            private Peak.Lin.TLINRcvMsg m_Msg;
            /// <summary>
            /// Timestamp of a previously received message
            /// </summary>
            private ulong m_oldTimeStamp;
            /// <summary>
            /// index of the message in the ListView component
            /// </summary>
            private int m_iIndex;
            /// <summary>
            /// Number of LIN message received with the same frame ID
            /// </summary>
            private int m_Count;
            /// <summary>
            /// Defines if the timestamp is displayed as a period
            /// </summary>
            private bool m_bShowPeriod;
            /// <summary>
            /// Defines if the message has been modified and its display needs to be updated
            /// </summary>
            private bool m_bWasChanged;

            /// <summary>
            /// Creates a new MessageStatus object
            /// </summary>
            /// <param name="linMsg">received LIN message</param>
            /// <param name="listIndex">index of the message in the ListView</param>
            public MessageStatus(Peak.Lin.TLINRcvMsg linMsg, int listIndex)
            {
                m_Msg = linMsg;
                m_oldTimeStamp = linMsg.TimeStamp;
                m_iIndex = listIndex;
                m_Count = 1;
                m_bShowPeriod = true;
                m_bWasChanged = false;
            }
            /// <summary>
            /// Updates an existing MessageStatus with a new LIN message
            /// </summary>
            /// <param name="linMsg">LIN message to update</param>
            public void Update(Peak.Lin.TLINRcvMsg linMsg)
            {
                m_oldTimeStamp = m_Msg.TimeStamp;
                m_Msg = linMsg;
                m_Count += 1;
                m_bWasChanged = true;
            }

            #region Getters and Setters
            /// <summary>
            /// The received LIN message
            /// </summary>
            public Peak.Lin.TLINRcvMsg LINMsg
            {
                get { return m_Msg; }
            }
            /// <summary>
            /// Index of the message in the ListView
            /// </summary>
            public int Position
            {
                get { return m_iIndex; }
            }
            /// <summary>
            /// Direction of the LIN message as a string
            /// </summary>
            public string DirectionString
            {
                get { return GetDirectionString(); }
            }
            /// <summary>
            /// Checksum type of the LIN message as a string
            /// </summary>
            public string CSTString
            {
                get { return GetCSTString(); }
            }
            /// <summary>
            /// Checksum of the LIN message as a string
            /// </summary>
            public string ChecksumString
            {
                get { return string.Format("{0:X}h", m_Msg.Checksum); }
            }
            /// <summary>
            /// Error field of the LIN message as a string
            /// </summary>
            public string ErrorString
            {
                get { return GetErrorString(); }
            }
            /// <summary>
            /// Protected ID of the LIN message as a string
            /// </summary>
            public string PIdString
            {
                get { return getProtectedIdString(); }
            }
            /// <summary>
            /// Data fields of the LIN message as a string
            /// </summary>
            public string DataString
            {
                get { return GetDataString(); }
            }
            /// <summary>
            /// Number of LIN messages received with the same frame ID
            /// </summary>
            public int Count
            {
                get { return m_Count; }
            }
            /// <summary>
            /// States wether the timestamp is displayed as a period or not
            /// </summary>
            public bool ShowingPeriod
            {
                get { return m_bShowPeriod; }
                set
                {
                    if (m_bShowPeriod ^ value)
                    {
                        m_bShowPeriod = value;
                        m_bWasChanged = true;
                    }
                }
            }
            /// <summary>
            /// Defines if the LIN message has been modified
            /// </summary>
            public bool MarkedAsUpdated
            {
                get { return m_bWasChanged; }
                set { m_bWasChanged = value; }
            }
            /// <summary>
            /// The timestamp or period of the LIN message
            /// </summary>
            public string TimeString
            {
                get { return GetTimeString(); }
            }

            #endregion

            #region private getters

            /// <summary>
            /// Returns the Protected ID as a string
            /// </summary>
            /// <returns>formatted protected ID</returns>
            private string getProtectedIdString()
            {
                return string.Format("{0:X3}h", m_Msg.FrameId);
            }
            /// <summary>
            /// Returns the Data array as a string
            /// </summary>
            /// <returns>the formatted data array</returns>
            private string GetDataString()
            {
                string strTemp;

                strTemp = "";
                for (int i = 0; i < m_Msg.Length; i++)
                {
                    strTemp += string.Format("{0:X2} ", m_Msg.Data[i]);
                }
                return strTemp;
            }
            /// <summary>
            /// Returns the timestamp or the period of the frame
            /// </summary>
            /// <returns>timestamp or period in milliseconds</returns>
            private string GetTimeString()
            {
                ulong time;

                time = m_Msg.TimeStamp;
                if (m_bShowPeriod)
                    time = (time - m_oldTimeStamp) / 1000;
                return time.ToString();
            }
            /// <summary>
            /// Returns the direction as a formatted string
            /// </summary>
            /// <returns>the formatted direction</returns>
            private string GetDirectionString()
            {
                switch (m_Msg.Direction)
                {
                    case Peak.Lin.TLINDirection.dirDisabled:
                        return Resources.SLinDirectionDisabled;
                    case Peak.Lin.TLINDirection.dirPublisher:
                        return Resources.SLinDirectionPublisher;
                    case Peak.Lin.TLINDirection.dirSubscriber:
                        return Resources.SLinDirectionSubscriber;
                    case Peak.Lin.TLINDirection.dirSubscriberAutoLength:
                        return Resources.SLinDirectionAuto;
                }
                return "";
            }
            /// <summary>
            /// Returns the Checksum type as a string
            /// </summary>
            /// <returns>formatted checksum type</returns>
            private string GetCSTString()
            {
                switch (m_Msg.ChecksumType)
                {
                    case Peak.Lin.TLINChecksumType.cstAuto:
                        return Resources.SLinCSTAuto;
                    case Peak.Lin.TLINChecksumType.cstClassic:
                        return Resources.SLinCSTClassic;
                    case Peak.Lin.TLINChecksumType.cstEnhanced:
                        return Resources.SLinCSTEnhanced;
                    case Peak.Lin.TLINChecksumType.cstCustom:
                        return Resources.SLinCSTCustom;
                }
                return "";
            }
            /// <summary>
            /// Returns the Error field of the LIN message as a string
            /// </summary>
            /// <returns>formatted Error field of the LIN message</returns>
            private string GetErrorString()
            {
                if (m_Msg.ErrorFlags == 0)
                    return "O.k.";

                return m_Msg.ErrorFlags.ToString();
            }

            #endregion
        }

        #endregion

        #region Attributes

        #region LIN
        /// <summary>
        /// Client handle
        /// </summary>
        private byte m_hClient;
        /// <summary>
        /// Hardware handle
        /// </summary>
        private ushort m_hHw;
        /// <summary>
        /// LIN Hardware Modus (Master/Slave)
        /// </summary>
        private Peak.Lin.TLINHardwareMode m_HwMode;
        /// <summary>
        /// Client filter mask
        /// </summary>
        private ulong m_lMask;
        /// <summary>
        /// Baudrate Index of Hardware
        /// </summary>
        private ushort m_wBaudrate;
        /// <summary>
        /// Last LIN error
        /// </summary>
        private Peak.Lin.TLINError m_LastLINErr;
        /// <summary>
        /// Constant value that indicate the mask of the client filter (64bit)
        /// </summary>
        private const ulong FRAME_FILTER_MASK = 0xFFFFFFFFFFFFFFFF;

        #endregion

        /// <summary>
        /// Global Frame Table object to hold all frames definitions.
        /// </summary>
        private CGlobalFrameTable m_pGFT;
        /// <summary>
        /// Stores the status of received messages for its display
        /// </summary>
        private System.Collections.ArrayList m_LastMsgsList;

        #endregion

        #region Form event Handlers

        /// <summary>
        /// Constructor of the main window
        /// </summary>
        public Main_Wnd()
        {
            InitializeComponent();

            // Create the Global Frame Table and add property change handler on its items
            m_pGFT = new CGlobalFrameTable(this);
            m_pGFT.OnPropertyChange += new PropertyChangeEventHandler(PropertyChange);
            // Populates FrameID combobox with global frame IDs
            UpdateFrameIds();
            // Populates Direction combobox
            cbbDirection.Items.Clear();
            cbbDirection.Items.Add(new ComboBoxItem(Resources.SLinDirectionDisabled, Peak.Lin.TLINDirection.dirDisabled));
            cbbDirection.Items.Add(new ComboBoxItem(Resources.SLinDirectionPublisher, Peak.Lin.TLINDirection.dirPublisher));
            cbbDirection.Items.Add(new ComboBoxItem(Resources.SLinDirectionSubscriber, Peak.Lin.TLINDirection.dirSubscriber));
            cbbDirection.Items.Add(new ComboBoxItem(Resources.SLinDirectionAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength));
            // Populates ChecksumType combobox
            cbbCST.Items.Clear(); 
            cbbCST.Items.Add(new ComboBoxItem(Resources.SLinCSTAuto, Peak.Lin.TLINChecksumType.cstAuto));
            cbbCST.Items.Add(new ComboBoxItem(Resources.SLinCSTClassic, Peak.Lin.TLINChecksumType.cstClassic));
            cbbCST.Items.Add(new ComboBoxItem(Resources.SLinCSTEnhanced, Peak.Lin.TLINChecksumType.cstEnhanced));
            cbbCST.Items.Add(new ComboBoxItem(Resources.SLinCSTCustom, Peak.Lin.TLINChecksumType.cstCustom));

            // Initialize the LIN attributes
            m_hClient = 0;
            m_hHw = 0;
            m_lMask = FRAME_FILTER_MASK;
            m_HwMode = Peak.Lin.TLINHardwareMode.modSlave;
            m_wBaudrate = 19200;

            // Creates the list for received messages
            m_LastMsgsList = new System.Collections.ArrayList();

            // Register this application with LIN as client.
            m_LastLINErr = Peak.Lin.PLinApi.RegisterClient(Resources.SPLinClientName, Handle, out m_hClient);
        }

        /// <summary>
        /// Occurs before the form is shown.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">the event data.</param>
        private void Form1_Shown(object sender, EventArgs e)
        {
            RefreshHardware();
            rdbFilter_CheckedChanged(this, new EventArgs());
            cbbDirection.SelectedIndex = 0;
            cbbCST.SelectedIndex = 1;
        }

        /// <summary>
        /// Occurs before the form is closed.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">A FormClosingEventArgs that contains the event data.</param>
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // disconnect from LIN if necessary
            if (m_hClient != 0)
            {
                DoLinDisconnect();
                m_hHw = 0;
                // Unregister the application
                Peak.Lin.PLinApi.RemoveClient(m_hClient);
                m_hClient = 0;
            }
        }

        #endregion

        #region Button event-handlers

        #region Connection buttons

        private void btnHwRefresh_Click(object sender, EventArgs e)
        {
            RefreshHardware();
        }

        private void btnIdent_Click(object sender, EventArgs e)
        {
            // Get hardware device from the selected combobox item
            ushort lwHw = (ushort)((ComboBoxItem)cbbChannel.SelectedItem).Value;
            if (lwHw != 0)
            {
                // makes the corresponding PCAN-USB-Pro's LED blink
                Peak.Lin.PLinApi.IdentifyHardware(lwHw);
            }
        }

        private void btnInit_Click(object sender, EventArgs e)
        {
            // Do the connection
            if (DoLinConnect())
            {
                // Sets the connection status of the main-form
                SetConnectionStatus(true);
            }
        }

        private void btnRelease_Click(object sender, EventArgs e)
        {
            // Releases the current connection
            if (DoLinDisconnect())
            {
                // stop reading timer
                tmrRead.Enabled = false;
                // Sets the connection status of the main-form
                SetConnectionStatus(false);
            }
        }

        #endregion

        #region Message filtering components

        private void btnConfigure_Click(object sender, EventArgs e)
        {
            // Open the "Global Frame Table" Dialog
            Frame_Dlg dlg = new Frame_Dlg(m_pGFT);
            dlg.ShowDialog();
            // Output filter information (as changes to Global Frame Table items modify it)
            btnFilterQuery_Click(this, new EventArgs());
            // Update the available frame ids (i.e. the IDs combobox in the "write message" UI group)
            UpdateFrameIds();
        }

        private void rdbFilter_CheckedChanged(object sender, EventArgs e)
        {
            if (rdbFilterCustom.Checked)
            {
                nudIdFrom.Enabled = true;
                nudIdTo.Enabled = true;
            }
            else
            {
                nudIdFrom.Enabled = false;
                nudIdTo.Enabled = false;
            }
        }

        private void btnFilterApply_Click(object sender, EventArgs e)
        {
            if (m_hHw != 0)
            {
                ulong lMask;
                // Sets the according to the selected filter
                if (rdbFilterOpen.Checked)
                {
                    // set no filtering : all frames are received
                    lMask = FRAME_FILTER_MASK;
                    m_LastLINErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, lMask);
                    if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                    {
                        m_lMask = lMask;
                        IncludeTextMessage(string.Format("The filter was successfully opened."));
                    }
                }
                else if (rdbFilterClose.Checked)
                {
                    // filters all : all frames are denied
                    lMask = 0x0;
                    m_LastLINErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, lMask);
                    if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                    {
                        m_lMask = lMask;
                        IncludeTextMessage(string.Format("The filter was successfully closed."));
                    }
                }
                else
                {
                    // custom filtering (i.e. rdbFilterCustom.Checked) : the frames for the given range will be opened
                    m_LastLINErr = Peak.Lin.PLinApi.RegisterFrameId(m_hClient, m_hHw, (byte)nudIdFrom.Value, (byte)nudIdTo.Value);
                    if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                    {
                        IncludeTextMessage(string.Format("The filter was customized. IDs from {0} to {1} will be received", nudIdFrom.Text, nudIdTo.Text));
                        m_LastLINErr = Peak.Lin.PLinApi.GetClientFilter(m_hClient, m_hHw, out lMask);
                        if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                            m_lMask = lMask;
                    }

                }
                // If success, an information message is written, if it is not, an error message is shown
                if (m_LastLINErr != Peak.Lin.TLINError.errOK)
                    MessageBox.Show(GetFormatedError(m_LastLINErr));
            }
        }

        private void btnFilterQuery_Click(object sender, EventArgs e)
        {
            UInt64 pRcvMask = 0;
            // Retrieves the filter corresponding to the current Client-Hardware pair
            if (m_hHw != 0)
            {
                if (Peak.Lin.PLinApi.GetClientFilter(m_hClient, m_hHw, out pRcvMask) == Peak.Lin.TLINError.errOK)
                    IncludeTextMessage(string.Format("The Status of the filter is {0}.", Convert.ToString((long)pRcvMask, 2).PadLeft(64, '0')));
            }
        }

        #endregion

        #region Information buttons

        private void btnInfoClear_Click(object sender, EventArgs e)
        {
            // The information contained in the Information List-Box is cleared
            lbxInfo.Items.Clear();
        }

        private void btnGetVersions_Click(object sender, EventArgs e)
        {
            Peak.Lin.TLINVersion lpVersion;
            StringBuilder strTemp;
            string[] strArrayVersion;

            lpVersion = new Peak.Lin.TLINVersion();
            strTemp = new StringBuilder(255);

            // We get the vesion of the PLIN API
            //
            m_LastLINErr = Peak.Lin.PLinApi.GetVersion(ref lpVersion);
            if (m_LastLINErr == Peak.Lin.TLINError.errOK)
            {
                IncludeTextMessage(string.Format("API Version: {0}.{1}.{2}.{3}", lpVersion.Major, lpVersion.Minor, lpVersion.Build, lpVersion.Revision));
                // We get the driver version
                //
                m_LastLINErr = Peak.Lin.PLinApi.GetVersionInfo(strTemp, 255);
                if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                {
                    // Because this information contains line control characters (several lines)
                    // we split this also in several entries in the Information List-Box
                    //
                    strArrayVersion = strTemp.ToString().Split(new char[] { '\n' });
                    IncludeTextMessage("Channel/Driver Version: ");
                    for (int i = 0; i < strArrayVersion.Length; i++)
                        IncludeTextMessage("     * " + strArrayVersion[i]);
                }
            }

            // If an error ccurred, a message is shown
            //
            if (m_LastLINErr != Peak.Lin.TLINError.errOK)
                MessageBox.Show(GetFormatedError(m_LastLINErr));
        }

        private void btnStatus_Click(object sender, EventArgs e)
        {
            Peak.Lin.TLINHardwareStatus lStatus;
            // Retrieves the status of the LIN Bus and outputs its state in the information listView
            m_LastLINErr = Peak.Lin.PLinApi.GetStatus(m_hHw, out lStatus);
            if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                switch (lStatus.Status)
                {
                    case Peak.Lin.TLINHardwareState.hwsActive:
                        IncludeTextMessage(Resources.SLINStatusActive);
                        break;
                    case Peak.Lin.TLINHardwareState.hwsAutobaudrate:
                        IncludeTextMessage(Resources.SLINStatusBaudDetect);
                        break;
                    case Peak.Lin.TLINHardwareState.hwsNotInitialized:
                        IncludeTextMessage(Resources.SLINStatusNotInit);
                        break;
                    case Peak.Lin.TLINHardwareState.hwsShortGround:
                        IncludeTextMessage(Resources.SLINStatusShortGround);
                        break;
                    case Peak.Lin.TLINHardwareState.hwsSleep:
                        IncludeTextMessage(Resources.SLINStatusSleep);
                        break;
                }
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            // Flushes the Receive Queue of the Client and resets its counters. 
            //
            m_LastLINErr = Peak.Lin.PLinApi.ResetClient(m_hClient);

            // If it fails, a error message is shown
            //
            if (m_LastLINErr != Peak.Lin.TLINError.errOK)
                MessageBox.Show(GetFormatedError(m_LastLINErr));
            else
            {
                // clears the message list-view
                btnMsgClear_Click(this, new EventArgs());
                IncludeTextMessage("Receive queue and counters successfully reset");
            }
        }

        #endregion

        #region Read message controls

        private void btnRead_Click(object sender, EventArgs e)
        {
            ReadMessages();
        }

        private void btnMsgClear_Click(object sender, EventArgs e)
        {
            // The information contained in the messages List-View
            // is cleared
            //
            m_LastMsgsList.Clear();
            lstMessages.Items.Clear();
        }

        private void rdbTimer_CheckedChanged(object sender, EventArgs e)
        {
            if (!btnRelease.Enabled)
                return;

            // According with the kind of reading, a timer or a button will be enabled
            //
            if (rdbTimer.Checked)
            {
                // Enable Timer
                //
                tmrRead.Enabled = btnRelease.Enabled;
            }
            if (rdbManual.Checked)
            {
                // Disable Timer
                //
                tmrRead.Enabled = false;
            }
            btnRead.Enabled = btnRelease.Enabled && rdbManual.Checked;
        }

        private void chbShowPeriod_CheckedChanged(object sender, EventArgs e)
        {
            // According with the check-value of this checkbox,
            // the recieved time of a messages will be interpreted as 
            // period (time between the two last messages) or as time-stamp
            // (the elapsed time since windows was started)
            //
            foreach (MessageStatus msg in m_LastMsgsList)
                msg.ShowingPeriod = chbShowPeriod.Checked;
        }

        #endregion

        #region Write message controls

        private void txtData0_Leave(object sender, EventArgs e)
        {
            TextBox txtbCurrentTextbox;

            // all the Textbox Data fields are represented with 2 characters.
            // Therefore if the Length of the text is smaller than 2, we add
            // a "0"
            //
            if (sender.GetType().Name == "TextBox")
            {
                txtbCurrentTextbox = (TextBox)sender;
                while (txtbCurrentTextbox.Text.Length != 2)
                    txtbCurrentTextbox.Text = ("0" + txtbCurrentTextbox.Text);
            }
        }

        private void nudLength_ValueChanged(object sender, EventArgs e)
        {
            txtData0.Enabled = false;
            txtData1.Enabled = false;
            txtData2.Enabled = false;
            txtData3.Enabled = false;
            txtData4.Enabled = false;
            txtData5.Enabled = false;
            txtData6.Enabled = false;
            txtData7.Enabled = false;
            // Only Publisher direction allows edition of Data textFields
            if (cbbDirection.SelectedIndex != 1)
                return;
            // Enable the same number of data textBox as the nudLength value
            switch (Convert.ToInt32(nudLength.Value))
            {
                case 1:
                    txtData0.Enabled = true;
                    break;
                case 2:
                    txtData0.Enabled = true;
                    txtData1.Enabled = true;
                    break;
                case 3:
                    txtData0.Enabled = true;
                    txtData1.Enabled = true;
                    txtData2.Enabled = true;
                    break;
                case 4:
                    txtData0.Enabled = true;
                    txtData1.Enabled = true;
                    txtData2.Enabled = true;
                    txtData3.Enabled = true;
                    break;
                case 5:
                    txtData0.Enabled = true;
                    txtData1.Enabled = true;
                    txtData2.Enabled = true;
                    txtData3.Enabled = true;
                    txtData4.Enabled = true;
                    break;
                case 6:
                    txtData0.Enabled = true;
                    txtData1.Enabled = true;
                    txtData2.Enabled = true;
                    txtData3.Enabled = true;
                    txtData4.Enabled = true;
                    txtData5.Enabled = true;
                    break;
                case 7:
                    txtData0.Enabled = true;
                    txtData1.Enabled = true;
                    txtData2.Enabled = true;
                    txtData3.Enabled = true;
                    txtData4.Enabled = true;
                    txtData5.Enabled = true;
                    txtData6.Enabled = true;
                    break;
                case 8:
                    txtData0.Enabled = true;
                    txtData1.Enabled = true;
                    txtData2.Enabled = true;
                    txtData3.Enabled = true;
                    txtData4.Enabled = true;
                    txtData5.Enabled = true;
                    txtData6.Enabled = true;
                    txtData7.Enabled = true;
                    break;
            }
        }

        private void txtData0_TextChanged(object sender, EventArgs e)
        {
            // Automatically change focus between data textBox 
            // when the length of the data equals 2
            if (sender is TextBox)
            {
                TextBox txtbox = (TextBox)sender;
                if (txtbox.TextLength == 2)
                    SelectNextControl(txtbox, true, true, true, true);
            }
        }

        private void btnWrite_Click(object sender, EventArgs e)
        {
            Peak.Lin.TLINMsg pMsg;
            TextBox txtbCurrentTextBox;
            CFrameDefinition lFD;

            // Assert that a frame ID has been selected
            if (txtID.Text.Length == 0)
            {
                txtID.Focus();
                return;
            }
            // Get the CFrameDefinition associated to the selected FrameID
            lFD = (CFrameDefinition)(((ComboBoxItem)cbbID.SelectedItem).Value);
            // Create a new LIN frame message and copy the data.
            pMsg = new Peak.Lin.TLINMsg();
            pMsg.Data = new byte[8];
            pMsg.FrameId = Convert.ToByte(lFD.ProtectedIdAsInt);
            pMsg.Direction = (Peak.Lin.TLINDirection)((ComboBoxItem)cbbDirection.SelectedItem).Value;
            pMsg.ChecksumType = (Peak.Lin.TLINChecksumType)((ComboBoxItem)cbbCST.SelectedItem).Value;
            pMsg.Length = Convert.ToByte(nudLength.Value);
            // Fill data array
            txtbCurrentTextBox = txtData0;
            for (int i = 0; i < pMsg.Length; i++)
            {
                if (txtbCurrentTextBox.Enabled && txtbCurrentTextBox.Text.Length != 0)
                    pMsg.Data[i] = Convert.ToByte(txtbCurrentTextBox.Text, 16);
                else
                    pMsg.Data[i] = 0;
                if (i < 7)
                    txtbCurrentTextBox = (TextBox)this.GetNextControl(txtbCurrentTextBox, true);
            }
            // Check if the hardware is initialize as master
            if (m_HwMode == Peak.Lin.TLINHardwareMode.modMaster)
            {
                // Calculate the checksum contained with the
                // checksum type that set some line before.
                Peak.Lin.PLinApi.CalculateChecksum(ref pMsg);
                // Try to send the LIN frame message with LIN_Write.
                m_LastLINErr = Peak.Lin.PLinApi.Write(m_hClient, m_hHw, ref pMsg);
            }
            else
            {
                // If the hardare is initialize as slave
                // only update the data in the LIN frame.
                m_LastLINErr = Peak.Lin.PLinApi.UpdateByteArray(m_hClient, m_hHw, lFD.IdAsByte, (byte)0, pMsg.Length, pMsg.Data);
            }
            // Show error if any
            if (m_LastLINErr != Peak.Lin.TLINError.errOK)
                MessageBox.Show(GetFormatedError(m_LastLINErr), Resources.SAppCaptionError);
        }

        #endregion

        #endregion

        #region Combobox event-handler

        private void cbbChannel_SelectedIndexChanged(object sender, EventArgs e)
        {
            int lnMode, lnCurrBaud;
            // get the handle from the comboBoxItem
            ushort lwHw = (ushort)((ComboBoxItem)cbbChannel.SelectedItem).Value;

            if (lwHw != 0)
            {
                btnInit.Enabled = true;
                btnIdent.Enabled = true;
                // Read the mode of the hardware with the handle lwHw (Master, Slave or None).
                m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpMode, out lnMode, 0);
                // Read the baudrate of the hardware with the handle lwHw.
                m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpBaudrate, out lnCurrBaud, 0);

                // Update hardware mode comboBox
                if ((Peak.Lin.TLINHardwareMode)lnMode == Peak.Lin.TLINHardwareMode.modMaster)
                    cbbHwMode.SelectedIndex = 1;
                else
                    cbbHwMode.SelectedIndex = 0;

                // Assign the Baudrate to the Control.
                cbbBaudrates.Text = (lnCurrBaud != 0) ? lnCurrBaud.ToString() : m_wBaudrate.ToString();
            }
            else
            {
                btnInit.Enabled = false;
                btnIdent.Enabled = false;
            }
        }

        private void cbbHwMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Change the "write" button text depending on the hardware mode
            if (cbbHwMode.SelectedIndex == 1)
                btnWrite.Text = Resources.SWriteAsMaster;
            else
                btnWrite.Text = Resources.SWriteAsSlave;
        }

        private void cbbID_SelectedIndexChanged(object sender, EventArgs e)
        {
            CFrameDefinition lFD = (CFrameDefinition)((ComboBoxItem)cbbID.SelectedItem).Value;
            // Check if a global frame is selected
            if (lFD != null)
            {
                // Update components according to the selected frame informations
                txtID.Text = lFD.ProtectedID;
                txtID.Enabled = false;
                nudLength.Value = lFD.Length;
                switch (lFD.Direction)
                {
                    case Peak.Lin.TLINDirection.dirDisabled:
                        cbbDirection.SelectedIndex = 0;
                        break;
                    case Peak.Lin.TLINDirection.dirPublisher:
                        cbbDirection.SelectedIndex = 1;
                        break;
                    case Peak.Lin.TLINDirection.dirSubscriber:
                        cbbDirection.SelectedIndex = 2;
                        break;
                    case Peak.Lin.TLINDirection.dirSubscriberAutoLength:
                        cbbDirection.SelectedIndex = 3;
                        break;
                }
                switch (lFD.ChecksumType)
                {
                    case Peak.Lin.TLINChecksumType.cstAuto:
                        cbbCST.SelectedIndex = 0;
                        break;
                    case Peak.Lin.TLINChecksumType.cstClassic:
                        cbbCST.SelectedIndex = 1;
                        break;
                    case Peak.Lin.TLINChecksumType.cstEnhanced:
                        cbbCST.SelectedIndex = 2;
                        break;
                    case Peak.Lin.TLINChecksumType.cstCustom:
                        cbbCST.SelectedIndex = 3;
                        break;
                }
                // Force update on Data fields (both Length and Direction impact on Enabled state of these components)
                // if length value is not changed but direction switch from subscriber to publisher
                // data textfield would not be disabled
                nudLength_ValueChanged(this, new EventArgs());
            }
        }

        #endregion

        #region Timer event-handler

        private void tmrRead_Tick(object sender, EventArgs e)
        {
            // Checks if in the receive-queue are currently messages for read
            ReadMessages();
        }

        private void tmrDisplay_Tick(object sender, EventArgs e)
        {
            // Update display of received messages
            DisplayMessages();
        }

        #endregion

        #region Custom functions

        #region LIN connection functions

        /// <summary>
        /// Updates the combobox 'cbbChannel' with currently available hardwares
        /// </summary>
        private void RefreshHardware()
        {
            byte i;
            ushort[] lwHwHandles;
            ushort lwBuffSize, lwCount;
            int lnHwType, lnDevNo, lnChannel, lnMode;
            Peak.Lin.TLINError lLINErr;
            ushort lwHw;
            String str;

            lwHwHandles = new ushort[8];
            lwBuffSize = 8;
            lwCount = 0;
            // Get all available LIN hardware.
            lLINErr = Peak.Lin.PLinApi.GetAvailableHardware(lwHwHandles, lwBuffSize, out lwCount);
            if (lLINErr == Peak.Lin.TLINError.errOK)
            {
                cbbChannel.Items.Clear();
                // If no error occurs
                if (lwCount == 0)
                {
                    // No LIN hardware was found.
                    // Show an empty entry
                    lwHw = 0;
                    cbbChannel.Items.Add(new ComboBoxItem(Resources.SHardwareNothingFound, (ushort)0));
                }
                // For each founded LIN hardware
                for (i = 0; i < lwCount; i++)
                {
                    // Get the handle of the hardware.
                    lwHw = lwHwHandles[i];
                    // Read the type of the hardware with the handle lwHw.
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpType, out lnHwType, 0);
                    // Read the device number of the hardware with the handle lwHw.
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpDeviceNumber, out lnDevNo, 0);
                    // Read the channel number of the hardware with the handle lwHw.
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpChannelNumber, out lnChannel, 0);
                    // Read the mode of the hardware with the handle lwHw (Master, Slave or None).
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpMode, out lnMode, 0);

                    // Create a comboboxItem
                    // If the hardware type is a knowing hardware
                    // show the name of that in the label of the entry.
                    if (lnHwType == Peak.Lin.PLinApi.LIN_HW_TYPE_USB)
                        str = Resources.SHardwareTypeLIN;
                    else
                        // Show as unknown hardware
                        str = Resources.SHardwareTypeUnkown;
                    str = string.Format("{0} - dev. {1}, chan. {2}", str, lnDevNo, lnChannel);
                    cbbChannel.Items.Add(new ComboBoxItem(str, lwHw));

                }
                cbbChannel.SelectedIndex = 0;
            }
        }

        /// <summary>
        /// Connects to the hardware with the setting values
        /// from the connection groupbox.
        /// </summary>
        /// <returns>
        /// Returns true if the function finished successfull. Otherwise
        /// returns false.
        /// </returns>
        private bool DoLinConnect()
        {
            bool fRet;
            ushort lwHw, lwBaud;
            int lnMode, lnCurrBaud;
            Peak.Lin.TLINHardwareMode lHwMode;

            // initialisation
            fRet = false;

            if (m_hHw != 0)
            {
                // If a connection to hardware already exits
                // disconnect this connection first.
                if (DoLinDisconnect() == false)
                    return fRet;
            }
            // Get the selected Hardware handle from the comboboxItem
            lwHw = (ushort)((ComboBoxItem)cbbChannel.SelectedItem).Value;
            if (lwHw != 0)
            {
                byte[] lhClients = new byte[255];
                m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpConnectedClients, lhClients, 255);

                // The local hardware handle is valid.
                // Get the current mode of the hardware
                Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpMode, out lnMode, 0);
                // Get the current baudrate of the hardware
                Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpBaudrate, out lnCurrBaud, 0);
                // Try to connect the application client to the
                // hardware with the local handle.
                m_LastLINErr = Peak.Lin.PLinApi.ConnectClient(m_hClient, lwHw);
                if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                {
                    // If the connection successfull
                    // assign the local handle to the
                    // member handle.
                    m_hHw = lwHw;
                    // Get the selected hardware mode.
                    if (cbbHwMode.SelectedIndex == 1)
                        lHwMode = Peak.Lin.TLINHardwareMode.modMaster;
                    else
                        lHwMode = Peak.Lin.TLINHardwareMode.modSlave;
                    // Get the selected baudrate
                    try
                    {
                        lwBaud = Convert.ToUInt16(cbbBaudrates.Text);
                    }
                    catch {
                        lwBaud = 0;
                    }
                    
                    // Get the selected hardware channel
                    if (((Peak.Lin.TLINHardwareMode)lnMode == Peak.Lin.TLINHardwareMode.modNone)
                                || (Convert.ToUInt16(lnCurrBaud) != lwBaud))
                    {
                        // Only if the current hardware is not initialize
                        // try to Intialize the hardware with mode and baudrate
                        m_LastLINErr = Peak.Lin.PLinApi.InitializeHardware(m_hClient,
                            m_hHw, lHwMode, lwBaud);
                    }
                    if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                    {
                        // Assign the Hardware Mode to member attribut
                        m_HwMode = lHwMode;
                        // Assign the baudrate index to member attribut
                        m_wBaudrate = lwBaud;
                        // Set the client filter with the mask.
                        m_LastLINErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask);
                        // Read the frame table from the connected hardware.
                        ReadFrameTableFromHw();
                        // Reset the last LIN error code to default.
                        m_LastLINErr = Peak.Lin.TLINError.errOK;
                        fRet = true;
                    }
                    else
                    {
                        // An error occured while initializing hardware.
                        // Set the member variable to default.
                        m_hHw = 0;
                        fRet = false;
                    }
                }
                else
                {
                    // The local hardware handle is invalid
                    // and/or an error occurs while connecting
                    // hardware with client.
                    // Set the member variable to default.
                    m_hHw = 0;
                    fRet = false;
                }

                // Check if any LIN error code was received.
                if (m_LastLINErr != Peak.Lin.TLINError.errOK)
                {
                    MessageBox.Show(GetFormatedError(m_LastLINErr), Resources.SAppCaptionError);
                    fRet = false;
                }

            }
            else // Should never occur
                m_hHw = 0; // But if it occurs, set handle to default

            return fRet;
        }

        /// <summary>
        /// Disconnects an existing connection to a LIN hardware and returns
        /// true if disconnection finished succesfully or if no connection exists.
        /// Returns false if the current connection can not be disconnected.
        /// </summary>
        /// <returns>
        /// Returns true if the function finished successfull. Otherwise
        /// returns false.
        /// </returns>
        private bool DoLinDisconnect()
        {
            // If the application was registered with LIN as client.
            if (m_hHw != 0)
            {
                // The client was connected to a LIN hardware.
                // Before disconnect from the hardware check
                // the connected clients and determine if the
                // hardware configuration have to reset or not.

                // Initialize the locale variables.
                bool lfOtherClient = false;
                bool lfOwnClient = false;
                byte[] lhClients = new byte[255];

                // Get the connected clients from the LIN hardware.
                m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(m_hHw, Peak.Lin.TLINHardwareParam.hwpConnectedClients, lhClients, 255);
                if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                {
                    // No errors !
                    // Check all client handles.
                    for (int i = 0; i < lhClients.Length; i++)
                    {
                        // If client handle is invalid
                        if (lhClients[i] == 0)
                            continue;
                        // Set the boolean to true if the handle isn't the
                        // handle of this application.
                        // Even the boolean is set to true it can never
                        // set to false.
                        lfOtherClient = lfOtherClient || (lhClients[i] != m_hClient);
                        // Set the boolean to true if the handle is the
                        // handle of this application.
                        // Even the boolean is set to true it can never
                        // set to false.
                        lfOwnClient = lfOwnClient || (lhClients[i] == m_hClient);
                    }
                }
                // If another application is also connected to
                // the LIN hardware do not reset the configuration.
                if (lfOtherClient == false)
                {
                    // No other application connected !
                    // Reset the configuration of the LIN hardware.
                    Peak.Lin.PLinApi.ResetHardwareConfig(m_hClient, m_hHw);
                }
                // If this application is connected to the hardware
                // then disconnect the client. Otherwise not.
                if (lfOwnClient == true)
                {
                    // Disconnect if the application was connected to a LIN hardware.
                    m_LastLINErr = Peak.Lin.PLinApi.DisconnectClient(m_hClient, m_hHw);
                    if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                    {
                        m_hHw = 0;
                        return true;
                    }
                    else
                    {
                        // Error while disconnecting from hardware.
                        MessageBox.Show(GetFormatedError(m_LastLINErr), Resources.SAppCaptionError);
                        return false;
                    }
                }
                else
                    return true;
            }
            else
                return true;
        }

        /// <summary>
        /// Reads all values from the frame table of the hardware
        /// and assign it to the GlobalFrameTable. Also refresh
        /// the Global Frame Table ListView with that values.
        /// </summary>
        private void ReadFrameTableFromHw()
        {
            int i, lnCnt;
            Peak.Lin.TLINFrameEntry lFrameEntry;
            Peak.Lin.TLINError lErr;
            ulong llMask;

            // Create a LIN frame entry object
            lFrameEntry = new Peak.Lin.TLINFrameEntry();
            // Get the count of Frame Definition from the
            // Global Frame Table.
            lnCnt = m_pGFT.Count;
            // Initialize the member attribute for the
            // client mask with 0.
            m_lMask = 0;
            // Each Frame Definition
            for (i = 0; i < lnCnt; i++)
            {
                // Before a frame entry can be read from the
                // hardware, the Frame-ID of the wanted entry 
                // must be set
                lFrameEntry.FrameId = m_pGFT[i].IdAsByte;
                // Read the information of the specified frame entry from the hardware.
                lErr = Peak.Lin.PLinApi.GetFrameEntry(m_hHw, ref lFrameEntry);
                // Check the result value of the LinApi function call.
                if (lErr == Peak.Lin.TLINError.errOK)
                {
                    // LinApi function call successfull.
                    // Copy the frame entry information to the Frame Definition.
                    m_pGFT[i].m_nLength = Convert.ToInt32(lFrameEntry.Length);
                    m_pGFT[i].m_bDirection = lFrameEntry.Direction;
                    m_pGFT[i].m_nChecksumType = lFrameEntry.ChecksumType;
                    if (m_pGFT[i].Direction != Peak.Lin.TLINDirection.dirDisabled)
                    {
                        // If the direction is not disabled then set the
                        // bit in the client filter mask.
                        llMask = ((ulong)1 << i) & FRAME_FILTER_MASK;
                        m_lMask = m_lMask | llMask;
                    }
                }
            }
            // If the Client and Hardware handles are valid.
            if ((m_hClient != 0) && (m_hHw != 0))
                // Set the client filter.
                Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask);
            // Updates the displayed frameIds
            UpdateFrameIds();
        }

        /// <summary>
        /// Occurs before a property value of at least one CFrameDefinition object changes.
        /// </summary>
        /// <param name="s">The source of the event</param>
        /// <param name="e">A PropertyValueChangeEventArg that contains the event data.</param>
        private void PropertyChange(object s, PropertyChangeEventArg e)
        {
            CFrameDefinition lFD;
            Peak.Lin.TLINFrameEntry lFrameEntry;
            Peak.Lin.TLINError lErr;
            ulong lMask;

            // Try to get the sender as CFrameDefinition
            lFD = (CFrameDefinition)s;
            // The sender of this event is the CFrameDefinition that
            // property should be change by User.
            if (lFD != null)
            {
                // If data length is to be set, check the value.
                if (e.Type == EProperty.Length)
                    // Only a value between 0 and 8 are valid.
                    e.Allowed = (e.Value >= 0) && (e.Value <= 8);
                // If DO NOT allow then return.
                if (!e.Allowed)
                    return;

                // Only if the hardware was initialized as Slave
                // set the direction of the LIN-Frame.
                // By the Master the direction will be used with
                // the LIN_Write and do not have to set here.
                if (m_HwMode == Peak.Lin.TLINHardwareMode.modSlave)
                {
                    // Temporary set Allowed to false is to easy
                    // return only later.
                    e.Allowed = false;

                    // Create a Frame Entry object to get
                    // and set the direction.
                    lFrameEntry = new Peak.Lin.TLINFrameEntry();
                    // Set the Frame-ID of the Frame to get and set.
                    // The ID have to set before get the entry.
                    lFrameEntry.FrameId = lFD.IdAsByte;
                    // Get the Frame Entry with the Frame-ID from
                    // the Hardware via the LinApi.
                    lErr = Peak.Lin.PLinApi.GetFrameEntry(m_hHw, ref lFrameEntry);
                    // If an error occurs do not allow to change
                    // the property and return.
                    // The Allowed parameter was set some lines before.
                    if (lErr != Peak.Lin.TLINError.errOK)
                        return;
                    // Switch between the different kind of property types.
                    switch (e.Type)
                    {
                        // Direction property should be set
                        case EProperty.Direction:
                            lFrameEntry.Direction = (Peak.Lin.TLINDirection)e.Value;
                            break;
                        // Length property should be set
                        case EProperty.Length:
                            lFrameEntry.Length = Convert.ToByte(e.Value);
                            break;
                        // ChecksumType property should be set
                        case EProperty.ChecksumType:
                            lFrameEntry.ChecksumType = (Peak.Lin.TLINChecksumType)e.Value;
                            break;
                    }
                    lFrameEntry.Flags = Peak.Lin.PLinApi.FRAME_FLAG_RESPONSE_ENABLE;
                    lErr = Peak.Lin.PLinApi.SetFrameEntry(m_hClient, m_hHw, ref lFrameEntry);
                    // If an error occurs do not allow to change
                    // the property and return.
                    // The Allowed parameter was set some lines before.
                    if (lErr != Peak.Lin.TLINError.errOK)
                        return;

                    // Temporary set Allowed to true for next check.
                    // The action was successfull on this line.
                    e.Allowed = true;
                }

                // If the property 'Direction' of one
                // CFrameDefinition will be changed,
                // here we need a special request to set
                // the client filter.
                if (e.Type == EProperty.Direction)
                {
                    // If the new value for the property 'Direction'
                    // should NOT be 'Disabled' check first if
                    // the CFrameDefinition is defined already with some
                    // other value then 'Disabled'.
                    if ((Peak.Lin.TLINDirection)e.Value != Peak.Lin.TLINDirection.dirDisabled)
                    {
                        if (lFD.Direction == Peak.Lin.TLINDirection.dirDisabled)
                        {
                            // If the old property of CFrameDefintion
                            // was set to 'Disabled' the new value
                            // means that the Frame-ID have to add to
                            // the client filter by the LinApi.
                            // Set the client filter.
                            // The Filter is a bit mask.
                            lMask = (ulong)1 << lFD.IdAsInt;
                            m_lMask = m_lMask | lMask;
                            lErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask);
                            // Only allow to change the property if the Frame-ID
                            // was added successfull to the Filter.
                            e.Allowed = lErr == Peak.Lin.TLINError.errOK;
                        }
                    }
                    else
                    {
                        // If the value of direction should set on 'disable'
                        // Remove the Frame-ID from the client filter.
                        lMask = (ulong)1 << lFD.IdAsInt;
                        m_lMask = m_lMask & ~lMask;
                        lErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask);
                        // Only allow to change the property if the Frame-ID
                        // was removed successfull from the Filter.
                        e.Allowed = lErr == Peak.Lin.TLINError.errOK;
                    }
                }
            }
        }

        #endregion

        #region Help and gui functions

        /// <summary>
        /// Activates/deactivates the different controls of the main-form according
        /// to the current connection status
        /// </summary>
        /// <param name="bConnected">Current status. True if connected, false otherwise</param>
        private void SetConnectionStatus(bool bConnected)
        {
            // Buttons
            //
            btnInit.Enabled = !bConnected;
            btnConfigure.Enabled = bConnected;
            btnRead.Enabled = bConnected && rdbManual.Checked;
            btnWrite.Enabled = bConnected;
            btnRelease.Enabled = bConnected;
            btnFilterApply.Enabled = bConnected;
            btnFilterQuery.Enabled = bConnected;
            btnGetVersions.Enabled = bConnected;
            btnHwRefresh.Enabled = !bConnected;
            btnStatus.Enabled = bConnected;
            btnReset.Enabled = bConnected;

            // ComboBoxs
            //
            cbbBaudrates.Enabled = !bConnected;
            cbbChannel.Enabled = !bConnected;
            cbbHwMode.Enabled = !bConnected;

            // Hardware configuration and read mode
            //
            if (!bConnected)
                cbbChannel_SelectedIndexChanged(this, new EventArgs());
            else
                rdbTimer_CheckedChanged(this, new EventArgs());

            // Display messages in grid
            //
            tmrDisplay.Enabled = bConnected;
        }

        /// <summary>
        /// Includes a new line of text into the information Listview
        /// </summary>
        /// <param name="strMsg">Text to be included</param>
        private void IncludeTextMessage(string strMsg)
        {
            lbxInfo.Items.Add(strMsg);
            lbxInfo.SelectedIndex = lbxInfo.Items.Count - 1;
        }

        /// <summary>
        /// Help Function used to get an error as text
        /// </summary>
        /// <param name="error">Error code to be translated</param>
        /// <returns>A text with the translated error</returns>
        private string GetFormatedError(Peak.Lin.TLINError error)
        {
            StringBuilder sErrText = new StringBuilder(255);
            // If any error are occured
            // display the error text in a message box.
            // 0x00 = Neutral
            // 0x07 = Language German
            // 0x09 = Language English
            if (Peak.Lin.PLinApi.GetErrorText(error, 0x09, sErrText, 255) != Peak.Lin.TLINError.errOK)
                return string.Format("An error occurred. Error-code's text ({0}) couldn't be retrieved", error);
            return sErrText.ToString();
        }

        /// <summary>
        /// Updates 'cbbID' comboBox with values from the global frame table
        /// </summary>
        private void UpdateFrameIds()
        {
            ComboBoxItem lItem;
            string lID;

            // Retrieves selected ID if it exist
            lItem = (ComboBoxItem)cbbID.SelectedItem;
            lID = (lItem != null) ? lItem.Text : null;

            // Clears and populates FrameID combobox with global frame IDs
            cbbID.Items.Clear();
            for (int i = 0; i < m_pGFT.Count; i++)
            {
                // add only non disabled frames
                if (m_pGFT[i].Direction == Peak.Lin.TLINDirection.dirDisabled)
                    continue;
                cbbID.Items.Add(new ComboBoxItem(m_pGFT[i].ID, m_pGFT[i]));
                // check if the new item was selected before the update
                if (lID == m_pGFT[i].ID)
                    cbbID.SelectedIndex = cbbID.Items.Count - 1;
            }
        }

        #endregion

        #region Read/write functions

        /// <summary>
        /// Function for reading PLIN messages
        /// </summary>
        private void ReadMessages()
        {
            Peak.Lin.TLINRcvMsg lpMsg;

            // We read at least one time the queue looking for messages.
            // If a message is found, we look again trying to find more.
            // If the queue is empty or an error occurs, we get out from
            // the dowhile statement.
            //	
            do
            {
                m_LastLINErr = Peak.Lin.PLinApi.Read(m_hClient, out lpMsg);
                // If at least one Frame is received by the LinApi.
                // Check if the received frame is a standard type.
                // If it is not a standard type than ignore it.
                if (lpMsg.Type != Peak.Lin.TLINMsgType.mstStandard)
                    continue;

                if (m_LastLINErr == Peak.Lin.TLINError.errOK)
                    ProcessMessage(lpMsg);

            } while (btnRelease.Enabled && (!Convert.ToBoolean(m_LastLINErr & Peak.Lin.TLINError.errRcvQueueEmpty)));
        }

        /// <summary>
        /// Processes a received message, in order to show it in the Message-ListView
        /// </summary>
        /// <param name="linMsg">The received PLIN message</param>
        private void ProcessMessage(Peak.Lin.TLINRcvMsg linMsg)
        {
            // We search if a message (Same ID and Type) has 
            // already been received or if this is a new message
            //
            foreach (MessageStatus msg in m_LastMsgsList)
            {
                if (msg.LINMsg.FrameId == linMsg.FrameId)
                {
                    // Modify the message and exit
                    //
                    msg.Update(linMsg);
                    return;
                }
            }
            // Message not found. It will be created
            //
            InsertMsgEntry(linMsg);
        }

        /// <summary>
        /// Inserts a new entry for a new message in the Message-ListView
        /// </summary>
        /// <param name="newMsg">The messasge to be inserted</param>
        private void InsertMsgEntry(Peak.Lin.TLINRcvMsg newMsg)
        {
            MessageStatus msgStsCurrentMsg;
            ListViewItem lviCurrentItem;
            string strId;

            // We add this status in the last message list
            //
            msgStsCurrentMsg = new MessageStatus(newMsg, lstMessages.Items.Count);
            m_LastMsgsList.Add(msgStsCurrentMsg);
            
            // Search and retrieve the ID in the global frame table associated with the frame Protected-ID
            strId = "";
            for (int i = 0; i < m_pGFT.Count; i++)
            {
                if (msgStsCurrentMsg.LINMsg.FrameId == m_pGFT[i].ProtectedIdAsInt)
                {
                    strId = m_pGFT[i].ID;
                    break;
                }
            }
            // Add the new ListView Item with the ID of the message
            //	
            lviCurrentItem = lstMessages.Items.Add(strId);
            // We set the length of the message
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.LINMsg.Length.ToString());
            // We set the data of the message. 	
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.DataString);
            // we set the message count message (this is the First, so count is 1)            
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.Count.ToString());
            // Add time stamp information if needed
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.TimeString);
            // We set the direction of the message
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.DirectionString);
            // We set the error of the message
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.ErrorString);
            // We set the CST of the message
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.CSTString);
            // We set the CRC of the message
            //
            lviCurrentItem.SubItems.Add(msgStsCurrentMsg.ChecksumString);
        }

        /// <summary>
        /// Displays and updates LIN messages in the Message-ListView
        /// </summary>
        private void DisplayMessages()
        {
            ListViewItem lviCurrentItem;

            foreach (MessageStatus msgStatus in m_LastMsgsList)
            {
                // Get the data to actualize
                //
                if (msgStatus.MarkedAsUpdated)
                {
                    msgStatus.MarkedAsUpdated = false;
                    lviCurrentItem = lstMessages.Items[msgStatus.Position];

                    lviCurrentItem.SubItems[1].Text = msgStatus.LINMsg.Length.ToString();
                    lviCurrentItem.SubItems[2].Text = msgStatus.DataString;
                    lviCurrentItem.SubItems[3].Text = msgStatus.Count.ToString();
                    lviCurrentItem.SubItems[4].Text = msgStatus.TimeString;
                    lviCurrentItem.SubItems[5].Text = msgStatus.DirectionString;
                    lviCurrentItem.SubItems[6].Text = msgStatus.ErrorString;
                    lviCurrentItem.SubItems[7].Text = msgStatus.CSTString;
                    lviCurrentItem.SubItems[8].Text = msgStatus.ChecksumString;
                }
            }
        }

        #endregion

        private void txtHexFormat_KeyPress(object sender, KeyPressEventArgs e)
        {
            char chCheck;

            // We convert the Character to its Upper case equivalent
            //
            chCheck = char.ToUpper(e.KeyChar);

            // The Key is the Delete (Backspace) Key
            //
            if (chCheck == 8)
                return;
            // The Key is a number between 0-9
            //
            if ((chCheck > 47) && (chCheck < 58))
                return;
            // The Key is a character between A-F
            //
            if ((chCheck > 64) && (chCheck < 71))
                return;

            // Is neither a number nor a character between A(a) and F(f)
            //
            e.Handled = true;
        }

        #endregion
    }
}
