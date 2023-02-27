#pragma once

#include ".\PLinApiCLR.h"
#include ".\GlobalFrameTableHandler.h"
#include ".\ComboBoxItem.h"
#include ".\MessageStatus.h"
#include ".\Frame_Dlg.h"


namespace PLINAPIExample {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

#define SAppCaption				"PLIN-API Example"
#define SAppCaptionError		"PLIN-API Example - Error"
#define SHardwareNothingFound	"<No hardware found>"
#define SHardwareTypeLIN		"PCAN-USB Pro"
#define SHardwareTypeUnkown		"Unknown"
#define SLINStatusActive		"Bus: Active"
#define SLINStatusBaudDetect	"Hardware: Baudrate Detection"
#define SLINStatusNotInit		"Hardware: Not Initialized"
#define SLINStatusShortGround	"Bus-Line: Shorted Ground"
#define SLINStatusSleep			"Bus: Sleep"
#define SPLinClientName			"PLIN_EXAMPLE"
#define SWriteAsMaster			"Transmit"
#define SWriteAsSlave			"Publish"


	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
#pragma region Attributes
	private:
#pragma region LIN
		/// <summary>
		/// Client handle
		/// </summary>
		HLINCLIENT m_hClient;
		/// <summary>
		/// Hardware handle
		/// </summary>
		HLINHW m_hHw;
		/// <summary>
		/// LIN Hardware Modus (Master/Slave)
		/// </summary>
		Peak::Lin::TLINHardwareMode m_HwMode;
		/// <summary>
		/// Client filter mask
		/// </summary>
		UInt64 m_lMask;
		/// <summary>
		/// Baudrate Index of Hardware
		/// </summary>
		unsigned short m_wBaudrate;
		/// <summary>
		/// Last LIN error
		/// </summary>
		Peak::Lin::TLINError m_LastLINErr;
		/// <summary>
		/// Constant value that indicate the mask of the client filter (64bit)
		/// </summary>
		static const UInt64 FRAME_FILTER_MASK = 0xFFFFFFFFFFFFFFFF;

#pragma endregion

		/// <summary>
		/// Global Frame Table object to hold all frames definitions.
		/// </summary>
		CGlobalFrameTable^ m_pGFT;
		/// <summary>
		/// Stores the status of received messages for its display
		/// </summary>
		System::Collections::ArrayList^ m_LastMsgsList;

#pragma endregion

