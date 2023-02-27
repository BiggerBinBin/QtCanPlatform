//---------------------------------------------------------------------------

#ifndef UnitFrameDlgH
#define UnitFrameDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
    
#include "CGlobalFrameTable.h"
//---------------------------------------------------------------------------
class TFrameDlg : public TForm
{
__published:	// Composants gérés par l'EDI
	TLabel *Label1;
	TLabel *Label2;
	TListView *lvGFT;
	TValueListEditor *pgGFTDef;
	void __fastcall lvGFTKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall lvGFTSelectItem(TObject *Sender, TListItem *Item,
          bool Selected);
	void __fastcall pgGFTDefStringsChange(TObject *Sender);
private:	// Déclarations utilisateur
    // Global Frame Table object that holds all frame definition.
    CGlobalFrameTable * m_pGFT;
    /// Boolean to register the current edit mode of different controls.
    /// To protect the control's event for recursive calling.
    bool m_fAutomatic;
	// Stores the selected frames in the Frame List Control
	TList * m_pSelectedFrames;

	// Fills the Listview 'Global Frame Table' with the data from CGloabalFrameTable.
	void FillListView();   
	// Updates the property grid with the selected frames from m_pSelectedFrames
	void UpdatePropertyGrid();
	// Returns the Frame Direction as a human readable string
	AnsiString getDirectionAsString(TLINDirection direction);
	// Returns the Frame checksum type as a human readable string
	AnsiString getChecksumAsString(TLINChecksumType checksumType);
	
public:		// Déclarations utilisateur
	__fastcall TFrameDlg(CGlobalFrameTable* pGFT, TComponent* Owner);
	__fastcall ~TFrameDlg();
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameDlg *FrameDlg;
//---------------------------------------------------------------------------
#endif
