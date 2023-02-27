object Form1: TForm1
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'PLIN-API Example'
  ClientHeight = 543
  ClientWidth = 754
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Visible = True
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 738
    Height = 65
    Caption = 'Connection'
    TabOrder = 0
    object Label1: TLabel
      Left = 11
      Top = 16
      Width = 51
      Height = 13
      Caption = 'Hardware:'
    end
    object Label2: TLabel
      Left = 305
      Top = 16
      Width = 48
      Height = 13
      Caption = 'Baudrate:'
    end
    object Label3: TLabel
      Left = 407
      Top = 16
      Width = 30
      Height = 13
      Caption = 'Mode:'
    end
    object cbbChannel: TComboBox
      Left = 11
      Top = 35
      Width = 216
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = cbbChannelChange
    end
    object btnHwRefresh: TButton
      Left = 233
      Top = 33
      Width = 65
      Height = 25
      Caption = 'Refresh'
      TabOrder = 1
      OnClick = btnHwRefreshClick
    end
    object cbbBaudrates: TComboBox
      Left = 305
      Top = 35
      Width = 96
      Height = 21
      ItemHeight = 13
      TabOrder = 2
      Items.Strings = (
        '2400'
        '9600'
        '10400'
        '19200')
    end
    object cbbHwMode: TComboBox
      Left = 407
      Top = 35
      Width = 82
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 3
      OnChange = cbbHwModeChange
      Items.Strings = (
        'Slave'
        'Master')
    end
    object btnIdent: TButton
      Left = 515
      Top = 33
      Width = 65
      Height = 25
      Caption = 'Identify'
      TabOrder = 4
      OnClick = btnIdentClick
    end
    object btnInit: TButton
      Left = 599
      Top = 33
      Width = 65
      Height = 25
      Caption = 'Initialize'
      TabOrder = 5
      OnClick = btnInitClick
    end
    object btnRelease: TButton
      Left = 670
      Top = 33
      Width = 65
      Height = 25
      Caption = 'Release'
      Enabled = False
      TabOrder = 6
      OnClick = btnReleaseClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 79
    Width = 738
    Height = 58
    Caption = 'Configuration & Message Filtering'
    TabOrder = 1
    object Label4: TLabel
      Left = 443
      Top = 13
      Width = 28
      Height = 13
      Caption = 'From:'
    end
    object Label5: TLabel
      Left = 511
      Top = 13
      Width = 16
      Height = 13
      Caption = 'To:'
    end
    object btnConfigure: TButton
      Left = 11
      Top = 24
      Width = 145
      Height = 25
      Caption = 'Configure Global Frames'
      Enabled = False
      TabOrder = 0
      OnClick = btnConfigureClick
    end
    object rdbFilterOpen: TRadioButton
      Left = 200
      Top = 28
      Width = 57
      Height = 17
      Caption = 'Open'
      Checked = True
      TabOrder = 1
      TabStop = True
      OnClick = rdbFilterChanged
    end
    object rdbFilterClose: TRadioButton
      Left = 263
      Top = 28
      Width = 57
      Height = 17
      Caption = 'Close'
      TabOrder = 2
      OnClick = rdbFilterChanged
    end
    object rdbFilterCustom: TRadioButton
      Left = 326
      Top = 28
      Width = 111
      Height = 17
      Caption = 'Custom (expand)'
      TabOrder = 3
      OnClick = rdbFilterChanged
    end
    object txtFilterFrom: TEdit
      Left = 440
      Top = 26
      Width = 46
      Height = 21
      Enabled = False
      TabOrder = 4
      Text = '0'
      OnExit = txtFilterFromExit
      OnKeyPress = txtFilterKeyPress
    end
    object nudIdFrom: TUpDown
      Left = 486
      Top = 26
      Width = 16
      Height = 21
      Associate = txtFilterFrom
      Enabled = False
      Max = 63
      TabOrder = 5
    end
    object txtFilterTo: TEdit
      Left = 511
      Top = 26
      Width = 53
      Height = 21
      Enabled = False
      TabOrder = 6
      Text = '0'
      OnExit = txtFilterToExit
      OnKeyPress = txtFilterKeyPress
    end
    object nudIdTo: TUpDown
      Left = 564
      Top = 26
      Width = 16
      Height = 21
      Associate = txtFilterTo
      Enabled = False
      Max = 63
      TabOrder = 7
    end
    object btnFilterApply: TButton
      Left = 599
      Top = 22
      Width = 65
      Height = 25
      Caption = 'Apply'
      Enabled = False
      TabOrder = 8
      OnClick = btnFilterApplyClick
    end
    object btnFilterQuery: TButton
      Left = 670
      Top = 22
      Width = 65
      Height = 25
      Caption = 'Query'
      Enabled = False
      TabOrder = 9
      OnClick = btnFilterQueryClick
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 143
    Width = 738
    Height = 146
    Caption = 'Messages Reading'
    TabOrder = 2
    object rdbTimer: TRadioButton
      Left = 11
      Top = 16
      Width = 126
      Height = 17
      Caption = 'Reading using a Timer'
      Checked = True
      TabOrder = 0
      TabStop = True
      OnClick = rdbTimerChanged
    end
    object rdbManual: TRadioButton
      Left = 143
      Top = 16
      Width = 84
      Height = 17
      Caption = 'Manual Read'
      TabOrder = 1
      OnClick = rdbTimerChanged
    end
    object chbShowPeriod: TCheckBox
      Left = 233
      Top = 16
      Width = 120
      Height = 17
      Caption = 'Timestamp as period'
      Checked = True
      State = cbChecked
      TabOrder = 2
      OnClick = chbShowPeriodClick
    end
    object lstMessages: TListView
      Left = 11
      Top = 39
      Width = 569
      Height = 98
      Columns = <
        item
          Caption = 'ID'
          Width = 30
        end
        item
          Caption = 'Length'
        end
        item
          Caption = 'Data'
          Width = 130
        end
        item
          Caption = 'Count'
        end
        item
          Caption = 'Rcv Time'
          Width = 70
        end
        item
          Caption = 'Direction'
        end
        item
          Caption = 'Errors'
        end
        item
          Caption = 'CST'
        end
        item
          Caption = 'Checksum'
        end>
      ReadOnly = True
      RowSelect = True
      TabOrder = 3
      ViewStyle = vsReport
    end
    object btnRead: TButton
      Left = 599
      Top = 39
      Width = 65
      Height = 25
      Caption = 'Read'
      Enabled = False
      TabOrder = 4
      OnClick = btnReadClick
    end
    object btnMsgClear: TButton
      Left = 670
      Top = 39
      Width = 65
      Height = 25
      Caption = 'Clear'
      TabOrder = 5
      OnClick = btnMsgClearClick
    end
  end
  object GroupBox4: TGroupBox
    Left = 8
    Top = 295
    Width = 738
    Height = 114
    Caption = 'Write Messages'
    TabOrder = 3
    object Label6: TLabel
      Left = 11
      Top = 16
      Width = 45
      Height = 13
      Caption = 'ID (Hex):'
    end
    object Label7: TLabel
      Left = 126
      Top = 16
      Width = 51
      Height = 13
      Caption = 'PID (Hex):'
    end
    object Label8: TLabel
      Left = 203
      Top = 16
      Width = 46
      Height = 13
      Caption = 'Direction:'
    end
    object Label9: TLabel
      Left = 341
      Top = 16
      Width = 79
      Height = 13
      Caption = 'Checksum Type:'
    end
    object Label10: TLabel
      Left = 11
      Top = 62
      Width = 37
      Height = 13
      Caption = 'Length:'
    end
    object Label11: TLabel
      Left = 126
      Top = 62
      Width = 58
      Height = 13
      Caption = 'Data (1..8):'
    end
    object cbbID: TComboBox
      Left = 11
      Top = 35
      Width = 86
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = cbbIDChange
    end
    object txtID: TEdit
      Left = 126
      Top = 35
      Width = 71
      Height = 21
      Enabled = False
      TabOrder = 1
      Text = 'txtID'
      OnKeyPress = txtData0KeyPress
    end
    object cbbDirection: TComboBox
      Left = 203
      Top = 35
      Width = 132
      Height = 21
      Style = csDropDownList
      Enabled = False
      ItemHeight = 13
      TabOrder = 2
    end
    object cbbCST: TComboBox
      Left = 341
      Top = 35
      Width = 123
      Height = 21
      Style = csDropDownList
      Enabled = False
      ItemHeight = 13
      TabOrder = 3
    end
    object txtLength: TEdit
      Left = 11
      Top = 81
      Width = 68
      Height = 21
      Enabled = False
      TabOrder = 4
      Text = '1'
    end
    object nudLength: TUpDown
      Left = 79
      Top = 81
      Width = 16
      Height = 21
      Associate = txtLength
      Enabled = False
      Min = 1
      Max = 8
      Position = 1
      TabOrder = 5
      OnClick = nudLengthClick
    end
    object txtData0: TEdit
      Left = 126
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 6
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object txtData1: TEdit
      Left = 169
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 7
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object txtData2: TEdit
      Left = 212
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 8
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object txtData3: TEdit
      Left = 255
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 9
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object txtData4: TEdit
      Left = 298
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 10
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object txtData5: TEdit
      Left = 341
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 11
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object txtData6: TEdit
      Left = 384
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 12
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object txtData7: TEdit
      Left = 427
      Top = 81
      Width = 37
      Height = 21
      Enabled = False
      MaxLength = 2
      TabOrder = 13
      Text = '00'
      OnExit = txtData0Exit
      OnKeyPress = txtData0KeyPress
    end
    object btnWrite: TButton
      Left = 670
      Top = 32
      Width = 65
      Height = 25
      Caption = 'Write'
      Enabled = False
      TabOrder = 14
      OnClick = btnWriteClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 8
    Top = 415
    Width = 738
    Height = 120
    Caption = 'Information'
    TabOrder = 4
    object btnGetVersions: TButton
      Left = 599
      Top = 21
      Width = 65
      Height = 25
      Caption = 'Version'
      Enabled = False
      TabOrder = 0
      OnClick = btnGetVersionsClick
    end
    object btnInfoClear: TButton
      Left = 670
      Top = 21
      Width = 65
      Height = 25
      Caption = 'Clear'
      TabOrder = 1
      OnClick = btnInfoClearClick
    end
    object btnStatus: TButton
      Left = 599
      Top = 52
      Width = 65
      Height = 25
      Caption = 'Status'
      Enabled = False
      TabOrder = 2
      OnClick = btnStatusClick
    end
    object btnReset: TButton
      Left = 670
      Top = 52
      Width = 65
      Height = 25
      Caption = 'Reset'
      Enabled = False
      TabOrder = 3
      OnClick = btnResetClick
    end
    object lbxInfo: TListBox
      Left = 11
      Top = 21
      Width = 572
      Height = 93
      ItemHeight = 13
      Items.Strings = (
        
          'Select a Hardware and a configuration for it. Then click "Initia' +
          'lize" button')
      TabOrder = 4
    end
  end
  object tmrDisplay: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrDisplayTimer
    Left = 696
    Top = 256
  end
  object tmrRead: TTimer
    Enabled = False
    Interval = 50
    OnTimer = tmrReadTimer
    Left = 656
    Top = 256
  end
end