	public:
		Form1(void)
		{
			InitializeComponent();

			// Create the Global Frame Table and add property change handler on its items
			m_pGFT = gcnew CGlobalFrameTable(this);
			m_pGFT->OnPropertyChange += gcnew PropertyChangeEventHandler(this, &Form1::PropertyChange);
			// Populates FrameID combobox with global frame IDs
			UpdateFrameIds();
			// Populates Direction combobox
			cbbDirection->Items->Clear();
			cbbDirection->Items->Add(gcnew ComboBoxItem(SLinDirectionDisabled, Peak::Lin::TLINDirection::dirDisabled));
			cbbDirection->Items->Add(gcnew ComboBoxItem(SLinDirectionPublisher, Peak::Lin::TLINDirection::dirPublisher));
			cbbDirection->Items->Add(gcnew ComboBoxItem(SLinDirectionSubscriber, Peak::Lin::TLINDirection::dirSubscriber));
			cbbDirection->Items->Add(gcnew ComboBoxItem(SLinDirectionAuto, Peak::Lin::TLINDirection::dirSubscriberAutoLength));
			// Populates ChecksumType combobox
			cbbCST->Items->Clear(); 
			cbbCST->Items->Add(gcnew ComboBoxItem(SLinCSTAuto, Peak::Lin::TLINChecksumType::cstAuto));
			cbbCST->Items->Add(gcnew ComboBoxItem(SLinCSTClassic, Peak::Lin::TLINChecksumType::cstClassic));
			cbbCST->Items->Add(gcnew ComboBoxItem(SLinCSTEnhanced, Peak::Lin::TLINChecksumType::cstEnhanced));
			cbbCST->Items->Add(gcnew ComboBoxItem(SLinCSTCustom, Peak::Lin::TLINChecksumType::cstCustom));

			// Initialize the LIN attributes
			m_hClient = 0;
			m_hHw = 0;
			m_lMask = FRAME_FILTER_MASK;
			m_HwMode = Peak::Lin::TLINHardwareMode::modSlave;
			m_wBaudrate = 19200;

			// Creates the list for received messages
			m_LastMsgsList = gcnew System::Collections::ArrayList();

			// Register this application with LIN as client.
			m_LastLINErr = Peak::Lin::PLinApi::RegisterClient(SPLinClientName, Handle, m_hClient);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Timer^  tmrRead;
	private: System::Windows::Forms::Timer^  tmrDisplay;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ComboBox^  cbbID;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::ComboBox^  cbbCST;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::ComboBox^  cbbDirection;
	private: System::Windows::Forms::NumericUpDown^  nudLength;
	private: System::Windows::Forms::TextBox^  txtData7;
	private: System::Windows::Forms::TextBox^  txtData6;
	private: System::Windows::Forms::TextBox^  txtData5;
	private: System::Windows::Forms::TextBox^  txtData4;
	private: System::Windows::Forms::TextBox^  txtData3;
	private: System::Windows::Forms::TextBox^  txtData2;
	private: System::Windows::Forms::TextBox^  txtData1;
	private: System::Windows::Forms::TextBox^  txtData0;
	private: System::Windows::Forms::TextBox^  txtID;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Button^  btnWrite;
	private: System::Windows::Forms::Button^  btnMsgClear;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Button^  btnIdent;
	private: System::Windows::Forms::Button^  btnHwRefresh;
	private: System::Windows::Forms::ComboBox^  cbbChannel;
	private: System::Windows::Forms::ComboBox^  cbbHwMode;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::ComboBox^  cbbBaudrates;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btnInit;
	private: System::Windows::Forms::Button^  btnRelease;
	private: System::Windows::Forms::ColumnHeader^  clhRcvTime;
	private: System::Windows::Forms::ColumnHeader^  clhCRC;
	private: System::Windows::Forms::ColumnHeader^  clhDirection;
	private: System::Windows::Forms::ColumnHeader^  clhCount;
	private: System::Windows::Forms::ColumnHeader^  clhErrors;
	private: System::Windows::Forms::RadioButton^  rdbTimer;
	private: System::Windows::Forms::ColumnHeader^  clhCST;
	private: System::Windows::Forms::Button^  btnRead;
	private: System::Windows::Forms::ColumnHeader^  clhData;
	private: System::Windows::Forms::ColumnHeader^  clhLength;
	private: System::Windows::Forms::ColumnHeader^  clhID;
	private: System::Windows::Forms::Button^  btnConfigure;
	private: System::Windows::Forms::RadioButton^  rdbFilterOpen;
	private: System::Windows::Forms::RadioButton^  rdbFilterCustom;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::RadioButton^  rdbFilterClose;
	private: System::Windows::Forms::Button^  btnFilterQuery;
	private: System::Windows::Forms::NumericUpDown^  nudIdTo;
	private: System::Windows::Forms::NumericUpDown^  nudIdFrom;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Button^  btnFilterApply;
	private: System::Windows::Forms::Button^  btnStatus;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::CheckBox^  chbShowPeriod;
	private: System::Windows::Forms::RadioButton^  rdbManual;
	private: System::Windows::Forms::ListView^  lstMessages;
	private: System::Windows::Forms::Button^  btnReset;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::Button^  btnGetVersions;
	private: System::Windows::Forms::ListBox^  lbxInfo;
	private: System::Windows::Forms::Button^  btnInfoClear;

	private: System::ComponentModel::IContainer^  components;

	private:

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->tmrRead = (gcnew System::Windows::Forms::Timer(this->components));
			this->tmrDisplay = (gcnew System::Windows::Forms::Timer(this->components));
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->cbbID = (gcnew System::Windows::Forms::ComboBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->cbbCST = (gcnew System::Windows::Forms::ComboBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->cbbDirection = (gcnew System::Windows::Forms::ComboBox());
			this->nudLength = (gcnew System::Windows::Forms::NumericUpDown());
			this->txtData7 = (gcnew System::Windows::Forms::TextBox());
			this->txtData6 = (gcnew System::Windows::Forms::TextBox());
			this->txtData5 = (gcnew System::Windows::Forms::TextBox());
			this->txtData4 = (gcnew System::Windows::Forms::TextBox());
			this->txtData3 = (gcnew System::Windows::Forms::TextBox());
			this->txtData2 = (gcnew System::Windows::Forms::TextBox());
			this->txtData1 = (gcnew System::Windows::Forms::TextBox());
			this->txtData0 = (gcnew System::Windows::Forms::TextBox());
			this->txtID = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->btnWrite = (gcnew System::Windows::Forms::Button());
			this->btnMsgClear = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->btnIdent = (gcnew System::Windows::Forms::Button());
			this->btnHwRefresh = (gcnew System::Windows::Forms::Button());
			this->cbbChannel = (gcnew System::Windows::Forms::ComboBox());
			this->cbbHwMode = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->cbbBaudrates = (gcnew System::Windows::Forms::ComboBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btnInit = (gcnew System::Windows::Forms::Button());
			this->btnRelease = (gcnew System::Windows::Forms::Button());
			this->clhRcvTime = (gcnew System::Windows::Forms::ColumnHeader());
			this->clhCRC = (gcnew System::Windows::Forms::ColumnHeader());
			this->clhDirection = (gcnew System::Windows::Forms::ColumnHeader());
			this->clhCount = (gcnew System::Windows::Forms::ColumnHeader());
			this->clhErrors = (gcnew System::Windows::Forms::ColumnHeader());
			this->rdbTimer = (gcnew System::Windows::Forms::RadioButton());
			this->clhCST = (gcnew System::Windows::Forms::ColumnHeader());
			this->btnRead = (gcnew System::Windows::Forms::Button());
			this->clhData = (gcnew System::Windows::Forms::ColumnHeader());
			this->clhLength = (gcnew System::Windows::Forms::ColumnHeader());
			this->clhID = (gcnew System::Windows::Forms::ColumnHeader());
			this->btnConfigure = (gcnew System::Windows::Forms::Button());
			this->rdbFilterOpen = (gcnew System::Windows::Forms::RadioButton());
			this->rdbFilterCustom = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->rdbFilterClose = (gcnew System::Windows::Forms::RadioButton());
			this->btnFilterQuery = (gcnew System::Windows::Forms::Button());
			this->nudIdTo = (gcnew System::Windows::Forms::NumericUpDown());
			this->nudIdFrom = (gcnew System::Windows::Forms::NumericUpDown());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->btnFilterApply = (gcnew System::Windows::Forms::Button());
			this->btnStatus = (gcnew System::Windows::Forms::Button());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->chbShowPeriod = (gcnew System::Windows::Forms::CheckBox());
			this->rdbManual = (gcnew System::Windows::Forms::RadioButton());
			this->lstMessages = (gcnew System::Windows::Forms::ListView());
			this->btnReset = (gcnew System::Windows::Forms::Button());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->btnGetVersions = (gcnew System::Windows::Forms::Button());
			this->lbxInfo = (gcnew System::Windows::Forms::ListBox());
			this->btnInfoClear = (gcnew System::Windows::Forms::Button());
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudLength))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudIdTo))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudIdFrom))->BeginInit();
			this->groupBox5->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->SuspendLayout();
			// 
			// tmrRead
			// 
			this->tmrRead->Interval = 50;
			this->tmrRead->Tick += gcnew System::EventHandler(this, &Form1::tmrRead_Tick);
			// 
			// tmrDisplay
			// 
			this->tmrDisplay->Tick += gcnew System::EventHandler(this, &Form1::tmrDisplay_Tick);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->cbbID);
			this->groupBox2->Controls->Add(this->label11);
			this->groupBox2->Controls->Add(this->label10);
			this->groupBox2->Controls->Add(this->cbbCST);
			this->groupBox2->Controls->Add(this->label9);
			this->groupBox2->Controls->Add(this->cbbDirection);
			this->groupBox2->Controls->Add(this->nudLength);
			this->groupBox2->Controls->Add(this->txtData7);
			this->groupBox2->Controls->Add(this->txtData6);
			this->groupBox2->Controls->Add(this->txtData5);
			this->groupBox2->Controls->Add(this->txtData4);
			this->groupBox2->Controls->Add(this->txtData3);
			this->groupBox2->Controls->Add(this->txtData2);
			this->groupBox2->Controls->Add(this->txtData1);
			this->groupBox2->Controls->Add(this->txtData0);
			this->groupBox2->Controls->Add(this->txtID);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->label4);
			this->groupBox2->Controls->Add(this->btnWrite);
			this->groupBox2->Location = System::Drawing::Point(5, 285);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(733, 102);
			this->groupBox2->TabIndex = 55;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Write Messages";
			// 
			// cbbID
			// 
			this->cbbID->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbbID->FormattingEnabled = true;
			this->cbbID->Location = System::Drawing::Point(10, 32);
			this->cbbID->Name = L"cbbID";
			this->cbbID->Size = System::Drawing::Size(98, 21);
			this->cbbID->TabIndex = 32;
			this->cbbID->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cbbID_SelectedIndexChanged);
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(7, 16);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(49, 13);
			this->label11->TabIndex = 31;
			this->label11->Text = L"ID (Hex):";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(396, 16);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(87, 13);
			this->label10->TabIndex = 37;
			this->label10->Text = L"Checksum Type:";
			// 
			// cbbCST
			// 
			this->cbbCST->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbbCST->Enabled = false;
			this->cbbCST->FormattingEnabled = true;
			this->cbbCST->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Classic", L"Enhanced", L"Automatic"});
			this->cbbCST->Location = System::Drawing::Point(394, 32);
			this->cbbCST->Name = L"cbbCST";
			this->cbbCST->Size = System::Drawing::Size(121, 21);
			this->cbbCST->TabIndex = 38;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(237, 16);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(52, 13);
			this->label9->TabIndex = 35;
			this->label9->Text = L"Direction:";
			// 
			// cbbDirection
			// 
			this->cbbDirection->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbbDirection->Enabled = false;
			this->cbbDirection->FormattingEnabled = true;
			this->cbbDirection->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Publisher", L"Subscriber"});
			this->cbbDirection->Location = System::Drawing::Point(240, 31);
			this->cbbDirection->Name = L"cbbDirection";
			this->cbbDirection->Size = System::Drawing::Size(142, 21);
			this->cbbDirection->TabIndex = 36;
			// 
			// nudLength
			// 
			this->nudLength->Enabled = false;
			this->nudLength->Location = System::Drawing::Point(10, 71);
			this->nudLength->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {8, 0, 0, 0});
			this->nudLength->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->nudLength->Name = L"nudLength";
			this->nudLength->Size = System::Drawing::Size(98, 20);
			this->nudLength->TabIndex = 40;
			this->nudLength->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->nudLength->ValueChanged += gcnew System::EventHandler(this, &Form1::nudLength_ValueChanged);
			// 
			// txtData7
			// 
			this->txtData7->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData7->Enabled = false;
			this->txtData7->Location = System::Drawing::Point(392, 71);
			this->txtData7->MaxLength = 2;
			this->txtData7->Name = L"txtData7";
			this->txtData7->Size = System::Drawing::Size(32, 20);
			this->txtData7->TabIndex = 49;
			this->txtData7->Text = L"00";
			this->txtData7->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData7->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData7->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtData6
			// 
			this->txtData6->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData6->Enabled = false;
			this->txtData6->Location = System::Drawing::Point(354, 71);
			this->txtData6->MaxLength = 2;
			this->txtData6->Name = L"txtData6";
			this->txtData6->Size = System::Drawing::Size(32, 20);
			this->txtData6->TabIndex = 48;
			this->txtData6->Text = L"00";
			this->txtData6->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData6->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData6->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtData5
			// 
			this->txtData5->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData5->Enabled = false;
			this->txtData5->Location = System::Drawing::Point(316, 71);
			this->txtData5->MaxLength = 2;
			this->txtData5->Name = L"txtData5";
			this->txtData5->Size = System::Drawing::Size(32, 20);
			this->txtData5->TabIndex = 47;
			this->txtData5->Text = L"00";
			this->txtData5->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData5->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData5->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtData4
			// 
			this->txtData4->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData4->Enabled = false;
			this->txtData4->Location = System::Drawing::Point(278, 71);
			this->txtData4->MaxLength = 2;
			this->txtData4->Name = L"txtData4";
			this->txtData4->Size = System::Drawing::Size(32, 20);
			this->txtData4->TabIndex = 46;
			this->txtData4->Text = L"00";
			this->txtData4->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData4->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData4->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtData3
			// 
			this->txtData3->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData3->Enabled = false;
			this->txtData3->Location = System::Drawing::Point(240, 71);
			this->txtData3->MaxLength = 2;
			this->txtData3->Name = L"txtData3";
			this->txtData3->Size = System::Drawing::Size(32, 20);
			this->txtData3->TabIndex = 45;
			this->txtData3->Text = L"00";
			this->txtData3->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData3->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData3->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtData2
			// 
			this->txtData2->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData2->Enabled = false;
			this->txtData2->Location = System::Drawing::Point(202, 71);
			this->txtData2->MaxLength = 2;
			this->txtData2->Name = L"txtData2";
			this->txtData2->Size = System::Drawing::Size(32, 20);
			this->txtData2->TabIndex = 44;
			this->txtData2->Text = L"00";
			this->txtData2->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData2->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData2->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtData1
			// 
			this->txtData1->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData1->Enabled = false;
			this->txtData1->Location = System::Drawing::Point(164, 71);
			this->txtData1->MaxLength = 2;
			this->txtData1->Name = L"txtData1";
			this->txtData1->Size = System::Drawing::Size(32, 20);
			this->txtData1->TabIndex = 43;
			this->txtData1->Text = L"00";
			this->txtData1->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData1->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData1->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtData0
			// 
			this->txtData0->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtData0->Enabled = false;
			this->txtData0->Location = System::Drawing::Point(126, 71);
			this->txtData0->MaxLength = 2;
			this->txtData0->Name = L"txtData0";
			this->txtData0->Size = System::Drawing::Size(32, 20);
			this->txtData0->TabIndex = 42;
			this->txtData0->Text = L"00";
			this->txtData0->TextChanged += gcnew System::EventHandler(this, &Form1::txtData0_TextChanged);
			this->txtData0->Leave += gcnew System::EventHandler(this, &Form1::txtData0_Leave);
			this->txtData0->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// txtID
			// 
			this->txtID->Enabled = false;
			this->txtID->Location = System::Drawing::Point(126, 32);
			this->txtID->MaxLength = 2;
			this->txtID->Name = L"txtID";
			this->txtID->Size = System::Drawing::Size(100, 20);
			this->txtID->TabIndex = 34;
			this->txtID->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::txtHexFormat_KeyPress);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(123, 55);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(60, 13);
			this->label6->TabIndex = 41;
			this->label6->Text = L"Data (1..8):";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(7, 55);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(43, 13);
			this->label5->TabIndex = 39;
			this->label5->Text = L"Length:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(123, 16);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(56, 13);
			this->label4->TabIndex = 33;
			this->label4->Text = L"PID (Hex):";
			// 
			// btnWrite
			// 
			this->btnWrite->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnWrite->Cursor = System::Windows::Forms::Cursors::Default;
			this->btnWrite->Enabled = false;
			this->btnWrite->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnWrite->Location = System::Drawing::Point(663, 29);
			this->btnWrite->Name = L"btnWrite";
			this->btnWrite->Size = System::Drawing::Size(65, 23);
			this->btnWrite->TabIndex = 50;
			this->btnWrite->Text = L"Write";
			this->btnWrite->Click += gcnew System::EventHandler(this, &Form1::btnWrite_Click);
			// 
			// btnMsgClear
			// 
			this->btnMsgClear->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnMsgClear->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnMsgClear->Location = System::Drawing::Point(663, 37);
			this->btnMsgClear->Name = L"btnMsgClear";
			this->btnMsgClear->Size = System::Drawing::Size(65, 23);
			this->btnMsgClear->TabIndex = 29;
			this->btnMsgClear->Text = L"Clear";
			this->btnMsgClear->UseVisualStyleBackColor = true;
			this->btnMsgClear->Click += gcnew System::EventHandler(this, &Form1::btnMsgClear_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->btnIdent);
			this->groupBox1->Controls->Add(this->btnHwRefresh);
			this->groupBox1->Controls->Add(this->cbbChannel);
			this->groupBox1->Controls->Add(this->cbbHwMode);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->cbbBaudrates);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->btnInit);
			this->groupBox1->Controls->Add(this->btnRelease);
			this->groupBox1->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->groupBox1->Location = System::Drawing::Point(5, 4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(734, 65);
			this->groupBox1->TabIndex = 52;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L" Connection ";
			// 
			// btnIdent
			// 
			this->btnIdent->Location = System::Drawing::Point(532, 30);
			this->btnIdent->Name = L"btnIdent";
			this->btnIdent->Size = System::Drawing::Size(54, 23);
			this->btnIdent->TabIndex = 9;
			this->btnIdent->Text = L"Identify";
			this->btnIdent->UseVisualStyleBackColor = true;
			this->btnIdent->Click += gcnew System::EventHandler(this, &Form1::btnIdent_Click);
			// 
			// btnHwRefresh
			// 
			this->btnHwRefresh->Cursor = System::Windows::Forms::Cursors::Default;
			this->btnHwRefresh->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnHwRefresh->Location = System::Drawing::Point(234, 31);
			this->btnHwRefresh->Name = L"btnHwRefresh";
			this->btnHwRefresh->Size = System::Drawing::Size(65, 23);
			this->btnHwRefresh->TabIndex = 4;
			this->btnHwRefresh->Text = L"Refresh";
			this->btnHwRefresh->Click += gcnew System::EventHandler(this, &Form1::btnHwRefresh_Click);
			// 
			// cbbChannel
			// 
			this->cbbChannel->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbbChannel->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cbbChannel->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"None", L"USB-Channel 1", L"USB-Channel 2", L"USB-Channel 3", 
				L"USB-Channel 4", L"USB-Channel 5", L"USB-Channel 6", L"USB-Channel 7", L"USB-Channel 8"});
			this->cbbChannel->Location = System::Drawing::Point(6, 31);
			this->cbbChannel->Name = L"cbbChannel";
			this->cbbChannel->Size = System::Drawing::Size(222, 21);
			this->cbbChannel->TabIndex = 3;
			this->cbbChannel->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cbbChannel_SelectedIndexChanged);
			// 
			// cbbHwMode
			// 
			this->cbbHwMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbbHwMode->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Slave", L"Master"});
			this->cbbHwMode->Location = System::Drawing::Point(427, 32);
			this->cbbHwMode->Name = L"cbbHwMode";
			this->cbbHwMode->Size = System::Drawing::Size(88, 21);
			this->cbbHwMode->TabIndex = 8;
			this->cbbHwMode->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cbbHwMode_SelectedIndexChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(428, 16);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(90, 23);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Mode:";
			// 
			// cbbBaudrates
			// 
			this->cbbBaudrates->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"2400", L"9600", L"10400", L"19200"});
			this->cbbBaudrates->Location = System::Drawing::Point(305, 32);
			this->cbbBaudrates->Name = L"cbbBaudrates";
			this->cbbBaudrates->Size = System::Drawing::Size(116, 21);
			this->cbbBaudrates->TabIndex = 6;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(305, 16);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(56, 23);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Baudrate:";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(7, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(56, 23);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Hardware:";
			// 
			// btnInit
			// 
			this->btnInit->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnInit->Cursor = System::Windows::Forms::Cursors::Default;
			this->btnInit->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnInit->Location = System::Drawing::Point(592, 30);
			this->btnInit->Name = L"btnInit";
			this->btnInit->Size = System::Drawing::Size(65, 23);
			this->btnInit->TabIndex = 10;
			this->btnInit->Text = L"Initialize";
			this->btnInit->Click += gcnew System::EventHandler(this, &Form1::btnInit_Click);
			// 
			// btnRelease
			// 
			this->btnRelease->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnRelease->Cursor = System::Windows::Forms::Cursors::Default;
			this->btnRelease->Enabled = false;
			this->btnRelease->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnRelease->Location = System::Drawing::Point(663, 30);
			this->btnRelease->Name = L"btnRelease";
			this->btnRelease->Size = System::Drawing::Size(65, 23);
			this->btnRelease->TabIndex = 11;
			this->btnRelease->Text = L"Release";
			this->btnRelease->Click += gcnew System::EventHandler(this, &Form1::btnRelease_Click);
			// 
			// clhRcvTime
			// 
			this->clhRcvTime->Text = L"Rcv Time";
			this->clhRcvTime->Width = 97;
			// 
			// clhCRC
			// 
			this->clhCRC->Text = L"Checksum";
			// 
			// clhDirection
			// 
			this->clhDirection->Text = L"Direction";
			// 
			// clhCount
			// 
			this->clhCount->Text = L"Count";
			this->clhCount->Width = 49;
			// 
			// clhErrors
			// 
			this->clhErrors->Text = L"Errors";
			// 
			// rdbTimer
			// 
			this->rdbTimer->AutoSize = true;
			this->rdbTimer->Checked = true;
			this->rdbTimer->Location = System::Drawing::Point(6, 14);
			this->rdbTimer->Name = L"rdbTimer";
			this->rdbTimer->Size = System::Drawing::Size(117, 17);
			this->rdbTimer->TabIndex = 23;
			this->rdbTimer->TabStop = true;
			this->rdbTimer->Text = L"Read using a Timer";
			this->rdbTimer->UseVisualStyleBackColor = true;
			this->rdbTimer->CheckedChanged += gcnew System::EventHandler(this, &Form1::rdbTimer_CheckedChanged);
			// 
			// clhCST
			// 
			this->clhCST->Text = L"CST";
			// 
			// btnRead
			// 
			this->btnRead->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnRead->Enabled = false;
			this->btnRead->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnRead->Location = System::Drawing::Point(592, 37);
			this->btnRead->Name = L"btnRead";
			this->btnRead->Size = System::Drawing::Size(65, 23);
			this->btnRead->TabIndex = 28;
			this->btnRead->Text = L"Read";
			this->btnRead->UseVisualStyleBackColor = true;
			this->btnRead->Click += gcnew System::EventHandler(this, &Form1::btnRead_Click);
			// 
			// clhData
			// 
			this->clhData->Text = L"Data";
			this->clhData->Width = 160;
			// 
			// clhLength
			// 
			this->clhLength->Text = L"Length";
			this->clhLength->Width = 50;
			// 
			// clhID
			// 
			this->clhID->Text = L"ID";
			this->clhID->Width = 90;
			// 
			// btnConfigure
			// 
			this->btnConfigure->Enabled = false;
			this->btnConfigure->Location = System::Drawing::Point(6, 26);
			this->btnConfigure->Name = L"btnConfigure";
			this->btnConfigure->Size = System::Drawing::Size(152, 23);
			this->btnConfigure->TabIndex = 22;
			this->btnConfigure->Text = L"Configure Global Frames";
			this->btnConfigure->UseVisualStyleBackColor = true;
			this->btnConfigure->Click += gcnew System::EventHandler(this, &Form1::btnConfigure_Click);
			// 
			// rdbFilterOpen
			// 
			this->rdbFilterOpen->Checked = true;
			this->rdbFilterOpen->Location = System::Drawing::Point(196, 29);
			this->rdbFilterOpen->Name = L"rdbFilterOpen";
			this->rdbFilterOpen->Size = System::Drawing::Size(53, 17);
			this->rdbFilterOpen->TabIndex = 13;
			this->rdbFilterOpen->TabStop = true;
			this->rdbFilterOpen->Text = L"Open";
			this->rdbFilterOpen->UseVisualStyleBackColor = true;
			this->rdbFilterOpen->CheckedChanged += gcnew System::EventHandler(this, &Form1::rdbFilter_CheckedChanged);
			// 
			// rdbFilterCustom
			// 
			this->rdbFilterCustom->Location = System::Drawing::Point(314, 29);
			this->rdbFilterCustom->Name = L"rdbFilterCustom";
			this->rdbFilterCustom->Size = System::Drawing::Size(104, 17);
			this->rdbFilterCustom->TabIndex = 15;
			this->rdbFilterCustom->Text = L"Custom (expand)";
			this->rdbFilterCustom->UseVisualStyleBackColor = true;
			this->rdbFilterCustom->CheckedChanged += gcnew System::EventHandler(this, &Form1::rdbFilter_CheckedChanged);
			// 
			// groupBox3
			// 
			this->groupBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox3->Controls->Add(this->btnConfigure);
			this->groupBox3->Controls->Add(this->rdbFilterOpen);
			this->groupBox3->Controls->Add(this->rdbFilterCustom);
			this->groupBox3->Controls->Add(this->rdbFilterClose);
			this->groupBox3->Controls->Add(this->btnFilterQuery);
			this->groupBox3->Controls->Add(this->nudIdTo);
			this->groupBox3->Controls->Add(this->nudIdFrom);
			this->groupBox3->Controls->Add(this->label8);
			this->groupBox3->Controls->Add(this->label7);
			this->groupBox3->Controls->Add(this->btnFilterApply);
			this->groupBox3->Location = System::Drawing::Point(5, 75);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(734, 58);
			this->groupBox3->TabIndex = 53;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Configuration && Message Filtering ";
			// 
			// rdbFilterClose
			// 
			this->rdbFilterClose->Location = System::Drawing::Point(253, 29);
			this->rdbFilterClose->Name = L"rdbFilterClose";
			this->rdbFilterClose->Size = System::Drawing::Size(58, 17);
			this->rdbFilterClose->TabIndex = 14;
			this->rdbFilterClose->Text = L"Close";
			this->rdbFilterClose->UseVisualStyleBackColor = true;
			this->rdbFilterClose->CheckedChanged += gcnew System::EventHandler(this, &Form1::rdbFilter_CheckedChanged);
			// 
			// btnFilterQuery
			// 
			this->btnFilterQuery->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnFilterQuery->Enabled = false;
			this->btnFilterQuery->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnFilterQuery->Location = System::Drawing::Point(663, 26);
			this->btnFilterQuery->Name = L"btnFilterQuery";
			this->btnFilterQuery->Size = System::Drawing::Size(65, 23);
			this->btnFilterQuery->TabIndex = 21;
			this->btnFilterQuery->Text = L"Query";
			this->btnFilterQuery->UseVisualStyleBackColor = true;
			this->btnFilterQuery->Click += gcnew System::EventHandler(this, &Form1::btnFilterQuery_Click);
			// 
			// nudIdTo
			// 
			this->nudIdTo->Location = System::Drawing::Point(499, 29);
			this->nudIdTo->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {63, 0, 0, 0});
			this->nudIdTo->Name = L"nudIdTo";
			this->nudIdTo->Size = System::Drawing::Size(69, 20);
			this->nudIdTo->TabIndex = 19;
			this->nudIdTo->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {63, 0, 0, 0});
			// 
			// nudIdFrom
			// 
			this->nudIdFrom->Location = System::Drawing::Point(424, 29);
			this->nudIdFrom->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {63, 0, 0, 0});
			this->nudIdFrom->Name = L"nudIdFrom";
			this->nudIdFrom->Size = System::Drawing::Size(69, 20);
			this->nudIdFrom->TabIndex = 17;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(497, 10);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(28, 23);
			this->label8->TabIndex = 18;
			this->label8->Text = L"To:";
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(422, 12);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(38, 23);
			this->label7->TabIndex = 16;
			this->label7->Text = L"From:";
			// 
			// btnFilterApply
			// 
			this->btnFilterApply->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnFilterApply->Enabled = false;
			this->btnFilterApply->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnFilterApply->Location = System::Drawing::Point(592, 26);
			this->btnFilterApply->Name = L"btnFilterApply";
			this->btnFilterApply->Size = System::Drawing::Size(65, 23);
			this->btnFilterApply->TabIndex = 20;
			this->btnFilterApply->Text = L"Apply";
			this->btnFilterApply->UseVisualStyleBackColor = true;
			this->btnFilterApply->Click += gcnew System::EventHandler(this, &Form1::btnFilterApply_Click);
			// 
			// btnStatus
			// 
			this->btnStatus->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnStatus->Enabled = false;
			this->btnStatus->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnStatus->Location = System::Drawing::Point(592, 48);
			this->btnStatus->Name = L"btnStatus";
			this->btnStatus->Size = System::Drawing::Size(65, 23);
			this->btnStatus->TabIndex = 55;
			this->btnStatus->Text = L"Status";
			this->btnStatus->UseVisualStyleBackColor = true;
			this->btnStatus->Click += gcnew System::EventHandler(this, &Form1::btnStatus_Click);
			// 
			// groupBox5
			// 
			this->groupBox5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox5->Controls->Add(this->chbShowPeriod);
			this->groupBox5->Controls->Add(this->rdbManual);
			this->groupBox5->Controls->Add(this->lstMessages);
			this->groupBox5->Controls->Add(this->btnMsgClear);
			this->groupBox5->Controls->Add(this->rdbTimer);
			this->groupBox5->Controls->Add(this->btnRead);
			this->groupBox5->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->groupBox5->Location = System::Drawing::Point(5, 139);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(734, 140);
			this->groupBox5->TabIndex = 54;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L" Messages Reading ";
			// 
			// chbShowPeriod
			// 
			this->chbShowPeriod->AutoSize = true;
			this->chbShowPeriod->Checked = true;
			this->chbShowPeriod->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chbShowPeriod->Location = System::Drawing::Point(227, 15);
			this->chbShowPeriod->Name = L"chbShowPeriod";
			this->chbShowPeriod->Size = System::Drawing::Size(123, 17);
			this->chbShowPeriod->TabIndex = 26;
			this->chbShowPeriod->Text = L"Timestamp as period";
			this->chbShowPeriod->UseVisualStyleBackColor = true;
			this->chbShowPeriod->CheckedChanged += gcnew System::EventHandler(this, &Form1::chbShowPeriod_CheckedChanged);
			// 
			// rdbManual
			// 
			this->rdbManual->AutoSize = true;
			this->rdbManual->Location = System::Drawing::Point(129, 14);
			this->rdbManual->Name = L"rdbManual";
			this->rdbManual->Size = System::Drawing::Size(89, 17);
			this->rdbManual->TabIndex = 25;
			this->rdbManual->Text = L"Manual Read";
			this->rdbManual->UseVisualStyleBackColor = true;
			this->rdbManual->CheckedChanged += gcnew System::EventHandler(this, &Form1::rdbTimer_CheckedChanged);
			// 
			// lstMessages
			// 
			this->lstMessages->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(9) {this->clhID, this->clhLength, 
				this->clhData, this->clhCount, this->clhRcvTime, this->clhDirection, this->clhErrors, this->clhCST, this->clhCRC});
			this->lstMessages->FullRowSelect = true;
			this->lstMessages->Location = System::Drawing::Point(8, 37);
			this->lstMessages->MultiSelect = false;
			this->lstMessages->Name = L"lstMessages";
			this->lstMessages->Size = System::Drawing::Size(560, 96);
			this->lstMessages->TabIndex = 27;
			this->lstMessages->UseCompatibleStateImageBehavior = false;
			this->lstMessages->View = System::Windows::Forms::View::Details;
			// 
			// btnReset
			// 
			this->btnReset->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnReset->Enabled = false;
			this->btnReset->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnReset->Location = System::Drawing::Point(663, 48);
			this->btnReset->Name = L"btnReset";
			this->btnReset->Size = System::Drawing::Size(65, 23);
			this->btnReset->TabIndex = 56;
			this->btnReset->Text = L"Reset";
			this->btnReset->UseVisualStyleBackColor = true;
			this->btnReset->Click += gcnew System::EventHandler(this, &Form1::btnReset_Click);
			// 
			// groupBox4
			// 
			this->groupBox4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox4->Controls->Add(this->btnReset);
			this->groupBox4->Controls->Add(this->btnStatus);
			this->groupBox4->Controls->Add(this->btnGetVersions);
			this->groupBox4->Controls->Add(this->lbxInfo);
			this->groupBox4->Controls->Add(this->btnInfoClear);
			this->groupBox4->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->groupBox4->Location = System::Drawing::Point(5, 393);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(734, 135);
			this->groupBox4->TabIndex = 56;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Information";
			// 
			// btnGetVersions
			// 
			this->btnGetVersions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnGetVersions->Enabled = false;
			this->btnGetVersions->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnGetVersions->Location = System::Drawing::Point(592, 19);
			this->btnGetVersions->Name = L"btnGetVersions";
			this->btnGetVersions->Size = System::Drawing::Size(65, 23);
			this->btnGetVersions->TabIndex = 53;
			this->btnGetVersions->Text = L"Versions";
			this->btnGetVersions->UseVisualStyleBackColor = true;
			this->btnGetVersions->Click += gcnew System::EventHandler(this, &Form1::btnGetVersions_Click);
			// 
			// lbxInfo
			// 
			this->lbxInfo->FormattingEnabled = true;
			this->lbxInfo->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"Select a Hardware and a configuration for it. Then click \"Initialize\" button"});
			this->lbxInfo->Location = System::Drawing::Point(10, 19);
			this->lbxInfo->Name = L"lbxInfo";
			this->lbxInfo->ScrollAlwaysVisible = true;
			this->lbxInfo->Size = System::Drawing::Size(558, 108);
			this->lbxInfo->TabIndex = 52;
			// 
			// btnInfoClear
			// 
			this->btnInfoClear->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnInfoClear->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnInfoClear->Location = System::Drawing::Point(663, 19);
			this->btnInfoClear->Name = L"btnInfoClear";
			this->btnInfoClear->Size = System::Drawing::Size(65, 23);
			this->btnInfoClear->TabIndex = 54;
			this->btnInfoClear->Text = L"Clear";
			this->btnInfoClear->UseVisualStyleBackColor = true;
			this->btnInfoClear->Click += gcnew System::EventHandler(this, &Form1::btnInfoClear_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(744, 533);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->groupBox4);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PLIN-API Example";
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudLength))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudIdTo))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nudIdFrom))->EndInit();
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			this->groupBox4->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

