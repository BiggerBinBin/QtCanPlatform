//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <list>
//---------------------------------------------------------------------------
// Includes
//
#include "PLinApiClass.h"
#include "CGlobalFrameTable.h"

// Message Status structure used to show LIN Messages in a ListView
class CMessageStatus
{
public:
	// Creates a new MessageStatus object
	CMessageStatus(TLINRcvMsg linMsg,	// received LIN message
		int listIndex);					// index of the message in the ListView

	// Updates an existing MessageStatus with a new LIN message
	void Update(TLINRcvMsg linMsg);

	// Getters & Setters
	//          

    // Returns the received LIN message   
	__property TLINRcvMsg LINMsg = {read=getLINMsg};
	TLINRcvMsg getLINMsg();
    // Returns the index of the message in the ListView  
	__property int Position = {read=getPosition};
	int getPosition();
    // Returns the Direction of the LIN message as a string  
	__property AnsiString DirectionString = {read=getDirectionString};
	AnsiString getDirectionString();
    // Returns the Checksum type of the LIN message as a string  
	__property AnsiString CSTString = {read=getCSTString};
	AnsiString getCSTString();
	// Returns the Checksum of the LIN message as a string
	__property AnsiString ChecksumString = {read=getChecksumString};
	AnsiString getChecksumString();
    // Returns the Error field of the LIN message as a string 
	__property AnsiString ErrorString = {read=getErrorString};
	AnsiString getErrorString();
	// Returns the Protected ID of the LIN message as a string
	__property AnsiString PIdString = {read=getPIdString};
	AnsiString getPIdString();
    // Returns the Data fields of the LIN message as a string   
	__property AnsiString DataString = {read=getDataString};
	AnsiString getDataString();
	// Returns the Number of LIN messages received with the same frame ID
	__property int Count = {read=getCount};
	int getCount();
	// States wether the timestamp is displayed as a period or not
	__property bool ShowingPeriod = {read=getShowingPeriod, write=setShowingPeriod};
	bool getShowingPeriod();
	// States wether the timestamp is displayed as a period or not
	void setShowingPeriod(bool value);
	// Defines if the LIN message has been modified
	__property bool MarkedAsUpdated = {read=isMarkedAsUpdated, write=setMarkedAsUpdated};
	bool isMarkedAsUpdated();
	// Defines if the LIN message has been modified
	void setMarkedAsUpdated(bool value);
	// Returns the timestamp or period of the LIN message
	__property AnsiString TimeString = {read=getTimeString};
	AnsiString getTimeString();

private:	
	// The received LIN message
	TLINRcvMsg m_Msg;
	// Timestamp of a previously received message
	unsigned __int64 m_oldTimeStamp;
	// index of the message in the ListView component
	int m_iIndex;
	// Number of LIN message received with the same frame ID
	int m_Count;
	// Defines if the timestamp is displayed as a period
	bool m_bShowPeriod;
	// Defines if the message has been modified and its display needs to be updated
	bool m_bWasChanged;

};

