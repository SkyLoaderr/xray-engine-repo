object frmPropertiesSector: TfrmPropertiesSector
  Left = 808
  Top = 484
  BorderStyle = bsDialog
  Caption = 'Sector properties'
  ClientHeight = 96
  ClientWidth = 275
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paBottom: TPanel
    Left = 0
    Top = 77
    Width = 275
    Height = 19
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 118
      Top = 1
      Width = 78
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 197
      Top = 1
      Width = 78
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object gbSector: TGroupBox
    Left = 0
    Top = 0
    Width = 275
    Height = 77
    Align = alClient
    Caption = ' Sector '
    TabOrder = 1
    object RxLabel5: TLabel
      Left = 5
      Top = 35
      Width = 63
      Height = 13
      Caption = 'Display color:'
    end
    object mcSectorColor: TMultiObjColor
      Left = 77
      Top = 34
      Width = 89
      Height = 18
      Shape = stRoundRect
      OnMouseDown = mcSectorColorMouseDown
    end
    object Bevel2: TBevel
      Left = 77
      Top = 14
      Width = 194
      Height = 17
    end
    object RxLabel1: TLabel
      Left = 6
      Top = 16
      Width = 31
      Height = 13
      Caption = 'Name:'
    end
    object Label1: TLabel
      Left = 5
      Top = 56
      Width = 44
      Height = 13
      Caption = 'Contains:'
    end
    object lbContains: TLabel
      Left = 76
      Top = 56
      Width = 9
      Height = 13
      Caption = '...'
    end
    object edName: TEdit
      Left = 78
      Top = 15
      Width = 192
      Height = 15
      AutoSize = False
      BorderStyle = bsNone
      Color = 15263976
      MaxLength = 32
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnChange = OnModified
    end
  end
end
