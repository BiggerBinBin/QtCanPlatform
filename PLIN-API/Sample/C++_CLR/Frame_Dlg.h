#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include ".\GlobalFrameTableHandler.h"


namespace PLINAPIExample {
/// <summary>
/// This class handles a dialog that displays and manages the LIN Global Frame Table
///
/// WARNING: If you change the name of this class, you will need to change the
///          'Resource File Name' property for the managed resource compiler tool
///          associated with all .resx files this class depends on.  Otherwise,
///          the designers will not be able to interact properly with localized
///          resources associated with this form.
/// </summary>
public ref class Frame_Dlg : public System::Windows::Forms::Form
{
#pragma region Members
	private:
		/// <summary>
		/// Global Frame Table object that holds all frame definition.
		/// </summary>
		CGlobalFrameTable^ m_pGFT;
		/// <summary>
		/// Boolean to register the current edit mode of different controls.
		/// To protect the control's event for recursive calling.
		/// </summary>
		Boolean m_fAutomatic;

#pragma endregion

	public:
		Frame_Dlg(CGlobalFrameTable^ pGFT) : m_pGFT(pGFT)
		{
			InitializeComponent();
			// Initialize the list view component with items from the global frame table object
			FillListViews();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Frame_Dlg()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::PropertyGrid^  pgGFTDef;
	private: System::Windows::Forms::ColumnHeader^  ColGFTChecksum;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ListView^  lvGFT;
	private: System::Windows::Forms::ColumnHeader^  ColGFTID;
	private: System::Windows::Forms::ColumnHeader^  ColGFTPID;
	private: System::Windows::Forms::ColumnHeader^  ColGFTDirect;
	private: System::Windows::Forms::ColumnHeader^  ColGFTLen;
	private: System::Windows::Forms::Label^  label2;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::ListViewItem^  listViewItem1 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(5) {L"0x01", 
				L"2", L"SubscriberAutoLength", L"8", L"Enhanced"}, -1));
			System::Windows::Forms::ListViewItem^  listViewItem2 = (gcnew System::Windows::Forms::ListViewItem(L""));
			System::Windows::Forms::ListViewItem^  listViewItem3 = (gcnew System::Windows::Forms::ListViewItem(L""));
			this->pgGFTDef = (gcnew System::Windows::Forms::PropertyGrid());
			this->ColGFTChecksum = (gcnew System::Windows::Forms::ColumnHeader());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->lvGFT = (gcnew System::Windows::Forms::ListView());
			this->ColGFTID = (gcnew System::Windows::Forms::ColumnHeader());
			this->ColGFTPID = (gcnew System::Windows::Forms::ColumnHeader());
			this->ColGFTDirect = (gcnew System::Windows::Forms::ColumnHeader());
			this->ColGFTLen = (gcnew System::Windows::Forms::ColumnHeader());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// pgGFTDef
			// 
			this->pgGFTDef->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->pgGFTDef->Location = System::Drawing::Point(354, 27);
			this->pgGFTDef->Name = L"pgGFTDef";
			this->pgGFTDef->Size = System::Drawing::Size(256, 219);
			this->pgGFTDef->TabIndex = 7;
			this->pgGFTDef->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &Frame_Dlg::pgGFTDef_PropertyValueChanged);
			// 
			// ColGFTChecksum
			// 
			this->ColGFTChecksum->Text = L"CST";
			this->ColGFTChecksum->Width = 61;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 11);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(102, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Global Frame Table:";
			// 
			// lvGFT
			// 
			this->lvGFT->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->lvGFT->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(5) {this->ColGFTID, this->ColGFTPID, 
				this->ColGFTDirect, this->ColGFTLen, this->ColGFTChecksum});
			this->lvGFT->FullRowSelect = true;
			this->lvGFT->HideSelection = false;
			this->lvGFT->Items->AddRange(gcnew cli::array< System::Windows::Forms::ListViewItem^  >(3) {listViewItem1, listViewItem2, 
				listViewItem3});
			this->lvGFT->Location = System::Drawing::Point(12, 27);
			this->lvGFT->Name = L"lvGFT";
			this->lvGFT->ShowItemToolTips = true;
			this->lvGFT->Size = System::Drawing::Size(338, 219);
			this->lvGFT->TabIndex = 5;
			this->lvGFT->UseCompatibleStateImageBehavior = false;
			this->lvGFT->View = System::Windows::Forms::View::Details;
			this->lvGFT->SelectedIndexChanged += gcnew System::EventHandler(this, &Frame_Dlg::lvGFT_SelectedIndexChanged);
			this->lvGFT->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Frame_Dlg::lvGFT_KeyDown);
			// 
			// ColGFTID
			// 
			this->ColGFTID->Text = L"ID";
			this->ColGFTID->Width = 41;
			// 
			// ColGFTPID
			// 
			this->ColGFTPID->Text = L"PID";
			this->ColGFTPID->Width = 38;
			// 
			// ColGFTDirect
			// 
			this->ColGFTDirect->Text = L"Direction";
			this->ColGFTDirect->Width = 124;
			// 
			// ColGFTLen
			// 
			this->ColGFTLen->Text = L"Length";
			this->ColGFTLen->Width = 50;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(356, 11);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Properties:";
			// 
			// Frame_Dlg
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(622, 256);
			this->Controls->Add(this->pgGFTDef);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->lvGFT);
			this->Controls->Add(this->label2);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(638, 295);
			this->Name = L"Frame_Dlg";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Global Frame Table";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

#pragma region helper methods
	private:
		/// <summary>
		/// Returns the Frame Direction as a human readable string
		/// </summary>
		/// <param name="direction">the direction object</param>
		/// <returns>The direction as a string</returns>
		String^ getDirectionAsString(Peak::Lin::TLINDirection direction);
		/// <summary>
		/// Returns the Frame checksum type as a human readable string
		/// </summary>
		/// <param name="checksumType">the checksumType object</param>
		/// <returns>The checksumType as a string</returns>
		String^ getChecksumAsString(Peak::Lin::TLINChecksumType checksumType);
#pragma endregion

#pragma region ListView event-handlers
	private:
		/// <summary>
		/// Occurs when a key is pressed while the ListView is active.
		/// Implements "select all items" feature.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains no event data.</param>		
		System::Void lvGFT_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);

		/// <summary>
		/// Occurs when the SelectedIndices collection changes. The selection
		/// will be assigned to the Property-Control 'pgGFTDef'.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains no event data.</param>
		System::Void lvGFT_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);

		/// <summary>
		/// Fills the Listview 'Global Frame Table' with the data from CGloabalFrameTable.
		/// </summary>
		System::Void FillListViews();

#pragma endregion 

#pragma region PropertyGrid "pgGFTDef"
	private:
		/// <summary>
		/// Occurs when a property value of at least one CFrameDefinition object
		/// was changed by the Property-Control.
		/// </summary>
		/// <param name="s">The source of the event.</param>
		/// <param name="e">A PropertyValueChangedEventArgs that contains the event data.</param>
		System::Void pgGFTDef_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e);

#pragma endregion

	};
}
