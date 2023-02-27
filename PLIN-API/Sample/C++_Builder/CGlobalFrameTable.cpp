#include <vcl.h>
#include "CGlobalFrameTable.h"


//////////////////////////////////////////////////////////////////////////////////////////////
// CFrameDefinition class
//

CFrameDefinition::CFrameDefinition(void * pParent, int id, int pid, int len, TLINChecksumType checksumType, TLINDirection direction)	:
	m_pParent(pParent), m_nID(id), m_nProtectedID(pid), m_nLength(len), m_nChecksumType(checksumType), m_bDirection(direction)
{
}

CFrameDefinition::~CFrameDefinition()
{
}

AnsiString CFrameDefinition::getId()
{	
	return AnsiString::Format("%.02Xh", ARRAYOFCONST((m_nID)));
}

int CFrameDefinition::getIdAsInt()	
{ 
	return m_nID; 
}

BYTE CFrameDefinition::getIdAsByte()	
{ 
	return (BYTE) m_nID; 
}

AnsiString CFrameDefinition::getProtectedId() 
{	
	return AnsiString::Format("%.02Xh", ARRAYOFCONST((m_nProtectedID)));
}

int CFrameDefinition::getProtectedIdAsInt()	
{ 
	return m_nProtectedID; 
}

int CFrameDefinition::getLength()	
{ 
	return m_nLength; 
}

AnsiString CFrameDefinition::getLengthAsString()	
{ 
	return AnsiString::Format("%d", ARRAYOFCONST((m_nLength)));
}

void CFrameDefinition::setLength(int length, bool doCallFramePropertyChanged /* = false */)
{
	bool allowed = true;
	if (m_pParent != NULL && doCallFramePropertyChanged)
		((CGlobalFrameTable*) m_pParent)->CallFramePropertyChanged(propLength, this, (void *) length, &allowed);
	if (allowed)
		m_nLength = length;
}

TLINChecksumType CFrameDefinition::getChecksumType() 
{ 
	return m_nChecksumType; 
}

void CFrameDefinition::setChecksumType(TLINChecksumType checksumType, bool doCallFramePropertyChanged /* = false */) 
{ 
	bool allowed = true;
	if (m_pParent != NULL && doCallFramePropertyChanged)
		((CGlobalFrameTable*) m_pParent)->CallFramePropertyChanged(propChecksumType, this, (void *) checksumType, &allowed);
	if (allowed)
		m_nChecksumType = checksumType; 
}

TLINDirection CFrameDefinition::getDirection()	
{ 
	return m_bDirection; 
}

void CFrameDefinition::setDirection(TLINDirection direction, bool doCallFramePropertyChanged /* = false */)	
{ 
	bool allowed = true;
	if (m_pParent != NULL && doCallFramePropertyChanged)
		((CGlobalFrameTable*) m_pParent)->CallFramePropertyChanged(propDirection, this, (void *) direction, &allowed);
	if (allowed)
		m_bDirection = direction; 
}


//////////////////////////////////////////////////////////////////////////////////////////////
// CGlobalFrameTable class
//

CGlobalFrameTable::CGlobalFrameTable(PLinApiClass * objPLinApi) :
	m_objPLinApi(objPLinApi), 
	m_hClient(0),
	m_hHw(0),
	m_HwMode(modNone),
	m_pMask(NULL)
{
    // Create all CFrameDefinition objects and set
    // them values to default. The length values is
    // set to LIN 1.2.
    // Default values:
    // - Direction = SubscriberAutoLength
    // - ChecksumType = Auto
    // - Length = see inside the loop.
    for (int i = 0; i < GLOBALFRAMETABLE_SIZE; i++)
    {
        // From Frame-ID 0 to 31 set the length 2.
        if ((i >= 0x00) && (i <= 0x1F))
            AddFrameDefinition(i, 2, cstAuto, dirSubscriberAutoLength);
        // From Frame-ID 32 to 47 set the length 4.
        else if ((i >= 0x20) && (i <= 0x2F))
            AddFrameDefinition(i, 4, cstAuto, dirSubscriberAutoLength);
        // From Frame-ID 48 to 63 set the length 8.
        else if ((i >= 0x30) && (i <= 0x3F))
            AddFrameDefinition(i, 8, cstAuto, dirSubscriberAutoLength);
    }
}

CGlobalFrameTable::~CGlobalFrameTable()
{
	// delete all frames
	for (int i = 0; i < GLOBALFRAMETABLE_SIZE; i++) {
		delete m_lFrameDefinitions[i];
	}
}

void CGlobalFrameTable::UpdateLinInfo(const HLINCLIENT hClient, const HLINHW hHw, const TLINHardwareMode hwMode, unsigned __int64 * pMask)
{
	// up-to-date LIN information is required by function CallFramePropertyChanged
	m_hClient = hClient;
	m_hHw = hHw;
	m_HwMode = hwMode;
	m_pMask = pMask;
}

