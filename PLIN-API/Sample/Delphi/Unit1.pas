unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, ComCtrls, PLinApi, GlobalFrameTable, Math;

Resourcestring
    SAppCaption             = 'PLIN-API Example';
    SAppCaptionError        = 'PLIN-API Example - Error';
    SHardwareNothingFound   = '<No hardware found>';
    SHardwareTypeLIN        = 'PCAN-USB Pro';
    SHardwareTypeUnkown     = 'Unknown';
    SLinCSTAuto             = 'Auto';
    SLinCSTClassic          = 'Classic';
    SLinCSTCustom           = 'Custom';
    SLinCSTEnhanced         = 'Enhanced';
    SLinDirectionAuto       = 'Subscriber Automatic Length';
    SLinDirectionDisabled   = 'Disabled';
    SLinDirectionPublisher  = 'Publisher';
    SLinDirectionSubscriber = 'Subscriber';
    SLINStatusActive        = 'Bus: Active';
    SLINStatusBaudDetect    = 'Hardware: Baudrate Detection';
    SLINStatusNotInit       = 'Hardware: Not Initialized';
    SLINStatusShortGround   = 'Bus-Line: Shorted Ground';
    SLINStatusSleep         = 'Bus: Sleep';
    SPLinClientName         = 'PLIN_EXAMPLE';
    SWriteAsMaster          = 'Transmit';
    SWriteAsSlave           = 'Publish';

type
    // Message Status structure used to show LIN Messages
    // in a ListView
    MessageStatus = class(TObject)
        private
            // The received LIN message
            m_Msg : TLINRcvMsg;
            // Timestamp of a previously received message
            m_oldTimeStamp : UInt64;
            // index of the message in the ListView component
            m_iIndex : integer;
            // Number of LIN message received with the same frame ID
            m_Count : integer;
            // Defines if the timestamp is displayed as a period
            m_bShowPeriod : boolean;
            // Defines if the message has been modified and its display needs to be updated
            m_bWasChanged : boolean;

            // Returns the direction as a formatted string
            function GetDirectionString : AnsiString;
            // Returns the Checksum type as a string
            function GetCSTString : AnsiString;
            // Returns the Checksum type as a string
            function GetChecksumString : AnsiString;
            // Returns the Error field of the LIN message as a string
            function GetErrorString : AnsiString;
            // Returns the Protected ID as a string
            function GetProtectedIdString : AnsiString;
            // Returns the Data array as a string
            function GetDataString : AnsiString;
            // Returns the timestamp or the period of the frame
            function GetTimeString : AnsiString;

            // Sets wether the timestamp is displayed as a period or not
            procedure SetShowingPeriod(value : boolean);

        public
            /// <summary>
            /// Creates a new MessageStatus object
            /// </summary>
            /// <param name="linMsg">received LIN message</param>
            /// <param name="listIndex">index of the message in the ListView</param>
            constructor Create(linMsg : TLINRcvMsg; listIndex : Integer);
            /// <summary>
            /// Updates an existing MessageStatus with a new LIN message
            /// </summary>
            /// <param name="linMsg">LIN message to update</param>
            procedure Update(linMsg : TLINRcvMsg);

            // The received LIN message
            property LINMsg : TLINRcvMsg read m_Msg;
            // Index of the message in the ListView
            property Position : Integer read m_iIndex;
            // Direction of the LIN message as a string
            property DirectionString : AnsiString read GetDirectionString;
            // Checksum type of the LIN message as a string
            property CSTString : AnsiString read GetCSTString;
            // Checksum of the LIN message as a string
            property ChecksumString : AnsiString read GetChecksumString;
            // Error field of the LIN message as a string
            property ErrorString : AnsiString read GetErrorString;
            // Protected ID of the LIN message as a string
            property PIdString : AnsiString read GetProtectedIdString;
            // Data fields of the LIN message as a string
            property DataString : AnsiString read GetDataString;
            // Number of LIN messages received with the same frame ID
            property Count : Integer read m_Count;
            // States wether the timestamp is displayed as a period or not
            property ShowingPeriod : boolean read m_bShowPeriod write SetShowingPeriod;
            // Defines if the LIN message has been modified
            property MarkedAsUpdated : boolean read m_bWasChanged write m_bWasChanged;
            // The timestamp or period of the LIN message
            property TimeString : AnsiString read GetTimeString;
    end;

    {
    // Represents a comboBoxItem with a value and its corresponding displayed value 
    ComboBoxItem = class(TObject)
        public
            // Displayed value
            Text: AnsiString;
            // Value object
            Value: TObject;

        public
            // Constructs and initializes a comboBoxItem
            constructor Create(text : AnsiString; value : TObject);

            function ToString() : AnsiString; 
    end;
    }

  TForm1 = class(TForm)
    tmrDisplay: TTimer;
    tmrRead: TTimer;
    GroupBox1: TGroupBox;
    Label1: TLabel;
    cbbChannel: TComboBox;
    btnHwRefresh: TButton;
    Label2: TLabel;
    cbbBaudrates: TComboBox;
    Label3: TLabel;
    cbbHwMode: TComboBox;
    btnIdent: TButton;
    btnInit: TButton;
    btnRelease: TButton;
    GroupBox2: TGroupBox;
    btnConfigure: TButton;
    rdbFilterOpen: TRadioButton;
    rdbFilterClose: TRadioButton;
    rdbFilterCustom: TRadioButton;
    Label4: TLabel;
    txtFilterFrom: TEdit;
    Label5: TLabel;
    nudIdFrom: TUpDown;
    txtFilterTo: TEdit;
    nudIdTo: TUpDown;
    btnFilterApply: TButton;
    btnFilterQuery: TButton;
    GroupBox3: TGroupBox;
    rdbTimer: TRadioButton;
    rdbManual: TRadioButton;
    chbShowPeriod: TCheckBox;
    lstMessages: TListView;
    btnRead: TButton;
    btnMsgClear: TButton;
    GroupBox4: TGroupBox;
    Label6: TLabel;
    cbbID: TComboBox;
    Label7: TLabel;
    txtID: TEdit;
    Label8: TLabel;
    cbbDirection: TComboBox;
    Label9: TLabel;
    cbbCST: TComboBox;
    txtLength: TEdit;
    Label10: TLabel;
    nudLength: TUpDown;
    Label11: TLabel;
    txtData0: TEdit;
    txtData1: TEdit;
    txtData2: TEdit;
    txtData3: TEdit;
    txtData4: TEdit;
    txtData5: TEdit;
    txtData6: TEdit;
    txtData7: TEdit;
    btnWrite: TButton;
    GroupBox5: TGroupBox;
    btnGetVersions: TButton;
    btnInfoClear: TButton;
    btnStatus: TButton;
    btnReset: TButton;
    lbxInfo: TListBox;
    procedure cbbChannelChange(Sender: TObject);
    procedure btnHwRefreshClick(Sender: TObject);
    procedure cbbHwModeChange(Sender: TObject);
    procedure btnIdentClick(Sender: TObject);
    procedure btnInitClick(Sender: TObject);
    procedure rdbFilterChanged(Sender: TObject);
    procedure txtFilterFromExit(Sender: TObject);
    procedure txtFilterToExit(Sender: TObject);
    procedure btnFilterApplyClick(Sender: TObject);
    procedure btnFilterQueryClick(Sender: TObject);
    procedure chbShowPeriodClick(Sender: TObject);
    procedure btnReadClick(Sender: TObject);
    procedure btnMsgClearClick(Sender: TObject);
    procedure cbbIDChange(Sender: TObject);
    procedure txtFilterKeyPress(Sender: TObject; var Key: Char);
    procedure tmrReadTimer(Sender: TObject);
    procedure tmrDisplayTimer(Sender: TObject);
    procedure txtData0Exit(Sender: TObject);
    procedure txtData0KeyPress(Sender: TObject; var Key: Char);
    procedure btnWriteClick(Sender: TObject);
    procedure btnGetVersionsClick(Sender: TObject);
    procedure btnInfoClearClick(Sender: TObject);
    procedure btnStatusClick(Sender: TObject);
    procedure btnResetClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure btnReleaseClick(Sender: TObject);
    procedure btnConfigureClick(Sender: TObject);   
    procedure rdbTimerChanged(Sender: TObject);
    procedure nudLengthClick(Sender: TObject; Button: TUDBtnType);
  private const
        // Constant value that indicate the mask of the client filter (64bit)
        FRAME_FILTER_MASK = UInt64($FFFFFFFFFFFFFFFF);
  private
    { Déclarations privées }

    // Client handle
    m_hClient : HLINCLIENT;
    // Hardware handle
    m_hHw : HLINHW;
    // LIN Hardware Modus (Master/Slave)
    m_HwMode : TLINHardwareMode;
    // Client filter mask
    m_lMask : UInt64;
    // Baudrate Index of Hardware
    m_wBaudrate : Word;
    // Last LIN error
    m_LastLINErr : TLINError;
    // Global Frame Table object to hold all frames definitions.
    m_pGFT : CGlobalFrameTable;
    // Stores the status of received messages for its display
    m_LastMsgsList : TList;

    //////////////////////////////////////////////////////////////////////////
	// LIN functions

	// Updates the combobox 'cbbChannel' with currently available hardwares
	procedure RefreshHardware();
    /// Connects to the hardware with the setting values
    /// from the connection groupbox.
	function DoLinConnect() : Boolean;
    /// Disconnects an existing connection to a LIN hardware and returns
    /// true if disconnection finished succesfully or if no connection exists.
    /// Returns false if the current connection can not be disconnected.
    function DoLinDisconnect() : Boolean;

    /// Reads all values from the frame table of the hardware
    /// and assign it to the GlobalFrameTable. Also refresh
    /// the Global Frame Table ListView with that values.
	procedure ReadFrameTableFromHw();

	///////////////////////////////////////////////////////////////////////////
	// LIN Message processing functions

    // Function for reading PLIN messages
	procedure ReadMessages();
    // Processes a received message, in order to show it in the Message-ListView
    procedure ProcessMessage(linMsg: TLINRcvMsg);
    // Inserts a new entry for a new message in the Message-ListView
    procedure InsertMsgEntry(newMsg: TLINRcvMsg);
    // Displays and updates LIN messages in the Message-ListView
    procedure DisplayMessages();
		
	///////////////////////////////////////////////////////////////////////////
	// Help functions
		
	//  Updates Frame IDs with values from the global frame table
	procedure UpdateFrameIds();
	// Includes a new line of text into the information Listview
	procedure IncludeTextMessage(strMsg: AnsiString);
	// Activates/deactivates the different controls of the main-form according
    // to the current connection status
	procedure SetConnectionStatus(bConnected: Boolean);
    // Returns a LIN error as a formatted string
	function GetFormatedError(error: TLINError) : AnsiString;
    // Shows a messageBox with a LIN error as a formatted string
    procedure MessageBoxErrorShow(error: TLINError);

  public
    { Déclarations publiques }
  end;

