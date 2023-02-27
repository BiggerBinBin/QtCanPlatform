
// PLIN-API ExampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PLIN-API Example.h"
#include "PLIN-API ExampleDlg.h"
#include <Math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
CString CMessageStatus::getDirectionString()
{
	switch (m_Msg.Direction)
    {
        case dirDisabled:
            return _T("Disabled");
        case dirPublisher:
            return _T("Publisher");
        case dirSubscriber:
            return _T("Subscriber");
        case dirSubscriberAutoLength:
            return _T("Subscriber Automatic Length");
    }
	return _T("");
}
CString CMessageStatus::getCSTString()
{
	switch (m_Msg.ChecksumType)
    {
        case cstAuto:
            return _T("Auto");
        case cstClassic:
            return _T("Classic");
        case cstEnhanced:
            return _T("Enhanced");
        case cstCustom:
            return _T("Custom");
    }
	return _T("");
}
CString CMessageStatus::getChecksumString()
{
	CString result;
	result.Format(_T("%Xh"), m_Msg.Checksum);
	return result;
}
CString CMessageStatus::getErrorString()
{
	CString result;
	if (m_Msg.ErrorFlags == 0)
		result = _T("O.k.");
	else
	{
		if (m_Msg.ErrorFlags & MSG_ERR_CHECKSUM)
			result.Append(_T("Checksum,"));
		if (m_Msg.ErrorFlags & MSG_ERR_GND_SHORT)
			result.Append(_T("GroundShort,"));
		if (m_Msg.ErrorFlags & MSG_ERR_ID_PARITY_BIT0)
			result.Append(_T("IdParityBit0,"));
		if (m_Msg.ErrorFlags & MSG_ERR_ID_PARITY_BIT1)
			result.Append(_T("IdParityBit1,"));
		if (m_Msg.ErrorFlags & MSG_ERR_INCONSISTENT_SYNC)
			result.Append(_T("InconsistentSynch,"));
		if (m_Msg.ErrorFlags & MSG_ERR_OTHER_RESPONSE)
			result.Append(_T("OtherResponse,"));
		if (m_Msg.ErrorFlags & MSG_ERR_SLAVE_NOT_RESPONDING)
			result.Append(_T("SlaveNotResponding,"));
		if (m_Msg.ErrorFlags & MSG_ERR_SLOT_DELAY)
			result.Append(_T("SlotDelay,"));
		if (m_Msg.ErrorFlags & MSG_ERR_TIMEOUT)
			result.Append(_T("Timeout,"));
		if (m_Msg.ErrorFlags & MSG_ERR_VBAT_SHORT)
			result.Append(_T("VBatShort,"));
		// remove ending comma
		if (result.GetLength() > 0)
			result.Delete(result.GetLength()-1, 1);
	}
	return result;
}
CString CMessageStatus::getPIdString()
{
	CString result;
	result.Format(_T("%03Xh"), m_Msg.FrameId);
	return result;
}
CString CMessageStatus::getDataString()
{
	CString result = _T("");
	CString strTemp = _T("");
		
    for (BYTE i = 0; i < m_Msg.Length; i++)
    {
		strTemp.Format("%02X ", m_Msg.Data[i]);
		result.Append(strTemp);
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
CString CMessageStatus::getTimeString()
{
	CString result;
	char buffer[65];
	unsigned __int64 time;

	memset(buffer,'\0',64);
    time = m_Msg.TimeStamp;
    if (m_bShowPeriod)
        time = (time - m_oldTimeStamp) / 1000;
	_ui64toa_s(time, buffer, 65, 10);
	result.Format(_T("%s"), buffer);
    
	return result;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// CGFTDialog class
//

CGFTDialog::CGFTDialog(CGlobalFrameTable * pGFT, CWnd* pParent)
	: CDialog(CGFTDialog::IDD, pParent),
	m_pGFT(pGFT),
	m_fAutomatic(false)
{
	m_pSelectedFrames = new CPtrList(m_pGFT->getCount());
}

CGFTDialog::~CGFTDialog()
{
	// this list contains only references, no need to delete its contents
	delete m_pSelectedFrames;
}


void CGFTDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// Connection
	DDX_Control(pDX, IDC_LSTFRAMES, lstFrames);
	DDX_Control(pDX, IDC_PROPLIST_LOCATION, wndPropListLocation);
}

BEGIN_MESSAGE_MAP(CGFTDialog, CDialog)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LSTFRAMES, &CGFTDialog::OnLvnItemchangedLstframes)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CGFTDialog::OnPropertyChanged)	
END_MESSAGE_MAP()

BOOL CGFTDialog::OnInitDialog()
{
	CString strTemp;
	CDialog::OnInitDialog();

	// Read Messages List Control
	lstFrames.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 30, 2);
	lstFrames.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 40, 2);
	lstFrames.InsertColumn(2, _T("Direction"), LVCFMT_LEFT, 150, 5);	
	lstFrames.InsertColumn(3, _T("Length"), LVCFMT_LEFT, 50, 3);		
	lstFrames.InsertColumn(4, _T("CST"), LVCFMT_LEFT, 50, 5);	
	lstFrames.SetExtendedStyle(lstFrames.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	
	// Setup MFC PropertyGrid Control
	CRect rectPropList;
	wndPropListLocation.GetClientRect(&rectPropList);
	wndPropListLocation.MapWindowPoints(this, &rectPropList);
	wndPropList.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectPropList, this, (UINT)IDC_PROPLIST_LOCATION);
	wndPropList.EnableHeaderCtrl();
	wndPropList.EnableDescriptionArea();
	wndPropList.SetVSDotNetLook(true);
	wndPropList.MarkModifiedProperties(false);
	wndPropList.SetAlphabeticMode(false);
	wndPropList.SetShowDragContext(true);
	// Create "changeable" category for the propertyGrid and fill it with its associated properties
	//
	CMFCPropertyGridProperty * pGroupChangeable = new CMFCPropertyGridProperty(_T("Changeable"));
	// Create and add checksum combobox property to the category
	m_pPropCST = new CMFCPropertyGridProperty(
		_T("Checksum Type"), _T(""), 
		_T("Defines the type for the checksum calculation of the LIN-Frame definition and can be classic, enhanced or auto."));
	m_pPropCST->AddOption(this->getChecksumAsString(cstAuto));
	m_pPropCST->AddOption(this->getChecksumAsString(cstClassic));
	m_pPropCST->AddOption(this->getChecksumAsString(cstEnhanced));
	m_pPropCST->AddOption(this->getChecksumAsString(cstCustom));	
	m_pPropCST->AllowEdit(FALSE);
	pGroupChangeable->AddSubItem(m_pPropCST);
	// Create and add direction combobox property to the category
	m_pPropDirection = new CMFCPropertyGridProperty(
		_T("Direction"), _T(""), 
		_T("Defines the direction of the LIN-Frame definition and can be publisher or subscriber. Default is subscriber."));
	m_pPropDirection->AddOption(this->getDirectionAsString(dirDisabled));
	m_pPropDirection->AddOption(this->getDirectionAsString(dirPublisher));
	m_pPropDirection->AddOption(this->getDirectionAsString(dirSubscriber));
	m_pPropDirection->AddOption(this->getDirectionAsString(dirSubscriberAutoLength));
	m_pPropDirection->AllowEdit(FALSE);
	pGroupChangeable->AddSubItem(m_pPropDirection);
	// Create and add length textBox property to the category
	m_pPropLength = new CMFCPropertyGridProperty(_T("Length"), (COleVariant) _T(""), _T("Defines the length of the LIN-Frame definition."));
	for (int i = 0 ; i < 9 ; i++) 
	{
		strTemp.Format(_T("%d"), i);
		m_pPropLength->AddOption(strTemp);
	}
	m_pPropLength->AllowEdit(FALSE);
	pGroupChangeable->AddSubItem(m_pPropLength);
	// Add "changeable" category and properties to the propertyGrid
	wndPropList.AddProperty(pGroupChangeable);
	// Create "readOnly" category for the propertyGrid and fill it with its associated properties
	//
	CMFCPropertyGridProperty * pGroupReadOnly = new CMFCPropertyGridProperty(_T("ReadOnly"));
	// Create and add ID textBox property to the category
	m_pPropID = new CMFCPropertyGridProperty(_T("ID"), (COleVariant) _T(""), _T("Defines a LIN-Frame identifier of the Frame definition and can be between 00h and 3Fh."));
	m_pPropID->AllowEdit(FALSE);
	pGroupReadOnly->AddSubItem(m_pPropID);
	// Create and add PID textBox property to the category
	m_pPropPID = new CMFCPropertyGridProperty(_T("PID"), (COleVariant) _T(""), _T("Defines the protected LIN-Frame identifier."));
	m_pPropPID->AllowEdit(FALSE);
	pGroupReadOnly->AddSubItem(m_pPropPID);
	// Add "changeable" category and properties to the propertyGrid
	wndPropList.AddProperty(pGroupReadOnly);

	// Populates the frame list
	FillListView();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGFTDialog::OnClose()
{
	
	CDialog::OnClose();
}

