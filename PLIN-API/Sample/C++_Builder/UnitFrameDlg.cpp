//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitFrameDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrameDlg *FrameDlg;
                                              
#define		PROP_CHECKSUMTYPE	"Checksum Type"
#define		PROP_DIRECTION		"Direction Type"
#define		PROP_LENGTH			"Length"
#define		PROP_ID				"ID"
#define		PROP_PID			"Protected ID"

//---------------------------------------------------------------------------
__fastcall TFrameDlg::TFrameDlg(CGlobalFrameTable* pGFT, TComponent* Owner)
	: TForm(Owner),
	m_pGFT(pGFT)
{
	int i, j;
	
	m_pSelectedFrames = new TList();
    // lock updates to prevent unnecessary events
    m_fAutomatic = true;
	FillListView();
    // changeable properties : Checksum Type
	i = pgGFTDef->Strings->Add(AnsiString::Format("%s=", ARRAYOFCONST((PROP_CHECKSUMTYPE))));
	pgGFTDef->ItemProps[i]->EditStyle = esPickList;
    pgGFTDef->ItemProps[i]->ReadOnly = false;
	pgGFTDef->ItemProps[i]->PickList->AddObject(getChecksumAsString(cstAuto), (TObject*)cstAuto);
	pgGFTDef->ItemProps[i]->PickList->AddObject(getChecksumAsString(cstClassic), (TObject*)cstClassic);
	pgGFTDef->ItemProps[i]->PickList->AddObject(getChecksumAsString(cstEnhanced), (TObject*)cstEnhanced);
	pgGFTDef->ItemProps[i]->PickList->AddObject(getChecksumAsString(cstCustom), (TObject*)cstCustom);
	// changeable properties : Direction
	i = pgGFTDef->Strings->Add(AnsiString::Format("%s=", ARRAYOFCONST((PROP_DIRECTION))));
    pgGFTDef->ItemProps[i]->EditStyle = esPickList;
    pgGFTDef->ItemProps[i]->ReadOnly = false;
	pgGFTDef->ItemProps[i]->PickList->AddObject(getDirectionAsString(dirDisabled), (TObject*)dirDisabled);
	pgGFTDef->ItemProps[i]->PickList->AddObject(getDirectionAsString(dirPublisher), (TObject*)dirPublisher);
	pgGFTDef->ItemProps[i]->PickList->AddObject(getDirectionAsString(dirSubscriber), (TObject*)dirSubscriber);
	pgGFTDef->ItemProps[i]->PickList->AddObject(getDirectionAsString(dirSubscriberAutoLength), (TObject*)dirSubscriberAutoLength);
    // changeable properties : Length
	i = pgGFTDef->Strings->Add(AnsiString::Format("%s=", ARRAYOFCONST((PROP_LENGTH))));
    pgGFTDef->ItemProps[i]->EditStyle = esPickList;
    pgGFTDef->ItemProps[i]->ReadOnly = false;
	for (j = 1 ; j < 9 ; j++) {
		pgGFTDef->ItemProps[i]->PickList->AddObject(IntToStr(j), (TObject*)j);
    }
	// read only properties : ID
	i = pgGFTDef->Strings->Add(AnsiString::Format("%s=", ARRAYOFCONST((PROP_ID))));
	pgGFTDef->ItemProps[i]->EditStyle = esSimple;
	pgGFTDef->ItemProps[i]->ReadOnly = true;
	// read only properties : Protected ID
	i = pgGFTDef->Strings->Add(AnsiString::Format("%s=", ARRAYOFCONST((PROP_PID))));
    pgGFTDef->ItemProps[i]->EditStyle = esSimple;
	pgGFTDef->ItemProps[i]->ReadOnly = true;
    
    // remove lock
	m_fAutomatic = False;
}

__fastcall TFrameDlg::~TFrameDlg()
{
	// TODO memCheck
	//delete m_pSelectedFrames;
}


void TFrameDlg::FillListView()
{
	int i;
	TListItem* CurrentItem;

	lvGFT->Items->Clear();
	for (i = 0; i < m_pGFT->getCount(); i++) {
		CurrentItem = lvGFT->Items->Add();
		CurrentItem->Caption = m_pGFT->get(i)->Id;
		CurrentItem->SubItems->Add(m_pGFT->get(i)->ProtectedId);
		CurrentItem->SubItems->Add(getDirectionAsString(m_pGFT->get(i)->Direction));
		CurrentItem->SubItems->Add(m_pGFT->get(i)->LengthAsString);
		CurrentItem->SubItems->Add(getChecksumAsString(m_pGFT->get(i)->ChecksumType));
	}
}

