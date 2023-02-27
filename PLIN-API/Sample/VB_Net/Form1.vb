'Imports PLIN_API_Example.Peak.Lin
Imports PLIN_API_Example.PLIN_API_Example
Imports PLIN_API_Example.Peak.Lin
Imports System.Text

Imports HLINHW = System.UInt16


Public Class Form1

#Region "Structures"

    ''' <summary>
    ''' Message Status structure used to show LIN Messages
    ''' in a ListView
    ''' </summary>
    Private Class MessageStatus
        ''' <summary>
        ''' The received LIN message
        ''' </summary>
        Private m_Msg As Peak.Lin.TLINRcvMsg
        ''' <summary>
        ''' Timestamp of a previously received message
        ''' </summary>
        Private m_oldTimeStamp As ULong
        ''' <summary>
        ''' index of the message in the ListView component
        ''' </summary>
        Private m_iIndex As Integer
        ''' <summary>
        ''' Number of LIN message received with the same frame ID
        ''' </summary>
        Private m_Count As Integer
        ''' <summary>
        ''' Defines if the timestamp is displayed as a period
        ''' </summary>
        Private m_bShowPeriod As Boolean
        ''' <summary>
        ''' Defines if the message has been modified and its display needs to be updated
        ''' </summary>
        Private m_bWasChanged As Boolean

        ''' <summary>
        ''' Creates a new MessageStatus object
        ''' </summary>
        ''' <param name="linMsg">received LIN message</param>
        ''' <param name="listIndex">index of the message in the ListView</param>
        Public Sub New(ByVal linMsg As Peak.Lin.TLINRcvMsg, ByVal listIndex As Integer)
            m_Msg = linMsg
            m_oldTimeStamp = linMsg.TimeStamp
            m_iIndex = listIndex
            m_Count = 1
            m_bShowPeriod = True
            m_bWasChanged = False
        End Sub

        ''' <summary>
        ''' Updates an existing MessageStatus with a new LIN message
        ''' </summary>
        ''' <param name="linMsg"></param>
        Public Sub Update(ByVal linMsg As Peak.Lin.TLINRcvMsg)
            m_oldTimeStamp = m_Msg.TimeStamp
            m_Msg = linMsg
            m_Count += 1
            m_bWasChanged = True
        End Sub

#Region "Getters and Setters"

        ''' <summary>
        ''' The received LIN message
        ''' </summary>
        Public ReadOnly Property LINMsg() As Peak.Lin.TLINRcvMsg
            Get
                Return m_Msg
            End Get
        End Property

        ''' <summary>
        ''' Index of the message in the ListView
        ''' </summary>
        Public ReadOnly Property Position() As Integer
            Get
                Return m_iIndex
            End Get
        End Property

        ''' <summary>
        ''' Direction of the LIN message as a string
        ''' </summary>
        Public ReadOnly Property DirectionString() As String
            Get
                Return GetDirectionString()
            End Get
        End Property

        ''' <summary>
        ''' Checksum type of the LIN message as a string
        ''' </summary>
        Public ReadOnly Property CSTString() As String
            Get
                Return GetCSTString()
            End Get
        End Property

        ''' <summary>
        ''' Checksum of the LIN message as a string
        ''' </summary>
        Public ReadOnly Property ChecksumString() As String
            Get
                Return String.Format("{0:X}h", m_Msg.Checksum)
            End Get
        End Property

        ''' <summary>
        ''' Error field of the LIN message as a string
        ''' </summary>
        Public ReadOnly Property ErrorString() As String
            Get
                Return GetErrorString()
            End Get
        End Property

        ''' <summary>
        ''' Protected ID of the LIN message as a string
        ''' </summary>
        Public ReadOnly Property PIdString() As String
            Get
                Return getProtectedIdString()
            End Get
        End Property

        ''' <summary>
        ''' Data fields of the LIN message as a string
        ''' </summary>
        Public ReadOnly Property DataString() As String
            Get
                Return GetDataString()
            End Get
        End Property

        ''' <summary>
        ''' Number of LIN messages received with the same frame ID
        ''' </summary>
        Public ReadOnly Property Count() As Integer
            Get
                Return m_Count
            End Get
        End Property

        ''' <summary>
        ''' States wether the timestamp is displayed as a period or not
        ''' </summary>
        Public Property ShowingPeriod() As Boolean
            Get
                Return m_bShowPeriod
            End Get
            Set(ByVal value As Boolean)
                If (m_bShowPeriod ^ value) Then
                    m_bShowPeriod = value
                    m_bWasChanged = True
                End If
            End Set
        End Property
        ''' <summary>
        ''' Defines if the LIN message has been modified
        ''' </summary>
        Public Property MarkedAsUpdated() As Boolean
            Get
                Return m_bWasChanged
            End Get
            Set(ByVal value As Boolean)
                m_bWasChanged = value
            End Set
        End Property

        ''' <summary>
        ''' The timestamp or period of the LIN message
        ''' </summary>
        Public ReadOnly Property TimeString() As String
            Get
                Return GetTimeString()
            End Get
        End Property

#End Region

#Region "private getters"

        ''' <summary>
        ''' Returns the Protected ID as a string
        ''' </summary>
        ''' <returns>formatted protected ID</returns>
        Private Function getProtectedIdString() As String
            Return String.Format("{0:X3}h", m_Msg.FrameId)
        End Function

        ''' <summary>
        ''' Returns the Data array as a string
        ''' </summary>
        ''' <returns>the formatted data array</returns>
        Private Function GetDataString() As String
            Dim strTemp As String

            strTemp = ""
            For i As Integer = 0 To m_Msg.Length - 1
                strTemp += String.Format("{0:X2} ", m_Msg.Data(i))
            Next
            Return strTemp.ToString()

        End Function

        ''' <summary>
        ''' Returns the timestamp or the period of the frame
        ''' </summary>
        ''' <returns>timestamp or period in milliseconds</returns>
        Private Function GetTimeString() As String
            Dim time As ULong = m_Msg.TimeStamp

            If m_bShowPeriod Then
                time = (time - m_oldTimeStamp) / 1000
            End If

            Return time.ToString()
        End Function

        ''' <summary>
        ''' Returns the direction as a formatted string
        ''' </summary>
        ''' <returns>the formatted direction</returns>
        Private Function GetDirectionString() As String
            Select Case m_Msg.Direction
                Case Peak.Lin.TLINDirection.dirDisabled
                    Return My.Resources.SLinDirectionDisabled
                Case Peak.Lin.TLINDirection.dirPublisher
                    Return My.Resources.SLinDirectionPublisher
                Case Peak.Lin.TLINDirection.dirSubscriber
                    Return My.Resources.SLinDirectionSubscriber
                Case Peak.Lin.TLINDirection.dirSubscriberAutoLength
                    Return My.Resources.SLinDirectionAuto
            End Select

            Return ""
        End Function

        ''' <summary>
        ''' Returns the Checksum type as a string
        ''' </summary>
        ''' <returns>formatted checksum type</returns>
        Private Function GetCSTString() As String
            Select Case m_Msg.ChecksumType
                Case Peak.Lin.TLINChecksumType.cstAuto
                    Return My.Resources.SLinCSTAuto
                Case Peak.Lin.TLINChecksumType.cstClassic
                    Return My.Resources.SLinCSTClassic
                Case Peak.Lin.TLINChecksumType.cstEnhanced
                    Return My.Resources.SLinCSTEnhanced
                Case Peak.Lin.TLINChecksumType.cstCustom
                    Return My.Resources.SLinCSTCustom
            End Select

            Return ""
        End Function
            
        ''' <summary>
        ''' Returns the Error field of the LIN message as a string
        ''' </summary>
        ''' <returns>formatted Error field of the LIN message</returns>
        Private Function GetErrorString() As String
            If m_Msg.ErrorFlags = 0 Then
                Return "O.k."
            End If

            Return m_Msg.ErrorFlags.ToString()
        End Function

#End Region

    End Class

    ''' <summary>
    ''' Represents a comboBoxItem with a value and its corresponding displayed value 
    ''' </summary>
    Public Class ComboBoxItem
        ''' <summary>
        ''' Displayed value
        ''' </summary>
        Public Text As String
        ''' <summary>
        ''' Value object
        ''' </summary>
        Public Value As Object

        ''' <summary>
        ''' Constructs and initializes a comboBoxItem
        ''' </summary>
        ''' <param name="text">displayed value</param>
        ''' <param name="value">the value object</param>
        Public Sub New(ByRef text As String, ByRef value As Object)
            Me.Text = text
            Me.Value = value
        End Sub

        Public Overrides Function ToString() As String
            Return Text
        End Function

    End Class

#End Region

#Region "Attributes"

#Region "LIN"

    ''' <summary>
    ''' Client handle
    ''' </summary>
    Private m_hClient As Byte
    ''' <summary>
    ''' Hardware handle
    ''' </summary>
    Private m_hHw As HLINHW
    ''' <summary>
    ''' LIN Hardware Modus (Master/Slave)
    ''' </summary>
    Private m_HwMode As Peak.Lin.TLINHardwareMode
    ''' <summary>
    ''' Client filter mask
    ''' </summary>
    Private m_lMask As ULong
    ''' <summary>
    ''' Baudrate Index of Hardware
    ''' </summary>
    Private m_wBaudrate As UShort
    ''' <summary>
    ''' Last LIN error
    ''' </summary>
    Private m_LastLINErr As Peak.Lin.TLINError
    ''' <summary>
    ''' Constant value that indicate the mask of the client filter (64bit)
    ''' </summary>
    Private Const FRAME_FILTER_MASK As ULong = ULong.MaxValue    '&HFFFFFFFFFFFFFFFF