var
  Form1: TForm1;

implementation

uses
  Frame_Dlg;

{ MessageStatus }

constructor MessageStatus.Create(linMsg: TLINRcvMsg; listIndex: Integer);
begin
    m_Msg := linMsg;
    m_oldTimeStamp := linMsg.TimeStamp;
    m_iIndex := listIndex;
    m_Count := 1;
    m_bShowPeriod := true;
    m_bWasChanged := false;
end;

procedure MessageStatus.Update(linMsg: TLINRcvMsg);
begin
    m_oldTimeStamp := m_Msg.TimeStamp;
    m_Msg := linMsg;
    m_Count := m_Count + 1;
    m_bWasChanged := true;
end;

function MessageStatus.GetDirectionString : AnsiString;
begin
    Result := '';

    Case m_Msg.Direction of
        dirDisabled:
            Result := SLinDirectionDisabled;
        dirPublisher:
            Result := SLinDirectionPublisher;
        dirSubscriber:
            Result := SLinDirectionSubscriber;
        dirSubscriberAutoLength:
            Result := SLinDirectionAuto;
    End;
end;

function MessageStatus.GetCSTString : AnsiString;
begin
    Result := '';

    case m_Msg.ChecksumType of
        cstCustom:
            Result := SLinCSTCustom;
        cstClassic:
            Result := SLinCSTClassic;
        cstEnhanced:
            Result := SLinCSTEnhanced;
        cstAuto:
            Result := SLinCSTAuto;
    end;
end;

function MessageStatus.GetChecksumString : AnsiString;
begin
    Result :=  Format('%Xh', [m_Msg.Checksum]);
end;

function MessageStatus.GetErrorString : AnsiString;
begin
    Result := '';

    if ( Integer(m_Msg.ErrorFlags) = 0) then
    begin
        Result := 'O.k.';
    end
    else
    begin
        if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_CHECKSUM) <> 0 then
    	    Result := Result + 'Checksum,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_GND_SHORT) <> 0 then
			Result := Result + 'GroundShort,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_ID_PARITY_BIT0) <> 0 then
			Result := Result + 'IdParityBit0,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_ID_PARITY_BIT1) <> 0 then
			Result := Result + 'IdParityBit1,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_INCONSISTENT_SYNC) <> 0 then
			Result := Result + 'InconsistentSynch,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_OTHER_RESPONSE) <> 0 then
			Result := Result + 'OtherResponse,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_SLAVE_NOT_RESPONDING) <> 0 then
			Result := Result + 'SlaveNotResponding,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_SLOT_DELAY) <> 0 then
			Result := Result + 'SlotDelay,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_TIMEOUT) <> 0 then
			Result := Result + 'Timeout,';
		if ( Integer(m_Msg.ErrorFlags) and MSG_ERR_VBAT_SHORT) <> 0 then
			Result := Result + 'VBatShort,';
		// remove ending comma
		if (length(result) > 0) then
            delete(Result, length(Result), 1);
    end;