void TFrameDlg::UpdatePropertyGrid()
{
	CFrameDefinition* frame;
	TLINDirection direction;
	TLINChecksumType checksumType;
	bool bSameDirection, bSameChecksumType, bSameLength;
	int len, i;

	if (m_pSelectedFrames->Count == 0)
	{
		// reset "property grid" values
		pgGFTDef->Values[PROP_CHECKSUMTYPE] = "";
		pgGFTDef->Values[PROP_DIRECTION] = "";
		pgGFTDef->Values[PROP_LENGTH] = "";
		pgGFTDef->Values[PROP_ID] = "";
		pgGFTDef->Values[PROP_PID] = "";
	}
	else
	{
		if (m_pSelectedFrames->Count == 1)
		{
			// fill "property grid" with selected frame  values
			frame = (CFrameDefinition*) m_pSelectedFrames->Items[0];
			pgGFTDef->Values[PROP_CHECKSUMTYPE] = getChecksumAsString(frame->ChecksumType);
			pgGFTDef->Values[PROP_DIRECTION] = getDirectionAsString(frame->Direction);
			pgGFTDef->Values[PROP_LENGTH] = frame->LengthAsString;
			pgGFTDef->Values[PROP_ID] = frame->Id;
			pgGFTDef->Values[PROP_PID] = frame->ProtectedId;
		}
		else
		{
            // get the first selected frame
			frame = (CFrameDefinition*) m_pSelectedFrames->Items[0];
            // get its properties
			checksumType = frame->ChecksumType;
			direction = frame->Direction;
			len = frame->Length;
            // We have to loop through all selected frames to 
            // search for identical properties in the selected frames.
            // If values for a property are different a blank value is displayed
			bSameDirection = true;
			bSameChecksumType = true;
			bSameLength = true;
			for (i = 0 ; i < m_pSelectedFrames->Count ; i++)
			{
				frame = (CFrameDefinition*)m_pSelectedFrames->Items[i];
				if (checksumType != frame->ChecksumType)
					bSameChecksumType = false;
				if (direction != frame->Direction)
					bSameDirection = false;
				if (len != frame->Length)
					bSameLength = false;
			}
			frame = (CFrameDefinition*)m_pSelectedFrames->Items[0];
			// if all the frames have the same CST, display it otherwise reset to the original blank value
			if (bSameChecksumType)
				pgGFTDef->Values[PROP_CHECKSUMTYPE] = getChecksumAsString(frame->ChecksumType);
			else
				pgGFTDef->Values[PROP_CHECKSUMTYPE] = "";
			// if all the frames have the same direction, display it otherwise reset to the original blank value
			if (bSameDirection)
				pgGFTDef->Values[PROP_DIRECTION] = getDirectionAsString(frame->Direction);
			else
				pgGFTDef->Values[PROP_DIRECTION] = "";
			// if all the frames have the same length, display it otherwise reset to the original blank value
			if (bSameLength)
				pgGFTDef->Values[PROP_LENGTH] = frame->LengthAsString;
			else
				pgGFTDef->Values[PROP_LENGTH] = "";
			// those properties are always different, reset to the original blank value
			pgGFTDef->Values[PROP_ID] = "";
			pgGFTDef->Values[PROP_PID] = "";
		}
	}
}

AnsiString TFrameDlg::getDirectionAsString(TLINDirection direction)
{
	AnsiString result;
    switch (direction)
    {
        case dirDisabled:
			result = "Disabled";
			break;
        case dirPublisher:
			result = "Publisher";
			break;
        case dirSubscriber:
			result = "Subscriber";
			break;
        case dirSubscriberAutoLength:
			result = "Subscriber Automatic Length";
			break;
		default:
			result = "";
			break;
    }
    return result;
}

AnsiString TFrameDlg::getChecksumAsString(TLINChecksumType checksumType)
{
	AnsiString result;
    switch (checksumType)
    {
        case cstAuto:
			result = "Auto";
			break;
        case cstClassic:
			result = "Classic";
			break;
        case cstEnhanced:
			result = "Enhanced";
			break;
        case cstCustom:
			result = "Custom";
			break;
		default:
			result = "";
			break;
    }
    return result;
}