void CGFTDialog::OnLvnItemchangedLstframes(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nCnt, nIdx, i;

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	// Check only selection changes 
	if ( !(pNMLV->uChanged & LVIF_STATE) ) 
		return;

	nIdx = -1;
	
    if (!m_fAutomatic)
    {
        m_fAutomatic = true;
		// Clear the selected frames list
		m_pSelectedFrames->RemoveAll();
        // Get the count of selected elements
        // in the ListView 'lvGFT'.
        // Each element is assigned with an
        // element in the CGlobalFrameTable.
        nCnt = lstFrames.GetSelectedCount();
		// add all items to the internal item selection list
		for (i = 0 ; i < nCnt ; i++)
		{
			nIdx = lstFrames.GetNextItem(nIdx, LVNI_SELECTED);
			m_pSelectedFrames->AddTail(m_pGFT->get(nIdx));
		}
        // Update the PropertyGrid control with the newly selected frames
		UpdatePropertyGrid();
        m_fAutomatic = false;
    }
}

LRESULT CGFTDialog::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	POSITION pos;
	CFrameDefinition * frame;
	COleVariant value;
	CString strTemp;
	CMFCPropertyGridProperty* pProp;
	
	pProp = (CMFCPropertyGridProperty*) lParam;

	// if no frame is selected, only update propertyGrid to display original/blank values and return
	if (m_pSelectedFrames->GetCount() == 0)
	{
		UpdatePropertyGrid();
		return 0;
	}

	// Checksum property changed
	if (pProp == m_pPropCST)
	{
		TLINChecksumType lCST = cstCustom;
		// Get selected option/value
		strTemp = (pProp->GetValue().vt == VT_BSTR) ? (CString) pProp->GetValue().bstrVal : _T("");
		if (pProp->GetOption(0) == strTemp)
			lCST = cstAuto;
		else if (pProp->GetOption(1) == strTemp)
			lCST = cstClassic;
		else if (pProp->GetOption(2) == strTemp)
			lCST = cstEnhanced;
		else if (pProp->GetOption(3) == strTemp)
			lCST = cstCustom;
		// Update each selected frames
		for( pos = m_pSelectedFrames->GetHeadPosition(); pos != NULL; ) 
		{
			frame = (CFrameDefinition*) m_pSelectedFrames->GetNext(pos);
			frame->setChecksumType(lCST, true);	// call for propertyChanged to update dialog controls
			// if changes were not validated, rollback/update display
			if (frame->getChecksumType() != lCST)
				UpdatePropertyGrid();
		}
	}
	// Direction property changed
	else if (pProp == m_pPropDirection)
	{
		TLINDirection lDirection = dirDisabled;
		// Get selected option/value
		strTemp = (pProp->GetValue().vt == VT_BSTR) ? (CString) pProp->GetValue().bstrVal : _T("");
		if (pProp->GetOption(0) == strTemp)
			lDirection = dirDisabled;
		else if (pProp->GetOption(1) == strTemp)
			lDirection = dirPublisher;
		else if (pProp->GetOption(2) == strTemp)
			lDirection = dirSubscriber;
		else if (pProp->GetOption(3) == strTemp)
			lDirection = dirSubscriberAutoLength;
		// Update each selected frames
		for( pos = m_pSelectedFrames->GetHeadPosition(); pos != NULL; ) 
		{
			frame = (CFrameDefinition*) m_pSelectedFrames->GetNext(pos);
			frame->setDirection(lDirection, true);	// call for propertyChanged to update dialog controls
			// if changes were not validated, rollback/update display
			if (frame->getDirection() != lDirection)
				UpdatePropertyGrid();
		}
	}
	// Length property changed
	else if (pProp == m_pPropLength)
	{
		int lLen;
		// Get selected option/value
		strTemp = (pProp->GetValue().vt == VT_BSTR) ? (CString) pProp->GetValue().bstrVal : _T("0");
		lLen = atoi(strTemp);
		// Update each selected frames
		for( pos = m_pSelectedFrames->GetHeadPosition(); pos != NULL; ) 
		{
			frame = (CFrameDefinition*) m_pSelectedFrames->GetNext(pos);
			frame->setLength(lLen, true);	// call for propertyChanged to update dialog controls
			// if changes were not validated, rollback/update display
			if (frame->getLength() != lLen)
				UpdatePropertyGrid();
		}
	}
	
	// Update the frames in the list view
	for( pos = m_pSelectedFrames->GetHeadPosition(); pos != NULL; )
	{
		frame = (CFrameDefinition *)m_pSelectedFrames->GetNext(pos);
		// update the frame in the list view
		UpdateListView(frame->getIdAsInt(), frame);
	}
  
    return 0;
}

void CGFTDialog::FillListView()
{
	int iCurrentItem;
    // Lock the ListView.
	lstFrames.LockWindowUpdate();

    // Clear the ListView that
    // will show the Frame Definition of the
    // Global Frame Table.
    lstFrames.DeleteAllItems();
    // Add every object, Frame Definition, from the
    // Global Frame Table into the ListView.
	for (int i = 0; i < m_pGFT->getCount(); i++)
    {
		// Insert a new Item in the List identified by its ID
		iCurrentItem = lstFrames.InsertItem(LVIF_TEXT|LVIF_STATE, i, m_pGFT->get(i)->getId(), 
				0, LVIS_SELECTED, 0, 0);
		// Update the row with the frame subItems
		UpdateListView(iCurrentItem, m_pGFT->get(i));
    }
    // Unlock and update the ListView now.
	lstFrames.UnlockWindowUpdate();
}
void CGFTDialog::UpdateListView(int index, CFrameDefinition * pFrame) 
{
	lstFrames.SetItemText(index, 1, pFrame->getProtectedId());
	lstFrames.SetItemText(index, 2, getDirectionAsString(pFrame->getDirection()) );
	lstFrames.SetItemText(index, 3, pFrame->getLengthAsString());
	lstFrames.SetItemText(index, 4, getChecksumAsString(pFrame->getChecksumType()));
}

void CGFTDialog::UpdatePropertyGrid()
{
	CFrameDefinition * frame;
	POSITION pos;
	TLINDirection direction;
	TLINChecksumType checksumType;
	bool bSameDirection, bSameChecksumType, bSameLength;
	int len;
	
	switch (m_pSelectedFrames->GetCount())
	{
	case 0:
		// no frame selected reset to original value (i.e. blank)
		wndPropList.ResetOriginalValues();
		break;
	case 1:
		frame = (CFrameDefinition *) m_pSelectedFrames->GetTail();
		// set the selected frame properties
		m_pPropCST->SetValue((COleVariant) getChecksumAsString(frame->getChecksumType()));
		m_pPropDirection->SetValue((COleVariant) getDirectionAsString(frame->getDirection()));
		m_pPropLength->SetValue((COleVariant) frame->getLengthAsString());
		m_pPropID->SetValue((COleVariant) frame->getId());
		m_pPropPID->SetValue((COleVariant) frame->getProtectedId());
		break;
	default:
		// get the first selected frame
		frame = (CFrameDefinition *)m_pSelectedFrames->GetHead();
		// get its properties
		checksumType = frame->getChecksumType();
		direction = frame->getDirection();
		len = frame->getLength();		
		// We have to loop through all selected frames to 
		// search for identical properties in the selected frames.
		// If values for a property are different a blank value is displayed
		bSameDirection = bSameChecksumType = bSameLength = true;
		for( pos = m_pSelectedFrames->GetHeadPosition(); pos != NULL; )
		{
			frame = (CFrameDefinition *)m_pSelectedFrames->GetNext(pos);
			if (checksumType != frame->getChecksumType())
				bSameChecksumType = false;
			if (direction != frame->getDirection())
				bSameDirection = false;
			if (len != frame->getLength())
				bSameLength = false;
		}
		frame = (CFrameDefinition *)m_pSelectedFrames->GetHead();	
		// if all the frames have the same CST, display it otherwise reset to the original blank value
		if (bSameChecksumType)
			m_pPropCST->SetValue((COleVariant) getChecksumAsString(frame->getChecksumType()));
		else 
			m_pPropCST->ResetOriginalValue();
		// if all the frames have the same direction, display it otherwise reset to the original blank value
		if (bSameDirection)
			m_pPropDirection->SetValue((COleVariant) getDirectionAsString(frame->getDirection()));
		else 
			m_pPropDirection->ResetOriginalValue();
		// if all the frames have the same length, display it otherwise reset to the original blank value
		if (bSameLength)
			m_pPropLength->SetValue((COleVariant) frame->getLengthAsString());
		else 
			m_pPropLength->ResetOriginalValue();
		// those properties are always different, reset to the original blank value
		m_pPropID->ResetOriginalValue();
		m_pPropPID->ResetOriginalValue();
		break;
	} 
}