end;

function MessageStatus.GetProtectedIdString : AnsiString;
begin
    Result := Format('%.03Xh', [m_Msg.FrameId])
end;

function MessageStatus.GetDataString : AnsiString;
var
    i: Integer;
begin
    Result := '';
    for i := 0 to m_Msg.Length - 1 do
        Result := Result + Format('%.02X ', [m_Msg.Data[i]]);
end;

function MessageStatus.GetTimeString : AnsiString;
var
    fTime : Double;
begin
    fTime := m_Msg.TimeStamp;

    if m_bShowPeriod then
        fTime := (fTime - m_oldTimeStamp) / 1000;
    Result := Format('%.0f',[fTime]);
end;

procedure MessageStatus.SetShowingPeriod(value : boolean);
begin
    if m_bShowPeriod xor value then
    begin
        m_bShowPeriod := value;
        m_bWasChanged := true;
    end;
end;


{$R *.dfm}

//////////////////////////////////////////////////////////////////////////////////////////////
{   Form event-handlers }

procedure TForm1.FormCreate(Sender: TObject);
begin
    // Initialize components

    // Create the Global Frame Table and add property change handler on its items
    m_pGFT := CGlobalFrameTable.Create();
    // Populates FrameID combobox with global frame IDs
    UpdateFrameIds();
    // Populates Direction combobox
    cbbDirection.Items.Clear();
    cbbDirection.Items.AddObject(SLinDirectionDisabled, TObject(dirDisabled));
    cbbDirection.Items.AddObject(SLinDirectionPublisher, TObject(dirPublisher));
    cbbDirection.Items.AddObject(SLinDirectionSubscriber, TObject(dirSubscriber));
    cbbDirection.Items.AddObject(SLinDirectionAuto, TObject(dirSubscriberAutoLength));
    // Populates ChecksumType combobox
    cbbCST.Items.Clear();
    cbbCST.Items.AddObject(SLinCSTAuto, TObject(cstAuto));
    cbbCST.Items.AddObject(SLinCSTClassic, TObject(cstClassic));
    cbbCST.Items.AddObject(SLinCSTEnhanced, TObject(cstEnhanced));
    cbbCST.Items.AddObject(SLinCSTCustom, TObject(cstCustom));

    // Initialize the LIN attributes
    m_hClient := 0;
    m_hHw := 0;
    m_lMask := UInt64(FRAME_FILTER_MASK);
    m_HwMode := modSlave;
    m_wBaudrate := 19200;

    // Creates the list for received messages
    m_LastMsgsList := TList.Create();

    // Register this application with LIN as client.
    m_LastLINErr := LIN_RegisterClient(PAnsiChar(PAnsiString(AnsiString(SPLinClientName))), Handle, m_hClient);
end;

procedure TForm1.FormShow(Sender: TObject);
begin
    // initialize default state of components
	btnHwRefresh.Click();	// refresh hardware list combobox
	rdbFilterChanged(nil);
    cbbDirection.ItemIndex := 0;
    cbbCST.ItemIndex := 1;
end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
var
  i: Integer;
begin
    // disconnect from LIN if necessary

    if (m_hClient <> 0) Then
    begin
        DoLinDisconnect();
        m_hHw := 0;
        // Unregister the application
        LIN_RemoveClient(m_hClient);
        m_hClient := 0;
    end;

	// Free ressources initialized upon OnInitDialog event
	m_pGFT.Destroy();
	for i := 0 to m_LastMsgsList.Count - 1 do
        MessageStatus(m_LastMsgsList.Items[i]).Destroy();
	m_LastMsgsList.Clear();
    m_LastMsgsList.Destroy();
end;

//////////////////////////////////////////////////////////////////////////////////////////////
{   Connection event-handlers }

procedure TForm1.cbbChannelChange(Sender: TObject);
var
    lnMode, lnCurrBaud: Integer;
    lwHw: HLINHW;
begin

    lwHw := HLINHW(cbbChannel.Items.Objects[cbbChannel.ItemIndex]);
    if (lwHw <> 0) then
    begin
        btnInit.Enabled := True;
        btnIdent.Enabled := True;
        // Read the mode of the hardware with the handle lwHw (Master, Slave or None).
        m_LastLINErr := LIN_GetHardwareParam(lwHw, hwpMode, Pointer(@lnMode), 0);
        // Read the baudrate of the hardware with the handle lwHw.
        m_LastLINErr := LIN_GetHardwareParam(lwHw, hwpBaudrate, Pointer(@lnCurrBaud), 0);
        // Update hardware mode comboBox
        if (lnMode = Integer(modMaster) ) then
            cbbHwMode.ItemIndex := 1
        else
            cbbHwMode.ItemIndex := 0;
        cbbHwModeChange(nil);
         // Assign the Baudrate to the Control.
        if(lnCurrBaud <> 0) then cbbBaudrates.Text := IntToStr(lnCurrBaud)
        else cbbBaudrates.Text := IntToStr(m_wBaudrate);
    end
    else
    begin
        btnInit.Enabled := False;
        btnIdent.Enabled := False;
    end;
end;

procedure TForm1.cbbHwModeChange(Sender: TObject);
begin               
	// Change the "write" button text depending on the hardware mode
    if (cbbHwMode.ItemIndex = 1) then
        btnWrite.Caption := SWriteAsMaster
    else
        btnWrite.Caption := SWriteAsSlave;
end;

procedure TForm1.btnHwRefreshClick(Sender: TObject);
begin   
	RefreshHardware();
end;   

procedure TForm1.btnIdentClick(Sender: TObject);
var
    lwHw: HLINHW;
begin
    // Get hardware device from the selected combobox item
    lwHw := HLINHW(cbbChannel.Items.Objects[cbbChannel.ItemIndex]);
    if (lwHw <> 0) Then
    begin
        // makes the corresponding PCAN-USB-Pro's LED blink
        LIN_IdentifyHardware(lwHw);
    end;
end;

procedure TForm1.btnInitClick(Sender: TObject);
begin
    // Do the connection
    if (DoLinConnect()) then
    begin
        // Sets the connection status of the main-form
        SetConnectionStatus(True);
        UpdateFrameIds();
        m_pGFT.UpdateLinInfo(m_hClient, m_hHw, m_HwMode, @m_lMask);
    end;
end;

procedure TForm1.btnReleaseClick(Sender: TObject);
begin
    // Releases the current connection
    if (DoLinDisconnect()) then
    begin
        // stop reading timer
        tmrRead.Enabled := False;
        // Sets the connection status of the main-form
        SetConnectionStatus(False);    
        m_pGFT.UpdateLinInfo(m_hClient, m_hHw, m_HwMode, @m_lMask);
    end;
