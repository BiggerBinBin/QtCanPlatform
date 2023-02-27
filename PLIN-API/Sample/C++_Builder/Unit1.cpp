//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "UnitFrameDlg.h"
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define	    SAppCaption             "PLIN-API Example"
#define	    SAppCaptionError        "PLIN-API Example - Error"
#define     SHardwareNothingFound   "<No hardware found>"
#define     SHardwareTypeLIN        "PCAN-USB Pro"
#define		SHardwareTypeUnkown     "Unknown"
#define    	SLinCSTAuto             "Auto"
#define     SLinCSTClassic          "Classic"
#define     SLinCSTCustom           "Custom"
#define     SLinCSTEnhanced         "Enhanced"
#define     SLinDirectionAuto       "Subscriber Automatic Length"
#define     SLinDirectionDisabled   "Disabled"
#define     SLinDirectionPublisher  "Publisher"
#define     SLinDirectionSubscriber "Subscriber"
#define     SLINStatusActive        "Bus: Active"
#define     SLINStatusBaudDetect    "Hardware: Baudrate Detection"
#define     SLINStatusNotInit       "Hardware: Not Initialized"
#define     SLINStatusShortGround   "Bus-Line: Shorted Ground"
#define     SLINStatusSleep         "Bus: Sleep"
#define     SPLinClientName         "PLIN_EXAMPLE"
#define     SWriteAsMaster          "Transmit"
#define     SWriteAsSlave           "Publish"

//////////////////////////////////////////////////////////////////////////////////////////////
// CMessageStatus class
//

CMessageStatus::CMessageStatus(TLINRcvMsg linMsg, int listIndex) :
	m_Msg(linMsg),
	m_oldTimeStamp(linMsg.TimeStamp),
	m_iIndex(listIndex),
	m_Count(1),
	m_bShowPeriod(true),
	m_bWasChanged(false)
{
}

void CMessageStatus::Update(TLINRcvMsg linMsg)
{
	m_oldTimeStamp = m_Msg.TimeStamp;
	m_Msg = linMsg;

	m_Count += 1;
	m_bWasChanged = true;
}

TLINRcvMsg CMessageStatus::getLINMsg()
{
	return m_Msg;
}
int CMessageStatus::getPosition()
{
	return m_iIndex;
}
AnsiString CMessageStatus::getDirectionString()
{
	switch (m_Msg.Direction)
    {
        case dirDisabled:
			return "Disabled";
		case dirPublisher:
			return "Publisher";
		case dirSubscriber:
			return "Subscriber";
		case dirSubscriberAutoLength:
			return "Subscriber Automatic Length";
    }
	return "";
}
AnsiString CMessageStatus::getCSTString()
{
	switch (m_Msg.ChecksumType)
    {
        case cstAuto:
			return "Auto";
		case cstClassic:
			return "Classic";
		case cstEnhanced:
			return "Enhanced";
        case cstCustom:
			return "Custom";
    }
	return "";
}
AnsiString CMessageStatus::getChecksumString()
{
	return AnsiString::Format("%Xh", ARRAYOFCONST((m_Msg.Checksum)));
}
AnsiString CMessageStatus::getErrorString()
{
	AnsiString result;
	if (m_Msg.ErrorFlags == 0)
		result = "O.k.";
	else
	{
		if (m_Msg.ErrorFlags & MSG_ERR_CHECKSUM)
			result += "Checksum,";
		if (m_Msg.ErrorFlags & MSG_ERR_GND_SHORT)
			result += "GroundShort,";
		if (m_Msg.ErrorFlags & MSG_ERR_ID_PARITY_BIT0)
			result += "IdParityBit0,";
		if (m_Msg.ErrorFlags & MSG_ERR_ID_PARITY_BIT1)
			result += "IdParityBit1,";
		if (m_Msg.ErrorFlags & MSG_ERR_INCONSISTENT_SYNC)
			result += "InconsistentSynch,";
		if (m_Msg.ErrorFlags & MSG_ERR_OTHER_RESPONSE)
			result += "OtherResponse,";
		if (m_Msg.ErrorFlags & MSG_ERR_SLAVE_NOT_RESPONDING)
			result += "SlaveNotResponding,";
		if (m_Msg.ErrorFlags & MSG_ERR_SLOT_DELAY)
			result += "SlotDelay,";
		if (m_Msg.ErrorFlags & MSG_ERR_TIMEOUT)
			result += "Timeout,";
		if (m_Msg.ErrorFlags & MSG_ERR_VBAT_SHORT)
			result += "VBatShort,";
		// remove ending comma
		if (result.Length() > 0)
			result.Delete(result.Length()-1, 1);
	}
	return result;
}
AnsiString CMessageStatus::getPIdString()
{
	return AnsiString::Format("%.03Xh", ARRAYOFCONST((m_Msg.FrameId)));
}
AnsiString CMessageStatus::getDataString()
{
	AnsiString result = "";
		
    for (BYTE i = 0; i < m_Msg.Length; i++)
    {
		result += AnsiString::Format("%.02X ", ARRAYOFCONST((m_Msg.Data[i])));
}
	return result;
}
int CMessageStatus::getCount()
{
	return m_Count;
}
bool CMessageStatus::getShowingPeriod()
{
	return m_bShowPeriod;
}
void CMessageStatus::setShowingPeriod(bool value)
{
	m_bShowPeriod = value;
	setMarkedAsUpdated(true);
}
bool CMessageStatus::isMarkedAsUpdated()
{
	return m_bWasChanged;
}
void CMessageStatus::setMarkedAsUpdated(bool value)
{
	m_bWasChanged = value;
}
AnsiString CMessageStatus::getTimeString()
{
	AnsiString result;
	char buffer[65];
	unsigned __int64 time;

	memset(buffer,'\0',64);
    time = m_Msg.TimeStamp;
    if (m_bShowPeriod)
        time = (time - m_oldTimeStamp) / 1000;
	_ui64toa(time, buffer, 10);
	result = Format("%s", ARRAYOFCONST((buffer)));
    
	return result;
}
    
