object frmText: TfrmText
  Left = 356
  Top = 323
  Width = 374
  Height = 286
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
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paBottomBar: TPanel
    Left = 0
    Top = 239
    Width = 366
    Height = 20
    Align = alBottom
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 2
      Top = 2
      Width = 62
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 65
      Top = 2
      Width = 62
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
    object ebApply: TExtBtn
      Left = 133
      Top = 2
      Width = 62
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = '&Apply'
      FlatAlwaysEdge = True
      OnClick = ebApplyClick
    end
    object ebLoad: TExtBtn
      Left = 197
      Top = 2
      Width = 37
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = '&Load'
      FlatAlwaysEdge = True
      OnClick = ebLoadClick
    end
    object ebSave: TExtBtn
      Left = 234
      Top = 2
      Width = 37
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = '&Save'
      FlatAlwaysEdge = True
      OnClick = ebSaveClick
    end
  end
  object mmText: TElAdvancedMemo
    Left = 0
    Top = 0
    Width = 366
    Height = 239
    Align = alClient
    Color = 10526880
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    MaxLength = -1
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 1
    WantTabs = True
    OnChange = mmTextChange
    Flat = True
    FlatFocusedScrollBars = True
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