//---------------------------------------------------------------------------
void __fastcall TFrameDlg::lvGFTKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	// select all items
	if (Key == 'A' && Shift.Contains(ssCtrl)) {
		((TListView*)Sender)->SelectAll();
		Key = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFrameDlg::lvGFTSelectItem(TObject *Sender, TListItem *Item,
      bool Selected)
{
	int nIdx;
	TListItem* curItem;
	TItemStates selected = TItemStates() << isSelected;


	if (!m_fAutomatic)
	{
		m_fAutomatic = true;
		// Clear the selected frames list
		m_pSelectedFrames->Clear();
		// Get the count of selected elements
		// in the ListView 'lvGFT'.
		// Each element is assigned with an
		// element in the CGlobalFrameTable.
		curItem = lvGFT->Selected;
		if (curItem != NULL)
		{
			// add all items to the internal item selection list
			nIdx = lvGFT->Items->IndexOf(curItem);
			m_pSelectedFrames->Add(m_pGFT->get(nIdx));
			curItem = lvGFT->GetNextItem(curItem, sdAll, selected);
			// if there are more than one
			while (curItem != NULL)
			{
				nIdx = lvGFT->Items->IndexOf(curItem);
				m_pSelectedFrames->Add(m_pGFT->get(nIdx));
				curItem = lvGFT->GetNextItem(curItem, sdAll, selected);
			}
		}
		// Update the PropertyGrid control with the newly selected frames
		UpdatePropertyGrid();
		m_fAutomatic = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFrameDlg::pgGFTDefStringsChange(TObject *Sender)
{
	AnsiString value;
	TLINDirection direction;
	TLINChecksumType checksumType;
	int length, i;
	CFrameDefinition* frame;
	TListItem* CurrentItem;
	bool bSetLen, bSetCST, bSetDir;

	// avoid internal calls
	if (!m_fAutomatic)
	{
        m_fAutomatic = true;
		// if no frame is selected, only update propertyGrid to display original/blank values and return
		if (m_pSelectedFrames->Count == 0)
		{
			// update and return
			UpdatePropertyGrid();
			// remove lock
			m_fAutomatic = false;
			return;
		}

        // since the event do not state which row was changed
        // the following booleans define wether the values are valid or not
        // (needed for multiple row selection)
		bSetDir = False;
		bSetCST = False;
		bSetLen = False;
		checksumType = cstAuto;
		direction = dirSubscriberAutoLength;
		length = 0;
                       
        // Checksum property changed
		value = pgGFTDef->Values[PROP_CHECKSUMTYPE];
		if (value != "")
		{
			bSetCST = true;
			if (value == getChecksumAsString(cstCustom))
				checksumType = cstCustom;
			else if (value == getChecksumAsString(cstClassic))
				checksumType = cstClassic;
			else if (value == getChecksumAsString(cstEnhanced))
				checksumType = cstEnhanced;
			else if (value == getChecksumAsString(cstAuto))
				checksumType = cstAuto;
		}
            
        // Direction property changed
        value = pgGFTDef->Values[PROP_DIRECTION];  
		if (value != "")
		{
			bSetDir = true;
			if (value == getDirectionAsString(dirDisabled))
				direction = dirDisabled;
			else if (value == getDirectionAsString(dirPublisher))
				direction = dirPublisher;
			else if (value == getDirectionAsString(dirSubscriber))
				direction = dirSubscriber;
			else if (value == getDirectionAsString(dirSubscriberAutoLength))
				direction = dirSubscriberAutoLength;
		}
            
		// Length property changed
		value = pgGFTDef->Values[PROP_LENGTH];
		if (value != "")
		{
			if ( TryStrToInt(value, length) )
				bSetLen = true;
		}

		for (i = 0 ; i < m_pSelectedFrames->Count ; i++)
		{
			//
			frame = (CFrameDefinition*) m_pSelectedFrames->Items[i];
			// Update CST : avoid unnecessary updates
			if ((bSetCST == True) && (frame->ChecksumType != checksumType))
			{
				// call for propertyChanged to update dialog controls
				frame->setChecksumType(checksumType, true);
				// if changes were not validated, rollback/update display
				if (frame->ChecksumType != checksumType)
					UpdatePropertyGrid();
			}
            // Update Direction : avoid unnecessary updates
			if ((bSetDir == true) && (frame->Direction != direction))
			{
            	// call for propertyChanged to update dialog controls
				frame->setDirection(direction, true);
				// if changes were not validated, rollback/update display
				if (frame->Direction != direction)
					UpdatePropertyGrid();
			}
			// Update length : avoid unnecessary updates
			if ((bSetLen == true) && (frame->Length != length))
			{
				// call for propertyChanged to update dialog controls
				frame->setLength(length, true);
				// if changes were not validated, rollback/update display
				if (frame->Length != length)
					UpdatePropertyGrid();
			}

			// update the frame in the list view
			lvGFT->Items->BeginUpdate();
			CurrentItem = lvGFT->Items->Item[frame->getIdAsInt()];
			CurrentItem->SubItems->Strings[0] = frame->ProtectedId;
			CurrentItem->SubItems->Strings[1] = getDirectionAsString(frame->Direction);
			CurrentItem->SubItems->Strings[2] = frame->LengthAsString;
			CurrentItem->SubItems->Strings[3] = getChecksumAsString(frame->ChecksumType);
			lvGFT->Items->EndUpdate();
		}
		// remove lock
		m_fAutomatic = false;
	}
}
//---------------------------------------------------------------------------