#End Region

    ''' <summary>
    ''' Global Frame Table object to hold all frames definitions.
    ''' </summary>
    Private m_pGFT As PLIN_API_Example.CGlobalFrameTable

    ''' <summary>
    ''' Stores the status of received messages for its display
    ''' </summary>
    Private m_LastMsgsList As System.Collections.ArrayList

#End Region

#Region "Form event Handlers"

    Public Sub New()
        ' This call is required by the Windows Form Designer.
        Try
            InitializeComponent()
        Catch ex As Exception
            Debug.Print("-------------------\n" + ex.ToString())
            Throw ex
        End Try


        ' Create the Global Frame Table and add property change handler on its items
        m_pGFT = New PLIN_API_Example.CGlobalFrameTable(Me)
        AddHandler m_pGFT.OnPropertyChange, AddressOf PropertyChange

        ' Populates FrameID combobox with global frame IDs
        UpdateFrameIds()
        ' Populates Direction combobox
        cbbDirection.Items.Clear()
        cbbDirection.Items.Add(New ComboBoxItem(My.Resources.SLinDirectionDisabled, Peak.Lin.TLINDirection.dirDisabled))
        cbbDirection.Items.Add(New ComboBoxItem(My.Resources.SLinDirectionPublisher, Peak.Lin.TLINDirection.dirPublisher))
        cbbDirection.Items.Add(New ComboBoxItem(My.Resources.SLinDirectionSubscriber, Peak.Lin.TLINDirection.dirSubscriber))
        cbbDirection.Items.Add(New ComboBoxItem(My.Resources.SLinDirectionAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength))
        ' Populates ChecksumType combobox
        cbbCST.Items.Clear()
        cbbCST.Items.Add(New ComboBoxItem(My.Resources.SLinCSTAuto, Peak.Lin.TLINChecksumType.cstAuto))
        cbbCST.Items.Add(New ComboBoxItem(My.Resources.SLinCSTClassic, Peak.Lin.TLINChecksumType.cstClassic))
        cbbCST.Items.Add(New ComboBoxItem(My.Resources.SLinCSTEnhanced, Peak.Lin.TLINChecksumType.cstEnhanced))
        cbbCST.Items.Add(New ComboBoxItem(My.Resources.SLinCSTCustom, Peak.Lin.TLINChecksumType.cstCustom))

        ' Initialize the LIN attributes
        m_hClient = 0
        m_hHw = 0
        m_lMask = FRAME_FILTER_MASK
        m_HwMode = Peak.Lin.TLINHardwareMode.modSlave
        m_wBaudrate = 19200

        ' Creates the list for received messages
        m_LastMsgsList = New System.Collections.ArrayList()

        ' Register this application with LIN as client.
        m_LastLINErr = Peak.Lin.PLinApi.RegisterClient(My.Resources.SPLinClientName, Handle, m_hClient)
    End Sub

    Private Sub Form1_Shown(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Shown
        RefreshHardware()
        rdbFilter_CheckedChanged(Me, New EventArgs())
        cbbDirection.SelectedIndex = 0
        cbbCST.SelectedIndex = 1
    End Sub

    Private Sub Form1_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        ' disconnect from LIN if necessary
        If (m_hClient <> 0) Then
            DoLinDisconnect()
            m_hHw = 0
            ' Unregister the application
            Peak.Lin.PLinApi.RemoveClient(m_hClient)
            m_hClient = 0
        End If
    End Sub

#End Region


#Region "Connection event-handlers"

    Private Sub cbbChannel_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbbChannel.SelectedIndexChanged
        Dim lnMode, lnCurrBaud As Integer
        ' get the handle from the comboBoxItem
        Dim lwHw As HLINHW = DirectCast((DirectCast(cbbChannel.SelectedItem, ComboBoxItem).Value), HLINHW)

        If (lwHw <> 0) Then
            btnInit.Enabled = True
            btnIdent.Enabled = True
            ' Read the mode of the hardware with the handle lwHw (Master, Slave or None).
            m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpMode, lnMode, CUShort(0))
            ' Read the baudrate of the hardware with the handle lwHw.
            m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpBaudrate, lnCurrBaud, CUShort(0))

            ' Update hardware mode comboBox
            If (lnMode = Peak.Lin.TLINHardwareMode.modMaster) Then
                cbbHwMode.SelectedIndex = 1
            Else
                cbbHwMode.SelectedIndex = 0
            End If
            ' Assign the Baudrate to the Control.
            cbbBaudrates.Text = IIf(lnCurrBaud <> 0, lnCurrBaud.ToString(), m_wBaudrate.ToString())
        Else
            btnInit.Enabled = False
            btnIdent.Enabled = False
        End If
    End Sub

    Private Sub btnHwRefresh_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHwRefresh.Click
        RefreshHardware()
    End Sub

    Private Sub cbbHwMode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbbHwMode.SelectedIndexChanged
        ' Change the "write" button text depending on the hardware mode
        If (cbbHwMode.SelectedIndex = 1) Then
            btnWrite.Text = My.Resources.SWriteAsMaster
        Else
            btnWrite.Text = My.Resources.SWriteAsSlave
        End If
    End Sub

    Private Sub btnIdent_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnIdent.Click
        ' Get hardware device from the selected combobox item
        Dim lwHw As UShort = CUShort(DirectCast(cbbChannel.SelectedItem, ComboBoxItem).Value)
        If (lwHw <> 0) Then
            ' makes the corresponding PCAN-USB-Pro's LED blink
            Peak.Lin.PLinApi.IdentifyHardware(lwHw)
        End If
    End Sub

    Private Sub btnInit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnInit.Click
        ' Do the connection
        If DoLinConnect() Then
            ' Sets the connection status of the main-form
            SetConnectionStatus(True)
        End If
    End Sub

    Private Sub btnRelease_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRelease.Click
        ' Releases the current connection
        If DoLinDisconnect() Then
            ' stop reading timer
            tmrRead.Enabled = False
            ' Sets the connection status of the main-form
            SetConnectionStatus(False)
        End If
    End Sub