//////////////////////////////////////////////////////////////////////////////////////////////
// TForm1 class
//

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	InitializeControls();
            
	// Create a PLin Api Class instance
	m_objPLinApi = new PLinApiClass();
	// Create Global Frame table (LIN information are set upon connection)
	m_pGFT = new CGlobalFrameTable(m_objPLinApi);
	// Populates FrameID combobox with global frame IDs
	UpdateFrameIds();       
	
	// Initialize the LIN attributes
    m_hClient = 0;
	m_hHw = 0;
	m_lMask = FRAME_FILTER_MASK;
    m_HwMode = modSlave;
	m_wBaudrate = 19200;

	// Create a list to store the displayed messages
	m_LastMsgsList = new TList();

	// Register this application with LIN as client.
	m_LastLINErr = m_objPLinApi->RegisterClient("PLIN-EXAMPLE", (DWORD)Handle, &m_hClient);
}
//---------------------------------------------------------------------------
void TForm1::InitializeControls(void)
{
	// Populates Direction combobox
	cbbDirection->Items->Clear();
	cbbDirection->Items->AddObject(SLinDirectionDisabled, (TObject*)dirDisabled);
	cbbDirection->Items->AddObject(SLinDirectionPublisher, (TObject*)dirPublisher);
	cbbDirection->Items->AddObject(SLinDirectionSubscriber, (TObject*)dirSubscriber);
	cbbDirection->Items->AddObject(SLinDirectionAuto, (TObject*)dirSubscriberAutoLength);   
    // Populates ChecksumType combobox
	cbbCST->Items->Clear();
	cbbCST->Items->AddObject(SLinCSTAuto, (TObject*)cstAuto);
	cbbCST->Items->AddObject(SLinCSTClassic, (TObject*)cstClassic);
	cbbCST->Items->AddObject(SLinCSTEnhanced, (TObject*)cstEnhanced);
	cbbCST->Items->AddObject(SLinCSTCustom, (TObject*)cstCustom);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
	// initialize default state of components
	btnHwRefreshClick(NULL);	// refresh hardware list combobox
	rdbFilterChanged(NULL);
	cbbDirection->ItemIndex = 0;
	cbbCST->ItemIndex = 1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	// disconnect from LIN if necessary
    if (m_hClient != 0)
    {
        DoLinDisconnect();
        m_hHw = 0;
        // Unregister the application
        m_objPLinApi->RemoveClient(m_hClient);
        m_hClient = 0;
    }

	// Free ressources initialized upon OnInitDialog event
	delete m_pGFT;
	delete m_objPLinApi;		
	for (int i = 0 ; i < m_LastMsgsList->Count ; i++)
		delete m_LastMsgsList->Items[i];
	delete m_LastMsgsList;
}
//---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////
//   Connection event-handlers