class TForm1 : public TForm
{
__published:	// Composants gérés par l'EDI
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TComboBox *cbbChannel;
	TButton *btnHwRefresh;
	TComboBox *cbbBaudRates;
	TComboBox *cbbHwMode;
	TButton *btnIdent;
	TButton *btnInit;
	TButton *btnRelease;
	TGroupBox *GroupBox2;
	TLabel *Label4;
	TLabel *Label5;
	TButton *btnConfigure;
	TRadioButton *rdbFilterOpen;
	TRadioButton *rdbFilterClose;
	TRadioButton *rdbFilterCustom;
	TEdit *txtFilterFrom;
	TUpDown *nudIdFrom;
	TEdit *txtFilterTo;
	TUpDown *nudIdTo;
	TButton *btnFilterApply;
	TButton *btnFilterQuery;
	TGroupBox *GroupBox3;
	TRadioButton *rdbTimer;
	TRadioButton *rdbManual;
	TCheckBox *chbShowPeriod;
	TListView *lstMessages;
	TButton *btnRead;
	TButton *btnMsgClear;
	TGroupBox *GroupBox4;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TComboBox *cbbID;
	TEdit *txtID;
	TComboBox *cbbDirection;
	TComboBox *cbbCST;
	TEdit *txtLength;
	TUpDown *nudLength;
	TEdit *txtData0;
	TEdit *txtData1;
	TEdit *txtData2;
	TEdit *txtData3;
	TEdit *txtData4;
	TEdit *txtData5;
	TEdit *txtData6;
	TEdit *txtData7;
	TButton *btnWrite;
	TGroupBox *GroupBox5;
	TButton *btnGetVersions;
	TButton *btnInfoClear;
	TButton *btnStatus;
	TButton *btnReset;
	TListBox *lbxInfo;
	TTimer *tmrDisplay;
	TTimer *tmrRead;
	void __fastcall cbbChannelChange(TObject *Sender);
	void __fastcall btnHwRefreshClick(TObject *Sender);
	void __fastcall cbbHwModeChange(TObject *Sender);
	void __fastcall btnIdentClick(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnReleaseClick(TObject *Sender);
	void __fastcall btnConfigureClick(TObject *Sender);
	void __fastcall rdbFilterChanged(TObject *Sender);
	void __fastcall txtFilterFromExit(TObject *Sender);
	void __fastcall txtFilterToExit(TObject *Sender);
	void __fastcall txtFilterKeyPress(TObject *Sender, char &Key);
	void __fastcall btnFilterApplyClick(TObject *Sender);
	void __fastcall btnFilterQueryClick(TObject *Sender);
	void __fastcall rdbTimerChanged(TObject *Sender);
	void __fastcall chbShowPeriodClick(TObject *Sender);
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall btnMsgClearClick(TObject *Sender);
	void __fastcall tmrReadTimer(TObject *Sender);
	void __fastcall tmrDisplayTimer(TObject *Sender);
	void __fastcall cbbIDChange(TObject *Sender);
	void __fastcall txtData0KeyPress(TObject *Sender, char &Key);
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall nudLengthClick(TObject *Sender, TUDBtnType Button);
	void __fastcall btnGetVersionsClick(TObject *Sender);
	void __fastcall btnInfoClearClick(TObject *Sender);
	void __fastcall btnStatusClick(TObject *Sender);
	void __fastcall btnResetClick(TObject *Sender);
	void __fastcall txtData0Exit(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
private:	// Déclarations utilisateur

    // LIN API object
	PLinApiClass * m_objPLinApi;
    // Client handle
	HLINCLIENT m_hClient;
    // Hardware handle
    HLINHW m_hHw;
    // LIN Hardware Modus (Master/Slave)
	TLINHardwareMode m_HwMode;
    // Client filter mask
    unsigned __int64 m_lMask;
    // Baudrate Index of Hardware
    WORD m_wBaudrate;
    // Last LIN error
    TLINError m_LastLINErr;
    // Constant value that indicate the mask of the client filter (64bit)
    static const unsigned __int64 FRAME_FILTER_MASK = 0xFFFFFFFFFFFFFFFF;
	
	// Read Timer identifier
	UINT_PTR m_tmrRead;
	//Display Timer identifier
	UINT_PTR m_tmrDisplay;

    // Global Frame Table object to hold all frames definitions.
    CGlobalFrameTable * m_pGFT;
    // Stores the status of received messages for its display
    TList * m_LastMsgsList;

	///////////////////////////////////////////////////////////////////////////
	// LIN functions

	// Updates the combobox 'cbbChannel' with currently available hardwares
	void RefreshHardware();
	
    /// <summary>
    /// Connects to the hardware with the setting values
    /// from the connection groupbox.
    /// </summary>
    /// <returns>
    /// Returns true if the function finished successfull. Otherwise
    /// returns false.
    /// </returns>
	bool DoLinConnect();

    /// <summary>
    /// Disconnects an existing connection to a LIN hardware and returns
    /// true if disconnection finished succesfully or if no connection exists.
    /// Returns false if the current connection can not be disconnected.
    /// </summary>
    /// <returns>
    /// Returns true if the function finished successfull. Otherwise
    /// returns false.
    /// </returns>
    bool DoLinDisconnect();

	/// <summary>
    /// Reads all values from the frame table of the hardware
    /// and assign it to the GlobalFrameTable. Also refresh
    /// the Global Frame Table ListView with that values.
    /// </summary>
	void ReadFrameTableFromHw();
	
	///////////////////////////////////////////////////////////////////////////
	// LIN Message processing functions

    /// <summary>
    /// Function for reading PLIN messages
    /// </summary>
	void ReadMessages();
	
    /// <summary>
    /// Processes a received message, in order to show it in the Message-ListView
    /// </summary>
    /// <param name="linMsg">The received PLIN message</param>
    void ProcessMessage(TLINRcvMsg linMsg);

	/// <summary>
    /// Inserts a new entry for a new message in the Message-ListView
    /// </summary>
    /// <param name="newMsg">The messasge to be inserted</param>
    void InsertMsgEntry(TLINRcvMsg newMsg);

	/// <summary>
    /// Displays and updates LIN messages in the Message-ListView
    /// </summary>
    void DisplayMessages();
		
	///////////////////////////////////////////////////////////////////////////
	// Help functions
	
	// Initializes UI controls
	void InitializeControls(void);
		
	//  Updates Frame IDs with values from the global frame table
	void UpdateFrameIds();

	// Includes a new line of text into the information Listview
	void IncludeTextMessage(AnsiString strMsg);
	
	// Activates/deactivates the different controls of the main-form according
    // to the current connection status
	void SetConnectionStatus(bool bConnected) ;

    // Returns a LIN error as a formatted string
	AnsiString GetFormatedError(TLINError error);

	// Check txtData in an hexadecimal value
	void CheckHexEditBox(AnsiString* txtData);

public:		// Déclarations utilisateur
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