#End Region

#Region "Configuration & Message filtering event-handlers"

    Private Sub btnConfigure_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnConfigure.Click
        ' Open the "Global Frame Table" Dialog
        Dim dlg As Frame_Dlg = New Frame_Dlg(m_pGFT)
        dlg.ShowDialog()
        ' Output filter information (as changes to Global Frame Table items modify it)
        btnFilterQuery_Click(Me, New EventArgs())
        ' Update the available frame ids (i.e. the IDs combobox in the "write message" UI group)
        UpdateFrameIds()
    End Sub

    Private Sub rdbFilter_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles rdbFilterOpen.CheckedChanged, rdbFilterCustom.CheckedChanged, rdbFilterClose.CheckedChanged
        If (rdbFilterCustom.Checked) Then
            nudIdFrom.Enabled = True
            nudIdTo.Enabled = True
        Else
            nudIdFrom.Enabled = False
            nudIdTo.Enabled = False
        End If
    End Sub

    Private Sub btnFilterApply_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFilterApply.Click
        If (m_hHw <> 0) Then
            Dim lMask As ULong
            ' Sets the according to the selected filter
            If (rdbFilterOpen.Checked) Then
                ' set no filtering : all frames are received
                lMask = FRAME_FILTER_MASK
                m_LastLINErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, lMask)
                If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                    m_lMask = lMask
                    IncludeTextMessage(String.Format("The filter was successfully opened."))
                End If
            ElseIf (rdbFilterClose.Checked) Then
                ' filters all : all frames are denied
                lMask = 0
                m_LastLINErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, lMask)
                If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                    m_lMask = lMask
                    IncludeTextMessage(String.Format("The filter was successfully closed."))
                End If
            Else
                ' custom filtering (i.e. rdbFilterCustom.Checked) : the frames for the given range will be opened
                m_LastLINErr = Peak.Lin.PLinApi.RegisterFrameId(m_hClient, m_hHw, CByte(nudIdFrom.Value), CByte(nudIdTo.Value))
                If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                    IncludeTextMessage(String.Format("The filter was customized. IDs from {0} to {1} will be received", nudIdFrom.Text, nudIdTo.Text))
                    m_LastLINErr = Peak.Lin.PLinApi.GetClientFilter(m_hClient, m_hHw, lMask)
                    If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                        m_lMask = lMask
                    End If
                End If
            End If
            ' If success, an information message is written, if it is not, an error message is shown
            If (m_LastLINErr <> Peak.Lin.TLINError.errOK) Then
                MessageBox.Show(GetFormatedError(m_LastLINErr))
            End If
        End If
    End Sub

    Private Sub btnFilterQuery_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFilterQuery.Click
        Dim pRcvMask As ULong = 0
        ' Retrieves the filter corresponding to the current Client-Hardware pair
        If (m_hHw <> 0) Then
            If (Peak.Lin.PLinApi.GetClientFilter(m_hClient, m_hHw, pRcvMask) = Peak.Lin.TLINError.errOK) Then
                ' Convert UInt64 to binary string
                Dim strTmp As String = ""
                Dim temp As ULong = pRcvMask
                For bit As SByte = 63 To 0 Step -1
                    If temp >= Convert.ToUInt64(2 ^ bit) Then
                        strTmp += "1"
                        temp = temp - Convert.ToUInt64(2 ^ bit)
                    Else
                        strTmp += "0"
                    End If
                Next
                ' add message
                IncludeTextMessage(String.Format("The Status of the filter is {0}.", strTmp.PadLeft(64, "0"c)))
            End If
        End If
    End Sub

#End Region

#Region "Messages Reading event-handlers"

    Private Sub rdbTimer_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles rdbTimer.CheckedChanged, rdbManual.CheckedChanged
        If (Not btnRelease.Enabled) Then
            Return
        End If

        ' According with the kind of reading, a timer or a button will be enabled
        If (rdbTimer.Checked) Then
            ' Enable Timer
            tmrRead.Enabled = btnRelease.Enabled
        End If
        If (rdbManual.Checked) Then
            ' Disable Timer
            tmrRead.Enabled = False
        End If
        btnRead.Enabled = btnRelease.Enabled And rdbManual.Checked
    End Sub

    Private Sub chbShowPeriod_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chbShowPeriod.CheckedChanged
        ' According with the check-value of this checkbox,
        ' the recieved time of a messages will be interpreted as 
        ' period (time between the two last messages) or as time-stamp
        ' (the elapsed time since windows was started)
        '
        If (m_LastMsgsList IsNot Nothing) Then
            For Each msg As MessageStatus In m_LastMsgsList
                msg.ShowingPeriod = chbShowPeriod.Checked
            Next
        End If
    End Sub

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        ReadMessages()
    End Sub

    Private Sub btnMsgClear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMsgClear.Click
        ' The information contained in the messages List-View is cleared
        m_LastMsgsList.Clear()
        lstMessages.Items.Clear()
    End Sub

#End Region