CString CGFTDialog::getDirectionAsString(TLINDirection direction)
{
	CString result;
    switch (direction)
    {
        case dirDisabled:
            result = _T("Disabled");
			break;
        case dirPublisher:
            result = _T("Publisher");
			break;
        case dirSubscriber:
            result = _T("Subscriber");
			break;
        case dirSubscriberAutoLength:
            result = _T("Subscriber Automatic Length");
			break;
		default:
			result = _T("");
			break;
    }
    return result;
}

CString CGFTDialog::getChecksumAsString(TLINChecksumType checksumType)
{
	CString result;
    switch (checksumType)
    {
        case cstAuto:
            result = _T("Auto");
			break;
        case cstClassic:
            result = _T("Classic");
			break;
        case cstEnhanced:
            result = _T("Enhanced");
			break;
        case cstCustom:
            result = _T("Custom");
			break;
		default:
			result = _T("");
			break;
    }
    return result;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// CPLINAPIExampleDlg class
//

CPLINAPIExampleDlg::CPLINAPIExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPLINAPIExampleDlg::IDD, pParent)
	, txtFilterFrom(_T("0"))
	, txtFilterTo(_T("63"))
	, txtID(_T(""))
	, txtLength(_T(""))
	, txtData0(_T(""))
	, txtData1(_T(""))
	, txtData2(_T(""))
	, txtData3(_T(""))
	, txtData4(_T(""))
	, txtData5(_T(""))
	, txtData6(_T(""))
	, txtData7(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pGFT = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	Initialization functions

void CPLINAPIExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// Connection
	DDX_Control(pDX, IDC_CBBCHANNEL, cbbChannel);
	DDX_Control(pDX, IDC_BTNHWREFRESH, btnRefresh);
	DDX_Control(pDX, IDC_CBBBAUDRATES, cbbBaudRates);
	DDX_Control(pDX, IDC_CBBHWMODE, cbbHwMode);
	DDX_Control(pDX, IDC_BTNIDENT, btnIdent);
	DDX_Control(pDX, IDC_BTNINIT, btnInit);
	DDX_Control(pDX, IDC_BTNRELEASE, btnRelease);
	// Configuration
	DDX_Control(pDX, IDC_BTNCONFIGFRMS, btnConfig);
	DDX_Control(pDX, IDC_RDBFILTEROPEN, rdbFilterOpen);
	DDX_Control(pDX, IDC_RDBFILTERCLOSE, rdbFilterClose);
	DDX_Control(pDX, IDC_RDBFILTERCUSTOM, rdbFilterCustom);
	DDX_Control(pDX, IDC_TXTFILTERFROM, txtEditFilterFrom);
	DDX_Text(pDX, IDC_TXTFILTERFROM, txtFilterFrom);
	DDX_Control(pDX, IDC_TXTFILTERTO, txtEditFilterTo);
	DDX_Text(pDX, IDC_TXTFILTERTO, txtFilterTo);
	DDX_Control(pDX, IDC_NUDFILTERFROM, nudFilterFrom);
	DDX_Control(pDX, IDC_NUDFILTERTO, nudFilterTo);
	DDX_Control(pDX, IDC_BTNFILTERAPPLY, btnFilterApply);
	DDX_Control(pDX, IDC_BTNFILTERQUERY, btnFilterQuery);
	// Reading
	DDX_Control(pDX, IDC_RDBTIMER, rdbReadingTimer);
	DDX_Control(pDX, IDC_RDBMANUAL, rdbReadingManual);
	DDX_Control(pDX, IDC_CHBTIMESTAMP, chbReadingTimeStamp);
	DDX_Control(pDX, IDC_BTNREAD, btnRead);
	DDX_Control(pDX, IDC_BTNREADCLEAR, btnReadingClear);
	DDX_Control(pDX, IDC_LSTMESSAGES, lstMessages);
	// Writing
	DDX_Control(pDX, IDC_CBBID, cbbID);
	DDX_Text(pDX, IDC_TXTID, txtID);
	DDX_Control(pDX, IDC_CBBDIRECTION, cbbDirection);
	DDX_Control(pDX, IDC_CBBCST, cbbCST);
	DDX_Text(pDX, IDC_TXTLENGTH, txtLength);
	DDX_Control(pDX, IDC_NUDLENGTH, nudLength);
	DDX_Control(pDX, IDC_TXTDATA0, txtEditData0);
	DDX_Control(pDX, IDC_TXTDATA1, txtEditData1);
	DDX_Control(pDX, IDC_TXTDATA2, txtEditData2);
	DDX_Control(pDX, IDC_TXTDATA3, txtEditData3);
	DDX_Control(pDX, IDC_TXTDATA4, txtEditData4);
	DDX_Control(pDX, IDC_TXTDATA5, txtEditData5);
	DDX_Control(pDX, IDC_TXTDATA6, txtEditData6);
	DDX_Control(pDX, IDC_TXTDATA7, txtEditData7);
	DDX_Text(pDX, IDC_TXTDATA0, txtData0);
	DDX_Text(pDX, IDC_TXTDATA1, txtData1);
	DDX_Text(pDX, IDC_TXTDATA2, txtData2);
	DDX_Text(pDX, IDC_TXTDATA3, txtData3);
	DDX_Text(pDX, IDC_TXTDATA4, txtData4);
	DDX_Text(pDX, IDC_TXTDATA5, txtData5);
	DDX_Text(pDX, IDC_TXTDATA6, txtData6);
	DDX_Text(pDX, IDC_TXTDATA7, txtData7);
	DDX_Control(pDX, IDC_BTNWRITE, btnWrite);
	// Information
	DDX_Control(pDX, IDC_LSTINFO, lstInfo);	
	DDX_Control(pDX, IDC_BTNVERSION, btnVersion);
	DDX_Control(pDX, IDC_BTNINFOCLEAR, btnInfoClear);
	DDX_Control(pDX, IDC_BTNSTATUS, btnStatus);
	DDX_Control(pDX, IDC_BTNRESET, btnReset);
}

