object frmPropertiesSceneObject: TfrmPropertiesSceneObject
  Left = 373
  Top = 358
  BorderStyle = bsDialog
  Caption = 'Object properties'
  ClientHeight = 81
  ClientWidth = 210
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
    Top = 59
    Width = 210
    Height = 22
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 87
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
      Left = 148
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
    Width = 210
    Height = 59
    Align = alClient
    TabOrder = 1
    object RxLabel2: TLabel
      Left = 6
      Top = 35
      Width = 53
      Height = 13
      Caption = 'Reference:'
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
    object ebReference: TExtBtn
      Left = 64
      Top = 34
      Width = 142
      Height = 16
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '...'
      Margin = 0
      Transparent = False
      OnClick = ebReferenceClick
    end
    object edName: TEdit
      Left = 64
      Top = 11
      Width = 142
      Height = 18
      AutoSize = False
      Color = 14671839
      TabOrder = 0
      OnChange = OnModified
    end
  end
end
