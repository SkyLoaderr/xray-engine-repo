object frmPropertiesRPoint: TfrmPropertiesRPoint
  Left = 395
  Top = 420
  BorderStyle = bsDialog
  Caption = 'RPoint properties'
  ClientHeight = 101
  ClientWidth = 186
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
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paBottom: TPanel
    Left = 0
    Top = 79
    Width = 186
    Height = 22
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 62
      Top = 3
      Width = 61
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
      Left = 123
      Top = 3
      Width = 61
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
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 186
    Height = 79
    Align = alClient
    TabOrder = 1
    object RxLabel2: TLabel
      Left = 6
      Top = 35
      Width = 44
      Height = 13
      Caption = 'Team ID:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object RxLabel1: TLabel
      Left = 6
      Top = 14
      Width = 31
      Height = 13
      Caption = 'Name:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object RxLabel3: TLabel
      Left = 6
      Top = 55
      Width = 43
      Height = 13
      Caption = 'Heading:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object seTeamID: TMultiObjSpinEdit
      Left = 56
      Top = 32
      Width = 126
      Height = 18
      LWSensitivity = 0.005
      ButtonKind = bkLightWave
      Decimal = 0
      MaxValue = 7
      ValueType = vtFloat
      AutoSize = False
      Color = 14671839
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnChange = OnModified
    end
    object edName: TEdit
      Left = 56
      Top = 11
      Width = 125
      Height = 18
      AutoSize = False
      Color = 14671839
      TabOrder = 1
      OnChange = OnModified
    end
    object seHeading: TMultiObjSpinEdit
      Left = 56
      Top = 53
      Width = 126
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 1
      Increment = 0.1
      MaxValue = 360
      ValueType = vtFloat
      AutoSize = False
      Color = 14671839
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnChange = OnModified
    end
  end
end