#pragma region Form Event Handlers

		/// <summary>
		/// Occurs before the form is closed.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A FormClosingEventArgs that contains the event data.</param>
	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
			 {
				 // disconnect from LIN if necessary
				 if (m_hClient != 0)
				 {
					 DoLinDisconnect();
					 m_hHw = 0;
					 // Unregister the application
					 Peak::Lin::PLinApi::RemoveClient(m_hClient);
					 m_hClient = 0;
				 }
			 }

			 /// <summary>
			 /// Occurs before the form is shown.
			 /// </summary>
			 /// <param name="sender">The source of the event.</param>
			 /// <param name="e">the event data.</param>
	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e)
			 {
				 RefreshHardware();
				 rdbFilter_CheckedChanged(this, gcnew EventArgs());
				 cbbDirection->SelectedIndex = 0;
				 cbbCST->SelectedIndex = 1;
			 }
#pragma endregion


#pragma region "Connection" Event Handlers
	private: System::Void cbbChannel_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
				 int lnMode, lnCurrBaud;
				 // get the handle from the comboBoxItem
				 HLINHW lwHw = (HLINHW) ((ComboBoxItem^)cbbChannel->SelectedItem)->Value;

				 if (lwHw != 0)
				 {
					 btnInit->Enabled = true;
					 btnIdent->Enabled = true;
					 // Read the mode of the hardware with the handle lwHw (Master, Slave or None).
					 m_LastLINErr = Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpMode, lnMode, 0);
					 // Read the baudrate of the hardware with the handle lwHw.
					 m_LastLINErr = Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpBaudrate, lnCurrBaud, 0);

					 // Update hardware mode comboBox
					 if ((Peak::Lin::TLINHardwareMode)lnMode == Peak::Lin::TLINHardwareMode::modMaster)
						 cbbHwMode->SelectedIndex = 1;
					 else
						 cbbHwMode->SelectedIndex = 0;

					 // Assign the Baudrate to the Control.
					 cbbBaudrates->Text = (lnCurrBaud != 0) ? lnCurrBaud.ToString() : m_wBaudrate.ToString();
				 }
				 else
				 {
					 btnInit->Enabled = false;
					 btnIdent->Enabled = false;
				 }
			 }
	private: System::Void btnHwRefresh_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 RefreshHardware();
			 }
	private: System::Void cbbHwMode_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // Change the "write" button text depending on the hardware mode
				 if (cbbHwMode->SelectedIndex == 1)
					 btnWrite->Text = SWriteAsMaster;
				 else
					 btnWrite->Text = SWriteAsSlave;
			 }
	private: System::Void btnIdent_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // Get hardware device from the selected combobox item
				 HLINHW lwHw = (HLINHW)((ComboBoxItem^)cbbChannel->SelectedItem)->Value;
				 if (lwHw != 0)
				 {
					 // makes the corresponding PCAN-USB-Pro's LED blink
					 Peak::Lin::PLinApi::IdentifyHardware(lwHw);
				 }
			 }
	private: System::Void btnInit_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // Do the connection
				 if (DoLinConnect())
				 {
					 // Sets the connection status of the main-form
					 SetConnectionStatus(true);
				 }
			 }
	private: System::Void btnRelease_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // Releases the current connection
				 if (DoLinDisconnect())
				 {
					 // stop reading timer
					 tmrRead->Enabled = false;
					 // Sets the connection status of the main-form
					 SetConnectionStatus(false);
				 }
			 }
