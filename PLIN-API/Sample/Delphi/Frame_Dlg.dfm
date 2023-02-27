object FrameDlg: TFrameDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Global Frame Table'
  ClientHeight = 309
  ClientWidth = 645
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  DesignSize = (
    645
    309)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 95
    Height = 13
    Caption = 'Global Frame Table:'
  end
  object Label2: TLabel
    Left = 431
    Top = 8
    Width = 53
    Height = 13
    Caption = 'Properties:'
  end
  object lvGFT: TListView
    Left = 8
    Top = 27
    Width = 417
    Height = 274
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Caption = 'ID'
        Width = 41
      end
      item
        Caption = 'PID'
        Width = 38
      end
      item
        Caption = 'Direction'
        Width = 124
      end
      item
        Caption = 'Length'
      end
      item
        Caption = 'CST'
        Width = 100
      end>
    MultiSelect = True
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnKeyDown = lvGFTKeyDown
    OnSelectItem = lvGFTSelectItem
  end
  object pgGFTDef: TValueListEditor
    Left = 431
    Top = 27
    Width = 206
    Height = 274
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 1
    OnStringsChange = pgGFTDefStringsChange
    ColWidths = (
      80
      120)
  end
end