BEGIN_MESSAGE_MAP(CPLINAPIExampleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTNHWREFRESH, &CPLINAPIExampleDlg::OnBnClickedButtonHwrefresh)
	ON_CBN_SELCHANGE(IDC_CBBCHANNEL, &CPLINAPIExampleDlg::OnCbnSelchangeCbbchannel)
	ON_CBN_SELCHANGE(IDC_CBBHWMODE, &CPLINAPIExampleDlg::OnCbnSelchangeCbbhwmode)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTNIDENT, &CPLINAPIExampleDlg::OnBnClickedBtnident)
	ON_BN_CLICKED(IDC_BTNINIT, &CPLINAPIExampleDlg::OnBnClickedBtninit)
	ON_BN_CLICKED(IDC_BTNRELEASE, &CPLINAPIExampleDlg::OnBnClickedBtnrelease)
	ON_BN_CLICKED(IDC_BTNCONFIGFRMS, &CPLINAPIExampleDlg::OnBnClickedBtnconfigfrms)
	ON_BN_CLICKED(IDC_BTNFILTERAPPLY, &CPLINAPIExampleDlg::OnBnClickedBtnfilterapply)
	ON_BN_CLICKED(IDC_BTNFILTERQUERY, &CPLINAPIExampleDlg::OnBnClickedBtnfilterquery)
	ON_BN_CLICKED(IDC_BTNREAD, &CPLINAPIExampleDlg::OnBnClickedBtnread)
	ON_BN_CLICKED(IDC_BTNREADCLEAR, &CPLINAPIExampleDlg::OnBnClickedBtnreadclear)
	ON_BN_CLICKED(IDC_BTNWRITE, &CPLINAPIExampleDlg::OnBnClickedBtnwrite)
	ON_BN_CLICKED(IDC_BTNVERSION, &CPLINAPIExampleDlg::OnBnClickedBtnversion)
	ON_BN_CLICKED(IDC_BTNINFOCLEAR, &CPLINAPIExampleDlg::OnBnClickedBtninfoclear)
	ON_BN_CLICKED(IDC_BTNSTATUS, &CPLINAPIExampleDlg::OnBnClickedBtnstatus)
	ON_BN_CLICKED(IDC_BTNRESET, &CPLINAPIExampleDlg::OnBnClickedBtnreset)
	ON_BN_CLICKED(IDC_RDBFILTEROPEN, &CPLINAPIExampleDlg::OnBnClickedRdbfilteropen)
	ON_BN_CLICKED(IDC_RDBFILTERCLOSE, &CPLINAPIExampleDlg::OnBnClickedRdbfilterclose)
	ON_BN_CLICKED(IDC_RDBFILTERCUSTOM, &CPLINAPIExampleDlg::OnBnClickedRdbfiltercustom)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NUDFILTERFROM, &CPLINAPIExampleDlg::OnDeltaposNudfilterfrom)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NUDFILTERTO, &CPLINAPIExampleDlg::OnDeltaposNudfilterto)
	ON_BN_CLICKED(IDC_RDBTIMER, &CPLINAPIExampleDlg::OnBnClickedRdbtimer)
	ON_BN_CLICKED(IDC_RDBMANUAL, &CPLINAPIExampleDlg::OnBnClickedRdbmanual)
	ON_BN_CLICKED(IDC_CHBTIMESTAMP, &CPLINAPIExampleDlg::OnBnClickedChbtimestamp)
	ON_CBN_SELCHANGE(IDC_CBBID, &CPLINAPIExampleDlg::OnCbnSelchangeCbbid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NUDLENGTH, &CPLINAPIExampleDlg::OnDeltaposNudlength)
	ON_EN_KILLFOCUS(IDC_TXTDATA0, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata0)
	ON_EN_KILLFOCUS(IDC_TXTDATA1, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata1)
	ON_EN_KILLFOCUS(IDC_TXTDATA2, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata2)
	ON_EN_KILLFOCUS(IDC_TXTDATA3, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata3)
	ON_EN_KILLFOCUS(IDC_TXTDATA4, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata4)
	ON_EN_KILLFOCUS(IDC_TXTDATA5, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata5)
	ON_EN_KILLFOCUS(IDC_TXTDATA6, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata6)
	ON_EN_KILLFOCUS(IDC_TXTDATA7, &CPLINAPIExampleDlg::OnEnKillfocusTxtdata7)
	ON_EN_KILLFOCUS(IDC_TXTFILTERFROM, &CPLINAPIExampleDlg::OnEnKillfocusTxtfilterfrom)
	ON_EN_KILLFOCUS(IDC_TXTFILTERTO, &CPLINAPIExampleDlg::OnEnKillfocusTxtfilterto)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////
// CPLINAPIExampleDlg message handlers

BOOL CPLINAPIExampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Extra initialization here
	InitializeControls();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPLINAPIExampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPLINAPIExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPLINAPIExampleDlg::InitializeControls() 
{
	// Read Messages List Control
	lstMessages.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 30, 2);
	lstMessages.InsertColumn(1, _T("Length"), LVCFMT_LEFT, 50, 3);		
	lstMessages.InsertColumn(2, _T("Data"), LVCFMT_LEFT, 150, 4);		
	lstMessages.InsertColumn(3, _T("Count"), LVCFMT_LEFT, 49, 5);	
	lstMessages.InsertColumn(4, _T("Rcv Time"), LVCFMT_LEFT, 100, 5);	
	lstMessages.InsertColumn(5, _T("Direction"), LVCFMT_LEFT, 60, 5);	
	lstMessages.InsertColumn(6, _T("Errors"), LVCFMT_LEFT, 100, 5);	
	lstMessages.SetExtendedStyle(lstMessages.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	// information list instructions
	IncludeTextMessage(_T("Select a Hardware and a configuration for it. Then click \"Initialize\" button"));

	// Set available baudrates and associated data
	cbbBaudRates.SetItemData(0, 2400);
	cbbBaudRates.SetItemData(1, 9600);
	cbbBaudRates.SetItemData(2, 10400);
	cbbBaudRates.SetItemData(3, 19200);

	// UpDown Filter From
	nudFilterFrom.SetRange(0,63);
	nudFilterFrom.SetPos(0);
	txtFilterFrom = _T("0");
	// UpDown Filter To
	nudFilterTo.SetRange(0,63);
	nudFilterTo.SetPos(63);
	txtFilterTo = _T("63");

	// UpDown Length
	nudLength.SetRange(1, 8);
	nudLength.SetPos(0);
	txtLength = _T("1");

	// Data fields
	txtData0 = txtData1 = txtData2 = txtData3 = \
		txtData4 = txtData5 = txtData6 = txtData7 = _T("00");
	// Set "write combobox" direction data
	cbbDirection.SetItemData(0, dirDisabled);
	cbbDirection.SetItemData(1, dirPublisher);
	cbbDirection.SetItemData(2, dirSubscriber);
	cbbDirection.SetItemData(3, dirSubscriberAutoLength);
	// Set "write combobox" checksumType data
	cbbCST.SetItemData(0, cstAuto);
	cbbCST.SetItemData(1, cstClassic);
	cbbCST.SetItemData(2, cstEnhanced);
	cbbCST.SetItemData(3, cstCustom);

	// Initialize radio and checkbox
	rdbFilterOpen.SetCheck(1);
	rdbReadingTimer.SetCheck(1);
	chbReadingTimeStamp.SetCheck(1);

	// Update UI
	UpdateData(FALSE);

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
	m_LastMsgsList = new CPtrList();
	
    // Register this application with LIN as client.
	m_LastLINErr = m_objPLinApi->RegisterClient(_T("PLIN-EXAMPLE"), (DWORD)GetSafeHwnd(), &m_hClient);
}


//////////////////////////////////////////////////////////////////////////////////////////////
//	Frame event-handlers

void CPLINAPIExampleDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// initialize default state of components
	OnBnClickedButtonHwrefresh();	// refresh hardware list combobox
	OnBnClickedRdbfilteropen();
    cbbDirection.SetCurSel(0);
	cbbCST.SetCurSel(1);
}

void CPLINAPIExampleDlg::OnClose()
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
	while(m_LastMsgsList->GetCount())
		delete m_LastMsgsList->RemoveHead();
	delete m_LastMsgsList;

	CDialog::OnClose();
}


void CPLINAPIExampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Read Pending Message
	if(nIDEvent == 1)
		ReadMessages();
	// Display messages
	if(nIDEvent == 2)
		DisplayMessages();

	CDialog::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////////////////////////////
//	Connection event-handlers

void CPLINAPIExampleDlg::OnCbnSelchangeCbbchannel()
{
	int lnMode, lnCurrBaud;
	CString strBaudRate;
	// get the handle from the comboBoxItem
	HLINHW lwHw = (HLINHW) cbbChannel.GetItemDataPtr(cbbChannel.GetCurSel());;

	if (lwHw != 0)
	{
		btnInit.EnableWindow(TRUE);
		btnIdent.EnableWindow(TRUE);
		// Read the mode of the hardware with the handle lwHw (Master, Slave or None).
		m_LastLINErr = m_objPLinApi->GetHardwareParam(lwHw, hwpMode, &lnMode, 0);
		// Read the baudrate of the hardware with the handle lwHw.
		m_LastLINErr = m_objPLinApi->GetHardwareParam(lwHw, hwpBaudrate, &lnCurrBaud, 0);

		// Update hardware mode comboBox
		if (lnMode == modMaster)
			cbbHwMode.SetCurSel(1);
		else
			cbbHwMode.SetCurSel(0);
		OnCbnSelchangeCbbhwmode();

		// Assign the Baudrate to the Control.
		strBaudRate.Format(_T("%d"), (lnCurrBaud != 0) ? lnCurrBaud : m_wBaudrate);
		cbbBaudRates.SelectString(0,strBaudRate);
	}
	else
	{
		btnInit.EnableWindow(FALSE);
		btnIdent.EnableWindow(FALSE);
	}
}

void CPLINAPIExampleDlg::OnCbnSelchangeCbbhwmode()
{
	// Change the "write" button text depending on the hardware mode
	if (cbbHwMode.GetCurSel() == 1)
		btnWrite.SetWindowText(_T("Transmit"));
	else
		btnWrite.SetWindowText(_T("Publish"));;
}

