object frmText: TfrmText
  Left = 331
  Top = 457
  Width = 302
  Height = 205
  BorderStyle = bsSizeToolWin
  Caption = 'Text'
  Color = 10528425
  Constraints.MinHeight = 205
  Constraints.MinWidth = 302
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
  object paBottomBar: TPanel
    Left = 0
    Top = 158
    Width = 294
    Height = 20
    Align = alBottom
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 2
      Top = 2
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 84
      Top = 2
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object mmText: TElAdvancedMemo
    Left = 0
    Top = 0
    Width = 294
    Height = 158
    Align = alClient
    Color = 10526880
    Lines.Strings = (
      '')
    MaxLength = -1
    ScrollBars = ssBoth
    TabOrder = 1
    OnChange = mmTextChange
    Flat = True
    FlatFocusedScrollBars = True
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
  end
  object fsStorage: TFormStorage
    IniSection = 'Text Form'
    RegistryRoot = prLocalMachine
    Version = 6
    StoredValues = <>
    Left = 65529
    Top = 65526
  end
end
