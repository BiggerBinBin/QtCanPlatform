#include "stdafx.h"

#include "GlobalFrameTableHandler.h"

namespace PLINAPIExample 
{

	void CFrameDefinition::Length::set(int value)
	{
		if (m_nLength != value)
		{
			bool fAllowed = true;
			((CGlobalFrameTable^ )m_pParent)->CallPropertyChangeHandler(this, EProperty::Length, value, fAllowed);
			if (fAllowed == true)
				m_nLength = value;
		}
	}

	void CFrameDefinition::ChecksumType::set(Peak::Lin::TLINChecksumType value)
	{
		if (m_nChecksumType != value)
		{
			bool fAllowed = true;
			((CGlobalFrameTable^ )m_pParent)->CallPropertyChangeHandler(this, EProperty::ChecksumType, (int)value, fAllowed);
			if (fAllowed == true)
				m_nChecksumType = value;
		}
	}

	void CFrameDefinition::Direction::set(Peak::Lin::TLINDirection value)
	{
		if (m_bDirection != value)
		{
			bool fAllowed = true;
			((CGlobalFrameTable^ )m_pParent)->CallPropertyChangeHandler(this, EProperty::Direction, (int)value, fAllowed);
			if (fAllowed == true)
				m_bDirection = value;
		}
	}

	CGlobalFrameTable::CGlobalFrameTable(ISynchronizeInvoke^ AOwner)
	{
		// Initialize the inherited attributes from the
		// base class CDispatch.
		m_pOwner = AOwner;
		m_fSynchronized = (m_pOwner != nullptr);
		// Create a list to handle the CFrameDefinition objects.
		m_lFrameDefinitions = gcnew List<CFrameDefinition^>(64);
		// Create all CFrameDefinition objects and set
		// them values to default. The length values is
		// set to LIN 1.2.
		// Default values:
		// - Direction = SubscriberAutoLength
		// - ChecksumType = Auto
		// - Length = see inside the loop.
		for (int i = 0; i < 64; i++)
		{
			// From Frame-ID 0 to 31 set the length 2.
			if ((i >= 0x00) && (i <= 0x1F))
				AddFrameDefinition(i, 2, Peak::Lin::TLINChecksumType::cstAuto, Peak::Lin::TLINDirection::dirSubscriberAutoLength);
			// From Frame-ID 32 to 47 set the length 4.
			else if ((i >= 0x20) && (i <= 0x2F))
				AddFrameDefinition(i, 4, Peak::Lin::TLINChecksumType::cstAuto, Peak::Lin::TLINDirection::dirSubscriberAutoLength);
			// From Frame-ID 48 to 63 set the length 8.
			if ((i >= 0x30) && (i <= 0x3F))
				AddFrameDefinition(i, 8, Peak::Lin::TLINChecksumType::cstAuto, Peak::Lin::TLINDirection::dirSubscriberAutoLength);
		}
	}

	int CGlobalFrameTable::AddFrameDefinition(CFrameDefinition^ AFrameDefinition)
	{
		// Check the parameter at first.
		if (AFrameDefinition == nullptr)
			// Delivered object invalid. Do not add it.
			return -1;
		else
			// Add the delivered object (CFrameDefinition) to the list.
			m_lFrameDefinitions->Add(AFrameDefinition);
		// Return the position of the new added object.
		// It should be added at the end so the position
		// must be the last entry in the list.
		return m_lFrameDefinitions->Count - 1;
	}

	int CGlobalFrameTable::AddFrameDefinition(int AId, int ALength, Peak::Lin::TLINChecksumType AChecksumType, Peak::Lin::TLINDirection ADirection)
	{
		// Check the Frame-ID for adding.
		// Only ID's from 0 to 63 are allowed to add.
		if ((AId < 0) || (AId > 63))
			// ID is invalid. Do not add it.
			return -1;
		else
		{
			// The delivered Frame-ID is valid.
			CFrameDefinition^ lFD;
			Byte lbID;

			// Create a Frame Definition object.
			// and assigned the delivered values to it.
			lFD = gcnew CFrameDefinition();
			lFD->m_pParent = this;
			lFD->m_nID = AId;
			lFD->m_nLength = ALength;
			lFD->m_nChecksumType = AChecksumType;
			lFD->m_bDirection = ADirection;
			// Calculate the Protected-ID with
			// the delivered Frame-ID.
			lbID = Convert::ToByte(AId);
			Peak::Lin::PLinApi::GetPID(lbID);
			// Assign the calculated Protected-ID.
			lFD->m_nProtectedID = lbID;
			// Add the created object to the list.
			m_lFrameDefinitions->Add(lFD);
		}
		// Return the position of the new added object.
		// It should be added at the end so the position
		// must be the last entry in the list.
		return m_lFrameDefinitions->Count - 1;
	}

	void CGlobalFrameTable::CallPropertyChangeHandler(Object^ sender, EProperty AType, int AValue, bool &AAllowed)
	{
		// Check Event Handler at first.
		if (OnPropertyChange != nullptr)
		{
			// The Event Handler is assigned.
			// Create the EventArg object.
			PropertyChangeEventArg^ pArg = gcnew PropertyChangeEventArg();
			// Assign the parameter values to
			// EventArg object.
			pArg->Type = AType;
			pArg->Value = AValue;
			pArg->Allowed = AAllowed;
			// Call the event.
			DispatchEvent(OnPropertyChange, gcnew array<Object^>{sender, pArg});
			// Check if the reference parameter "Allowed"
			// was set from the process the validate
			// the event.
			AAllowed = pArg->Allowed;
		}
	}
}