void CPLINAPIExampleDlg::OnBnClickedButtonHwrefresh()
{
	RefreshHardware();
	// Selects first channel and updates connection information
	cbbChannel.SetCurSel(0);
	OnCbnSelchangeCbbchannel();
}

void CPLINAPIExampleDlg::OnBnClickedBtnident()
{
	// get the handle from the selected comboBoxItem
	HLINHW lwHw = (HLINHW) cbbChannel.GetItemDataPtr(cbbChannel.GetCurSel());;
	if (lwHw != 0)
	{
		// makes the corresponding PCAN-USB-Pro's LED blink
		m_objPLinApi->IdentifyHardware(lwHw);
	}
}

void CPLINAPIExampleDlg::OnBnClickedBtninit()
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

void CPLINAPIExampleDlg::OnBnClickedBtnrelease()
{
	if (DoLinDisconnect()) 
	{
		// Stop reading timer
		SetTimerRead(false);
		// Sets the connection status of the main-form
        SetConnectionStatus(false);
		// Update Global Frame Table LIN information
		m_pGFT->UpdateLinInfo(m_hClient, m_hHw, m_HwMode, &m_lMask);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	Global frames and filter message event-handlers

void CPLINAPIExampleDlg::OnBnClickedBtnconfigfrms()
{
	// Open the "Global Frame Table" Dialog
	CGFTDialog dlgGFT(m_pGFT, this);
	dlgGFT.DoModal();
	// Output filter information (as changes to Global Frame Table items modify it)
	OnBnClickedBtnfilterquery();
    // Update the available frame ids (i.e. the IDs combobox in the "write message" UI group)
    UpdateFrameIds();
}

void CPLINAPIExampleDlg::OnBnClickedBtnfilterapply()
{
	CString strTemp;

	if (m_hHw != 0)
    {
        unsigned __int64 lMask;
        // Sets the according to the selected filter
		if (rdbFilterOpen.GetCheck() == BST_CHECKED)
        {
            // set no filtering : all frames are received
            lMask = FRAME_FILTER_MASK;
            m_LastLINErr = m_objPLinApi->SetClientFilter(m_hClient, m_hHw, lMask);
            if (m_LastLINErr == errOK)
            {
                m_lMask = lMask;
				IncludeTextMessage(_T("The filter was successfully opened."));
            }
        }
        else if (rdbFilterClose.GetCheck() == BST_CHECKED)
        {
            // filters all : all frames are denied
            lMask = 0x0;
            m_LastLINErr = m_objPLinApi->SetClientFilter(m_hClient, m_hHw, lMask);
            if (m_LastLINErr == errOK)
            {
                m_lMask = lMask;
                IncludeTextMessage(_T("The filter was successfully closed."));
            }
        }
        else
        {
            // custom filtering (i.e. rdbFilterCustom.Checked) : the frames for the given range will be opened
			BYTE nudFrom = (BYTE)atoi(txtFilterFrom);
			BYTE nudTo = (BYTE)atoi(txtFilterTo);
            m_LastLINErr = m_objPLinApi->RegisterFrameId(m_hClient, m_hHw, nudFrom, nudTo);
            if (m_LastLINErr == errOK)
            {
				strTemp.Format(_T("The filter was customized. IDs from %s to %s will be received"), txtFilterFrom, txtFilterTo);
                IncludeTextMessage(strTemp);
                m_LastLINErr = m_objPLinApi->GetClientFilter(m_hClient, m_hHw, &lMask);
                if (m_LastLINErr == errOK)
                    m_lMask = lMask;
            }

        }
        // If success, an information message is written, if it is not, an error message is shown
        if (m_LastLINErr != errOK)
            MessageBox(GetFormatedError(m_LastLINErr), SAPPCAPTION_ERROR, MB_ICONERROR);
    }
}

void CPLINAPIExampleDlg::OnBnClickedBtnfilterquery()
{
	char buffer[65], bufferTemp[65];
	int len;
	CString strTemp;
    unsigned __int64 pRcvMask;
	
    // Retrieves the filter corresponding to the current Client-Hardware pair
    if (m_hHw != 0)
    {
		if (m_objPLinApi->GetClientFilter(m_hClient, m_hHw, &pRcvMask) == errOK) {
			// convert to binary string
			_ui64toa_s(pRcvMask, bufferTemp, 65, 2);
			// pad with '0'
			memset(buffer, '0', 64);
			buffer[64] = '\0';
			len = strlen(bufferTemp);
			for (int i = 0 ; i < len ; i++)
				buffer[64-len+i] = bufferTemp[i];
			strTemp.Format(_T("The Status of the filter is %s."), buffer);
            IncludeTextMessage(strTemp);
		}
    }
}
void CPLINAPIExampleDlg::OnBnClickedRdbfilteropen()
{
	txtEditFilterFrom.EnableWindow(FALSE);
	txtEditFilterTo.EnableWindow(FALSE);
	nudFilterFrom.EnableWindow(FALSE);
	nudFilterTo.EnableWindow(FALSE);
}

void CPLINAPIExampleDlg::OnBnClickedRdbfilterclose()
{
	OnBnClickedRdbfilteropen();
}

void CPLINAPIExampleDlg::OnBnClickedRdbfiltercustom()
{
	txtEditFilterFrom.EnableWindow(TRUE);
	txtEditFilterTo.EnableWindow(TRUE);
	nudFilterFrom.EnableWindow(TRUE);
	nudFilterTo.EnableWindow(TRUE);
}

void CPLINAPIExampleDlg::OnDeltaposNudfilterfrom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int iNewVal;

	// Compute new selected From value
	iNewVal =  pNMUpDown->iPos + ((pNMUpDown->iDelta > 0) ? 1 : -1);
	if (iNewVal < 0)
		iNewVal = 0;
	else if (iNewVal > 63)
		iNewVal = 63;
	// Update textBox value
	txtFilterFrom.Format(_T("%d"), iNewVal);
	UpdateData(FALSE);

	*pResult = 0;
}

void CPLINAPIExampleDlg::OnDeltaposNudfilterto(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int iNewVal;

	// Compute new selected From value
	iNewVal =  pNMUpDown->iPos + ((pNMUpDown->iDelta > 0) ? 1 : -1);
	if (iNewVal < 0)
		iNewVal = 0;
	else if (iNewVal > 63)
		iNewVal = 63;
	// Update textBox value
	txtFilterTo.Format(_T("%d"), iNewVal);
	UpdateData(FALSE);

	*pResult = 0;
}
void CPLINAPIExampleDlg::OnEnKillfocusTxtfilterfrom()
{
	int newValue;
	UpdateData(TRUE);

	// Compute new edited value
	newValue = atoi(txtFilterFrom);
	if(newValue > 63)
		newValue = 63;
	else if(newValue < 0)
		newValue = 0;
	// Update Nud control
	nudFilterFrom.SetPos(newValue);
	txtFilterFrom.Format(_T("%d"), newValue);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtfilterto()
{
	int newValue;
	UpdateData(TRUE);

	// Compute new edited value
	newValue = atoi(txtFilterTo);
	if(newValue > 63)
		newValue = 63;
	else if(newValue < 0)
		newValue = 0;
	// Update Nud control
	nudFilterTo.SetPos(newValue);
	txtFilterTo.Format(_T("%d"), newValue);
	UpdateData(FALSE);
}


//////////////////////////////////////////////////////////////////////////////////////////////
//	Read message event-handlers

void CPLINAPIExampleDlg::OnBnClickedBtnread()
{
	ReadMessages();
}

void CPLINAPIExampleDlg::OnBnClickedBtnreadclear()
{	
    // The information contained in the messages List-View is cleared
	while(m_LastMsgsList->GetCount())
		delete m_LastMsgsList->RemoveHead();
	m_LastMsgsList->RemoveAll();
	lstMessages.DeleteAllItems();
}

void CPLINAPIExampleDlg::OnBnClickedRdbtimer()
{
	RdbReadingChanged();
}

void CPLINAPIExampleDlg::OnBnClickedRdbmanual()
{
	RdbReadingChanged();
}

void CPLINAPIExampleDlg::OnBnClickedChbtimestamp()
{
	POSITION pos;
	CMessageStatus * lMsg;
	// According with the check-value of this checkbox,
    // the recieved time of a messages will be interpreted as 
    // period (time between the two last messages) or as time-stamp
    // (the elapsed time since windows was started)
    //
	for( pos = m_LastMsgsList->GetHeadPosition(); pos != NULL; )
	{
		lMsg = (CMessageStatus *) m_LastMsgsList->GetNext(pos);
		lMsg->setShowingPeriod(chbReadingTimeStamp.GetCheck() == BST_CHECKED);
    }
}

void CPLINAPIExampleDlg::RdbReadingChanged() 
{
	int val = rdbReadingTimer.GetState();
	// According with the kind of reading, a timer or a button will be enabled
	if (btnRelease.IsWindowEnabled()) 
		if (rdbReadingTimer.GetState() & BST_CHECKED)
		SetTimerRead(true);
	else 
		SetTimerRead(false);

	btnRead.EnableWindow( !(rdbReadingTimer.GetState() & BST_CHECKED) && btnRelease.IsWindowEnabled());
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	Write message event-handlers

void CPLINAPIExampleDlg::OnBnClickedBtnwrite()
{
	TLINMsg pMsg;
    CString txtbCurrentTextBox;
    CFrameDefinition * lFD;

    // Assert that a frame ID has been selected
    if (cbbID.GetCurSel() == -1)
    {
		cbbID.SetFocus();
        return;
    }

    // Get the CFrameDefinition associated to the selected FrameID
	lFD = (CFrameDefinition *)(cbbID.GetItemDataPtr(cbbID.GetCurSel()));
    // Create a new LIN frame message and copy the data.
	pMsg.FrameId = (BYTE) lFD->getProtectedIdAsInt();
	pMsg.Direction = (TLINDirection) cbbDirection.GetItemDataPtr(cbbDirection.GetCurSel());
	pMsg.ChecksumType = (TLINChecksumType)cbbCST.GetItemDataPtr(cbbCST.GetCurSel());
	pMsg.Length = (BYTE) atoi(txtLength);
    // Fill data array
	pMsg.Data[0] = (BYTE)HexTextToInt(txtData0);
	pMsg.Data[1] = (BYTE)HexTextToInt(txtData1);
	pMsg.Data[2] = (BYTE)HexTextToInt(txtData2);
	pMsg.Data[3] = (BYTE)HexTextToInt(txtData3);
	pMsg.Data[4] = (BYTE)HexTextToInt(txtData4);
	pMsg.Data[5] = (BYTE)HexTextToInt(txtData5);
	pMsg.Data[6] = (BYTE)HexTextToInt(txtData6);
	pMsg.Data[7] = (BYTE)HexTextToInt(txtData7);
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
		m_LastLINErr = m_objPLinApi->UpdateByteArray(m_hClient, m_hHw, lFD->getIdAsByte(), (BYTE)0, pMsg.Length, pMsg.Data);
    }
    // Show error if any
    if (m_LastLINErr != errOK)
		MessageBox(GetFormatedError(m_LastLINErr), SAPPCAPTION_ERROR, MB_ICONERROR);
}
void CPLINAPIExampleDlg::OnCbnSelchangeCbbid()
{
	CFrameDefinition * lFD = (CFrameDefinition *)(cbbID.GetItemDataPtr(cbbID.GetCurSel()));
	// Check if a global frame is selected
	if (lFD != NULL)
	{
		// Update components according to the selected frame informations
		txtID = lFD->getProtectedId();
		txtLength.Format("%d", lFD->getLength());
		switch (lFD->getDirection())
		{
			case dirDisabled:
				cbbDirection.SetCurSel(0);
				break;
			case dirPublisher:
				cbbDirection.SetCurSel(1);
				break;
			case dirSubscriber:
				cbbDirection.SetCurSel(2);
				break;
			case dirSubscriberAutoLength:
				cbbDirection.SetCurSel(3);
				break;
		}
		switch (lFD->getChecksumType())
		{
			case cstAuto:
				cbbCST.SetCurSel(0);
				break;
			case cstClassic:
				cbbCST.SetCurSel(1);
				break;
			case cstEnhanced:
				cbbCST.SetCurSel(2);
				break;
			case cstCustom:
				cbbCST.SetCurSel(3);
				break;
		}
		// Force update on Data fields (both Length and Direction impact on Enabled state of these components)
		// if length value is not changed but direction switch from subscriber to publisher
		// data textfield would not be disabled
		OnDeltaposNudlength(NULL, NULL);
		UpdateData(FALSE);
	}
}

void CPLINAPIExampleDlg::OnDeltaposNudlength(NMHDR *pNMHDR, LRESULT *pResult)
{
	int len, iNewVal;
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	txtEditData0.EnableWindow(FALSE);
    txtEditData1.EnableWindow(FALSE);
    txtEditData2.EnableWindow(FALSE);
    txtEditData3.EnableWindow(FALSE);
    txtEditData4.EnableWindow(FALSE);
    txtEditData5.EnableWindow(FALSE);
    txtEditData6.EnableWindow(FALSE);
    txtEditData7.EnableWindow(FALSE);
    // Only Publisher direction allows edition of Data textFields
	if (cbbDirection.GetCurSel() != 1)
        return;
	
	// Assert the function is called via an Event
	if (pNMUpDown != NULL) 
	{
		// Compute new selected From value
		iNewVal =  pNMUpDown->iPos + ((pNMUpDown->iDelta > 0) ? 1 : -1);
		if (iNewVal < 1)
			iNewVal = 1;
		else if (iNewVal > 8)
			iNewVal = 8;
		// Update textBox value
		txtLength.Format(_T("%d"), iNewVal);
		UpdateData(FALSE);
		
		*pResult = 0;
	}

    // Enable the same number of data textBox as the nudLength value
	len = atoi(txtLength);
    switch (len)
    {
        case 1:
            txtEditData0.EnableWindow(TRUE);
            break;
        case 2:
            txtEditData0.EnableWindow(TRUE);
            txtEditData1.EnableWindow(TRUE);
            break;
        case 3:
            txtEditData0.EnableWindow(TRUE);
            txtEditData1.EnableWindow(TRUE);
            txtEditData2.EnableWindow(TRUE);
            break;
        case 4:
            txtEditData0.EnableWindow(TRUE);
            txtEditData1.EnableWindow(TRUE);
            txtEditData2.EnableWindow(TRUE);
            txtEditData3.EnableWindow(TRUE);
            break;
        case 5:
            txtEditData0.EnableWindow(TRUE);
            txtEditData1.EnableWindow(TRUE);
            txtEditData2.EnableWindow(TRUE);
            txtEditData3.EnableWindow(TRUE);
            txtEditData4.EnableWindow(TRUE);
            break;
        case 6:
            txtEditData0.EnableWindow(TRUE);
            txtEditData1.EnableWindow(TRUE);
            txtEditData2.EnableWindow(TRUE);
            txtEditData3.EnableWindow(TRUE);
            txtEditData4.EnableWindow(TRUE);
            txtEditData5.EnableWindow(TRUE);
            break;
        case 7:
            txtEditData0.EnableWindow(TRUE);
            txtEditData1.EnableWindow(TRUE);
            txtEditData2.EnableWindow(TRUE);
            txtEditData3.EnableWindow(TRUE);
            txtEditData4.EnableWindow(TRUE);
            txtEditData5.EnableWindow(TRUE);
            txtEditData6.EnableWindow(TRUE);
            break;
        case 8:
            txtEditData0.EnableWindow(TRUE);
            txtEditData1.EnableWindow(TRUE);
            txtEditData2.EnableWindow(TRUE);
            txtEditData3.EnableWindow(TRUE);
            txtEditData4.EnableWindow(TRUE);
            txtEditData5.EnableWindow(TRUE);
            txtEditData6.EnableWindow(TRUE);
            txtEditData7.EnableWindow(TRUE);
            break;
    }
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata0()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData0);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata1()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData1);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata2()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData2);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata3()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData3);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata4()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData4);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata5()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData5);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata6()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData6);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::OnEnKillfocusTxtdata7()
{
	UpdateData(TRUE);
	CheckHexEditBox(&txtData7);
	UpdateData(FALSE);
}

