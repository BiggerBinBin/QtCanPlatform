
// PLIN-API ExampleDlg.h : header file
//

#pragma once

// Includes
//
#include "PLinApiClass.h"
#include "CGlobalFrameTable.h"

#pragma region CMessageStatus

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
    TLINRcvMsg getLINMsg();
    // Returns the index of the message in the ListView
    int getPosition();
    // Returns the Direction of the LIN message as a string
    CString getDirectionString();
    // Returns the Checksum type of the LIN message as a string
    CString getCSTString();
    // Returns the Checksum of the LIN message as a string
    CString getChecksumString();
    // Returns the Error field of the LIN message as a string
    CString getErrorString();
    // Returns the Protected ID of the LIN message as a string
    CString getPIdString();
    // Returns the Data fields of the LIN message as a string
    CString getDataString();
    // Returns the Number of LIN messages received with the same frame ID
    int getCount();
    // States wether the timestamp is displayed as a period or not
    bool getShowingPeriod();
    // States wether the timestamp is displayed as a period or not
    void setShowingPeriod(bool value);
    // Defines if the LIN message has been modified
    bool isMarkedAsUpdated();
    // Defines if the LIN message has been modified
    void setMarkedAsUpdated(bool value);
    // Returns the timestamp or period of the LIN message
    CString getTimeString();

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

#pragma endregion

#pragma region CGFTDialog 
	
// Global Frame Table configuration dialog
class CGFTDialog : public CDialog
{
// Construction
public:
	CGFTDialog(CGlobalFrameTable * pGFT, CWnd* pParent = NULL);	// standard constructor
	~CGFTDialog();

// Dialog Data
	enum { IDD = IDD_GFT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Frames List Control
	CListCtrl lstFrames;
	// PropertyGrid location
	CStatic wndPropListLocation;
	// PropertyGrid control
	CMFCPropertyGridCtrl wndPropList;
	
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP();
	afx_msg void OnClose();
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedLstframes(NMHDR *pNMHDR, LRESULT *pResult);

private: 
    // Global Frame Table object that holds all frame definition.
    CGlobalFrameTable * m_pGFT;
    /// Boolean to register the current edit mode of different controls.
    /// To protect the control's event for recursive calling.
    bool m_fAutomatic;
	// Stores the selected frames in the Frame List Control
    CPtrList * m_pSelectedFrames;
	// 
	CMFCPropertyGridProperty * m_pPropCST;
	CMFCPropertyGridProperty * m_pPropDirection;
	CMFCPropertyGridProperty * m_pPropLength;
	CMFCPropertyGridProperty * m_pPropID;
	CMFCPropertyGridProperty * m_pPropPID;


    /// Fills the Listview 'Global Frame Table' with the data from CGloabalFrameTable.
	void FillListView();
	// Updates the specified row of the Listview 'Global Frame Table' with the data from pFrame
	void UpdateListView(int index, CFrameDefinition * pFrame);
	// Updates the property grid with the selected frames from m_pSelectedFrames
	void UpdatePropertyGrid();
	/// <summary>
    /// Returns the Frame Direction as a human readable string
    /// </summary>
    /// <param name="direction">the direction object</param>
    /// <returns>The direction as a string</returns>
	CString getDirectionAsString(TLINDirection direction);
    /// <summary>
    /// Returns the Frame checksum type as a human readable string
    /// </summary>
    /// <param name="checksumType">the checksumType object</param>
    /// <returns>The checksumType as a string</returns>
	CString getChecksumAsString(TLINChecksumType checksumType);

};

#pragma endregion

#pragma region CPLINAPIExampleDlg

#define SAPPCAPTION_ERROR	"PLIN-API Example - Error"

// CPLINAPIExampleDlg main dialog
class CPLINAPIExampleDlg : public CDialog
{
// Construction
public:
	CPLINAPIExampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PLINAPIEXAMPLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Connection
	CComboBox cbbChannel;
	CButton btnRefresh;
	CComboBox cbbBaudRates;
	CComboBox cbbHwMode;
	CButton btnIdent;
	CButton btnInit;
	CButton btnRelease;

	// Configuration
	CButton btnConfig;
	CButton rdbFilterOpen;
	CButton rdbFilterClose;
	CButton rdbFilterCustom;
	CString txtFilterFrom;
	CString txtFilterTo;
	CEdit txtEditFilterFrom;
	CEdit txtEditFilterTo;
	CSpinButtonCtrl nudFilterFrom;
	CSpinButtonCtrl nudFilterTo;
	CButton btnFilterApply;
	CButton btnFilterQuery;