void __fastcall TForm1::cbbChannelChange(TObject *Sender)
{
	int lnMode, lnCurrBaud;
	AnsiString strBaudRate;
	// get the handle from the comboBoxItem
	HLINHW lwHw = (HLINHW) cbbChannel->Items->Objects[cbbChannel->ItemIndex];

	if (lwHw != 0)
	{
		btnInit->Enabled = True;
		btnIdent->Enabled = True;
		// Read the mode of the hardware with the handle lwHw (Master, Slave or None).
		m_LastLINErr = m_objPLinApi->GetHardwareParam(lwHw, hwpMode, &lnMode, 0);
		// Read the baudrate of the hardware with the handle lwHw.
		m_LastLINErr = m_objPLinApi->GetHardwareParam(lwHw, hwpBaudrate, &lnCurrBaud, 0);

		// Update hardware mode comboBox
		if (lnMode == modMaster)
			cbbHwMode->ItemIndex = 1;
		else
			cbbHwMode->ItemIndex = 0;
		cbbHwModeChange(NULL);
		// Assign the Baudrate to the Control.
		cbbBaudRates->Text = (lnCurrBaud != 0) ? IntToStr(lnCurrBaud) : IntToStr(m_wBaudrate);
	}
	else
	{
		btnInit->Enabled = False;
		btnIdent->Enabled = False;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnHwRefreshClick(TObject *Sender)
{
	RefreshHardware();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cbbHwModeChange(TObject *Sender)
{
	// Change the "write" button text depending on the hardware mode
	if (cbbHwMode->ItemIndex == 1)
		btnWrite->Caption = SWriteAsMaster;
	else
		btnWrite->Caption = SWriteAsSlave;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnIdentClick(TObject *Sender)
{
	 // Get hardware device from the selected combobox item
	HLINHW lwHw = (HLINHW) cbbChannel->Items->Objects[cbbChannel->ItemIndex];
	if (lwHw != 0) {
		// makes the corresponding PCAN-USB-Pro's LED blink
		m_objPLinApi->IdentifyHardware(lwHw);
	}                                        
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnInitClick(TObject *Sender)
{
	// Do the connection
	if (DoLinConnect())
	{
		// Sets the connection status of the main-form
		SetConnectionStatus(true);
		// Update Global Frame Table LIN information
		m_pGFT->UpdateLinInfo(m_hClient, m_hHw, m_HwMode, &m_lMask);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnReleaseClick(TObject *Sender)
{
	if (DoLinDisconnect()) 
	{
		// Stop reading timer
		tmrRead->Enabled = False;
		// Sets the connection status of the main-form
        SetConnectionStatus(false);
		// Update Global Frame Table LIN information
		m_pGFT->UpdateLinInfo(m_hClient, m_hHw, m_HwMode, &m_lMask);
	}
}
//---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////
//	Global frames and filter message event-handlers

void __fastcall TForm1::btnConfigureClick(TObject *Sender)
{              
    // Open the "Global Frame Table" Dialog
	TFrameDlg * dlg = new TFrameDlg(m_pGFT, this);
	dlg->ShowModal();
	delete dlg;        
	// Output filter information (as changes to Global Frame Table items modify it)
	btnFilterQueryClick(NULL);         
    // Update the available frame ids (i.e. the IDs combobox in the "write message" UI group)
	UpdateFrameIds();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rdbFilterChanged(TObject *Sender)
{
	// enable/disable filter fields
	if (rdbFilterCustom->Checked)
	{
		nudIdFrom->Enabled = true;
		nudIdTo->Enabled = true;
		txtFilterFrom->Enabled = true;
		txtFilterTo->Enabled = true;
	}
	else
	{
		nudIdFrom->Enabled = false;
		nudIdTo->Enabled = false;
		txtFilterFrom->Enabled = false;
		txtFilterTo->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::txtFilterFromExit(TObject *Sender)
{
	int newValue;

	// Compute new edited value
	if (txtFilterFrom->Text != "")
		newValue = StrToInt(txtFilterFrom->Text);
	else
		newValue = 0;

	if (newValue > 63)
		newValue = 63;
	else if (newValue < 0)
		newValue = 0;
	// Update Nud control
	nudIdFrom->Position = newValue;
	txtFilterFrom->Text = IntToStr(nudIdFrom->Position);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::txtFilterToExit(TObject *Sender)
{
	int newValue;

	// Compute new edited value
	if (txtFilterTo->Text != "")
		newValue = StrToInt(txtFilterTo->Text);
	else
		newValue = 0;              

	if (newValue > 63)
		newValue = 63;
	else if (newValue < 0)
		newValue = 0;
	// Update Nud control
	nudIdTo->Position = newValue;
	txtFilterTo->Text = IntToStr(nudIdTo->Position);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::txtFilterKeyPress(TObject *Sender, char &Key)
{
	// Convert the Character to its Upper case equivalent
	Key = *(UpperCase((AnsiString)Key)).c_str();

	// The Key is the Delete (Backspace) Key
	if(Key == 8)
		return;
	// The Key is a number between 0-9
	if((Key > 47)&&(Key < 58))
		return;

	// Is not a number
	Key = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnFilterApplyClick(TObject *Sender)
{
	AnsiString strTemp;
	unsigned __int64 lMask;
	Byte nudFrom, nudTo;

	if (m_hHw != 0)
	{
		// Sets the according to the selected filter
		if (rdbFilterOpen->Checked == true)
		{
			// set no filtering : all frames are received
			lMask = FRAME_FILTER_MASK;
			m_LastLINErr = m_objPLinApi->SetClientFilter(m_hClient, m_hHw, lMask);
			if (m_LastLINErr == errOK)
			{
				m_lMask = lMask;
				IncludeTextMessage("The filter was successfully opened.");
			}
		}
		else if (rdbFilterClose->Checked == true)
		{
			// filters all : all frames are denied
			lMask = 0x0;
			m_LastLINErr = m_objPLinApi->SetClientFilter(m_hClient, m_hHw, lMask);
			if (m_LastLINErr == errOK)
			{
				m_lMask = lMask;
				IncludeTextMessage("The filter was successfully closed.");
			}
		}
		else
		{
			// custom filtering (i.e. rdbFilterCustom.Checked) : the frames for the given range will be opened
			nudFrom = (Byte) nudIdFrom->Position;
			nudTo = (Byte) nudIdTo->Position;
			m_LastLINErr = m_objPLinApi->RegisterFrameId(m_hClient, m_hHw, nudFrom, nudTo);
			if (m_LastLINErr == errOK)
			{
				strTemp = AnsiString::Format("The filter was customized. IDs from %d to %d will be received", ARRAYOFCONST((nudFrom, nudTo)));
				IncludeTextMessage(strTemp);
				m_LastLINErr = m_objPLinApi->GetClientFilter(m_hClient, m_hHw, &lMask);
				if (m_LastLINErr == errOK)
					m_lMask = lMask;
			}
		}           
		// If success, an information message is written, if it is not, an error message is shown
		if (m_LastLINErr != errOK)
			MessageBox(Handle, GetFormatedError(m_LastLINErr).c_str(), SAppCaptionError, MB_ICONERROR);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnFilterQueryClick(TObject *Sender)
{
	AnsiString strTmp;
	unsigned __int64 pRcvMask;
	Extended temp;
	int i;

	// Retrieves the filter corresponding to the current Client-Hardware pair
	if (m_hHw != 0)
	{
		if (m_objPLinApi->GetClientFilter(m_hClient, m_hHw, &pRcvMask) == errOK)
		{
			double power = 0;
			strTmp = "";
			temp = pRcvMask;
			// convert to binary string
			for (i = 63; i > -1; i--) {
				power = pow(2, i);
				if (temp >= power )
				{
					strTmp = strTmp + "1";
					temp = temp - power;
				}
				else
					strTmp = strTmp + "0";
			}
			// add message
			IncludeTextMessage(AnsiString::Format("The Status of the filter is %.064s.", ARRAYOFCONST((strTmp))));
		}
	}
}
//---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////
//	Read message event-handlers

void __fastcall TForm1::rdbTimerChanged(TObject *Sender)
{
	// According to the kind of reading, a timer or a button will be enabled
	if (btnRelease->Enabled && rdbTimer->Checked)
		tmrRead->Enabled = true;
	else
		tmrRead->Enabled = false;

	btnRead->Enabled = (!rdbTimer->Checked && btnRelease->Enabled);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::chbShowPeriodClick(TObject *Sender)
{
	CMessageStatus* lMsg;

	for (int i = 0; i < m_LastMsgsList->Count; i++) {
		lMsg = (CMessageStatus*) (m_LastMsgsList->Items[i]);
		lMsg->setShowingPeriod(chbShowPeriod->Checked);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnReadClick(TObject *Sender)
{
	ReadMessages();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnMsgClearClick(TObject *Sender)
{
    // The information contained in the messages List-View is cleared
	m_LastMsgsList->Clear();
	lstMessages->Clear();
}
//---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////
//	Timer event-handlers

void __fastcall TForm1::tmrReadTimer(TObject *Sender)
{
    // Checks if in the receive-queue are currently messages for read
	ReadMessages();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tmrDisplayTimer(TObject *Sender)
{
    // Update display of received messages
    DisplayMessages();
}
//---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////////////
//	Write message event-handlers

//---------------------------------------------------------------------------
void __fastcall TForm1::cbbIDChange(TObject *Sender)
{
	CFrameDefinition* lFD;

	lFD = (CFrameDefinition*) cbbID->Items->Objects[cbbID->ItemIndex];
    // Check if a global frame is selected
	if (lFD != NULL)
	{
		// Update components according to the selected frame informations
		txtID->Text = lFD->ProtectedId;
		txtLength->Text = lFD->LengthAsString;
		nudLength->Position = lFD->Length;

		switch(lFD->Direction)
		{
			case dirDisabled:
				cbbDirection->ItemIndex = 0;
				break;
			case dirPublisher:
				cbbDirection->ItemIndex = 1;
				break;
			case dirSubscriber:
				cbbDirection->ItemIndex = 2;
				break;
			case dirSubscriberAutoLength:
				cbbDirection->ItemIndex = 3;
				break;
		}

		switch (lFD->ChecksumType)
		{
			case cstAuto:
				cbbCST->ItemIndex = 0;
				break;
			case cstClassic:
				cbbCST->ItemIndex = 1;
				break;
			case cstEnhanced:
				cbbCST->ItemIndex = 2;
				break;
			case cstCustom:
				cbbCST->ItemIndex = 3;
				break;
		}
		// Force update on Data fields (both Length and Direction impact on Enabled state of these components)
		// if length value is not changed but direction switch from subscriber to publisher
		// data textfield would not be disabled
		nudLengthClick(NULL, btNext);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::txtData0KeyPress(TObject *Sender, char &Key)
{
	// We convert the Character to its Upper case equivalent
	//
	Key = *(UpperCase((AnsiString)Key)).c_str();

	// The Key is the Delete (Backspace) Key
	//
	if(Key == 8)
		return;
	// The Key is a number between 0-9
	//
	if((Key > 47)&&(Key < 58))
		return;
	// The Key is a character between A-F
	//
	if((Key > 64)&&(Key < 71))
		return;

	// Is neither a number nor a character between A(a) and F(f)
	//
	Key = NULL;
}    
//---------------------------------------------------------------------------
void __fastcall TForm1::txtData0Exit(TObject *Sender)
{
	TEdit *CurrentEdit;

	// Checks the length of the given value
	if(String(Sender->ClassName()) == "TEdit"){
		CurrentEdit = (TEdit*)Sender;
		while(CurrentEdit->Text.Length() != 2)
            CurrentEdit->Text = ("0" + CurrentEdit->Text);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnWriteClick(TObject *Sender)
{
	TLINMsg pMsg;
	CFrameDefinition* lFD;

	// Assert that a frame ID has been selected
	if (cbbID->ItemIndex == -1)
	{
		cbbID->SetFocus();
		return;
    }
     // Get the CFrameDefinition associated to the selected FrameID
	lFD = (CFrameDefinition*) cbbID->Items->Objects[cbbID->ItemIndex];
    // Create a new LIN frame message and copy the data.
	pMsg.FrameId = (Byte) lFD->ProtectedIdAsInt;
	pMsg.Direction = (TLINDirection) cbbDirection->Items->Objects[cbbDirection->ItemIndex];
	pMsg.ChecksumType = (TLINChecksumType) cbbCST->Items->Objects[cbbCST->ItemIndex];
	pMsg.Length = (Byte) nudLength->Position;
	// Fill data array
	pMsg.Data[0] = (Byte)StrToInt("0x" + txtData0->Text);
	pMsg.Data[1] = (Byte)StrToInt("0x" + txtData1->Text);
	pMsg.Data[2] = (Byte)StrToInt("0x" + txtData2->Text);
	pMsg.Data[3] = (Byte)StrToInt("0x" + txtData3->Text);
	pMsg.Data[4] = (Byte)StrToInt("0x" + txtData4->Text);
	pMsg.Data[5] = (Byte)StrToInt("0x" + txtData5->Text);
	pMsg.Data[6] = (Byte)StrToInt("0x" + txtData6->Text);
	pMsg.Data[7] = (Byte)StrToInt("0x" + txtData7->Text);
    // Check if the hardware is initialize as master
	if (m_HwMode == modMaster)
	{
		// Calculate the checksum contained with the
		// checksum type that set some line before.
		m_objPLinApi->CalculateChecksum(&pMsg);
		// Try to send the LIN frame message with LIN_Write.
		m_LastLINErr = m_objPLinApi->Write(m_hClient, m_hHw, &pMsg);
	}
	else
	{
		// If the hardare is initialize as slave
		// only update the data in the LIN frame.
		m_LastLINErr = m_objPLinApi->UpdateByteArray(m_hClient, m_hHw, lFD->IdAsByte, Byte(0), pMsg.Length, pMsg.Data);
	}
    // Show error if any
	if (m_LastLINErr != errOK)
		MessageBox(Handle, GetFormatedError(m_LastLINErr).c_str(), SAppCaptionError, MB_ICONERROR);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::nudLengthClick(TObject *Sender, TUDBtnType Button)
{
	txtData0->Enabled = false;
	txtData1->Enabled = false;
	txtData2->Enabled = false;
	txtData3->Enabled = false;
	txtData4->Enabled = false;
	txtData5->Enabled = false;
	txtData6->Enabled = false;
	txtData7->Enabled = false;
	// Only Publisher direction allows edition of Data textFields
	if (cbbDirection->ItemIndex != 1)
    	return;
	// Enable the same number of data textBox as the nudLength value
	switch(nudLength->Position)
	{
		case 1:
			txtData0->Enabled = true;
			break;
		case 2:
			txtData0->Enabled = true;
			txtData1->Enabled = true; 
			break;
		case 3:
			txtData0->Enabled = true;
			txtData1->Enabled = true;
			txtData2->Enabled = true; 
			break;
		case 4:
			txtData0->Enabled = true;
			txtData1->Enabled = true;
			txtData2->Enabled = true;
			txtData3->Enabled = true; 
			break;
		case 5:
            txtData0->Enabled = true;
			txtData1->Enabled = true;
            txtData2->Enabled = true;
			txtData3->Enabled = true;
            txtData4->Enabled = true;  
			break;
		case 6:
			txtData0->Enabled = true;
			txtData1->Enabled = true;
			txtData2->Enabled = true;
			txtData3->Enabled = true;
			txtData4->Enabled = true;
			txtData5->Enabled = true;  
			break;
		case 7:
			txtData0->Enabled = true;
			txtData1->Enabled = true;
			txtData2->Enabled = true;
			txtData3->Enabled = true;
			txtData4->Enabled = true;
            txtData5->Enabled = true;
			txtData6->Enabled = true;  
			break;
		case 8:
            txtData0->Enabled = true;
            txtData1->Enabled = true;
            txtData2->Enabled = true;
			txtData3->Enabled = true;
			txtData4->Enabled = true;
			txtData5->Enabled = true;
			txtData6->Enabled = true;
			txtData7->Enabled = true;
			break;
	}
}
//---------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////
// Information group event-handlers

//---------------------------------------------------------------------------
void __fastcall TForm1::btnGetVersionsClick(TObject *Sender)
{
	TLINVersion lpVersion;
	char buffer[255];
	AnsiString info, strTemp;
	int iPos;

	// We get the vesion of the PLIN API
	m_LastLINErr = m_objPLinApi->GetVersion(&lpVersion);
	if (m_LastLINErr == errOK)
	{
		strTemp = AnsiString::Format("API Version: %d.%d.%d.%d", ARRAYOFCONST((lpVersion.Major, lpVersion.Minor, lpVersion.Build, lpVersion.Revision)));
		IncludeTextMessage(strTemp);
		// We get the driver version
		m_LastLINErr = m_objPLinApi->GetVersionInfo(buffer, 255);
		if (m_LastLINErr == errOK)
		{
			IncludeTextMessage("Channel/Driver Version: ");

            // Because this information contains line control characters (several lines)
            // we split this also in several entries in the Information List-Box
			info = (AnsiString)buffer;
			while(info != "")
			{
				iPos = info.Pos("\n");
				if(iPos == 0)
					iPos = info.Length();
				IncludeTextMessage("     * " + info.SubString(1, iPos));
				info.Delete(1, iPos);
			}
		}
	}
	// If an error ccurred, a message is shown
	if (m_LastLINErr != errOK)
		MessageBox(Handle, GetFormatedError(m_LastLINErr).c_str(), SAppCaptionError, MB_ICONERROR);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnInfoClearClick(TObject *Sender)
{
    // The information contained in the Information List-Box is cleared
    lbxInfo->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnStatusClick(TObject *Sender)
{
	TLINHardwareStatus lStatus;
    // Retrieves the status of the LIN Bus and outputs its state in the information listView
	m_LastLINErr = m_objPLinApi->GetStatus(m_hHw, &lStatus);
	if (m_LastLINErr == errOK)
	{
		switch(lStatus.Status)
		{
			case hwsNotInitialized:
				IncludeTextMessage("Hardware: Not Initialized");
				break;
			case hwsAutobaudrate:
				IncludeTextMessage("Hardware: Baudrate Detection");
				break;
			case hwsActive:
				IncludeTextMessage("Bus: Active");
				break;
			case hwsSleep:
				IncludeTextMessage("Bus: Sleep");
				break;
			case hwsShortGround:
				IncludeTextMessage("Bus-Line: Shorted Ground");
				break;
		}
	}
	else
		MessageBox(Handle, GetFormatedError(m_LastLINErr).c_str(), SAppCaptionError, MB_ICONERROR);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnResetClick(TObject *Sender)
{
    // Flushes the Receive Queue of the Client and resets its counters. 
	m_LastLINErr = m_objPLinApi->ResetClient(m_hClient);

	// If it fails, a error message is shown
	if (m_LastLINErr != errOK)
		MessageBox(Handle, GetFormatedError(m_LastLINErr).c_str(), SAppCaptionError, MB_ICONERROR);
	else
	{
		// clears the message list-view
		btnMsgClear->Click();
		IncludeTextMessage("Receive queue and counters successfully reset");
	}
}
//---------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////
// LIN functions

void TForm1::RefreshHardware()
{
	BYTE i;
	HLINHW lwHwHandles[8];
	WORD lwBuffSize;
	int lwCount, lnHwType, lnDevNo, lnChannel, lnMode;
	TLINError lLINErr;
	HLINHW lwHw;
	AnsiString str;

	//lwHwHandles = new HLINHW[8];
	lwBuffSize = 8;
	lwCount = 0;
	// Get all available LIN hardware.
	lLINErr = m_objPLinApi->GetAvailableHardware(lwHwHandles, lwBuffSize, &lwCount);

	if (lLINErr == errOK)
	{
		// combobox ItemData only holds DWORD, no need to free resources
		cbbChannel->Clear();
		// If no error occurs
		if (lwCount == 0)
		{
			// No LIN hardware was found.
			// Show an empty entry
			lwHw = 0;
			cbbChannel->AddItem("<No hardware found>", NULL);
		}
		// For each founded LIN hardware
		for (i = 0; i < lwCount; i++)
		{
			// Get the handle of the hardware.
			lwHw = lwHwHandles[i];
			// Read the type of the hardware with the handle lwHw.
			m_objPLinApi->GetHardwareParam(lwHw, hwpType, &lnHwType, 0);
			// Read the device number of the hardware with the handle lwHw.
			m_objPLinApi->GetHardwareParam(lwHw, hwpDeviceNumber, &lnDevNo, 0);
			// Read the channel number of the hardware with the handle lwHw.
			m_objPLinApi->GetHardwareParam(lwHw, hwpChannelNumber, &lnChannel, 0);
			// Read the mode of the hardware with the handle lwHw (Master, Slave or None).
			m_objPLinApi->GetHardwareParam(lwHw, hwpMode, &lnMode, 0);

			// Create a comboboxItem
			// If the hardware type is a knowing hardware
			// show the name of that in the label of the entry.
			if (lnHwType == LIN_HW_TYPE_USB)
				str = Format("PCAN-USB Pro - dev. %d, chan. %d", ARRAYOFCONST((lnDevNo, lnChannel)));
			else
				// Show as unknown hardware
				str = Format("Unknown - dev. %d, chan. %d", ARRAYOFCONST((lnDevNo, lnChannel)));

			cbbChannel->AddItem(str, (TObject *)lwHw);
		}
		cbbChannel->ItemIndex = 0;   
		cbbChannelChange(cbbChannel);   // call change selection
	}
}

bool TForm1::DoLinConnect()
{
	bool fRet;
	WORD lwHw, lwBaud;
	int lnMode, lnCurrBaud;
	TLINHardwareMode lHwMode;

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
	lwHw = (HLINHW) cbbChannel->Items->Objects[cbbChannel->ItemIndex];
	if (lwHw != 0)
	{
		// The local hardware handle is valid.
		// Get the current mode of the hardware
		m_objPLinApi->GetHardwareParam(lwHw, hwpMode, &lnMode, 0);
		// Get the current baudrate of the hardware
		m_objPLinApi->GetHardwareParam(lwHw, hwpBaudrate, &lnCurrBaud, 0);
		// Try to connect the application client to the
		// hardware with the local handle.
		m_LastLINErr = m_objPLinApi->ConnectClient(m_hClient, lwHw);
		if (m_LastLINErr == errOK)
		{
			// If the connection successfull
			// assign the local handle to the
			// member handle.
			m_hHw = lwHw;
			// Get the selected hardware mode.
			if (cbbHwMode->ItemIndex == 1)
				lHwMode = modMaster;
			else
				lHwMode = modSlave;
			// Get the selected baudrate
			lwBaud = (WORD) StrToIntDef(cbbBaudRates->Text, 0);
			// Get the selected hardware channel
			if ( (lnMode == modNone) || (lnCurrBaud != lwBaud))
			{
				// Only if the current hardware is not initialize
				// try to Intialize the hardware with mode and baudrate
				m_LastLINErr = m_objPLinApi->InitializeHardware(m_hClient, m_hHw, lHwMode, lwBaud);
			}
			if (m_LastLINErr == errOK)
			{
				// Assign the Hardware Mode to member attribut
				m_HwMode = lHwMode;
				// Assign the baudrate index to member attribut
				m_wBaudrate = lwBaud;
				// Set the client filter with the mask.
				m_LastLINErr = m_objPLinApi->SetClientFilter(m_hClient, m_hHw, m_lMask);
				// Read the frame table from the connected hardware.
				ReadFrameTableFromHw();
				// Reset the last LIN error code to default.
				m_LastLINErr = errOK;
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
		if (m_LastLINErr != errOK)
		{
			MessageBox(Handle, GetFormatedError(m_LastLINErr).c_str(), SAppCaptionError, MB_ICONERROR);
			fRet = false;
		}
		
	}
	else // Should never occur
		m_hHw = 0; // But if it occurs, set handle to default

	return fRet;
}

bool TForm1::DoLinDisconnect()
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
		BYTE lhClients[255];

		// Get the connected clients from the LIN hardware.
		m_LastLINErr = m_objPLinApi->GetHardwareParam(m_hHw, hwpConnectedClients, lhClients, 255);
		if (m_LastLINErr == errOK)
		{
			// No errors !
			// Check all client handles.
			for (int i = 0; i < 255; i++)
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
			m_objPLinApi->ResetHardwareConfig(m_hClient, m_hHw);
		}
		// If this application is connected to the hardware
		// then disconnect the client. Otherwise not.
		if (lfOwnClient == true)
		{
			// Disconnect if the application was connected to a LIN hardware.
			m_LastLINErr = m_objPLinApi->DisconnectClient(m_hClient, m_hHw);
			if (m_LastLINErr == errOK)
			{
				m_hHw = 0;
				return true;
			}
			else
			{
				// Error while disconnecting from hardware.
				MessageBox(Handle, GetFormatedError(m_LastLINErr).c_str(), SAppCaptionError, MB_ICONERROR);
				return false;
			}
		}
		else
			return true;
	}
	else
		return true;
}

void TForm1::ReadFrameTableFromHw()
{
	int i, lnCnt;
    TLINFrameEntry lFrameEntry;
    TLINError lErr;
	unsigned __int64 llMask;
	CFrameDefinition * lFrame;

    // Get the count of Frame Definition from the
    // Global Frame Table.
	lnCnt = m_pGFT->getCount();
    // Initialize the member attribute for the
    // client mask with 0.
    m_lMask = 0;
    // Each Frame Definition
    for (i = 0; i < lnCnt; i++)
    {
		lFrame = m_pGFT->get(i);
        // Before a frame entry can be read from the
        // hardware, the Frame-ID of the wanted entry 
        // must be set
		lFrameEntry.FrameId = lFrame->getIdAsByte();
        // Read the information of the specified frame entry from the hardware.
        lErr = m_objPLinApi->GetFrameEntry(m_hHw, &lFrameEntry);
        // Check the result value of the LinApi function call.
        if (lErr == errOK)
        {
            // LinApi function call successfull.
            // Copy the frame entry information to the Frame Definition.
			lFrame->setLength(lFrameEntry.Length);
            lFrame->setDirection(lFrameEntry.Direction);
            lFrame->setChecksumType(lFrameEntry.ChecksumType);
            if (lFrame->getDirection() != dirDisabled)
            {
                // If the direction is not disabled than set the
                // bit in the client filter mask.
                llMask = ((unsigned __int64)1 << i) & FRAME_FILTER_MASK;
                m_lMask = m_lMask | llMask;
            }
        }
    }
    // If the Client and Hardware handles are valid.
    if ((m_hClient != 0) && (m_hHw != 0))
        // Set the client filter.
		m_objPLinApi->SetClientFilter(m_hClient, m_hHw, m_lMask);
	// Updates the displayed frameIds
	UpdateFrameIds();
}
	
///////////////////////////////////////////////////////////////////////////
// LIN Message processing functions

void TForm1::ReadMessages()
{
	TLINRcvMsg lpMsg;

    // We read at least one time the queue looking for messages.
    // If a message is found, we look again trying to find more.
    // If the queue is empty or an error occurs, we get out from
    // the dowhile statement.
    //	
    do
	{
        m_LastLINErr = m_objPLinApi->Read(m_hClient, &lpMsg);
        // If at least one Frame is received by the LinApi.
        // Check if the received frame is a standard type.
        // If it is not a standard type than ignore it.
		if (lpMsg.Type != mstStandard)
            continue;

        if (m_LastLINErr == errOK)
            ProcessMessage(lpMsg);

	} while (btnRelease->Enabled && (!(m_LastLINErr & errRcvQueueEmpty)));
}

void TForm1::ProcessMessage(TLINRcvMsg linMsg)
{
	int pos;
	CMessageStatus * lMsg;

	// We search if a message (Same ID and Type) has
	// already been received or if this is a new message
	//
	for( int i = 0 ; i < m_LastMsgsList->Count ; i++ )
	{
		lMsg = (CMessageStatus *) m_LastMsgsList->Items[i];
		if (lMsg->LINMsg.FrameId == linMsg.FrameId)
		{
			// Modify the message and exit
			//
			lMsg->Update(linMsg);
			return;
		}
	}
	// Message not found. It will be created
	//
	InsertMsgEntry(linMsg);
}

void TForm1::InsertMsgEntry(TLINRcvMsg newMsg)
{
    CMessageStatus * lMsg;
	AnsiString strId;
	TListItem * item;
	int i;

    // We add this status in the last message list
    //
	lMsg = new CMessageStatus(newMsg, lstMessages->Items->Count);
	m_LastMsgsList->Add(lMsg);

	// Search and retrieve the ID in the global frame table associated with the frame Protected-ID
	strId = "";
	for (i = 0; i < m_pGFT->getCount(); i++)
	{
		if (lMsg->LINMsg.FrameId == (BYTE) m_pGFT->get(i)->getProtectedIdAsInt() )
		{
			strId = m_pGFT->get(i)->getId();
			break;
        }        
    }

	// Add the new ListView Item with the ID of the message
	item = lstMessages->Items->Add();
	item->Caption = strId;
	// We set the length of the message
	item->SubItems->Add(IntToStr(lMsg->LINMsg.Length));
	// We set the data of the message.
	item->SubItems->Add(lMsg->DataString);
    // we set the message count message (this is the First, so count is 1)
	item->SubItems->Add(IntToStr(lMsg->Count));
    // Add time stamp information if needed
	item->SubItems->Add(lMsg->TimeString);
    // We set the direction of the message
	item->SubItems->Add(lMsg->DirectionString);
    // We set the error of the message
	item->SubItems->Add(lMsg->ErrorString);
    // We set the CST of the message
	item->SubItems->Add(lMsg->CSTString);
    // We set the CRC of the message
	item->SubItems->Add(lMsg->ChecksumString);
}

void TForm1::DisplayMessages()
{
	CMessageStatus * lMsg;
	for (int i = 0 ; i < m_LastMsgsList->Count ; i++)
	{
		lMsg = (CMessageStatus*) m_LastMsgsList->Items[i];

		if (lMsg->MarkedAsUpdated)
		{
			lMsg->MarkedAsUpdated = False;
			// Update the length of the message
			lstMessages->Items->Item[i]->SubItems->Strings[0] = IntToStr(lMsg->LINMsg.Length);
			// Update the data of the message.
			lstMessages->Items->Item[i]->SubItems->Strings[1] = lMsg->DataString;
			// Update the message count message
			lstMessages->Items->Item[i]->SubItems->Strings[2] = IntToStr(lMsg->Count);
			// Update time stamp information
			lstMessages->Items->Item[i]->SubItems->Strings[3] = lMsg->TimeString;
			// Update the direction of the message
			lstMessages->Items->Item[i]->SubItems->Strings[4] = lMsg->DirectionString;
			// Update the error of the message
			lstMessages->Items->Item[i]->SubItems->Strings[5] = lMsg->ErrorString;
			//  Update the CST of the message
			lstMessages->Items->Item[i]->SubItems->Strings[6] = lMsg->CSTString;
			// Update the CRC of the message
			lstMessages->Items->Item[i]->SubItems->Strings[7] = lMsg->ChecksumString;
		}
	}
}
		
///////////////////////////////////////////////////////////////////////////
// Help functions

void TForm1::UpdateFrameIds()
{
	AnsiString lID;
	CFrameDefinition * lFrame, * lSelectedFrame;
	int idx;
	
	lFrame = lSelectedFrame = NULL;
	// Retrieves selected ID if it exists, before clearing comboBox
	if	(cbbID->ItemIndex != -1)
		lSelectedFrame = (CFrameDefinition*) cbbID->Items->Objects[cbbID->ItemIndex];

    // Clears and populates FrameID combobox with global frame IDs
	cbbID->Clear();
	for (int i = 0; i < m_pGFT->getCount() ; i++)
    {
		lFrame = m_pGFT->get(i);
        // add only non disabled frames
		if (lFrame->getDirection() == dirDisabled)
			continue;
		// Add frame and data
		cbbID->Items->AddObject(lFrame->getId(), (TObject*) lFrame);
        // Check if the new item was selected before the update
        if (lSelectedFrame == lFrame)
			cbbID->ItemIndex = cbbID->Items->Count - 1;
	}
}

void TForm1::IncludeTextMessage(AnsiString strMsg)
{
	lbxInfo->Items->Add(strMsg);
	lbxInfo->ItemIndex = lbxInfo->Items->Count - 1;
}

void TForm1::SetConnectionStatus(bool bConnected)
{
    // Buttons
	btnInit->Enabled = !bConnected;
	btnConfigure->Enabled = bConnected;
	btnRead->Enabled = bConnected && rdbManual->Checked;
    btnWrite->Enabled = bConnected;
    btnRelease->Enabled = bConnected;
    btnFilterApply->Enabled = bConnected;
    btnFilterQuery->Enabled = bConnected;
    btnGetVersions->Enabled = bConnected;
	btnHwRefresh->Enabled = !bConnected;
    btnStatus->Enabled = bConnected;
    btnReset->Enabled = bConnected;

    // ComboBoxs
	cbbBaudRates->Enabled = !bConnected;
	cbbChannel->Enabled = !bConnected;
	cbbHwMode->Enabled = !bConnected;

    // Hardware configuration and read mode
	if (!bConnected)
		cbbChannelChange(NULL);
	else
		rdbTimerChanged(NULL);

    // Display messages in grid
	tmrDisplay->Enabled = bConnected;
}

AnsiString TForm1::GetFormatedError(TLINError error)
{
	AnsiString result = "";   
	char buffer[256];   

	memset(buffer,'\0',256);
	// If any error are occured
	// display the error text in a message box.
	// 0x00 = Neutral
	// 0x07 = Language German
	// 0x09 = Language English
	if (m_objPLinApi->GetErrorText(error, 0x09, buffer, 255) != errOK)
		result = Format("An error occurred. Error-code's text (%d) couldn't be retrieved", ARRAYOFCONST((Integer(error))));    
	else
		result = buffer;

	return result;
}

void TForm1::CheckHexEditBox(AnsiString* txtData)
{
	// TODO
}
