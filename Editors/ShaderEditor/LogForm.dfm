object frmLog: TfrmLog
  Left = 664
  Top = 783
  Width = 400
  Height = 83
  BorderStyle = bsSizeToolWin
  Caption = 'Log'
  Color = clBtnFace
  Constraints.MinHeight = 80
  Constraints.MinWidth = 400
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 36
    Width = 392
    Height = 20
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object ebClear: TExtBtn
      Left = 104
      Top = 2
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Clear'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebClearClick
    end
    object ebClearSelected: TExtBtn
      Left = 186
      Top = 2
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Clear Selected'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebClearSelectedClick
    end
    object ebClose: TExtBtn
      Left = 0
      Top = 2
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Close'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCloseClick
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 392
    Height = 36
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object lbLog: TListBox
      Left = 0
      Top = 0
      Width = 392
      Height = 36
      Style = lbOwnerDrawFixed
      Align = alClient
      Color = 15263976
      ItemHeight = 13
      MultiSelect = True
      TabOrder = 0
      OnDrawItem = lbLogDrawItem
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Log Form'
    Version = 1
    StoredValues = <>
    Left = 8
    Top = 8
  end
end