	// Reading
	CButton rdbReadingTimer;
	CButton rdbReadingManual;
	CButton chbReadingTimeStamp;
	CButton btnRead;
	CButton btnReadingClear;
	CListCtrl lstMessages;

	//Writing
	CComboBox cbbID;
	CString txtID;
	CComboBox cbbDirection;
	CComboBox cbbCST;
	CString txtLength;
	CSpinButtonCtrl nudLength;
	CEdit txtEditData0;
	CEdit txtEditData1;
	CEdit txtEditData2;
	CEdit txtEditData3;
	CEdit txtEditData4;
	CEdit txtEditData5;
	CEdit txtEditData6;
	CEdit txtEditData7;
	CString txtData0;
	CString txtData1;
	CString txtData2;
	CString txtData3;
	CString txtData4;
	CString txtData5;
	CString txtData6;
	CString txtData7;
	CButton btnWrite;

	// Information
	CListBox lstInfo;
	CButton btnStatus;
	CButton btnReset;
	CButton btnVersion;
	CButton btnInfoClear;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonHwrefresh();
	afx_msg void OnCbnSelchangeCbbchannel();
	afx_msg void OnCbnSelchangeCbbhwmode();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnident();
	afx_msg void OnBnClickedBtninit();
	afx_msg void OnBnClickedBtnrelease();
	afx_msg void OnBnClickedBtnconfigfrms();
	afx_msg void OnBnClickedBtnfilterapply();
	afx_msg void OnBnClickedBtnfilterquery();
	afx_msg void OnBnClickedBtnread();
	afx_msg void OnBnClickedBtnreadclear();
	afx_msg void OnBnClickedBtnwrite();
	afx_msg void OnBnClickedBtnversion();
	afx_msg void OnBnClickedBtninfoclear();
	afx_msg void OnBnClickedBtnstatus();
	afx_msg void OnBnClickedBtnreset();
	afx_msg void OnBnClickedRdbfilteropen();
	afx_msg void OnBnClickedRdbfilterclose();
	afx_msg void OnBnClickedRdbfiltercustom();
	afx_msg void OnDeltaposNudfilterfrom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposNudfilterto(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRdbtimer();
	afx_msg void OnBnClickedRdbmanual();
	afx_msg void OnBnClickedChbtimestamp();
	afx_msg void OnCbnSelchangeCbbid();
	afx_msg void OnDeltaposNudlength(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusTxtdata0();
	afx_msg void OnEnKillfocusTxtdata1();
	afx_msg void OnEnKillfocusTxtdata2();
	afx_msg void OnEnKillfocusTxtdata3();
	afx_msg void OnEnKillfocusTxtdata4();
	afx_msg void OnEnKillfocusTxtdata5();
	afx_msg void OnEnKillfocusTxtdata6();
	afx_msg void OnEnKillfocusTxtdata7();
	afx_msg void OnEnKillfocusTxtfilterfrom();
	afx_msg void OnEnKillfocusTxtfilterto();

private:
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
    CPtrList * m_LastMsgsList;

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
	
	// Handles changes on radio buttons from the "read message" group
	void RdbReadingChanged();
	// Enable/Disable Read Timer
	void SetTimerRead(bool bEnable);
	// Enable/Disable Display Timer
	void SetTimerDisplay(bool bEnable);
	
	//  Updates Frame IDs with values from the global frame table
	void UpdateFrameIds();

	// Includes a new line of text into the information Listview
	void IncludeTextMessage(CString strMsg);
	
	// Activates/deactivates the different controls of the main-form according
    // to the current connection status
	void SetConnectionStatus(bool bConnected) ;

    // Returns a LIN error as a formatted string
	CString GetFormatedError(TLINError error);

	// Convert a int value to a CString
	CString IntToStr(int iValue);
	// Convert a int value to a CString formated in Hexadecimal
	CString IntToHex(int iValue, short iDigits);
	// Convert hexadecimal Cstring into int value (Zero if error)
	DWORD HexTextToInt(CString ToConvert);
	// Check txtData in an hexadecimal value
	void CheckHexEditBox(CString* txtData);
};

#pragma endregion