end;

//////////////////////////////////////////////////////////////////////////////////////////////
//	Global frames and filter message event-handlers

procedure TForm1.btnConfigureClick(Sender: TObject);
var
  dlg: TFrameDlg;
begin
    // Open the "Global Frame Table" Dialog
    dlg := TFrameDlg.Create(m_pGFT, self);
    try
        dlg.ShowModal();
    finally
        dlg.Free();
    end;         
    // Output filter information (as changes to Global Frame Table items modify it)
    btnFilterQuery.Click();
    // Update the available frame ids (i.e. the IDs combobox in the "write message" UI group)
    UpdateFrameIds();
end;

procedure TForm1.rdbFilterChanged(Sender: TObject);
begin
    // enable/disable filter fields
    if (rdbFilterCustom.Checked) then
    begin
        txtFilterFrom.Enabled := True;
        txtFilterTo.Enabled := True;
        nudIdFrom.Enabled := True;
        nudIdTo.Enabled := True;
    end
    else
    begin                           
        txtFilterFrom.Enabled := False;
        txtFilterTo.Enabled := False;
        nudIdFrom.Enabled := False;
        nudIdTo.Enabled := False;
    end;
end;

procedure TForm1.btnFilterApplyClick(Sender: TObject);
var
    strTemp: string;
    lMask: UInt64;
    nudFrom, nudTo: Byte;
begin
    if (m_hHw <> 0) then
    begin
    // Sets the according to the selected filter
        if (rdbFilterOpen.Checked = True) then
        begin
            // set no filtering : all frames are received
            lMask := UInt64(FRAME_FILTER_MASK);
            m_LastLINErr := LIN_SetClientFilter(m_hClient, m_hHw, lMask);
            if (m_LastLINErr = errOK) then
            begin
                m_lMask := lMask;
				IncludeTextMessage('The filter was successfully opened.');
            end;
        end
        else if (rdbFilterClose.Checked = True) then
        begin
            // filters all : all frames are denied
            lMask := $0;
            m_LastLINErr := LIN_SetClientFilter(m_hClient, m_hHw, lMask);
            if (m_LastLINErr = errOK) then
            begin
                m_lMask := lMask;
				IncludeTextMessage('The filter was successfully closed.');
            end;
        end
        else
        begin
            // custom filtering (i.e. rdbFilterCustom.Checked) : the frames for the given range will be opened
			nudFrom := Byte(nudIdFrom.Position);
			nudTo := Byte(nudIdTo.Position);
            m_LastLINErr := LIN_RegisterFrameId(m_hClient, m_hHw, nudFrom, nudTo);
            if (m_LastLINErr = errOK) then
            begin
				strTemp := Format('The filter was customized. IDs from %d to %d will be received', [nudFrom, nudTo]);
                IncludeTextMessage(strTemp);
                m_LastLINErr := LIN_GetClientFilter(m_hClient, m_hHw, lMask);
                if (m_LastLINErr = errOK) then
                    m_lMask := lMask;
            end;
        end;

        // If success, an information message is written, if it is not, an error message is shown
        if (m_LastLINErr <> errOK) then
            MessageBoxErrorShow(m_LastLINErr);
    end;
end;

procedure TForm1.btnFilterQueryClick(Sender: TObject);
var
    strTmp: string;
    pRcvMask: UInt64;
    temp: Extended;
    bit: Byte;
begin
    // Retrieves the filter corresponding to the current Client-Hardware pair
    if (m_hHw <> 0) then
    begin
		if (LIN_GetClientFilter(m_hClient, m_hHw, pRcvMask) = errOK) then
        begin
            strTmp := '';
            temp := pRcvMask;
        	// convert to binary string
            For bit := 63 downto 0 do
            begin
                if temp >= Power(2, bit) then
                begin
                    strTmp := strTmp + '1';
                    temp := temp - Power(2, bit);
                end
                else
                    strTmp := strTmp + '0';
            end;
            // add message
            IncludeTextMessage(Format('The Status of the filter is %.064s.', [strTmp]));
		end;
    end;
end;

procedure TForm1.txtFilterFromExit(Sender: TObject);
var
    newValue: Integer;
begin
    // Compute new edited value
	if (txtFilterFrom.Text <> '') then
    	newValue := StrToInt(txtFilterFrom.Text)
    else
        newValue := 0;
        
	if (newValue > 63) then
		newValue := 63
	else if (newValue < 0) then
        newValue := 0;
	// Update Nud control
	nudIdFrom.Position := newValue;
    txtFilterFrom.Text := IntToStr(nudIdFrom.Position);
end;      

procedure TForm1.txtFilterToExit(Sender: TObject);
var
    newValue: Integer;
begin
    // Compute new edited value
    if (txtFilterTo.Text <> '') then
    	newValue := StrToInt(txtFilterTo.Text)
    else
        newValue := 0;

	if (newValue > 63) then
		newValue := 63
	else if (newValue < 0) then
        newValue := 0;
	// Update Nud control
	nudIdTo.Position := newValue;
    txtFilterTo.Text := IntToStr(nudIdTo.Position);
end;

procedure TForm1.txtFilterKeyPress(Sender: TObject; var Key: Char);
begin
    // Convert the Character to its Upper case equivalent
    Key := (UpperCase(Key))[1];

    // The Key is the Delete (Backspace) Key
    if(Ord(Key) = 8)then
        exit;
    // The Key is a number between 0-9
    if((Ord(Key) > 47)AND(Ord(Key) < 58))then
        exit;
    // Is not a number
    Key := #0;

end;

//////////////////////////////////////////////////////////////////////////////////////////////
//	Read message event-handlers

procedure TForm1.rdbTimerChanged(Sender: TObject);
begin
	// According to the kind of reading, a timer or a button will be enabled
	if (btnRelease.Enabled and rdbTimer.Checked) then
		tmrRead.Enabled := True
	else
		tmrRead.Enabled := False;

	btnRead.Enabled := (not rdbTimer.Checked  and btnRelease.Enabled);
end;

procedure TForm1.btnMsgClearClick(Sender: TObject);
begin
    // The information contained in the messages List-View is cleared
	m_LastMsgsList.Clear();
	lstMessages.Clear();
end;

procedure TForm1.btnReadClick(Sender: TObject);
begin
	ReadMessages();
end;

procedure TForm1.chbShowPeriodClick(Sender: TObject);
var
  i: Integer;
  lMsg: MessageStatus;
begin
    for i := 0 to m_LastMsgsList.Count - 1 do
    begin
        lMsg := MessageStatus(m_LastMsgsList.Items[i]);
        lMsg.SetShowingPeriod(chbShowPeriod.Checked);
    end;
end;