#Region "Write Messages event-handlers"

    Private Sub cbbID_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbbID.SelectedIndexChanged
        Dim lFD As CFrameDefinition = DirectCast(DirectCast(cbbID.SelectedItem, ComboBoxItem).Value, CFrameDefinition)
        ' Check if a global frame is selected
        If (lFD IsNot Nothing) Then
            ' Update components according to the selected frame informations
            txtID.Text = lFD.ProtectedID
            txtID.Enabled = False
            nudLength.Value = lFD.Length
            Select Case (lFD.Direction)
                Case Peak.Lin.TLINDirection.dirDisabled
                    cbbDirection.SelectedIndex = 0
                Case Peak.Lin.TLINDirection.dirPublisher
                    cbbDirection.SelectedIndex = 1
                Case Peak.Lin.TLINDirection.dirSubscriber
                    cbbDirection.SelectedIndex = 2
                Case Peak.Lin.TLINDirection.dirSubscriberAutoLength
                    cbbDirection.SelectedIndex = 3
            End Select
            Select Case (lFD.ChecksumType)
                Case Peak.Lin.TLINChecksumType.cstAuto
                    cbbCST.SelectedIndex = 0
                Case Peak.Lin.TLINChecksumType.cstClassic
                    cbbCST.SelectedIndex = 1
                Case Peak.Lin.TLINChecksumType.cstEnhanced
                    cbbCST.SelectedIndex = 2
                Case Peak.Lin.TLINChecksumType.cstCustom
                    cbbCST.SelectedIndex = 3
            End Select
            ' Force update on Data fields (both Length and Direction impact on Enabled state of these components)
            ' if length value is not changed but direction switch from subscriber to publisher
            ' data textfield would not be disabled
            nudLength_ValueChanged(Me, New EventArgs())
        End If
    End Sub

    Private Sub nudLength_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudLength.ValueChanged
        txtData0.Enabled = False
        txtData1.Enabled = False
        txtData2.Enabled = False
        txtData3.Enabled = False
        txtData4.Enabled = False
        txtData5.Enabled = False
        txtData6.Enabled = False
        txtData7.Enabled = False
        ' Only Publisher direction allows edition of Data textFields
        If (cbbDirection.SelectedIndex <> 1) Then
            Return
        End If
        ' Enable the same number of data textBox as the nudLength value
        Select Case (Convert.ToInt32(nudLength.Value))
            Case 1
                txtData0.Enabled = True
            Case 2
                txtData0.Enabled = True
                txtData1.Enabled = True
            Case 3
                txtData0.Enabled = True
                txtData1.Enabled = True
                txtData2.Enabled = True
            Case 4
                txtData0.Enabled = True
                txtData1.Enabled = True
                txtData2.Enabled = True
                txtData3.Enabled = True
            Case 5
                txtData0.Enabled = True
                txtData1.Enabled = True
                txtData2.Enabled = True
                txtData3.Enabled = True
                txtData4.Enabled = True
            Case 6
                txtData0.Enabled = True
                txtData1.Enabled = True
                txtData2.Enabled = True
                txtData3.Enabled = True
                txtData4.Enabled = True
                txtData5.Enabled = True
            Case 7
                txtData0.Enabled = True
                txtData1.Enabled = True
                txtData2.Enabled = True
                txtData3.Enabled = True
                txtData4.Enabled = True
                txtData5.Enabled = True
                txtData6.Enabled = True
            Case 8
                txtData0.Enabled = True
                txtData1.Enabled = True
                txtData2.Enabled = True
                txtData3.Enabled = True
                txtData4.Enabled = True
                txtData5.Enabled = True
                txtData6.Enabled = True
                txtData7.Enabled = True
        End Select
    End Sub

    Private Sub txtData0_Leave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtData7.Leave, txtData6.Leave, txtData5.Leave, txtData4.Leave, txtData3.Leave, txtData2.Leave, txtData1.Leave, txtData0.Leave
        Dim txtbCurrentTextbox As TextBox

        ' all the Textbox Data fields are represented with 2 characters.
        ' Therefore if the Length of the text is smaller than 2, we add
        ' a "0"
        If (TypeOf sender Is TextBox) Then
            txtbCurrentTextbox = DirectCast(sender, TextBox)
            While (txtbCurrentTextbox.Text.Length <> 2)
                txtbCurrentTextbox.Text = ("0" + txtbCurrentTextbox.Text)
            End While
        End If
    End Sub

    Private Sub txtData0_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtData0.TextChanged, txtData7.TextChanged, txtData6.TextChanged, txtData5.TextChanged, txtData4.TextChanged, txtData3.TextChanged, txtData2.TextChanged, txtData1.TextChanged
        ' Automatically change focus between data textBox 
        ' when the length of the data equals 2
        If (TypeOf sender Is TextBox) Then
            Dim txtbox As TextBox = DirectCast(sender, TextBox)
            If (txtbox.TextLength = 2) Then
                SelectNextControl(txtbox, True, True, True, True)
            End If
        End If
    End Sub

    Private Sub txtHexFormat_KeyPress(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtData7.KeyPress, txtData6.KeyPress, txtData5.KeyPress, txtData4.KeyPress, txtData3.KeyPress, txtData2.KeyPress, txtData1.KeyPress, txtData0.KeyPress, txtID.KeyPress
        Dim chCheck As Int16

        ' We convert the Character to its Upper case equivalent
        chCheck = Microsoft.VisualBasic.Asc(e.KeyChar.ToString().ToUpper())

        ' The Key is the Delete (Backspace) Key
        If chCheck = 8 Then
            Return
        End If
        ' The Key is a number between 0-9
        If (chCheck > 47) AndAlso (chCheck < 58) Then
            Return
        End If
        ' The Key is a character between A-F
        If (chCheck > 64) AndAlso (chCheck < 71) Then
            Return
        End If

        ' Is neither a number nor a character between A(a) and F(f)
        e.Handled = True
    End Sub

    Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click
        Dim pMsg As Peak.Lin.TLINMsg
        Dim txtbCurrentTextBox As TextBox
        Dim lFD As CFrameDefinition

        ' Assert that a frame ID has been selected
        If (txtID.Text.Length = 0) Then
            txtID.Focus()
            Return
        End If
        ' Get the CFrameDefinition associated to the selected FrameID
        lFD = DirectCast(DirectCast(cbbID.SelectedItem, ComboBoxItem).Value, CFrameDefinition)
        ' Create a new LIN frame message and copy the data.
        pMsg = New Peak.Lin.TLINMsg()
        pMsg.Data = New Byte(8) {}
        pMsg.FrameId = Convert.ToByte(lFD.ProtectedIdAsInt)
        pMsg.Direction = DirectCast(DirectCast(cbbDirection.SelectedItem, ComboBoxItem).Value, Peak.Lin.TLINDirection)
        pMsg.ChecksumType = DirectCast(DirectCast(cbbCST.SelectedItem, ComboBoxItem).Value, Peak.Lin.TLINChecksumType)
        pMsg.Length = Convert.ToByte(nudLength.Value)
        ' Fill data array
        txtbCurrentTextBox = txtData0
        For i As Integer = 0 To pMsg.Length - 1
            If (txtbCurrentTextBox.Enabled And txtbCurrentTextBox.Text.Length <> 0) Then
                pMsg.Data(i) = Convert.ToByte(txtbCurrentTextBox.Text, 16)
            Else
                pMsg.Data(i) = 0
            End If
            If (i < 7) Then
                txtbCurrentTextBox = DirectCast(Me.GetNextControl(txtbCurrentTextBox, True), TextBox)
            End If
        Next
        ' Check if the hardware is initialize as master
        If (m_HwMode = Peak.Lin.TLINHardwareMode.modMaster) Then

            ' Calculate the checksum contained with the
            ' checksum type that set some line before.
            Peak.Lin.PLinApi.CalculateChecksum(pMsg)
            ' Try to send the LIN frame message with LIN_Write.
            m_LastLINErr = Peak.Lin.PLinApi.Write(m_hClient, m_hHw, pMsg)
        Else
            ' If the hardare is initialize as slave
            ' only update the data in the LIN frame.
            m_LastLINErr = Peak.Lin.PLinApi.UpdateByteArray(m_hClient, m_hHw, lFD.IdAsByte, CByte(0), pMsg.Length, pMsg.Data)
        End If
        ' Show error if any
        If (m_LastLINErr <> Peak.Lin.TLINError.errOK) Then
            MessageBox.Show(GetFormatedError(m_LastLINErr), My.Resources.SAppCaptionError)
        End If
    End Sub

#End Region

#Region "Information event-handlers"

    Private Sub btnGetVersions_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGetVersions.Click
        Dim lpVersion As Peak.Lin.TLINVersion
        Dim strTemp As StringBuilder
        Dim strArrayVersion() As String

        lpVersion = New Peak.Lin.TLINVersion()
        strTemp = New StringBuilder(255)

        ' We get the vesion of the PLIN API
        m_LastLINErr = Peak.Lin.PLinApi.GetVersion(lpVersion)
        If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
            IncludeTextMessage(String.Format("API Version: {0}.{1}.{2}.{3}", lpVersion.Major, lpVersion.Minor, lpVersion.Build, lpVersion.Revision))
            ' We get the driver version
            m_LastLINErr = Peak.Lin.PLinApi.GetVersionInfo(strTemp, 255)
            If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                ' Because this information contains line control characters (several lines)
                ' we split this also in several entries in the Information List-Box
                strArrayVersion = strTemp.ToString().Split(New Char() {ControlChars.Lf})
                IncludeTextMessage("Channel/Driver Version: ")
                For i As Integer = 0 To strArrayVersion.Length - 1
                    IncludeTextMessage("     * " + strArrayVersion(i))
                Next
            End If
        End If

        ' If an error ccurred, a message is shown
        If (m_LastLINErr <> Peak.Lin.TLINError.errOK) Then
            MessageBox.Show(GetFormatedError(m_LastLINErr))
        End If
    End Sub

    Private Sub btnInfoClear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnInfoClear.Click
        ' The information contained in the Information List-Box is cleared
        lbxInfo.Items.Clear()
    End Sub

    Private Sub btnStatus_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStatus.Click
        Dim lStatus As Peak.Lin.TLINHardwareStatus = New Peak.Lin.TLINHardwareStatus()
        ' Retrieves the status of the LIN Bus and outputs its state in the information listView
        m_LastLINErr = Peak.Lin.PLinApi.GetStatus(m_hHw, lStatus)
        If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
            Select (lStatus.Status)
                Case Peak.Lin.TLINHardwareState.hwsActive
                    IncludeTextMessage(My.Resources.SLINStatusActive)
                Case Peak.Lin.TLINHardwareState.hwsAutobaudrate
                    IncludeTextMessage(My.Resources.SLINStatusBaudDetect)
                Case Peak.Lin.TLINHardwareState.hwsNotInitialized
                    IncludeTextMessage(My.Resources.SLINStatusNotInit)
                Case Peak.Lin.TLINHardwareState.ShortGround
                    IncludeTextMessage(My.Resources.SLINStatusShortGround)
                Case Peak.Lin.TLINHardwareState.hwsSleep
                    IncludeTextMessage(My.Resources.SLINStatusSleep)
            End Select
        End If
    End Sub

    Private Sub btnReset_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReset.Click
        ' Flushes the Receive Queue of the Client and resets its counters. 
        m_LastLINErr = Peak.Lin.PLinApi.ResetClient(m_hClient)

        ' If it fails, a error message is shown
        If (m_LastLINErr <> Peak.Lin.TLINError.errOK) Then
            MessageBox.Show(GetFormatedError(m_LastLINErr))
        Else
            ' clears the message list-view
            btnMsgClear_Click(Me, New EventArgs())
            IncludeTextMessage("Receive queue and counters successfully reset")
        End If
    End Sub

#End Region

#Region "Timer event-handlers"

    Private Sub tmrRead_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrRead.Tick
        ' Checks if in the receive-queue are currently messages for read
        ReadMessages()
    End Sub

    Private Sub tmrDisplay_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrDisplay.Tick
        ' Update display of received messages
        DisplayMessages()
    End Sub

#End Region

#Region "Custom functions"

#Region "LIN connection functions"

    ''' <summary>
    ''' Updates the combobox 'cbbChannel' with currently available hardwares
    ''' </summary>
    Private Sub RefreshHardware()
        Dim i As Byte
        Dim lwHwHandles(7) As HLINHW
        Dim lwBuffSize, lwCount As UShort
        Dim lnHwType, lnDevNo, lnChannel, lnMode As Integer
        Dim lLINErr As Peak.Lin.TLINError
        Dim lwHw As UShort
        Dim str As String

        lwBuffSize = lwHwHandles.Length
        lwCount = 0
        ' Get all available LIN hardware.
        lLINErr = Peak.Lin.PLinApi.GetAvailableHardware(lwHwHandles, lwBuffSize, lwCount)
        If (lLINErr = Peak.Lin.TLINError.errOK) Then
            cbbChannel.Items.Clear()
            ' If no error occurs
            If (lwCount = 0) Then
                ' No LIN hardware was found.
                ' Show an empty entry
                lwHw = 0
                cbbChannel.Items.Add(New ComboBoxItem(My.Resources.SHardwareNothingFound, lwHw))
            Else
                ' For each founded LIN hardware
                For i = 0 To lwCount - 1
                    ' Get the handle of the hardware.
                    lwHw = lwHwHandles(i)
                    ' Read the type of the hardware with the handle lwHw.
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpType, lnHwType, CUShort(0))
                    ' Read the device number of the hardware with the handle lwHw.
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpDeviceNumber, lnDevNo, CUShort(0))
                    ' Read the channel number of the hardware with the handle lwHw.
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpChannelNumber, lnChannel, CUShort(0))
                    ' Read the mode of the hardware with the handle lwHw (Master, Slave or None).
                    Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpMode, lnMode, CUShort(0))

                    ' Create a comboboxItem
                    ' If the hardware type is a knowing hardware
                    ' show the name of that in the label of the entry.
                    If (lnHwType = Peak.Lin.PLinApi.LIN_HW_TYPE_USB) Then
                        str = My.Resources.SHardwareTypeLIN
                    Else
                        ' Show as unknown hardware
                        str = My.Resources.SHardwareTypeUnkown
                    End If
                    str = String.Format("{0} - dev. {1}, chan. {2}", str, lnDevNo, lnChannel)
                    cbbChannel.Items.Add(New ComboBoxItem(str, lwHw))

                Next
            End If
            cbbChannel.SelectedIndex = 0
        End If
    End Sub

    ''' <summary>
    ''' Connects to the hardware with the setting values
    ''' from the connection groupbox.
    ''' </summary>
    ''' <returns>
    ''' Returns true if the function finished successfull. Otherwise
    ''' returns false.
    ''' </returns>
    Private Function DoLinConnect() As Boolean
        Dim fRet As Boolean
        Dim lwHw As HLINHW
        Dim lwBaud As UShort
        Dim lnMode, lnCurrBaud As Integer
        Dim lHwMode As Peak.Lin.TLINHardwareMode

        ' initialisation
        fRet = False

        If (m_hHw <> 0) Then
            ' If a connection to hardware already exits
            ' disconnect this connection first.
            If (DoLinDisconnect() = False) Then
                Return fRet
            End If
        End If
        ' Get the selected Hardware handle from the comboboxItem
        lwHw = CUShort(DirectCast(cbbChannel.SelectedItem, ComboBoxItem).Value)
        If (lwHw <> 0) Then
            Dim lhClients(254) As Byte
            m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpConnectedClients, lhClients, lhClients.Length)

            ' The local hardware handle is valid.
            ' Get the current mode of the hardware
            Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpMode, lnMode, CUShort(0))
            ' Get the current baudrate of the hardware
            Peak.Lin.PLinApi.GetHardwareParam(lwHw, Peak.Lin.TLINHardwareParam.hwpBaudrate, lnCurrBaud, CUShort(0))
            ' Try to connect the application client to the
            ' hardware with the local handle.
            m_LastLINErr = Peak.Lin.PLinApi.ConnectClient(m_hClient, lwHw)
            If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                ' If the connection successfull
                ' assign the local handle to the
                ' member handle.
                m_hHw = lwHw
                ' Get the selected hardware mode.
                If (cbbHwMode.SelectedIndex = 1) Then
                    lHwMode = Peak.Lin.TLINHardwareMode.modMaster
                Else
                    lHwMode = Peak.Lin.TLINHardwareMode.modSlave
                End If
                ' Get the selected baudrate
                Try
                    lwBaud = Convert.ToUInt16(cbbBaudrates.Text)
                Catch ex As Exception
                    lwBaud = 0
                End Try

                ' Get the selected hardware channel
                If ((lnMode = Peak.Lin.TLINHardwareMode.modNone) Or (Convert.ToUInt16(lnCurrBaud) <> lwBaud)) Then
                    ' Only if the current hardware is not initialize
                    ' try to Intialize the hardware with mode and baudrate
                    m_LastLINErr = Peak.Lin.PLinApi.InitializeHardware(m_hClient, m_hHw, lHwMode, lwBaud)
                End If
                If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                    ' Assign the Hardware Mode to member attribut
                    m_HwMode = lHwMode
                    ' Assign the baudrate index to member attribut
                    m_wBaudrate = lwBaud
                    ' Set the client filter with the mask.
                    m_LastLINErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask)
                    ' Read the frame table from the connected hardware.
                    ReadFrameTableFromHw()
                    ' Reset the last LIN error code to default.
                    m_LastLINErr = Peak.Lin.TLINError.errOK
                    fRet = True
                Else
                    ' An error occured while initializing hardware.
                    ' Set the member variable to default.
                    m_hHw = 0
                    fRet = False
                End If
            Else
                ' The local hardware handle is invalid
                ' and/or an error occurs while connecting
                ' hardware with client.
                ' Set the member variable to default.
                m_hHw = 0
                fRet = False
            End If

            ' Check if any LIN error code was received.
            If (m_LastLINErr <> Peak.Lin.TLINError.errOK) Then
                MessageBox.Show(GetFormatedError(m_LastLINErr), My.Resources.SAppCaptionError)
                fRet = False
            End If


        Else ' Should never occur
            m_hHw = 0 ' But if it occurs, set handle to default
        End If

        Return fRet
    End Function

    ''' <summary>
    ''' Disconnects an existing connection to a LIN hardware and returns
    ''' true if disconnection finished succesfully or if no connection exists.
    ''' Returns false if the current connection can not be disconnected.
    ''' </summary>
    ''' <returns>
    ''' Returns true if the function finished successfull. Otherwise
    ''' returns false.
    ''' </returns>
    Private Function DoLinDisconnect() As Boolean
        ' If the application was registered with LIN as client.
        If (m_hHw <> 0) Then
            ' The client was connected to a LIN hardware.
            ' Before disconnect from the hardware check
            ' the connected clients and determine if the
            ' hardware configuration have to reset or not.

            ' Initialize the locale variables.
            Dim lfOtherClient As Boolean = False
            Dim lfOwnClient As Boolean = False
            Dim lhClients(254) As Byte

            ' Get the connected clients from the LIN hardware.
            m_LastLINErr = Peak.Lin.PLinApi.GetHardwareParam(m_hHw, Peak.Lin.TLINHardwareParam.hwpConnectedClients, lhClients, CUShort(lhClients.Length))
            If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                ' No errors !
                ' Check all client handles.
                For i As Integer = 0 To lhClients.Length - 1
                    ' If client handle is invalid
                    If (lhClients(i) = 0) Then
                        Continue For
                    End If
                    ' Set the boolean to true if the handle isn't the
                    ' handle of this application.
                    ' Even the boolean is set to true it can never
                    ' set to false.
                    lfOtherClient = lfOtherClient Or (lhClients(i) <> m_hClient)
                    ' Set the boolean to true if the handle is the
                    ' handle of this application.
                    ' Even the boolean is set to true it can never
                    ' set to false.
                    lfOwnClient = lfOwnClient Or (lhClients(i) = m_hClient)
                Next
            End If
            ' If another application is also connected to
            ' the LIN hardware do not reset the configuration.
            If (lfOtherClient = False) Then
                ' No other application connected !
                ' Reset the configuration of the LIN hardware.
                Peak.Lin.PLinApi.ResetHardwareConfig(m_hClient, m_hHw)
            End If
            ' If this application is connected to the hardware
            ' then disconnect the client. Otherwise not.
            If (lfOwnClient = True) Then
                ' Disconnect if the application was connected to a LIN hardware.
                m_LastLINErr = Peak.Lin.PLinApi.DisconnectClient(m_hClient, m_hHw)
                If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                    m_hHw = 0
                    Return True
                Else
                    ' Error while disconnecting from hardware.
                    MessageBox.Show(GetFormatedError(m_LastLINErr), My.Resources.SAppCaptionError)
                    Return False
                End If
            Else
                Return True
            End If
        Else
            Return True
        End If
    End Function

    ''' <summary>
    ''' Reads all values from the frame table of the hardware
    ''' and assign it to the GlobalFrameTable. Also refresh
    ''' the Global Frame Table ListView with that values.
    ''' </summary>
    Private Sub ReadFrameTableFromHw()
        Dim i, lnCnt As Integer
        Dim lFrameEntry As Peak.Lin.TLINFrameEntry
        Dim lErr As Peak.Lin.TLINError
        Dim llMask As ULong

        ' Create a LIN frame entry object
        lFrameEntry = New Peak.Lin.TLINFrameEntry()
        ' Get the count of Frame Definition from the
        ' Global Frame Table.
        lnCnt = m_pGFT.Count
        ' Initialize the member attribute for the
        ' client mask with 0.
        m_lMask = 0
        ' Each Frame Definition
        For i = 0 To lnCnt - 1
            ' Before a frame entry can be read from the
            ' hardware, the Frame-ID of the wanted entry 
            ' must be set
            lFrameEntry.FrameId = m_pGFT(i).IdAsByte
            ' Read the information of the specified frame entry from the hardware.
            lErr = Peak.Lin.PLinApi.GetFrameEntry(m_hHw, lFrameEntry)
            ' Check the result value of the LinApi function call.
            If (lErr = Peak.Lin.TLINError.errOK) Then
                ' LinApi function call successfull.
                ' Copy the frame entry information to the Frame Definition.
                m_pGFT(i).m_nLength = Convert.ToInt32(lFrameEntry.Length)
                m_pGFT(i).m_bDirection = lFrameEntry.Direction
                m_pGFT(i).m_nChecksumType = lFrameEntry.ChecksumType
                If (m_pGFT(i).Direction <> Peak.Lin.TLINDirection.dirDisabled) Then
                    ' If the direction is not disabled than set the
                    ' bit in the client filter mask.
                    llMask = (CULng(1) << i) And FRAME_FILTER_MASK
                    m_lMask = m_lMask Or llMask
                End If
            End If
        Next
        ' If the Client and Hardware handles are valid.
        If ((m_hClient <> 0) And (m_hHw <> 0)) Then
            ' Set the client filter.
            Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask)
        End If
        ' Updates the displayed frameIds
        UpdateFrameIds()
    End Sub

    ''' <summary>
    ''' Occurs before a property value of at least one CFrameDefinition object changes.
    ''' </summary>
    ''' <param name="s">The source of the event</param>
    ''' <param name="e">A PropertyValueChangeEventArg that contains the event data.</param>
    Private Sub PropertyChange(ByVal s As Object, ByVal e As PropertyChangeEventArg)
        Dim lFD As CFrameDefinition
        Dim lFrameEntry As Peak.Lin.TLINFrameEntry
        Dim lErr As Peak.Lin.TLINError
        Dim lMask As ULong

        ' Try to get the sender as CFrameDefinition
        lFD = DirectCast(s, CFrameDefinition)
        ' The sender of this event is the CFrameDefinition that
        ' property should be change by User.
        If (lFD IsNot Nothing) Then
            ' If data length is to be set, check the value.
            If (e.Type = EProperty.Length) Then
                ' Only a value between 0 and 8 are valid.
                e.Allowed = (e.Value >= 0) And (e.Value <= 8)
            End If
            ' If DO NOT allow then return.
            If (Not e.Allowed) Then
                Return
            End If

            ' Only if the hardware was initialized as Slave
            ' set the direction of the LIN-Frame.
            ' By the Master the direction will be used with
            ' the LIN_Write and do not have to set here.
            If (m_HwMode = Peak.Lin.TLINHardwareMode.modSlave) Then
                ' Temporary set Allowed to false is to easy
                ' return only later.
                e.Allowed = False

                ' Create a Frame Entry object to get
                ' and set the direction.
                lFrameEntry = New Peak.Lin.TLINFrameEntry()
                ' Set the Frame-ID of the Frame to get and set.
                ' The ID have to set before get the entry.
                lFrameEntry.FrameId = lFD.IdAsByte
                ' Get the Frame Entry with the Frame-ID from
                ' the Hardware via the LinApi.
                lErr = Peak.Lin.PLinApi.GetFrameEntry(m_hHw, lFrameEntry)
                ' If an error occurs do not allow to change
                ' the property and return.
                ' The Allowed parameter was set some lines before.
                If (lErr <> Peak.Lin.TLINError.errOK) Then
                    Return
                End If
                ' Switch between the different kind of property types.
                Select Case (e.Type)
                    ' Direction property should be set
                    Case EProperty.Direction
                        lFrameEntry.Direction = CType(e.Value, Peak.Lin.TLINDirection)
                        ' Length property should be set
                    Case EProperty.Length
                        lFrameEntry.Length = Convert.ToByte(e.Value)
                        ' ChecksumType property should be set
                    Case EProperty.ChecksumType
                        lFrameEntry.ChecksumType = CType(e.Value, Peak.Lin.TLINChecksumType)
                End Select
                lFrameEntry.Flags = Peak.Lin.PLinApi.FRAME_FLAG_RESPONSE_ENABLE
                lErr = Peak.Lin.PLinApi.SetFrameEntry(m_hClient, m_hHw, lFrameEntry)
                ' If an error occurs do not allow to change
                ' the property and return.
                ' The Allowed parameter was set some lines before.
                If (lErr <> Peak.Lin.TLINError.errOK) Then
                    Return
                End If

                ' Temporary set Allowed to true for next check.
                ' The action was successfull on this line.
                e.Allowed = True
            End If

            ' If the property 'Direction' of one
            ' CFrameDefinition will be changed,
            ' here we need a special request to set
            ' the client filter.
            If (e.Type = EProperty.Direction) Then
                ' If the new value for the property 'Direction'
                ' should NOT be 'Disabled' check first if
                ' the CFrameDefinition is defined already with some
                ' other value then 'Disabled'.
                If (CType(e.Value, Peak.Lin.TLINDirection) <> Peak.Lin.TLINDirection.dirDisabled) Then
                    If (lFD.Direction = Peak.Lin.TLINDirection.dirDisabled) Then
                        ' If the old property of CFrameDefintion
                        ' was set to 'Disabled' the new value
                        ' means that the Frame-ID have to add to
                        ' the client filter by the LinApi.
                        ' Set the client filter.
                        ' The Filter is a bit mask.
                        lMask = CULng(1) << lFD.IdAsInt
                        m_lMask = m_lMask Or lMask
                        lErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask)
                        ' Only allow to change the property if the Frame-ID
                        ' was added successfull to the Filter.
                        e.Allowed = (lErr = Peak.Lin.TLINError.errOK)
                    End If
                Else
                    ' If the value of direction should set on 'disable'
                    ' Remove the Frame-ID from the client filter.
                    lMask = CULng(1) << lFD.IdAsInt
                    m_lMask = m_lMask And Not lMask
                    lErr = Peak.Lin.PLinApi.SetClientFilter(m_hClient, m_hHw, m_lMask)
                    ' Only allow to change the property if the Frame-ID
                    ' was removed successfull from the Filter.
                    e.Allowed = (lErr = Peak.Lin.TLINError.errOK)
                End If
            End If
        End If
    End Sub