#pragma endregion


#pragma region "Configuration & Message filtering" Event Handlers
	private: System::Void btnConfigure_Click(System::Object^  sender, System::EventArgs^  e) 
			 {				 
				 // Open the "Global Frame Table" Dialog
				 Frame_Dlg^ dlg = gcnew Frame_Dlg(m_pGFT);
				 dlg->ShowDialog();
				 // Output filter information (as changes to Global Frame Table items modify it)
				 btnFilterQuery_Click(this, gcnew EventArgs());
				 // Update the available frame ids (i.e. the IDs combobox in the "write message" UI group)
				 UpdateFrameIds();
			 }
	private: System::Void rdbFilter_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
				 if (rdbFilterCustom->Checked)
				 {
					 nudIdFrom->Enabled = true;
					 nudIdTo->Enabled = true;
				 }
				 else
				 {
					 nudIdFrom->Enabled = false;
					 nudIdTo->Enabled = false;
				 }

			 }
	private: System::Void btnFilterApply_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 if (m_hHw != 0)
				 {
					 UInt64 lMask;
					 // Sets the according to the selected filter
					 if (rdbFilterOpen->Checked)
					 {
						 // set no filtering : all frames are received
						 lMask = FRAME_FILTER_MASK;
						 m_LastLINErr = Peak::Lin::PLinApi::SetClientFilter(m_hClient, m_hHw, lMask);
						 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
						 {
							 m_lMask = lMask;
							 IncludeTextMessage(String::Format("The filter was successfully opened."));
						 }
					 }
					 else if (rdbFilterClose->Checked)
					 {
						 // filters all : all frames are denied
						 lMask = 0x0;
						 m_LastLINErr = Peak::Lin::PLinApi::SetClientFilter(m_hClient, m_hHw, lMask);
						 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
						 {
							 m_lMask = lMask;
							 IncludeTextMessage(String::Format("The filter was successfully closed."));
						 }
					 }
					 else
					 {
						 // custom filtering (i.e. rdbFilterCustom.Checked) : the frames for the given range will be opened
						 m_LastLINErr = Peak::Lin::PLinApi::RegisterFrameId(m_hClient, m_hHw, (Byte)nudIdFrom->Value, (Byte)nudIdTo->Value);
						 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
						 {
							 IncludeTextMessage(String::Format("The filter was customized. IDs from {0} to {1} will be received", nudIdFrom->Text, nudIdTo->Text));
							 m_LastLINErr = Peak::Lin::PLinApi::GetClientFilter(m_hClient, m_hHw, lMask);
							 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
								 m_lMask = lMask;
						 }

					 }
					 // If success, an information message is written, if it is not, an error message is shown
					 if (m_LastLINErr != Peak::Lin::TLINError::errOK)
						 MessageBox::Show(GetFormatedError(m_LastLINErr));
				 }
			 }
	private: System::Void btnFilterQuery_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 UInt64 pRcvMask = 0;
				 // Retrieves the filter corresponding to the current Client-Hardware pair
				 if (m_hHw != 0)
				 {
					 if (Peak::Lin::PLinApi::GetClientFilter(m_hClient, m_hHw, pRcvMask) == Peak::Lin::TLINError::errOK)
						 IncludeTextMessage(String::Format("The Status of the filter is {0}.", Convert::ToString((__int64)pRcvMask, 2)->PadLeft(64, '0')));
				 }
			 }
