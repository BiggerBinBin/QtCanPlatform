#include "StdAfx.h"
#include "Frame_Dlg.h"

#define SLinCSTAuto				"Auto"
#define SLinCSTClassic			"Classic"
#define SLinCSTCustom			"Custom"
#define SLinCSTEnhanced			"Enhanced"
#define SLinDirectionAuto		"Subscriber Automatic Length"
#define SLinDirectionDisabled	"Disabled"
#define SLinDirectionPublisher	"Publisher"
#define SLinDirectionSubscriber	"Subscriber"

namespace PLINAPIExample
{
	/////////////////////////////////
	//	helper methods
	String^ Frame_Dlg::getDirectionAsString(Peak::Lin::TLINDirection direction)
	{
		switch (direction)
		{
		case Peak::Lin::TLINDirection::dirDisabled:
			return SLinDirectionDisabled;
		case Peak::Lin::TLINDirection::dirPublisher:
			return SLinDirectionPublisher;
		case Peak::Lin::TLINDirection::dirSubscriber:
			return SLinDirectionSubscriber;
		case Peak::Lin::TLINDirection::dirSubscriberAutoLength:
			return SLinDirectionAuto;
		}
		
		return "";
	}
	
	String^ Frame_Dlg::getChecksumAsString(Peak::Lin::TLINChecksumType checksumType)
	{
		switch (checksumType)
		{
		case Peak::Lin::TLINChecksumType::cstAuto:
			return SLinCSTAuto;
		case Peak::Lin::TLINChecksumType::cstClassic:
			return SLinCSTClassic;
		case Peak::Lin::TLINChecksumType::cstEnhanced:
			return SLinCSTEnhanced;
		case Peak::Lin::TLINChecksumType::cstCustom:
			return SLinCSTCustom;
		}
		return "";
	}
	
	/////////////////////////////////
	//	ListView event-handlers
	System::Void Frame_Dlg::lvGFT_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
	{
		// Adds "select all items" (Ctrl-A) feature
		if (e->KeyCode == Keys::A && e->Control)
		{
			// Lock the ListView.
			lvGFT->BeginUpdate();
			// prevent multiple calls of selectionChange
			m_fAutomatic = true;
			for each(ListViewItem^ item in lvGFT->Items)
			{
				item->Selected = true;
			}
			m_fAutomatic = false;
			// Unlock the ListView
			lvGFT->EndUpdate();
			// Call selection change
			lvGFT_SelectedIndexChanged(this, gcnew EventArgs());
		}
	}
	
	System::Void Frame_Dlg::lvGFT_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		int nCnt, nIdx, i;

		if (!m_fAutomatic)
		{
			m_fAutomatic = true;
			// Get the count of selected elements
			// in the ListView 'lvGFT'.
			// Each element is assigned with an
			// element in the CGlobalFrameTable.
			nCnt = lvGFT->SelectedItems->Count;
			if (nCnt == 1)
			{
				// If only one element is selected
				// it can be assigned directly to the
				// Property-Control.
				nIdx = lvGFT->SelectedItems[0]->Index;
				// Assign the CFrameDefinition element
				// from the CGlobalFrameTable.
				pgGFTDef->SelectedObject = m_pGFT[nIdx];
			}
			else if (nCnt > 1)
			{
				// If more than one element are selected
				// in the ListView means that the user
				// want to change at least one property of
				// all elements to a new value with
				// the Property-Control.
				// So get first the selected elements and the
				// assigned objects from the CGlobalFrameTable
				// into a local Array of elements.
				array<CFrameDefinition^>^ lDefFrames = gcnew array<CFrameDefinition^>(nCnt);
				for (i = 0; i < nCnt; i++)
				{
					nIdx = lvGFT->SelectedItems[i]->Index;
					lDefFrames[i] = m_pGFT[nIdx];
				}
				// Assign the local array of elements
				// from the CGlobalFrameTable.
				pgGFTDef->SelectedObjects = lDefFrames;
			}
			m_fAutomatic = false;
		}
	}
	
	System::Void Frame_Dlg::FillListViews()
	{
		ListViewItem^ lvi;
		// Lock the ListView.
		lvGFT->BeginUpdate();
		// Clear the ListView that
		// will show the Frame Definition of the
		// Global Frame Table.
		lvGFT->Items->Clear();
		// Add every object, Frame Definition, from the
		// Global Frame Table into the ListView.
		for (int i = 0; i < m_pGFT->Count; i++)
		{
			lvi = lvGFT->Items->Add(Convert::ToString(m_pGFT[i]->ID));
			lvi->SubItems->Add(Convert::ToString(m_pGFT[i]->ProtectedID));
			lvi->SubItems->Add(getDirectionAsString(m_pGFT[i]->Direction));
			lvi->SubItems->Add(Convert::ToString(m_pGFT[i]->Length));
			lvi->SubItems->Add(getChecksumAsString(m_pGFT[i]->ChecksumType));
		}
		// Unlock and update the ListView now.
		lvGFT->EndUpdate();
	}
	
	/////////////////////////////////
	//	PropertyGrid "pgGFTDef"
	System::Void Frame_Dlg::pgGFTDef_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e)
	{
		int i, nCnt, nIdx;
		CFrameDefinition^ lFD;
		ListViewItem^ lvi;

		lFD = nullptr;
		// Lock the ListView for update elements.
		lvGFT->BeginUpdate();
		// Get the selected count of CFrameDefinition.
		nCnt = pgGFTDef->SelectedObjects->GetLength(0);
		// For each selected CFrameDefinition in
		// the ListView, update their property values.
		for (i = 0; i < nCnt; i++)
		{
			// Get the selected CFrameDefinition X from the
			// Property-Control that has changed the
			// property values.
			lFD = (CFrameDefinition^)pgGFTDef->SelectedObjects[i];
			if (lFD != nullptr)
			{
				// Get the position in the ListView of
				// the CFrameDefinition to refresh the
				// property value.
				nIdx = m_pGFT->IndexOf(lFD);
				if ((nIdx >= 0) && (nIdx < lvGFT->Items->Count))
				{
					// Get the item that is assign with the
					// CFrameDefinition from ListView 'lvGFT'
					// and refesh it.
					lvi = lvGFT->Items[nIdx];
					lvi->SubItems[1]->Text = Convert::ToString(lFD->ProtectedID);
					lvi->SubItems[2]->Text = getDirectionAsString(lFD->Direction);
					lvi->SubItems[3]->Text = Convert::ToString(lFD->Length);
					lvi->SubItems[4]->Text = getChecksumAsString(lFD->ChecksumType);
				}

			}
		}
		// Free the lock of ListView and
		// update all elements now.
		lvGFT->EndUpdate();
	}
}