#End Region

#Region "Help and gui functions"

    ''' <summary>
    ''' Activates/deactivates the different controls of the main-form according
    ''' to the current connection status
    ''' </summary>
    ''' <param name="bConnected">Current status. True if connected, false otherwise</param>
    Private Sub SetConnectionStatus(ByVal bConnected As Boolean)
        ' Buttons
        btnInit.Enabled = Not bConnected
        btnConfigure.Enabled = bConnected
        btnRead.Enabled = bConnected And rdbManual.Checked
        btnWrite.Enabled = bConnected
        btnRelease.Enabled = bConnected
        btnFilterApply.Enabled = bConnected
        btnFilterQuery.Enabled = bConnected
        btnGetVersions.Enabled = bConnected
        btnHwRefresh.Enabled = Not bConnected
        btnStatus.Enabled = bConnected
        btnReset.Enabled = bConnected

        ' ComboBoxs
        cbbBaudrates.Enabled = Not bConnected
        cbbChannel.Enabled = Not bConnected
        cbbHwMode.Enabled = Not bConnected

        ' Hardware configuration and read mode
        If (Not bConnected) Then
            cbbChannel_SelectedIndexChanged(Me, New EventArgs())
        Else
            rdbTimer_CheckedChanged(Me, New EventArgs())
        End If

        ' Display messages in grid
        tmrDisplay.Enabled = bConnected
    End Sub

    ''' <summary>
    ''' Includes a new line of text into the information Listview
    ''' </summary>
    ''' <param name="strMsg">Text to be included</param>
    Private Sub IncludeTextMessage(ByRef strMsg As String)
        lbxInfo.Items.Add(strMsg)
        lbxInfo.SelectedIndex = lbxInfo.Items.Count - 1
    End Sub

    ''' <summary>
    ''' Help Function used to get an error as text
    ''' </summary>
    ''' <param name="err">Error code to be translated</param>
    ''' <returns>A text with the translated error</returns>
    Private Function GetFormatedError(ByRef err As Peak.Lin.TLINError) As String
        Dim sErrText As StringBuilder = New StringBuilder(255)
        ' If any error are occured
        ' display the error text in a message box.
        ' 0x00 = Neutral
        ' 0x07 = Language German
        ' 0x09 = Language English
        If (Peak.Lin.PLinApi.GetErrorText(err, &H9, sErrText, 255) <> Peak.Lin.TLINError.errOK) Then
            Return String.Format("An error occurred. Error-code's text ({0}) couldn't be retrieved", err)
        End If
        Return sErrText.ToString()
    End Function

    ''' <summary>
    ''' Updates 'cbbID' comboBox with values from the global frame table
    ''' </summary>
    Private Sub UpdateFrameIds()
        Dim lItem As ComboBoxItem
        Dim lID As String = Nothing

        ' Retrieves selected ID if it exist
        lItem = DirectCast(cbbID.SelectedItem, ComboBoxItem)
        If (lItem IsNot Nothing) Then
            lID = lItem.Text
        End If

        ' Clears and populates FrameID combobox with global frame IDs
        cbbID.Items.Clear()
        For i As Integer = 0 To m_pGFT.Count - 1
            ' add only non disabled frames
            If (m_pGFT(i).Direction = Peak.Lin.TLINDirection.dirDisabled) Then
                Continue For
            End If

            cbbID.Items.Add(New ComboBoxItem(m_pGFT(i).ID, m_pGFT(i)))
            ' check if the new item was selected before the update
            If (lID = m_pGFT(i).ID) Then
                cbbID.SelectedIndex = cbbID.Items.Count - 1
            End If
        Next
    End Sub

#End Region

#Region "Read/write functions"

    ''' <summary>
    ''' Function for reading PLIN messages
    ''' </summary>
    Private Sub ReadMessages()
        Dim lpMsg As Peak.Lin.TLINRcvMsg = New TLINRcvMsg()

        ' We read at least one time the queue looking for messages.
        ' If a message is found, we look again trying to find more.
        ' If the queue is empty or an error occurs, we get out from
        ' the dowhile statement.
        '	
        Do
            m_LastLINErr = Peak.Lin.PLinApi.Read(m_hClient, lpMsg)
            ' If at least one Frame is received by the LinApi.
            ' Check if the received frame is a standard type.
            ' If it is not a standard type than ignore it.
            If (lpMsg.Type <> Peak.Lin.TLINMsgType.mstStandard) Then
                Continue Do
            End If

            If (m_LastLINErr = Peak.Lin.TLINError.errOK) Then
                ProcessMessage(lpMsg)
            End If
        Loop While (btnRelease.Enabled And Not Convert.ToBoolean(m_LastLINErr And Peak.Lin.TLINError.errRcvQueueEmpty))
    End Sub

    ''' <summary>
    ''' Processes a received message, in order to show it in the Message-ListView
    ''' </summary>
    ''' <param name="linMsg">The received PLIN message</param>
    Private Sub ProcessMessage(ByVal linMsg As Peak.Lin.TLINRcvMsg)
        ' We search if a message (Same ID and Type) has 
        ' already been received or if this is a new message
        For Each msg As MessageStatus In m_LastMsgsList
            If (msg.LINMsg.FrameId = linMsg.FrameId) Then
                ' Modify the message and exit
                msg.Update(linMsg)
                Return
            End If
        Next
        ' Message not found. It will be created
        InsertMsgEntry(linMsg)
    End Sub

    ''' <summary>
    ''' Inserts a new entry for a new message in the Message-ListView
    ''' </summary>
    ''' <param name="newMsg">The messasge to be inserted</param>
    Private Sub InsertMsgEntry(ByVal newMsg As Peak.Lin.TLINRcvMsg)
        Dim msgStsCurrentMsg As MessageStatus
        Dim lviCurrentItem As ListViewItem
        Dim strId As String

        ' We add this status in the last message list
        '
        msgStsCurrentMsg = New MessageStatus(newMsg, lstMessages.Items.Count)
        m_LastMsgsList.Add(msgStsCurrentMsg)

        ' Search and retrieve the ID in the global frame table associated with the frame Protected-ID
        strId = ""
        For i As Integer = 0 To m_pGFT.Count - 1
            If (msgStsCurrentMsg.LINMsg.FrameId = m_pGFT(i).ProtectedIdAsInt) Then
                strId = m_pGFT(i).ID
                Exit For
            End If
        Next
        ' Add the new ListView Item with the ID of the message
        lviCurrentItem = lstMessages.Items.Add(strId)
        ' We set the length of the message
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.LINMsg.Length.ToString())
        ' We set the data of the message. 	
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.DataString)
        ' we set the message count message (this is the First, so count is 1)            
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.Count.ToString())
        ' Add time stamp information if needed
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.TimeString)
        ' We set the direction of the message
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.DirectionString)
        ' We set the error of the message
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.ErrorString)
        ' We set the CST of the message
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.CSTString)
        ' We set the CRC of the message
        lviCurrentItem.SubItems.Add(msgStsCurrentMsg.ChecksumString)
    End Sub

    ''' <summary>
    ''' Displays and updates LIN messages in the Message-ListView
    ''' </summary>
    Private Sub DisplayMessages()
        Dim lviCurrentItem As ListViewItem

        For Each msgStatus As MessageStatus In m_LastMsgsList
            ' Get the data to actualize
            If (msgStatus.MarkedAsUpdated) Then
                msgStatus.MarkedAsUpdated = False
                lviCurrentItem = lstMessages.Items(msgStatus.Position)

                lviCurrentItem.SubItems(1).Text = msgStatus.LINMsg.Length.ToString()
                lviCurrentItem.SubItems(2).Text = msgStatus.DataString
                lviCurrentItem.SubItems(3).Text = msgStatus.Count.ToString()
                lviCurrentItem.SubItems(4).Text = msgStatus.TimeString
                lviCurrentItem.SubItems(5).Text = msgStatus.DirectionString
                lviCurrentItem.SubItems(6).Text = msgStatus.ErrorString
                lviCurrentItem.SubItems(7).Text = msgStatus.CSTString
                lviCurrentItem.SubItems(8).Text = msgStatus.ChecksumString
            End If
        Next
    End Sub

#End Region

#End Region

End Class