#pragma endregion


#pragma region "Messages Reading" Event Handlers
	private: System::Void rdbTimer_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
				 if (!btnRelease->Enabled)
					 return;

				 // According with the kind of reading, a timer or a button will be enabled
				 if (rdbTimer->Checked)
				 {
					 // Enable Timer
					 tmrRead->Enabled = btnRelease->Enabled;
				 }
				 if (rdbManual->Checked)
				 {
					 // Disable Timer
					 tmrRead->Enabled = false;
				 }
				 btnRead->Enabled = btnRelease->Enabled && rdbManual->Checked;
			 }
	private: System::Void chbShowPeriod_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
				 for each(MessageStatus^ msg in m_LastMsgsList)
					 msg->ShowingPeriod = chbShowPeriod->Checked;
			 }
	private: System::Void btnRead_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 ReadMessages();
			 }
	private: System::Void btnMsgClear_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 m_LastMsgsList->Clear();
				 lstMessages->Items->Clear();
			 }
#pragma endregion


#pragma region "Write messages" Event Handlers
	private: System::Void cbbID_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
				 CFrameDefinition^ lFD = (CFrameDefinition^)((ComboBoxItem^)cbbID->SelectedItem)->Value;
				 // Check if a global frame is selected
				 if (lFD != nullptr)
				 {
					 // Update components according to the selected frame informations
					 txtID->Text = lFD->ProtectedID;
					 txtID->Enabled = false;
					 nudLength->Value = lFD->Length;
					 switch (lFD->Direction)
					 {
					 case Peak::Lin::TLINDirection::dirDisabled:
						 cbbDirection->SelectedIndex = 0;
						 break;
					 case Peak::Lin::TLINDirection::dirPublisher:
						 cbbDirection->SelectedIndex = 1;
						 break;
					 case Peak::Lin::TLINDirection::dirSubscriber:
						 cbbDirection->SelectedIndex = 2;
						 break;
					 case Peak::Lin::TLINDirection::dirSubscriberAutoLength:
						 cbbDirection->SelectedIndex = 3;
						 break;
					 }
					 switch (lFD->ChecksumType)
					 {
					 case Peak::Lin::TLINChecksumType::cstAuto:
						 cbbCST->SelectedIndex = 0;
						 break;
					 case Peak::Lin::TLINChecksumType::cstClassic:
						 cbbCST->SelectedIndex = 1;
						 break;
					 case Peak::Lin::TLINChecksumType::cstEnhanced:
						 cbbCST->SelectedIndex = 2;
						 break;
					 case Peak::Lin::TLINChecksumType::cstCustom:
						 cbbCST->SelectedIndex = 3;
						 break;
					 }
					 // Force update on Data fields (both Length and Direction impact on Enabled state of these components)
					 // if length value is not changed but direction switch from subscriber to publisher
					 // data textfield would not be disabled
					 nudLength_ValueChanged(this, gcnew EventArgs());
				 }
			 }
	private: System::Void txtHexFormat_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 Char chCheck;

				 // We convert the Character to its Upper case equivalent
				 //
				 chCheck = Char::ToUpper(e->KeyChar);

				 // The Key is the Delete (Backspace) Key
				 //
				 if (chCheck == 8)
					 return;
				 // The Key is a number between 0-9
				 //
				 if ((chCheck > 47) && (chCheck < 58))
					 return;
				 // The Key is a character between A-F
				 //
				 if ((chCheck > 64) && (chCheck < 71))
					 return;

				 // Is neither a number nor a character between A(a) and F(f)
				 //
				 e->Handled = true;
			 }

	private: System::Void txtData0_Leave(System::Object^  sender, System::EventArgs^  e) 
			 {
				 TextBox^ txtbCurrentTextbox;

				// all the Textbox Data fields are represented with 2 characters.
				// Therefore if the Length of the text is smaller than 2, we add
				// a "0"
				//
				if (sender->GetType()->Name == "TextBox")
				{
					txtbCurrentTextbox = (TextBox^)sender;
					while (txtbCurrentTextbox->Text->Length != 2)
						txtbCurrentTextbox->Text = ("0" + txtbCurrentTextbox->Text);
				}
			}

	private: System::Void nudLength_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
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
				 if (cbbDirection->SelectedIndex != 1)
					 return;
				 // Enable the same number of data textBox as the nudLength value
				 switch (Convert::ToInt32(nudLength->Value))
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
	private: System::Void txtData0_TextChanged(System::Object^  sender, System::EventArgs^  e) 
			 {				 
				 // Automatically change focus between data textBox 
				 // when the length of the data equals 2
				 if (dynamic_cast<TextBox^>(sender) != nullptr) 
				 {
					 TextBox^ txtbox = (TextBox^)sender;
					 if (txtbox->TextLength == 2)
						 SelectNextControl(txtbox, true, true, true, true);
				 }
			 }
	private: System::Void btnWrite_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 Peak::Lin::TLINMsg pMsg;
				 TextBox^ txtbCurrentTextBox;
				 CFrameDefinition^ lFD;

				 // Assert that a frame ID has been selected
				 if (txtID->Text->Length == 0)
				 {
					 txtID->Focus();
					 return;
				 }
				 // Get the CFrameDefinition associated to the selected FrameID
				 lFD = (CFrameDefinition^)(((ComboBoxItem^)cbbID->SelectedItem)->Value);
				 // Create a new LIN frame message and copy the data.
				 pMsg.Data = gcnew array<Byte>(8);
				 pMsg.FrameId = Convert::ToByte(lFD->ProtectedIdAsInt);
				 pMsg.Direction = (Peak::Lin::TLINDirection)((ComboBoxItem^)cbbDirection->SelectedItem)->Value;
				 pMsg.ChecksumType = (Peak::Lin::TLINChecksumType)((ComboBoxItem^)cbbCST->SelectedItem)->Value;
				 pMsg.Length = Convert::ToByte(nudLength->Value);
				 // Fill data array
				 txtbCurrentTextBox = txtData0;
				 for (int i = 0; i < pMsg.Length; i++)
				 {
					 if (txtbCurrentTextBox->Enabled && txtbCurrentTextBox->Text->Length != 0)
						 pMsg.Data[i] = Convert::ToByte(txtbCurrentTextBox->Text, 16);
					 else
						 pMsg.Data[i] = 0;
					 if (i < 7)
						 txtbCurrentTextBox = (TextBox^)this->GetNextControl(txtbCurrentTextBox, true);
				 }
				 // Check if the hardware is initialize as master
				 if (m_HwMode == Peak::Lin::TLINHardwareMode::modMaster)
				 {
					 // Calculate the checksum contained with the
					 // checksum type that set some line before.
					 Peak::Lin::PLinApi::CalculateChecksum(pMsg);
					 // Try to send the LIN frame message with LIN_Write.
					 m_LastLINErr = Peak::Lin::PLinApi::Write(m_hClient, m_hHw, pMsg);
				 }
				 else
				 {
					 // If the hardare is initialize as slave
					 // only update the data in the LIN frame.
					 m_LastLINErr = Peak::Lin::PLinApi::UpdateByteArray(m_hClient, m_hHw, lFD->IdAsByte, (Byte)0, pMsg.Length, pMsg.Data);
				 }
				 // Show error if any
				 if (m_LastLINErr != Peak::Lin::TLINError::errOK)
					 MessageBox::Show(GetFormatedError(m_LastLINErr), SAppCaptionError);
			 }
