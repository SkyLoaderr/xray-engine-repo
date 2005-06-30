object frmText: TfrmText
  Left = -669
  Top = 527
  Width = 443
  Height = 283
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
  OnActivate = FormActivate
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnDeactivate = FormDeactivate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paBottomBar: TPanel
    Left = 0
    Top = 0
    Width = 435
    Height = 20
    Align = alTop
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 2
      Top = 1
      Width = 50
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 52
      Top = 1
      Width = 50
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
    object ebApply: TExtBtn
      Left = 107
      Top = 1
      Width = 50
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = '&Apply'
      FlatAlwaysEdge = True
      OnClick = ebApplyClick
    end
    object ebLoad: TExtBtn
      Left = 214
      Top = 1
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
      Left = 251
      Top = 1
      Width = 37
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = '&Save'
      FlatAlwaysEdge = True
      OnClick = ebSaveClick
    end
    object ebClear: TExtBtn
      Left = 288
      Top = 1
      Width = 37
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = '&Clear'
      FlatAlwaysEdge = True
      OnClick = ebClearClick
    end
  end
  object sbStatusPanel: TElStatusBar
    Left = 0
    Top = 237
    Width = 435
    Height = 19
    Panels = <
      item
        Alignment = taLeftJustify
        Width = 55
        IsHTML = False
      end
      item
        Alignment = taLeftJustify
        IsHTML = False
      end>
    SimplePanel = False
    SimpleTextIsHTML = False
    SizeGrip = False
    ResizablePanels = False
    Bevel = epbNone
    UseXPThemes = False
    Align = alBottom
    Color = clBtnFace
    ParentColor = False
    ParentShowHint = False
    ShowHint = True
    DockOrientation = doNoOrient
    DoubleBuffered = False
  end
  object mmText: TMemo
    Left = 0
    Top = 20
    Width = 435
    Height = 217
    Align = alClient
    Color = 10526880
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Console'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 2
    OnChange = mmTextChange
    OnKeyUp = mmTextKeyUp
  end
  object fsStorage: TFormStorage
    IniSection = 'Text Form'
    RegistryRoot = prLocalMachine
    Version = 6
    StoredValues = <>
    Left = 5
    Top = 6
  end
  object pmTextMenu: TMxPopupMenu
    Alignment = paCenter
    AutoHotkeys = maManual
    TrackButton = tbLeftButton
    MarginStartColor = 10921638
    MarginEndColor = 2763306
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 8
    Top = 34
  end
  object tmIdle: TTimer
    Interval = 250
    OnTimer = tmIdleTimer
    Left = 8
    Top = 64
  end
end