//////////////////////////////////////////////////////////////////////////////////////////////
//	Write message event-handlers

procedure TForm1.cbbIDChange(Sender: TObject);
var
    lFD: PCFrameDefinition;
begin
	lFD := PCFrameDefinition(cbbID.Items.Objects[cbbID.ItemIndex]);
    // Check if a global frame is selected
	if (lFD <> nil) then
    begin
		// Update components according to the selected frame informations
		txtID.Text := lFD.ProtectedId;
		txtLength.Text := lFD.LengthAsString;
        nudLength.Position := lFD.Length;

		case lFD.Direction of
            dirDisabled:
				cbbDirection.ItemIndex := 0;
			dirPublisher:
				cbbDirection.ItemIndex := 1;
			dirSubscriber:
				cbbDirection.ItemIndex := 2;
			dirSubscriberAutoLength:
				cbbDirection.ItemIndex := 3;
        end;

        Case lFD.ChecksumType of
            cstAuto:
				cbbCST.ItemIndex := 0;
			cstClassic:
				cbbCST.ItemIndex := 1;
			cstEnhanced:
				cbbCST.ItemIndex := 2;
			cstCustom:
				cbbCST.ItemIndex := 3;
        End;
		// Force update on Data fields (both Length and Direction impact on Enabled state of these components)
		// if length value is not changed but direction switch from subscriber to publisher
		// data textfield would not be disabled
        nudLengthClick(nil, btNext);
	end;

end;

procedure TForm1.nudLengthClick(Sender: TObject; Button: TUDBtnType);
begin
    txtData0.Enabled := False;
    txtData1.Enabled := False;
    txtData2.Enabled := False;
    txtData3.Enabled := False;
    txtData4.Enabled := False;
    txtData5.Enabled := False;
    txtData6.Enabled := False;
    txtData7.Enabled := False;
    // Only Publisher direction allows edition of Data textFields
    if (cbbDirection.ItemIndex <> 1) then
        Exit;                     
    // Enable the same number of data textBox as the nudLength value
    case nudLength.Position of
        1: begin
            txtData0.Enabled := True;
        end;
        2: begin
            txtData0.Enabled := True;
            txtData1.Enabled := True;
        end;
        3: begin
            txtData0.Enabled := True;
            txtData1.Enabled := True;
            txtData2.Enabled := True;
        end;
        4: begin
            txtData0.Enabled := True;
            txtData1.Enabled := True;
            txtData2.Enabled := True;
            txtData3.Enabled := True;
        end;
        5: begin
            txtData0.Enabled := True;
            txtData1.Enabled := True;
            txtData2.Enabled := True;
            txtData3.Enabled := True;
            txtData4.Enabled := True;
        end;
        6: begin
            txtData0.Enabled := True;
            txtData1.Enabled := True;
            txtData2.Enabled := True;
            txtData3.Enabled := True;
            txtData4.Enabled := True;
            txtData5.Enabled := True;
        end;
        7: begin
            txtData0.Enabled := True;
            txtData1.Enabled := True;
            txtData2.Enabled := True;
            txtData3.Enabled := True;
            txtData4.Enabled := True;
            txtData5.Enabled := True;
            txtData6.Enabled := True;
        end;
        8: begin
            txtData0.Enabled := True;
            txtData1.Enabled := True;
            txtData2.Enabled := True;
            txtData3.Enabled := True;
            txtData4.Enabled := True;
            txtData5.Enabled := True;
            txtData6.Enabled := True;
            txtData7.Enabled := True;
        end;
    end;

end;

procedure TForm1.txtData0Exit(Sender: TObject);    
var
    CurrentEdit : TEdit;
begin
    // all the Textbox Data fields are represented with 2 characters.
    // Therefore if the Length of the text is smaller than 2, we add
    // a "0"
    if(AnsiString(Sender.ClassName) = 'TEdit')then
    begin
        CurrentEdit := TEdit(Sender);
        while(Length(CurrentEdit.Text) <> 2)do
            CurrentEdit.Text := ('0' + CurrentEdit.Text);
    end;
end;

procedure TForm1.txtData0KeyPress(Sender: TObject; var Key: Char);
begin
    // We convert the Character to its Upper case equivalent
    Key := (UpperCase(Key))[1];

    // The Key is the Delete (Backspace) Key
    if(Ord(Key) = 8)then
        exit;
    // The Key is a number between 0-9
    if((Ord(Key) > 47)AND(Ord(Key) < 58))then
        exit;
    // The Key is a character between A(a) and F(f)
    if((Ord(Key) > 64)AND(Ord(Key) < 71))then
        exit;

    // Is neither a number nor a character between A(a) and F(f)
    Key := #0;
end;

procedure TForm1.btnWriteClick(Sender: TObject);
var
    pMsg: TLINMsg;
    lFD: PCFrameDefinition;
begin

    // Assert that a frame ID has been selected
    if (cbbID.ItemIndex = -1) then
    begin
		cbbID.SetFocus();
        Exit;
    end;

     // Get the CFrameDefinition associated to the selected FrameID
	lFD := PCFrameDefinition(cbbID.Items.Objects[cbbID.ItemIndex]);
    // Create a new LIN frame message and copy the data.
	pMsg.FrameId := Byte(lFD.ProtectedIdAsInt);
	pMsg.Direction := TLINDirection(cbbDirection.Items.Objects[cbbDirection.ItemIndex]);
	pMsg.ChecksumType := TLINChecksumType(cbbCST.Items.Objects[cbbCST.ItemIndex]);
	pMsg.Length := Byte(nudLength.Position);
    // Fill data array
	pMsg.Data[0] := Byte(StrToInt('0x' + txtData0.Text));
	pMsg.Data[1] := Byte(StrToInt('0x' + txtData1.Text));
	pMsg.Data[2] := Byte(StrToInt('0x' + txtData2.Text));
	pMsg.Data[3] := Byte(StrToInt('0x' + txtData3.Text));
	pMsg.Data[4] := Byte(StrToInt('0x' + txtData4.Text));
	pMsg.Data[5] := Byte(StrToInt('0x' + txtData5.Text));
	pMsg.Data[6] := Byte(StrToInt('0x' + txtData6.Text));
	pMsg.Data[7] := Byte(StrToInt('0x' + txtData7.Text));
    // Check if the hardware is initialize as master
    if (m_HwMode = modMaster) then
    begin
        // Calculate the checksum contained with the
        // checksum type that set some line before.
        LIN_CalculateChecksum(pMsg);
        // Try to send the LIN frame message with LIN_Write.
		m_LastLINErr := LIN_Write(m_hClient, m_hHw, pMsg);
    end
    else
    begin
        // If the hardare is initialize as slave
        // only update the data in the LIN frame.
		m_LastLINErr := LIN_UpdateByteArray(m_hClient, m_hHw, lFD.IdAsByte, Byte(0), pMsg.Length, @pMsg.Data);
    end;
    // Show error if any
    if (m_LastLINErr <> errOK) then
		MessageBoxErrorShow(m_LastLINErr);