#pragma endregion


#pragma region "Information" Event Handlers
	private: System::Void btnInfoClear_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // The information contained in the Information List-Box is cleared
				 lbxInfo->Items->Clear();
			 }
	private: System::Void btnGetVersions_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 Peak::Lin::TLINVersion lpVersion;
				 StringBuilder^ strTemp;
				 array<String^>^ strArrayVersion;

				 strTemp = gcnew StringBuilder(255);

				 // We get the vesion of the PLIN API
				 //
				 m_LastLINErr = Peak::Lin::PLinApi::GetVersion(lpVersion);
				 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
				 {
					 IncludeTextMessage(String::Format("API Version: {0}.{1}.{2}.{3}", lpVersion.Major, lpVersion.Minor, lpVersion.Build, lpVersion.Revision));
					 // We get the driver version
					 //
					 m_LastLINErr = Peak::Lin::PLinApi::GetVersionInfo(strTemp, 255);
					 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
					 {
						 // Because this information contains line control characters (several lines)
						 // we split this also in several entries in the Information List-Box
						 //
						 strArrayVersion = strTemp->ToString()->Split(gcnew array<wchar_t>{ '\n' });
						 IncludeTextMessage("Channel/Driver Version: ");
						 for (int i = 0; i < strArrayVersion->Length; i++)
							 IncludeTextMessage("     * " + strArrayVersion[i]);
					 }
				 }

				 // If an error ccurred, a message is shown
				 //
				 if (m_LastLINErr != Peak::Lin::TLINError::errOK)
					 MessageBox::Show(GetFormatedError(m_LastLINErr));
			 }
	private: System::Void btnStatus_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 Peak::Lin::TLINHardwareStatus lStatus;
				 // Retrieves the status of the LIN Bus and outputs its state in the information listView
				 m_LastLINErr = Peak::Lin::PLinApi::GetStatus(m_hHw, lStatus);
				 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
					 switch (lStatus.Status)
					{
					 case Peak::Lin::TLINHardwareState::hwsActive:
						 IncludeTextMessage(SLINStatusActive);
						 break;
					 case Peak::Lin::TLINHardwareState::hwsAutobaudrate:
						 IncludeTextMessage(SLINStatusBaudDetect);
						 break;
					 case Peak::Lin::TLINHardwareState::hwsNotInitialized:
						 IncludeTextMessage(SLINStatusNotInit);
						 break;
					 case Peak::Lin::TLINHardwareState::hwsShortGround:
						 IncludeTextMessage(SLINStatusShortGround);
						 break;
					 case Peak::Lin::TLINHardwareState::hwsSleep:
						 IncludeTextMessage(SLINStatusSleep);
						 break;
					}

			 }
	private: System::Void btnReset_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // Flushes the Receive Queue of the Client and resets its counters. 
				 //
				 m_LastLINErr = Peak::Lin::PLinApi::ResetClient(m_hClient);

				 // If it fails, a error message is shown
				 //
				 if (m_LastLINErr != Peak::Lin::TLINError::errOK)
					 MessageBox::Show(GetFormatedError(m_LastLINErr));
				 else
				 {
					 // clears the message list-view
					 btnMsgClear_Click(this, gcnew EventArgs());
					 IncludeTextMessage("Receive queue and counters successfully reset");
				 }
			 }
#pragma endregion

	private: System::Void tmrRead_Tick(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // Checks if in the receive-queue are currently messages for read
				 ReadMessages();
			 }
	private: System::Void tmrDisplay_Tick(System::Object^  sender, System::EventArgs^  e) 
			 {
				 // Update display of received messages
				 DisplayMessages();
			 }

#pragma region Custom functions