void CGlobalFrameTable::CallFramePropertyChanged(EFRAMEPROPERTY propertyType, CFrameDefinition * pFrame, void * value, bool * allowed)
{
    TLINFrameEntry lFrameEntry;
    TLINError lErr;
    unsigned __int64 lMask;
	
    if (pFrame != NULL)
    {
        // If data length is to be set, check the value.
		if (propertyType == propLength) 
            // Only a value between 0 and 8 are valid.
			*allowed = ((int) value >= 0) && ((int) value <= 8);
        // If DO NOT allow then return.
        if (!*allowed)
            return;

        // Only if the hardware was initialized as Slave
        // set the direction of the LIN-Frame.
        // By the Master the direction will be used with
        // the LIN_Write and do not have to set here.
        if (m_HwMode == modSlave)
        {
            // Temporary set Allowed to false is to easy
            // return only later.
            *allowed = false;

            // Set the Frame-ID of the Frame to get and set.
            // The ID have to set before get the entry.
			lFrameEntry.FrameId = pFrame->getIdAsByte();
            // Get the Frame Entry with the Frame-ID from
            // the Hardware via the LinApi.
            lErr = m_objPLinApi->GetFrameEntry(m_hHw, &lFrameEntry);
            // If an error occurs do not allow to change
            // the property and return.
            // The Allowed parameter was set some lines before.
            if (lErr != errOK)
                return;
            // Switch between the different kind of property types.
            switch (propertyType)
            {
                // Direction property should be set
                case propDirection:
					lFrameEntry.Direction = (TLINDirection) value;
                    break;
                // Length property should be set
                case propLength:
                    lFrameEntry.Length = (BYTE) value;;
                    break;
                // ChecksumType property should be set
                case propChecksumType:
                    lFrameEntry.ChecksumType = (TLINChecksumType) value;
                    break;
            }
            lFrameEntry.Flags = FRAME_FLAG_RESPONSE_ENABLE;
			lErr = m_objPLinApi->SetFrameEntry(m_hClient, m_hHw, &lFrameEntry);
            // If an error occurs do not allow to change
            // the property and return.
            // The Allowed parameter was set some lines before.
            if (lErr != errOK)
                return;

            // Temporary set Allowed to true for next check.
            // The action was successfull on this line.
            *allowed = true;
        }

        // If the property 'Direction' of one
        // CFrameDefinition will be changed,
        // here we need a special request to set
        // the client filter.
        if (propertyType == propDirection)
        {
            // If the new value for the property 'Direction'
            // should NOT be 'Disabled' check first if
            // the CFrameDefinition is defined already with some
            // other value then 'Disabled'.
            if ((TLINDirection)value != dirDisabled)
            {
				if (pFrame->getDirection() == dirDisabled)
                {
                    // If the old property of CFrameDefintion
                    // was set to 'Disabled' the new value
                    // means that the Frame-ID have to add to
                    // the client filter by the LinApi.
                    // Set the client filter.
                    // The Filter is a bit mask.
					lMask = (unsigned __int64)1 << pFrame->getIdAsInt();
					*m_pMask = *m_pMask | lMask;
                    lErr = m_objPLinApi->SetClientFilter(m_hClient, m_hHw, *m_pMask);
                    // Only allow to change the property if the Frame-ID
                    // was added successfull to the Filter.
                    *allowed = lErr == errOK;
                }
            }
            else
            {
                // If the value of direction should set on 'disable'
                // Remove the Frame-ID from the client filter.
				lMask = (unsigned __int64)1 << pFrame->getIdAsInt();
                *m_pMask = *m_pMask & ~lMask;
                lErr = m_objPLinApi->SetClientFilter(m_hClient, m_hHw, *m_pMask);
                // Only allow to change the property if the Frame-ID
                // was removed successfull from the Filter.
                *allowed = lErr == errOK;
            }
        }
    }
}

int CGlobalFrameTable::AddFrameDefinition(int AId, int ALength, TLINChecksumType AChecksumType, TLINDirection ADirection)
{
	// Check the Frame-ID for before adding it.
	// Only ID's from 0 to 63 are allowed.
	if ((AId < 0) || (AId > 63))
		// ID is invalid. Do not add it.
		return -1;
	else
	{
		// The delivered Frame-ID is valid.
		CFrameDefinition * lFD;
		BYTE lbID;
		// Calculate the Protected-ID with the delivered Frame-ID.
		lbID = (BYTE) AId;
		m_objPLinApi->GetPID(&lbID);
		// Create a Frame Definition object.
		// and assigned the delivered values to it.
		lFD = new CFrameDefinition(this, AId, lbID, ALength, AChecksumType, ADirection);
		// Add the created object to the list
		// and return the position of the new added object.
		// It should be added at the end so the position
		// must be the last entry in the list.
		m_lFrameDefinitions[AId] = lFD;
	}
	return AId;
}

CFrameDefinition * CGlobalFrameTable::get(int index)
{
	if (index >= 0 && index < GLOBALFRAMETABLE_SIZE)
		return m_lFrameDefinitions[index];
	return NULL;
}

int CGlobalFrameTable::getCount()
{
	return GLOBALFRAMETABLE_SIZE;
}

int CGlobalFrameTable::indexOf(CFrameDefinition * pFrame)
{
	for (int i = 0; i < GLOBALFRAMETABLE_SIZE; i++)	{
		if (m_lFrameDefinitions[i] == pFrame)
			return i;
	}
	return -1;
}