end;


//////////////////////////////////////////////////////////////////////////////////////////////
//	Information group event-handlers

procedure TForm1.btnGetVersionsClick(Sender: TObject);
var
    lpVersion: TLINVersion;
	buffer : array[0..255] of AnsiChar;
    info, strTemp: string;
	iPos: Integer;
begin
	// We get the vesion of the PLIN API
    m_LastLINErr := LIN_GetVersion(lpVersion);
    if (m_LastLINErr = errOK) then
    begin
		strTemp := Format('API Version: %d.%d.%d.%d', [lpVersion.Major, lpVersion.Minor, lpVersion.Build, lpVersion.Revision]);
        IncludeTextMessage(strTemp);
        // We get the driver version
        m_LastLINErr := LIN_GetVersionInfo(buffer, 255);
        if (m_LastLINErr = errOK) then
        begin
		    IncludeTextMessage('Channel/Driver Version: ');

            // Because this information contains line control characters (several lines)
            // we split this also in several entries in the Information List-Box
            info := buffer;
            while(info <> '') do
            begin
                iPos := Pos(#$A, info);
				if(iPos = 0) then
					iPos := Length(info);
				IncludeTextMessage('     * ' + Copy(info, 1, iPos));
				Delete(info, 1, iPos);
			end;
        end;
    end;
    // If an error ccurred, a message is shown
    if (m_LastLINErr <> errOK) then
        MessageBoxErrorShow(m_LastLINErr);
end;

procedure TForm1.btnInfoClearClick(Sender: TObject);
begin     
    // The information contained in the Information List-Box is cleared
    lbxInfo.Clear();
end;

procedure TForm1.btnResetClick(Sender: TObject);
begin
    // Flushes the Receive Queue of the Client and resets its counters. 
	m_LastLINErr := LIN_ResetClient(m_hClient);

	// If it fails, a error message is shown
	if (m_LastLINErr <> errOK) then
		MessageBoxErrorShow(m_LastLINErr)
	else
    begin
		// clears the message list-view
		btnMsgClear.Click();
		IncludeTextMessage('Receive queue and counters successfully reset');
    end;
end;

procedure TForm1.btnStatusClick(Sender: TObject);
var
    lStatus: TLINHardwareStatus;
begin
    // Retrieves the status of the LIN Bus and outputs its state in the information listView
    m_LastLINErr := LIN_GetStatus(m_hHw, lStatus);
    if (m_LastLINErr = errOK) then
    begin
        case lStatus.Status of
            hwsNotInitialized: IncludeTextMessage('Hardware: Not Initialized');
            hwsAutobaudrate: IncludeTextMessage('Hardware: Baudrate Detection');
            hwsActive: IncludeTextMessage('Bus: Active');
            hwsSleep: IncludeTextMessage('Bus: Sleep');
            hwsShortGround: IncludeTextMessage('Bus-Line: Shorted Ground');
        end;
    end
	else
		MessageBoxErrorShow(m_LastLINErr);
end;

//////////////////////////////////////////////////////////////////////////////////////////////
//	Timer event-handler & functions

procedure TForm1.tmrDisplayTimer(Sender: TObject);
begin    
    // Update display of received messages
    DisplayMessages();
end;

procedure TForm1.tmrReadTimer(Sender: TObject);
begin     
    // Checks if in the receive-queue are currently messages for read
    ReadMessages();
end;

//////////////////////////////////////////////////////////////////////////////////////////////
//	LIN connection functions

procedure TForm1.RefreshHardware();
var
    i: Byte;
    lwHwHandles: array[0..7] of HLINHW;
    lwBuffSize: Word;
    lwCount: Integer;
    lnHwType, lnDevNo, lnChannel, lnMode: Integer;
    lLINErr: TLINError;
    lwHw: HLINHW;
    str: string;
begin
    lwBuffSize := Length(lwHwHandles);
    lwCount := 0;
    // Get all available LIN hardware.
    lLINErr := LIN_GetAvailableHardware(PHLINHW(@lwHwHandles), lwBuffSize, lwCount);
    if (lLINErr = errOK) Then
    begin
        cbbChannel.Items.Clear();
        // If no error occurs
        if (lwCount = 0) Then
        begin
            // No LIN hardware was found.
            // Show an empty entry
            lwHw := 0;
            cbbChannel.Items.AddObject(SHardwareNothingFound, TObject(lwHw));
        end
        else
        begin
            // For each founded LIN hardware
            for i := 0 to lwCount - 1 do
            begin
                // Get the handle of the hardware.
                lwHw := lwHwHandles[i];
                // Read the type of the hardware with the handle lwHw.
                LIN_GetHardwareParam(lwHw, hwpType, Pointer(@lnHwType), 0);
                // Read the device number of the hardware with the handle lwHw.
                LIN_GetHardwareParam(lwHw, hwpDeviceNumber, Pointer(@lnDevNo), 0);
                // Read the channel number of the hardware with the handle lwHw.
                LIN_GetHardwareParam(lwHw, hwpChannelNumber, Pointer(@lnChannel), 0);
                // Read the mode of the hardware with the handle lwHw (Master, Slave or None).
                LIN_GetHardwareParam(lwHw, hwpMode, Pointer(@lnMode), 0);

                // Create a comboboxItem
                // If the hardware type is a knowing hardware
                // show the name of that in the label of the entry.
                if (lnHwType = LIN_HW_TYPE_USB) then
                    str := SHardwareTypeLIN
                else
                    // Show as unknown hardware
                    str := SHardwareTypeUnkown;
                str := Format('%s - dev. %d, chan. %d', [str, lnDevNo, lnChannel]);
                cbbChannel.Items.AddObject(str, TObject(lwHw));
            end;
        end;
        cbbChannel.ItemIndex := 0;
        cbbChannelChange(cbbChannel);   // call change selection
    end;

end;

function TForm1.DoLinConnect() : Boolean;
var
    lwHw: HLINHW;
    lwBaud: Word;
    lnMode, lnCurrBaud: Integer;
    lHwMode: TLINHardwareMode;
    lhClients: array[0..254] of Byte;
begin
    // Initialization
    Result := False;

    if (m_hHw <> 0) then
    begin
        // If a connection to hardware already exits
        // disconnect this connection first.
        if (DoLinDisconnect() = False) then
            exit;
    end;
    // Get the selected Hardware handle from the comboboxItem
    lwHw := HLINHW(cbbChannel.Items.Objects[cbbChannel.ItemIndex]);
    if (lwHw <> 0) then
    begin
        m_LastLINErr := LIN_GetHardwareParam(lwHw, hwpConnectedClients, Pointer(@lhClients), Length(lhClients));
        // The local hardware handle is valid.
        // Get the current mode of the hardware
        LIN_GetHardwareParam(lwHw, hwpMode, Pointer(@lnMode), 0);
        // Get the current baudrate of the hardware
        LIN_GetHardwareParam(lwHw, hwpBaudrate, Pointer(@lnCurrBaud), 0);
        // Try to connect the application client to the
        // hardware with the local handle.
        m_LastLINErr := LIN_ConnectClient(m_hClient, lwHw);
        if (m_LastLINErr = errOK) then
        begin
            // If the connection successfull
            // assign the local handle to the
            // member handle.
            m_hHw := lwHw;
            // Get the selected hardware mode.
            if (cbbHwMode.ItemIndex = 1) then
                lHwMode := modMaster
            else
                lHwMode := modSlave;

            // Get the selected baudrate
            lwBaud := StrToIntDef(cbbBaudrates.Text, 0);
            // Get the selected hardware channel
            if ((lnMode = Integer(modNone)) Or (Word(lnCurrBaud) <> lwBaud)) then
            begin
                // Only if the current hardware is not initialize
                // try to Intialize the hardware with mode and baudrate
                m_LastLINErr := LIN_InitializeHardware(m_hClient, m_hHw, lHwMode, lwBaud)
            end;
            if (m_LastLINErr = errOK) then
            begin
                // Assign the Hardware Mode to member attribut
                m_HwMode := lHwMode;
                // Assign the baudrate index to member attribut
                m_wBaudrate := lwBaud;
                // Set the client filter with the mask.
                m_LastLINErr := LIN_SetClientFilter(m_hClient, m_hHw, m_lMask);
                // Read the frame table from the connected hardware.
                ReadFrameTableFromHw();
                // Reset the last LIN error code to default.
                m_LastLINErr := errOK;
                Result := True;
            end
            else
            begin
                // An error occured while initializing hardware.
                // Set the member variable to default.
                m_hHw := 0;
                Result := False;
            end;
        end
        else
        begin
            // The local hardware handle is invalid
            // and/or an error occurs while connecting
            // hardware with client.
            // Set the member variable to default.
            m_hHw := 0;
            Result := False;
        end;

        // Check if any LIN error code was received.
        if (m_LastLINErr <> errOK) Then
        begin
            MessageBoxErrorShow(m_LastLINErr);
            Result := False;
        end;
    end
    else
        m_hHw := 0;

end;

function TForm1.DoLinDisconnect() : Boolean;
var
    lfOtherClient: Boolean;
    lfOwnClient: Boolean;
    lhClients: array[0..254] of Byte;
    i: Integer;
begin
    Result := False;

    // If the application was registered with LIN as client.
    if (m_hHw <> 0) then
    begin
        // The client was connected to a LIN hardware.
        // Before disconnect from the hardware check
        // the connected clients and determine if the
        // hardware configuration have to reset or not.

        // Initialize the locale variables.
        lfOtherClient := False;
        lfOwnClient := False;

        // Get the connected clients from the LIN hardware.
        m_LastLINErr := LIN_GetHardwareParam(m_hHw, hwpConnectedClients, Pointer(@lhClients), Length(lhClients));
        if (m_LastLINErr = errOK) then
        begin
            // No errors !
            // Check all client handles.
            for i := 0 to Length(lhClients) - 1 do
            begin
                // If client handle is invalid
                If (lhClients[i] = 0) Then
                    Continue;
                // Set the boolean to true if the handle isn't the
                // handle of this application.
                // Even the boolean is set to true it can never
                // set to false.
                lfOtherClient := lfOtherClient Or (lhClients[i] <> m_hClient);
                // Set the boolean to true if the handle is the
                // handle of this application.
                // Even the boolean is set to true it can never
                // set to false.
                lfOwnClient := lfOwnClient Or (lhClients[i] = m_hClient);
            end;
        end;
        // If another application is also connected to
        // the LIN hardware do not reset the configuration.
        if (lfOtherClient = False) then
        begin
            // No other application connected !
            // Reset the configuration of the LIN hardware.
            LIN_ResetHardwareConfig(m_hClient, m_hHw);
        end;
        // If this application is connected to the hardware
        // then disconnect the client. Otherwise not.
        if (lfOwnClient = True) then
        begin
            // Disconnect if the application was connected to a LIN hardware.
            m_LastLINErr := LIN_DisconnectClient(m_hClient, m_hHw);
            if (m_LastLINErr = errOK) then
            begin
                m_hHw := 0;
                Result := True;
            end
            else
            begin
                // Error while disconnecting from hardware.
                MessageBoxErrorShow(m_LastLINErr);
                Result := False;
            end;
        end;
    end
    else
            Result := True;
end;

procedure TForm1.ReadFrameTableFromHw();
var
    i, lnCnt: Integer;
    lFrameEntry: TLINFrameEntry;
    lErr: TLINError;
    llMask: UInt64;
begin
    // Get the count of Frame Definition from the
    // Global Frame Table.
    lnCnt := m_pGFT.Count;
    // Initialize the member attribute for the
    // client mask with 0.
    m_lMask := 0;
    //Each Frame Definition
    for i := 0 to lnCnt - 1 do
    begin
        // Before a frame entry can be read from the
        // hardware, the Frame-ID of the wanted entry
        // must be set
        lFrameEntry.FrameId := m_pGFT[i].IdAsByte;
        // Read the information of the specified frame entry from the hardware.
        lErr := LIN_GetFrameEntry(m_hHw, lFrameEntry);
        // Check the result value of the LinApi function call.
        if (lErr = errOK) then
        begin
            // LinApi function call successfull.
            // Copy the frame entry information to the Frame Definition.
            m_pGFT[i].setLength(lFrameEntry.Length);
            m_pGFT[i].setDirection(lFrameEntry.Direction);
            m_pGFT[i].setChecksumType(lFrameEntry.ChecksumType);
            if (m_pGFT[i].Direction <> dirDisabled) then
            begin
                // If the direction is not disabled than set the
                // bit in the client filter mask.
                llMask := (UInt64(1) shl i) And FRAME_FILTER_MASK;
                m_lMask := m_lMask Or llMask;
            end;
        end;
    end;
end;


///////////////////////////////////////////////////////////////////////////
// LIN Message processing functions

procedure TForm1.ReadMessages();
var
    lpMsg: TLINRcvMsg;
begin                           
    // We read at least one time the queue looking for messages.
    // If a message is found, we look again trying to find more.
    // If the queue is empty or an error occurs, we get out from
    // the dowhile statement.
    //	
    repeat
    begin
        m_LastLINErr := LIN_Read(m_hClient, lpMsg);
        // If at least one Frame is received by the LinApi.
        // Check if the received frame is a standard type.
        // If it is not a standard type than ignore it.
		if (lpMsg.MsgType <> mstStandard) then
            Continue;

        if (m_LastLINErr = errOK) then
            ProcessMessage(lpMsg);
    end;
    until (not btnRelease.Enabled) or (Integer(m_LastLINErr) and Integer(errRcvQueueEmpty) <> 0);
end;

procedure TForm1.ProcessMessage(linMsg: TLINRcvMsg);
var
  i: Integer;
  lMsg: MessageStatus;
begin
	// We search if a message (Same ID and Type) has
    // already been received or if this is a new message
    //
    for i := 0 to m_LastMsgsList.Count - 1 do
    begin
		lMsg := MessageStatus(m_LastMsgsList.Items[i]);
		if (lMsg.LINMsg.FrameId = linMsg.FrameId) then
        begin
            // Modify the message and exit
            //
            lMsg.Update(linMsg);
            Exit;
        end;
    end;
    // Message not found. It will be created
    //
    InsertMsgEntry(linMsg);
end;

procedure TForm1.InsertMsgEntry(newMsg: TLINRcvMsg);
var
  i: Integer;
  lMsg: MessageStatus;
  strId: string;
  item: TListItem;
begin
    // We add this status in the last message list
    //
	lMsg := MessageStatus.Create(newMsg, lstMessages.Items.Count);
    m_LastMsgsList.Add(lMsg);
    
    // Search and retrieve the ID in the global frame table associated with the frame Protected-ID
    strId := '';
    for i := 0 to m_pGFT.Count - 1 do
    begin
        if (lMsg.LINMsg.FrameId = Byte(m_pGFT[i].ProtectedIdAsInt)) then
        begin
            strId := m_pGFT[i].Id;
            Break;
        end;
    end;

    // Add the new ListView Item with the ID of the message
    item := lstMessages.Items.Add();
    item.Caption := strId;
    // We set the length of the message
	item.SubItems.Add(IntToStr(lMsg.LINMsg.Length));
    // We set the data of the message.
	item.SubItems.Add(lMsg.DataString);
    // we set the message count message (this is the First, so count is 1)
	item.SubItems.Add(IntToStr(lMsg.Count));
    // Add time stamp information if needed
	item.SubItems.Add(lMsg.TimeString);
    // We set the direction of the message
	item.SubItems.Add(lMsg.DirectionString);
    // We set the error of the message
	item.SubItems.Add(lMsg.ErrorString);
    // We set the CST of the message
	item.SubItems.Add(lMsg.CSTString);
    // We set the CRC of the message
	item.SubItems.Add(lMsg.ChecksumString);
end;

procedure TForm1.DisplayMessages();    
var
  i: Integer; 
  lMsg: MessageStatus;
begin

    for i := 0 to m_LastMsgsList.Count - 1 do
    begin                       
		lMsg := MessageStatus(m_LastMsgsList.Items[i]);
        if (lMsg.MarkedAsUpdated) then
        begin
            lMsg.MarkedAsUpdated := False;
            // Update the length of the message
        	lstMessages.Items[i].SubItems[0] := IntToStr(lMsg.LINMsg.Length);
			// Update the data of the message.
	        lstMessages.Items[i].SubItems[1] := lMsg.DataString;
			// Update the message count message
	        lstMessages.Items[i].SubItems[2] := IntToStr(lMsg.Count);
			// Update time stamp information
            lstMessages.Items[i].SubItems[3] := lMsg.TimeString;
			// Update the direction of the message
            lstMessages.Items[i].SubItems[4] := lMsg.DirectionString;
			// Update the error of the message
            lstMessages.Items[i].SubItems[5] := lMsg.ErrorString;
			//  Update the CST of the message
            lstMessages.Items[i].SubItems[6] := lMsg.CSTString;
			// Update the CRC of the message
            lstMessages.Items[i].SubItems[7] := lMsg.ChecksumString;
        end;

    end;
end;

///////////////////////////////////////////////////////////////////////////
// Helping functions

procedure TForm1.UpdateFrameIds();
var
    lFrame, lSelectedFrame: PCFrameDefinition;
    i: Integer;
begin
    lSelectedFrame := nil;
	// Retrieves selected ID if it exists, before clearing comboBox
	if	(cbbID.ItemIndex >= 0) then
    begin
		lSelectedFrame := PCFrameDefinition(cbbID.Items.Objects[cbbID.ItemIndex]);
    end;

    // Clears and populates FrameID combobox with global frame IDs
	cbbID.Clear();
	for i := 0 to m_pGFT.Count - 1 do
    begin
		lFrame := m_pGFT[i];
        // add only non disabled frames
		if (lFrame.Direction = dirDisabled) then
            Continue;
		// Add frame and data
		cbbID.AddItem(lFrame.Id, TObject(lFrame));
        // Check if the new item was selected before the update
        if (lSelectedFrame = lFrame) then
			cbbID.ItemIndex := cbbID.Items.Count - 1;
    end;
end;

procedure TForm1.IncludeTextMessage(strMsg: AnsiString);
begin
    lbxInfo.Items.Add(strMsg);
    lbxInfo.ItemIndex := lbxInfo.Items.Count - 1;
end;

procedure TForm1.SetConnectionStatus(bConnected: Boolean);
begin
    // Buttons
    btnInit.Enabled := Not bConnected;
    btnConfigure.Enabled := bConnected;
    btnRead.Enabled := bConnected And rdbManual.Checked;
    btnWrite.Enabled := bConnected;
    btnRelease.Enabled := bConnected;
    btnFilterApply.Enabled := bConnected;
    btnFilterQuery.Enabled := bConnected;
    btnGetVersions.Enabled := bConnected;
    btnHwRefresh.Enabled := Not bConnected;
    btnStatus.Enabled := bConnected;
    btnReset.Enabled := bConnected;

    // ComboBoxs
    cbbBaudrates.Enabled := Not bConnected;
    cbbChannel.Enabled := Not bConnected;
    cbbHwMode.Enabled := Not bConnected;

    // Hardware configuration and read mode
    if (not bConnected) then
        cbbChannelChange(nil)
    else
        rdbTimerChanged(nil);

    // Display messages in grid
    tmrDisplay.Enabled := bConnected;
end;

function TForm1.GetFormatedError(error: TLINError) : AnsiString;
var
	buffer : array[0..255] of Ansichar;
begin
    Result := '';
    // If any error are occured
    // display the error text in a message box.
    // 0x00 = Neutral
    // 0x07 = Language German
    // 0x09 = Language English
    if (LIN_GetErrorText(error, $09, buffer, 255) <> errOK) then
        Result := Format('An error occurred. Error-code''s text (%d) couldn''t be retrieved', [Integer(error)])
    else           
		Result := buffer;

end;

procedure TForm1.MessageBoxErrorShow(error: TLINError);begin
    MessageBox(0, PAnsiChar(GetFormatedError(error)), PAnsiChar(SAppCaptionError), MB_ICONERROR);
end;

end.