#pragma region LIN connection functions

			 /// <summary>
			 /// Updates the combobox 'cbbChannel' with currently available hardwares
			 /// </summary>
			 void RefreshHardware()
			 {
				 Byte i;
				 array<HLINHW>^ lwHwHandles;
				 unsigned short lwBuffSize, lwCount;
				 int lnHwType, lnDevNo, lnChannel, lnMode;
				 Peak::Lin::TLINError lLINErr;
				 HLINHW lwHw;
				 String^ str;

				 lwHwHandles = gcnew array<HLINHW>(8);
				 lwBuffSize = 8;
				 lwCount = 0;
				 // Get all available LIN hardware.
				 lLINErr = Peak::Lin::PLinApi::GetAvailableHardware(lwHwHandles, lwBuffSize, lwCount);
				 if (lLINErr == Peak::Lin::TLINError::errOK)
				 {
					 cbbChannel->Items->Clear();
					 // If no error occurs
					 if (lwCount == 0)
					 {
						 // No LIN hardware was found.
						 // Show an empty entry
						 lwHw = 0;
						 cbbChannel->Items->Add(gcnew ComboBoxItem(SHardwareNothingFound, lwHw));
					 }
					 // For each founded LIN hardware
					 for (i = 0; i < lwCount; i++)
					 {
						 // Get the handle of the hardware.
						 lwHw = lwHwHandles[i];
						 // Read the type of the hardware with the handle lwHw.
						 Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpType, lnHwType, 0);
						 // Read the device number of the hardware with the handle lwHw.
						 Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpDeviceNumber, lnDevNo, 0);
						 // Read the channel number of the hardware with the handle lwHw.
						 Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpChannelNumber, lnChannel, 0);
						 // Read the mode of the hardware with the handle lwHw (Master, Slave or None).
						 Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpMode, lnMode, 0);

						 // Create a comboboxItem
						 // If the hardware type is a knowing hardware
						 // show the name of that in the label of the entry.
						 if (lnHwType == Peak::Lin::PLinApi::LIN_HW_TYPE_USB)
							 str = SHardwareTypeLIN;
						 else
							 // Show as unknown hardware
							 str = SHardwareTypeUnkown;
						 str = String::Format("{0} - dev. {1}, chan. {2}", str, lnDevNo, lnChannel);
						 cbbChannel->Items->Add(gcnew ComboBoxItem(str, lwHw));

					 }
					 cbbChannel->SelectedIndex = 0;
				 }
			 }

			 /// <summary>
			 /// Connects to the hardware with the setting values
			 /// from the connection groupbox.
			 /// </summary>
			 /// <returns>
			 /// Returns true if the function finished successfull. Otherwise
			 /// returns false.
			 /// </returns>
			 bool DoLinConnect()
			 {
				 bool fRet;
				 HLINHW lwHw;
				 unsigned short lwBaud;
				 int lnMode, lnCurrBaud;
				 Peak::Lin::TLINHardwareMode lHwMode;

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
				 lwHw = (HLINHW)((ComboBoxItem^)cbbChannel->SelectedItem)->Value;
				 if (lwHw != 0)
				 {
					 array<HLINCLIENT>^ lhClients = gcnew array<HLINCLIENT>(255);
					 m_LastLINErr = Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpConnectedClients, lhClients, 255);

					 // The local hardware handle is valid.
					 // Get the current mode of the hardware
					 Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpMode, lnMode, 0);
					 // Get the current baudrate of the hardware
					 Peak::Lin::PLinApi::GetHardwareParam(lwHw, Peak::Lin::TLINHardwareParam::hwpBaudrate, lnCurrBaud, 0);
					 // Try to connect the application client to the
					 // hardware with the local handle.
					 m_LastLINErr = Peak::Lin::PLinApi::ConnectClient(m_hClient, lwHw);
					 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
					 {
						 // If the connection successfull
						 // assign the local handle to the
						 // member handle.
						 m_hHw = lwHw;
						 // Get the selected hardware mode.
						 if (cbbHwMode->SelectedIndex == 1)
							 lHwMode = Peak::Lin::TLINHardwareMode::modMaster;
						 else
							 lHwMode = Peak::Lin::TLINHardwareMode::modSlave;
						 // Get the selected baudrate
						 try {
							 lwBaud = Convert::ToUInt16(cbbBaudrates->Text);
						 } catch(...) {
							 lwBaud = 0;
						 }
						 // Get the selected hardware channel
						 if (((Peak::Lin::TLINHardwareMode)lnMode == Peak::Lin::TLINHardwareMode::modNone)
							 || (Convert::ToUInt16(lnCurrBaud) != lwBaud))
						 {
							 // Only if the current hardware is not initialize
							 // try to Intialize the hardware with mode and baudrate
							 m_LastLINErr = Peak::Lin::PLinApi::InitializeHardware(m_hClient,
								 m_hHw, lHwMode, lwBaud);
						 }
						 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
						 {
							 // Assign the Hardware Mode to member attribut
							 m_HwMode = lHwMode;
							 // Assign the baudrate index to member attribut
							 m_wBaudrate = lwBaud;
							 // Set the client filter with the mask.
							 m_LastLINErr = Peak::Lin::PLinApi::SetClientFilter(m_hClient, m_hHw, m_lMask);
							 // Read the frame table from the connected hardware.
							 ReadFrameTableFromHw();
							 // Reset the last LIN error code to default.
							 m_LastLINErr = Peak::Lin::TLINError::errOK;
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
					 if (m_LastLINErr != Peak::Lin::TLINError::errOK)
					 {
						 MessageBox::Show(GetFormatedError(m_LastLINErr), SAppCaptionError);
						 fRet = false;
					 }

				 }
				 else // Should never occur
					 m_hHw = 0; // But if it occurs, set handle to default

				 return fRet;
			 }

			 /// <summary>
			 /// Disconnects an existing connection to a LIN hardware and returns
			 /// true if disconnection finished succesfully or if no connection exists.
			 /// Returns false if the current connection can not be disconnected.
			 /// </summary>
			 /// <returns>
			 /// Returns true if the function finished successfull. Otherwise
			 /// returns false.
			 /// </returns>
			 bool DoLinDisconnect()
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
					 array<HLINCLIENT>^ lhClients = gcnew array<HLINCLIENT>(255);

					 // Get the connected clients from the LIN hardware.
					 m_LastLINErr = Peak::Lin::PLinApi::GetHardwareParam(m_hHw, Peak::Lin::TLINHardwareParam::hwpConnectedClients, lhClients, 255);
					 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
					 {
						 // No errors !
						 // Check all client handles.
						 for (int i = 0; i < lhClients->Length; i++)
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
						 Peak::Lin::PLinApi::ResetHardwareConfig(m_hClient, m_hHw);
					 }
					 // If this application is connected to the hardware
					 // then disconnect the client. Otherwise not.
					 if (lfOwnClient == true)
					 {
						 // Disconnect if the application was connected to a LIN hardware.
						 m_LastLINErr = Peak::Lin::PLinApi::DisconnectClient(m_hClient, m_hHw);
						 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
						 {
							 m_hHw = 0;
							 return true;
						 }
						 else
						 {
							 // Error while disconnecting from hardware.
							 MessageBox::Show(GetFormatedError(m_LastLINErr), SAppCaptionError);
							 return false;
						 }
					 }
					 else
						 return true;
				 }
				 else
					 return true;
			 }

			 /// <summary>
			 /// Reads all values from the frame table of the hardware
			 /// and assign it to the GlobalFrameTable. Also refresh
			 /// the Global Frame Table ListView with that values.
			 /// </summary>
			 void ReadFrameTableFromHw()
			 {
				 int i, lnCnt;
				 Peak::Lin::TLINFrameEntry lFrameEntry;
				 Peak::Lin::TLINError lErr;
				 UInt64 llMask;

				 // Create a LIN frame entry object
				 //TODO ? lFrameEntry = gcnew Peak::Lin::TLINFrameEntry();
				 // Get the count of Frame Definition from the
				 // Global Frame Table.
				 lnCnt = m_pGFT->Count;
				 // Initialize the member attribute for the
				 // client mask with 0.
				 m_lMask = 0;
				 // Each Frame Definition
				 for (i = 0; i < lnCnt; i++)
				 {
					 // Before a frame entry can be read from the
					 // hardware, the Frame-ID of the wanted entry 
					 // must be set
					 lFrameEntry.FrameId = m_pGFT[i]->IdAsByte;
					 // Read the information of the specified frame entry from the hardware.
					 lErr = Peak::Lin::PLinApi::GetFrameEntry(m_hHw, lFrameEntry);
					 // Check the result value of the LinApi function call.
					 if (lErr == Peak::Lin::TLINError::errOK)
					 {
						 // LinApi function call successfull.
						 // Copy the frame entry information to the Frame Definition.
						 m_pGFT[i]->m_nLength = Convert::ToInt32(lFrameEntry.Length);
						 m_pGFT[i]->m_bDirection = lFrameEntry.Direction;
						 m_pGFT[i]->m_nChecksumType = lFrameEntry.ChecksumType;
						 if (m_pGFT[i]->Direction != Peak::Lin::TLINDirection::dirDisabled)
						 {
							 // If the direction is not disabled than set the
							 // bit in the client filter mask.
							 llMask = ((UInt64)1 << i) & FRAME_FILTER_MASK;
							 m_lMask = m_lMask | llMask;
						 }
					 }
				 }
				 // If the Client and Hardware handles are valid.
				 if ((m_hClient != 0) && (m_hHw != 0))
					 // Set the client filter.
					 Peak::Lin::PLinApi::SetClientFilter(m_hClient, m_hHw, m_lMask);
				 // Updates the displayed frameIds
				 UpdateFrameIds();
			 }

			 /// <summary>
			 /// Occurs before a property value of at least one CFrameDefinition object changes.
			 /// </summary>
			 /// <param name="s">The source of the event</param>
			 /// <param name="e">A PropertyValueChangeEventArg that contains the event data.</param>
			 void PropertyChange(Object^ s, PropertyChangeEventArg^ e)
			 {
				 CFrameDefinition^ lFD;
				 Peak::Lin::TLINFrameEntry lFrameEntry;
				 Peak::Lin::TLINError lErr;
				 UInt64 lMask;

				 // Try to get the sender as CFrameDefinition
				 lFD = (CFrameDefinition^)s;
				 // The sender of this event is the CFrameDefinition that
				 // property should be change by User.
				 if (lFD != nullptr)
				 {
					 // If data length is to be set, check the value.
					 if (e->Type == EProperty::Length)
						 // Only a value between 0 and 8 are valid.
						 e->Allowed = (e->Value >= 0) && (e->Value <= 8);
					 // If DO NOT allow then return.
					 if (!e->Allowed)
						 return;

					 // Only if the hardware was initialized as Slave
					 // set the direction of the LIN-Frame.
					 // By the Master the direction will be used with
					 // the LIN_Write and do not have to set here.
					 if (m_HwMode == Peak::Lin::TLINHardwareMode::modSlave)
					 {
						 // Temporary set Allowed to false is to easy
						 // return only later.
						 e->Allowed = false;
						 // Set the Frame-ID of the Frame to get and set.
						 // The ID have to set before get the entry.
						 lFrameEntry.FrameId = lFD->IdAsByte;
						 // Get the Frame Entry with the Frame-ID from
						 // the Hardware via the LinApi.
						 lErr = Peak::Lin::PLinApi::GetFrameEntry(m_hHw, lFrameEntry);
						 // If an error occurs do not allow to change
						 // the property and return.
						 // The Allowed parameter was set some lines before.
						 if (lErr != Peak::Lin::TLINError::errOK)
							 return;
						 // Switch between the different kind of property types.
						 switch (e->Type)
						 {
							 // Direction property should be set
						 case EProperty::Direction:
							 lFrameEntry.Direction = (Peak::Lin::TLINDirection)e->Value;
							 break;
							 // Length property should be set
						 case EProperty::Length:
							 lFrameEntry.Length = Convert::ToByte(e->Value);
							 break;
							 // ChecksumType property should be set
						 case EProperty::ChecksumType:
							 lFrameEntry.ChecksumType = (Peak::Lin::TLINChecksumType)e->Value;
							 break;
						 }
						 lFrameEntry.Flags = Peak::Lin::PLinApi::FRAME_FLAG_RESPONSE_ENABLE;
						 lErr = Peak::Lin::PLinApi::SetFrameEntry(m_hClient, m_hHw, lFrameEntry);
						 // If an error occurs do not allow to change
						 // the property and return.
						 // The Allowed parameter was set some lines before.
						 if (lErr != Peak::Lin::TLINError::errOK)
							 return;

						 // Temporary set Allowed to true for next check.
						 // The action was successfull on this line.
						 e->Allowed = true;
					 }

					 // If the property 'Direction' of one
					 // CFrameDefinition will be changed,
					 // here we need a special request to set
					 // the client filter.
					 if (e->Type == EProperty::Direction)
					 {
						 // If the new value for the property 'Direction'
						 // should NOT be 'Disabled' check first if
						 // the CFrameDefinition is defined already with some
						 // other value then 'Disabled'.
						 if ((Peak::Lin::TLINDirection)e->Value != Peak::Lin::TLINDirection::dirDisabled)
						 {
							 if (lFD->Direction == Peak::Lin::TLINDirection::dirDisabled)
							 {
								 // If the old property of CFrameDefintion
								 // was set to 'Disabled' the new value
								 // means that the Frame-ID have to add to
								 // the client filter by the LinApi.
								 // Set the client filter.
								 // The Filter is a bit mask.
								 lMask = (UInt64)1 << lFD->IdAsInt;
								 m_lMask = m_lMask | lMask;
								 lErr = Peak::Lin::PLinApi::SetClientFilter(m_hClient, m_hHw, m_lMask);
								 // Only allow to change the property if the Frame-ID
								 // was added successfull to the Filter.
								 e->Allowed = lErr == Peak::Lin::TLINError::errOK;
							 }
						 }
						 else
						 {
							 // If the value of direction should set on 'disable'
							 // Remove the Frame-ID from the client filter.
							 lMask = (UInt64)1 << lFD->IdAsInt;
							 m_lMask = m_lMask & ~lMask;
							 lErr = Peak::Lin::PLinApi::SetClientFilter(m_hClient, m_hHw, m_lMask);
							 // Only allow to change the property if the Frame-ID
							 // was removed successfull from the Filter.
							 e->Allowed = lErr == Peak::Lin::TLINError::errOK;
						 }
					 }
				 }
			 }