void CPLINAPIExampleDlg::UpdateFrameIds()
{
	CString lID;
	CFrameDefinition * lFrame, * lSelectedFrame;
	int idx;
	
	lFrame = lSelectedFrame = NULL;
	// Retrieves selected ID if it exists, before clearing comboBox
	if	(cbbID.GetCurSel() != CB_ERR)
		lSelectedFrame = (CFrameDefinition*) cbbID.GetItemData(cbbID.GetCurSel());

    // Clears and populates FrameID combobox with global frame IDs
	cbbID.ResetContent();
	for (int i = 0; i < m_pGFT->getCount() ; i++)
    {
		lFrame = m_pGFT->get(i);
        // add only non disabled frames
		if (lFrame->getDirection() == dirDisabled)
            continue;
		// Add frame and data
		idx = cbbID.AddString(lFrame->getId());
		cbbID.SetItemDataPtr(idx, lFrame);
        // Check if the new item was selected before the update
        if (lSelectedFrame == lFrame)
			cbbID.SetCurSel(idx);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	Information group event-handlers

void CPLINAPIExampleDlg::OnBnClickedBtnversion()
{
    TLINVersion lpVersion;
	char buffer[256];
    CString info, strTemp;
	int iPos = 0;

	memset(buffer,'\0',255);

	// We get the vesion of the PLIN API
    m_LastLINErr = m_objPLinApi->GetVersion(&lpVersion);
    if (m_LastLINErr == errOK)
    {
		strTemp.Format(_T("API Version: %d.%d.%d.%d"), lpVersion.Major, lpVersion.Minor, lpVersion.Build, lpVersion.Revision);
        IncludeTextMessage(strTemp);
        // We get the driver version
        m_LastLINErr = m_objPLinApi->GetVersionInfo(buffer, 255);
        if (m_LastLINErr == errOK)
        {
			info = buffer;
            IncludeTextMessage(_T("Channel/Driver Version: "));
            // Because this information contains line control characters (several lines)
            // we split this also in several entries in the Information List-Box
			strTemp = info.Tokenize(_T("\n"),iPos); 
			while(strTemp != _T(""))
			{			
				strTemp.Insert(0, _T("     * "));
				IncludeTextMessage(strTemp);
				strTemp = info.Tokenize(_T("\n"),iPos);			
			}
        }
    }
    // If an error ccurred, a message is shown
    if (m_LastLINErr != errOK)
        MessageBox(GetFormatedError(m_LastLINErr), SAPPCAPTION_ERROR, MB_ICONERROR);
}

void CPLINAPIExampleDlg::OnBnClickedBtninfoclear()
{
    // The information contained in the Information List-Box is cleared
	lstInfo.ResetContent();
}

void CPLINAPIExampleDlg::OnBnClickedBtnstatus()
{
	TLINHardwareStatus lStatus;
    // Retrieves the status of the LIN Bus and outputs its state in the information listView
    m_LastLINErr = m_objPLinApi->GetStatus(m_hHw, &lStatus);
    if (m_LastLINErr == errOK)
        switch (lStatus.Status)
        {
            case hwsActive:
                IncludeTextMessage(_T("Bus: Active"));
                break;
            case hwsAutobaudrate:
                IncludeTextMessage(_T("Hardware: Baudrate Detection"));
                break;
            case hwsNotInitialized:
                IncludeTextMessage(_T("Hardware: Not Initialized"));
                break;
            case hwsShortGround:
                IncludeTextMessage(_T("Bus-Line: Shorted Ground"));
                break;
            case hwsSleep:
                IncludeTextMessage(_T("Bus: Sleep"));
				break;
        }
	else
		MessageBox(GetFormatedError(m_LastLINErr), SAPPCAPTION_ERROR, MB_ICONERROR);
}

void CPLINAPIExampleDlg::OnBnClickedBtnreset()
{
	// Flushes the Receive Queue of the Client and resets its counters. 
	m_LastLINErr = m_objPLinApi->ResetClient(m_hClient);

	// If it fails, a error message is shown
	if (m_LastLINErr != errOK)
		MessageBox(GetFormatedError(m_LastLINErr), SAPPCAPTION_ERROR, MB_ICONERROR);
	else
	{
		// clears the message list-view
		OnBnClickedBtnreadclear();
		IncludeTextMessage(_T("Receive queue and counters successfully reset"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	Timer event-handler & functions

void CPLINAPIExampleDlg::SetTimerRead(bool bEnable)
{
	// Init Timer
	if(bEnable)
		m_tmrRead = SetTimer(1, 50, 0);
	else
	{
		//Delete Timer
		KillTimer(m_tmrRead);
		m_tmrRead = 0;
	}
}

void CPLINAPIExampleDlg::SetTimerDisplay(bool bEnable)
{
	// Init Timer
	if(bEnable)
		m_tmrDisplay = SetTimer(2, 100, 0);
	else
	{
		//Delete Timer
		KillTimer(m_tmrDisplay);
		m_tmrDisplay = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	LIN connection functions

void CPLINAPIExampleDlg::RefreshHardware()
{
	 BYTE i;
	 HLINHW lwHwHandles[8];
	 WORD lwBuffSize;
	 int lwCount, lnHwType, lnDevNo, lnChannel, lnMode;
	 TLINError lLINErr;
	 HLINHW lwHw;
	 CString str;

	 //lwHwHandles = new HLINHW[8];
	 lwBuffSize = 8;
	 lwCount = 0;
	 // Get all available LIN hardware.
	 lLINErr = m_objPLinApi->GetAvailableHardware(lwHwHandles, lwBuffSize, &lwCount);
	 
	 if (lLINErr == errOK)
	 {
		 // combobox ItemData only holds DWORD, no need to free resources
		 cbbChannel.ResetContent();
		 // If no error occurs
		 if (lwCount == 0)
		 {
			 // No LIN hardware was found.
			 // Show an empty entry
			 lwHw = 0;
			 cbbChannel.AddString(_T("<No hardware found>"));
			 cbbChannel.SetItemData(0, NULL);
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
				 str.Format(_T("PCAN-USB Pro - dev. %d, chan. %d"), lnDevNo, lnChannel);
			 else
				 // Show as unknown hardware
				 str.Format(_T("Unknown - dev. %d, chan. %d"), lnDevNo, lnChannel);
			 
			 cbbChannel.AddString(str);
			 cbbChannel.SetItemData(cbbChannel.GetCount()-1, lwHw);
		 }
		 cbbChannel.SetCurSel(0);
	 }
}



bool CPLINAPIExampleDlg::DoLinConnect()
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
	lwHw = (HLINHW) cbbChannel.GetItemDataPtr(cbbChannel.GetCurSel());;
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
			if (cbbHwMode.GetCurSel() == 1)
				lHwMode = modMaster;
			else
				lHwMode = modSlave;
			// Get the selected baudrate
			lwBaud = (WORD) cbbBaudRates.GetItemData(cbbBaudRates.GetCurSel());
			// Get the selected hardware channel
			if ( (lnMode == modNone) || (lnCurrBaud != lwBaud))
			{
				// Only if the current hardware is not initialize
				// try to Intialize the hardware with mode and baudrate
				m_LastLINErr = m_objPLinApi->InitializeHardware(m_hClient,
					m_hHw, lHwMode, lwBaud);
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
			MessageBox(GetFormatedError(m_LastLINErr), SAPPCAPTION_ERROR, MB_ICONERROR);
			fRet = false;
		}

	}
	else // Should never occur
		m_hHw = 0; // But if it occurs, set handle to default

	return fRet;
}

bool CPLINAPIExampleDlg::DoLinDisconnect()
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
				MessageBox(GetFormatedError(m_LastLINErr), SAPPCAPTION_ERROR, MB_ICONERROR);
				return false;
			}
		}
		else
			return true;
	}
	else
		return true;
}

void CPLINAPIExampleDlg::ReadFrameTableFromHw()
{
    int i, lnCnt;
    TLINFrameEntry lFrameEntry;
    TLINError lErr;
    UINT64 llMask;
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

void CPLINAPIExampleDlg::ReadMessages() 
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

	} while (btnRelease.IsWindowEnabled() && (!(m_LastLINErr & errRcvQueueEmpty)));
}

void CPLINAPIExampleDlg::ProcessMessage(TLINRcvMsg linMsg)
{
	POSITION pos;
	CMessageStatus * lMsg;

	// We search if a message (Same ID and Type) has 
    // already been received or if this is a new message
    //
   	for( pos = m_LastMsgsList->GetHeadPosition(); pos != NULL; )
	{
		lMsg = (CMessageStatus *) m_LastMsgsList->GetNext(pos);
		if (lMsg->getLINMsg().FrameId == linMsg.FrameId)
        {
            // Modify the message and exit
            //
            lMsg->Update(linMsg);
			TLINMsg tmsg;
			tmsg.FrameId = 0x30;
			tmsg.Data[0] = linMsg.Data[0];
			tmsg.Data[1] = linMsg.Data[1];;
			tmsg.Data[2] = linMsg.Data[2];;
			tmsg.Data[3] = linMsg.Data[3];;
			tmsg.Data[4] = linMsg.Data[4];;
			tmsg.Data[5] = linMsg.Data[5];;
			tmsg.Data[6] = linMsg.Data[6];;
			tmsg.Data[7] = linMsg.Data[7];;
			tmsg.Direction = dirSubscriber;
			tmsg.ChecksumType = linMsg.ChecksumType;
			tmsg.Checksum = linMsg.Checksum;
			tmsg.Length = 8;
			//TLINError ERR = m_objPLinApi->Write(m_hClient, m_hHw, &tmsg);
			/*if (ERR==errOK)
			{
				printf("err: %d\n", ERR);
			}
			else
			{
				printf("err: %d\n", ERR);
				printf("It's have somewrong\n");
			}*/
            return;
        }
    }
    // Message not found. It will be created
    //
    InsertMsgEntry(linMsg);
}

void CPLINAPIExampleDlg::InsertMsgEntry(TLINRcvMsg newMsg)
{
    CMessageStatus * lMsg;
    CString strId, strTmp;
	int i;

    // We add this status in the last message list
    //
	lMsg = new CMessageStatus(newMsg, lstMessages.GetItemCount());
	m_LastMsgsList->AddTail(lMsg);
    
    // Search and retrieve the ID in the global frame table associated with the frame Protected-ID
    strId = "";
    for (i = 0; i < m_pGFT->getCount(); i++)
    {
		if (lMsg->getLINMsg().FrameId == (BYTE) m_pGFT->get(i)->getProtectedIdAsInt() )
        {
			strId = m_pGFT->get(i)->getId();
            break;
        }

    }

    // Add the new ListView Item with the ID of the message
	i = lstMessages.InsertItem(LVIF_TEXT|LVIF_STATE, lstMessages.GetItemCount(), strId, 
				0, LVIS_SELECTED, 0, 0);
    // We set the length of the message
	strTmp.Format(_T("%d"), lMsg->getLINMsg().Length);
	lstMessages.SetItemText(i, 1, strTmp);
    // We set the data of the message. 	
	lstMessages.SetItemText(i, 2, lMsg->getDataString());
    // we set the message count message (this is the First, so count is 1)            
	strTmp.Format(_T("%d"), lMsg->getCount());
	lstMessages.SetItemText(i, 3, strTmp);
    // Add time stamp information if needed
    lstMessages.SetItemText(i, 4, lMsg->getTimeString());
    // We set the direction of the message
	lstMessages.SetItemText(i, 5, lMsg->getDirectionString());
    // We set the error of the message
	lstMessages.SetItemText(i, 6, lMsg->getErrorString());
    // We set the CST of the message
	lstMessages.SetItemText(i, 7, lMsg->getCSTString());
    // We set the CRC of the message
	lstMessages.SetItemText(i, 8, lMsg->getChecksumString());
}

void CPLINAPIExampleDlg::DisplayMessages()
{
	POSITION pos;
	CMessageStatus * lMsg;
	CString strTmp;

	for( pos = m_LastMsgsList->GetHeadPosition(); pos != NULL; )
	{
        // Get the data to actualize
		lMsg = (CMessageStatus *) m_LastMsgsList->GetNext(pos);
	

		if (lMsg->isMarkedAsUpdated())
        {
			lMsg->setMarkedAsUpdated(false);
			// Update the length of the message
			strTmp.Format(_T("%d"), lMsg->getLINMsg().Length);
			lstMessages.SetItemText(lMsg->getPosition(), 1, strTmp);
			// Update the data of the message. 	
			lstMessages.SetItemText(lMsg->getPosition(), 2, lMsg->getDataString());
			// Update the message count message
			strTmp.Format(_T("%d"), lMsg->getCount());
			lstMessages.SetItemText(lMsg->getPosition(), 3, strTmp);
			// Update time stamp information
			lstMessages.SetItemText(lMsg->getPosition(), 4, lMsg->getTimeString());
			// Update the direction of the message
			lstMessages.SetItemText(lMsg->getPosition(), 5, lMsg->getDirectionString());
			// Update the error of the message
			lstMessages.SetItemText(lMsg->getPosition(), 6, lMsg->getErrorString());
			//  Update the CST of the message
			lstMessages.SetItemText(lMsg->getPosition(), 7, lMsg->getCSTString());
			// Update the CRC of the message
			lstMessages.SetItemText(lMsg->getPosition(), 8, lMsg->getChecksumString());
        }
    }
}


///////////////////////////////////////////////////////////////////////////
// Helping functions

void CPLINAPIExampleDlg::SetConnectionStatus(bool bConnected) 
{
	// Buttons
	btnInit.EnableWindow(!bConnected);
	btnConfig.EnableWindow(bConnected);
	btnRead.EnableWindow(bConnected && rdbReadingManual.GetState());
	btnWrite.EnableWindow(bConnected);
	btnRelease.EnableWindow(bConnected);
	btnFilterApply.EnableWindow(bConnected);
	btnFilterQuery.EnableWindow(bConnected);
	btnVersion.EnableWindow(bConnected);
	btnRefresh.EnableWindow(!bConnected);
	btnStatus.EnableWindow(bConnected);
	btnReset.EnableWindow(bConnected);

	// ComboBoxs
	cbbBaudRates.EnableWindow(!bConnected);
	cbbChannel.EnableWindow(!bConnected);
	cbbHwMode.EnableWindow(!bConnected);

	// Hardware configuration and read mode
	if (!bConnected)
		// not connected, update connection information comboboxes
		OnCbnSelchangeCbbchannel();
	else	
		// connected, update reading mode
		RdbReadingChanged();

	// Display messages in grid
	SetTimerDisplay(bConnected);
}

CString CPLINAPIExampleDlg::GetFormatedError(TLINError error)
{
	char buffer[256];
	CString result;

	memset(buffer,'\0',255);
	// If any error are occured
	// display the error text in a message box.
	// 0x00 = Neutral
	// 0x07 = Language German
	// 0x09 = Language English
	if (m_objPLinApi->GetErrorText(error, 0x09, buffer, 255) != errOK)
		result.Format(_T("An error occurred. Error-code's text (%Xh) couldn't be retrieved"), error);
	else 
		result = buffer;
	return result;
}

void CPLINAPIExampleDlg::IncludeTextMessage(CString strMsg)
{
	lstInfo.SetCurSel(lstInfo.InsertString(lstInfo.GetCount(), strMsg));
}

CString CPLINAPIExampleDlg::IntToStr(int iValue)
{
	char chToReceive[20];

	_itoa_s(iValue,chToReceive,10);
	return chToReceive;
}

CString CPLINAPIExampleDlg::IntToHex(int iValue, short iDigits)
{	
	CString strTemp, strtest;

	strTemp.Format("%0" + IntToStr(iDigits) + "X",iValue);
	if (strTemp.GetLength() > iDigits)
		strTemp = "00";

	return strTemp;
}

DWORD CPLINAPIExampleDlg::HexTextToInt(CString ToConvert)
{
	DWORD iToReturn = 0;
	int iExp = 0;
	char chByte;

	// The string to convert is empty
	//
	if(ToConvert == "")
		return 0;
	// The string have more than 8 character (the equivalent value
	// exeeds the DWORD capacyty
	//
	if(ToConvert.GetLength() > 8)
		return 0;
	// We convert any character to its Upper case
	//
	ToConvert = ToConvert.MakeUpper();

	try
	{
		// We calculate the number using the Hex To Decimal formula
		//
		for(int i= ToConvert.GetLength()-1; i >= 0; i--){
			chByte = ToConvert[i];
			switch(int(chByte)){
				case 65:
					iToReturn += (DWORD)(10*pow(16.0f,iExp));
					break;
				case 66:
					iToReturn += (DWORD)(11*pow(16.0f,iExp));
					break;
				case 67:
					iToReturn += (DWORD)(12*pow(16.0f,iExp));
					break;
				case 68:
					iToReturn += (DWORD)(13*pow(16.0f,iExp));
					break;
				case 69:
					iToReturn += (DWORD)(14*pow(16.0f,iExp));
					break;
				case 70:
					iToReturn += (DWORD)(15*pow(16.0f,iExp));
					break;
				default:
					if((int(chByte) <48)||(int(chByte)>57))
						return -1;
					iToReturn += (DWORD)(atoi(&chByte)*pow(16.0f,iExp));
					break;

			}
			iExp++;
		}
	}
	catch(...)
	{
		// Error, return 0
		//
		return 0;
	}

	return iToReturn;
}

void CPLINAPIExampleDlg::CheckHexEditBox(CString* txtData) 
{
	int iTest;

	txtData->MakeUpper();

	// Test if the given ID is a valid hexadecimal number.
	iTest = HexTextToInt(*txtData);
	if(iTest > 0)
		*txtData = IntToHex(iTest,2);
	else
		*txtData = "00";
}