#pragma endregion

#pragma region Help and gui functions

			 /// <summary>
			 /// Activates/deactivates the different controls of the main-form according
			 /// to the current connection status
			 /// </summary>
			 /// <param name="bConnected">Current status. True if connected, false otherwise</param>
			 void SetConnectionStatus(bool bConnected)
			 {
				 // Buttons
				 //
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
				 //
				 cbbBaudrates->Enabled = !bConnected;
				 cbbChannel->Enabled = !bConnected;
				 cbbHwMode->Enabled = !bConnected;

				 // Hardware configuration and read mode
				 //
				 if (!bConnected)
					 cbbChannel_SelectedIndexChanged(this, gcnew EventArgs());
				 else
					 rdbTimer_CheckedChanged(this, gcnew EventArgs());

				 // Display messages in grid
				 //
				 tmrDisplay->Enabled = bConnected;
			 }

			 /// <summary>
			 /// Includes a new line of text into the information Listview
			 /// </summary>
			 /// <param name="strMsg">Text to be included</param>
			 void IncludeTextMessage(String^ strMsg)
			 {
				 lbxInfo->Items->Add(strMsg);
				 lbxInfo->SelectedIndex = lbxInfo->Items->Count - 1;
			 }

			 /// <summary>
			 /// Help Function used to get an error as text
			 /// </summary>
			 /// <param name="error">Error code to be translated</param>
			 /// <returns>A text with the translated error</returns>
			 String^ GetFormatedError(Peak::Lin::TLINError error)
			 {
				 StringBuilder^ sErrText = gcnew StringBuilder(255);
				 // If any error are occured
				 // display the error text in a message box.
				 // 0x00 = Neutral
				 // 0x07 = Language German
				 // 0x09 = Language English
				 if (Peak::Lin::PLinApi::GetErrorText(error, 0x09, sErrText, 255) != Peak::Lin::TLINError::errOK)
					 return String::Format("An error occurred. Error-code's text ({0}) couldn't be retrieved", error);
				 return sErrText->ToString();
			 }

			 /// <summary>
			 /// Updates 'cbbID' comboBox with values from the global frame table
			 /// </summary>
			 void UpdateFrameIds()
			 {
				 ComboBoxItem^ lItem;
				 String^ lID;

				 // Retrieves selected ID if it exist
				 lItem = (ComboBoxItem^) cbbID->SelectedItem;
				 lID = (lItem != nullptr) ? lItem->Text : nullptr;

				 // Clears and populates FrameID combobox with global frame IDs
				 cbbID->Items->Clear();
				 for (int i = 0; i < m_pGFT->Count; i++)
				 {
					 // add only non disabled frames
					 if (m_pGFT[i]->Direction == Peak::Lin::TLINDirection::dirDisabled)
						 continue;
					 cbbID->Items->Add(gcnew ComboBoxItem(m_pGFT[i]->ID, m_pGFT[i]));
					 // check if the new item was selected before the update
					 if (lID == m_pGFT[i]->ID)
						 cbbID->SelectedIndex = cbbID->Items->Count - 1;
				 }
			 }
		
			

#pragma endregion

#pragma region Read/write functions

			 /// <summary>
			 /// Function for reading PLIN messages
			 /// </summary>
			 void ReadMessages()
			 {
				 Peak::Lin::TLINRcvMsg lpMsg;

				 // We read at least one time the queue looking for messages.
				 // If a message is found, we look again trying to find more.
				 // If the queue is empty or an error occurs, we get out from
				 // the dowhile statement.
				 //	
				 do
				 {
					 m_LastLINErr = Peak::Lin::PLinApi::Read(m_hClient, lpMsg);
					 // If at least one Frame is received by the LinApi.
					 // Check if the received frame is a standard type.
					 // If it is not a standard type than ignore it.
					 if (lpMsg.Type != Peak::Lin::TLINMsgType::mstStandard)
						 continue;

					 if (m_LastLINErr == Peak::Lin::TLINError::errOK)
						 ProcessMessage(lpMsg);

				 } while (btnRelease->Enabled && (!Convert::ToBoolean(m_LastLINErr & Peak::Lin::TLINError::errRcvQueueEmpty)));
			 }

			 /// <summary>
			 /// Processes a received message, in order to show it in the Message-ListView
			 /// </summary>
			 /// <param name="linMsg">The received PLIN message</param>
			 void ProcessMessage(Peak::Lin::TLINRcvMsg linMsg)
			 {
				 // We search if a message (Same ID and Type) has 
				 // already been received or if this is a new message
			     for each(MessageStatus^ msg in m_LastMsgsList)
			     {
			         if (msg->LINMsg.FrameId == linMsg.FrameId)
			         {
			             // Modify the message and exit
			             msg->Update(linMsg);
			             return;
			         }
			     }
			     // Message not found. It will be created
			     InsertMsgEntry(linMsg);
			 }

			 /// <summary>
			 /// Inserts a new entry for a new message in the Message-ListView
			 /// </summary>
			 /// <param name="newMsg">The messasge to be inserted</param>
			 void InsertMsgEntry(Peak::Lin::TLINRcvMsg newMsg)
			 {
				 MessageStatus^ msgStsCurrentMsg;
				 ListViewItem^ lviCurrentItem;
				 String^ strId;

			     // We add this status in the last message list
			     msgStsCurrentMsg = gcnew MessageStatus(newMsg, lstMessages->Items->Count);
			     m_LastMsgsList->Add(msgStsCurrentMsg);
			     
			     // Search and retrieve the ID in the global frame table associated with the frame Protected-ID
			     strId = "";
			     for (int i = 0; i < m_pGFT->Count; i++)
			     {
			         if (msgStsCurrentMsg->LINMsg.FrameId == m_pGFT[i]->ProtectedIdAsInt)
			         {
			             strId = m_pGFT[i]->ID;
			             break;
			         }

			     }
			     // Add the new ListView Item with the ID of the message
			     lviCurrentItem = lstMessages->Items->Add(strId);
			     // We set the length of the message
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->LINMsg.Length.ToString());
			     // We set the data of the message. 	
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->DataString);
			     // we set the message count message (this is the First, so count is 1)            
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->Count.ToString());
			     // Add time stamp information if needed
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->TimeString);
			     // We set the direction of the message
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->DirectionString);
			     // We set the error of the message
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->ErrorString);
			     // We set the CST of the message
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->CSTString);
			     // We set the CRC of the message
			     lviCurrentItem->SubItems->Add(msgStsCurrentMsg->ChecksumString);

			 }

			 /// <summary>
			 /// Displays and updates LIN messages in the Message-ListView
			 /// </summary>
			 void DisplayMessages()
			 {
				 ListViewItem^ lviCurrentItem;

			     for each(MessageStatus^ msgStatus in m_LastMsgsList)
			     {
			         // Get the data to actualize
			         //
			         if (msgStatus->MarkedAsUpdated)
			         {
			             msgStatus->MarkedAsUpdated = false;
			             lviCurrentItem = lstMessages->Items[msgStatus->Position];

			             lviCurrentItem->SubItems[1]->Text = msgStatus->LINMsg.Length.ToString();
			             lviCurrentItem->SubItems[2]->Text = msgStatus->DataString;
			             lviCurrentItem->SubItems[3]->Text = msgStatus->Count.ToString();
			             lviCurrentItem->SubItems[4]->Text = msgStatus->TimeString;
			             lviCurrentItem->SubItems[5]->Text = msgStatus->DirectionString;
			             lviCurrentItem->SubItems[6]->Text = msgStatus->ErrorString;
			             lviCurrentItem->SubItems[7]->Text = msgStatus->CSTString;
			             lviCurrentItem->SubItems[8]->Text = msgStatus->ChecksumString;
			         }
			     }
			 }

#pragma endregion

#pragma endregion

};

}

