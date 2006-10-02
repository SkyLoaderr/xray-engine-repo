object frmMain: TfrmMain
  Left = 6
  Top = 79
  Width = 1104
  Height = 787
  Caption = 'Memory Dump'
  Color = clBtnFace
  Constraints.MinHeight = 550
  Constraints.MinWidth = 700
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel3: TBevel
    Left = 834
    Top = 0
    Width = 2
    Height = 722
    Align = alRight
    Shape = bsLeftLine
    Style = bsRaised
  end
  object paMainDesc: TPanel
    Left = 836
    Top = 0
    Width = 260
    Height = 722
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object paInfo: TPanel
      Left = 0
      Top = 18
      Width = 260
      Height = 428
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
    end
    object paCaptionDesc: TPanel
      Left = 0
      Top = 0
      Width = 260
      Height = 18
      Align = alTop
      Alignment = taLeftJustify
      BevelInner = bvLowered
      Caption = ' Description'
      Color = clGray
      TabOrder = 1
    end
    object paMainDet: TPanel
      Left = 0
      Top = 446
      Width = 260
      Height = 276
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 2
      object Bevel6: TBevel
        Left = 258
        Top = 18
        Width = 2
        Height = 256
        Align = alRight
        Shape = bsRightLine
        Style = bsRaised
      end
      object Bevel7: TBevel
        Left = 0
        Top = 274
        Width = 260
        Height = 2
        Align = alBottom
        Shape = bsBottomLine
        Style = bsRaised
      end
      object paCaptionDet: TPanel
        Left = 0
        Top = 0
        Width = 260
        Height = 18
        Align = alTop
        Alignment = taLeftJustify
        BevelInner = bvLowered
        Caption = ' Detailed current cell (16384 bytes)'
        Color = clGray
        TabOrder = 0
      end
      object paDetMem: TMxPanel
        Left = 0
        Top = 18
        Width = 258
        Height = 256
        Cursor = crCross
        Align = alClient
        BevelOuter = bvNone
        Color = 15790320
        TabOrder = 1
        OnMouseMove = paDetMemMouseMove
        OnPaint = paDetMemPaint
      end
    end
  end
  object paMainMem: TPanel
    Left = 0
    Top = 0
    Width = 834
    Height = 722
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object Bevel2: TBevel
      Left = 816
      Top = 18
      Width = 2
      Height = 702
      Align = alRight
      Shape = bsRightLine
      Style = bsRaised
    end
    object Bevel4: TBevel
      Left = 0
      Top = 720
      Width = 834
      Height = 2
      Align = alBottom
      Shape = bsBottomLine
      Style = bsRaised
    end
    object Bevel1: TBevel
      Left = 0
      Top = 18
      Width = 2
      Height = 702
      Align = alLeft
      Shape = bsLeftLine
      Style = bsRaised
    end
    object paMemBase: TPanel
      Left = 2
      Top = 18
      Width = 814
      Height = 702
      Align = alClient
      BevelOuter = bvNone
      Color = 15790320
      TabOrder = 0
      OnResize = paMemBaseResize
      object paMem: TMxPanel
        Left = 10
        Top = 10
        Width = 375
        Height = 263
        Cursor = crCross
        BevelOuter = bvNone
        Color = 15790320
        TabOrder = 0
        OnMouseDown = paMemMouseDown
        OnMouseMove = paMemMouseMove
        OnPaint = paMemPaint
      end
    end
    object paCaptionMem: TPanel
      Left = 0
      Top = 0
      Width = 834
      Height = 18
      Align = alTop
      Alignment = taLeftJustify
      BevelInner = bvLowered
      Caption = ' Memory'
      Color = clGray
      TabOrder = 1
    end
    object sbMem: TScrollBar
      Left = 818
      Top = 18
      Width = 16
      Height = 702
      Align = alRight
      Ctl3D = True
      Kind = sbVertical
      PageSize = 0
      ParentCtl3D = False
      TabOrder = 2
      OnChange = sbMemChange
    end
  end
  object paStatusBar: TPanel
    Left = 0
    Top = 722
    Width = 1096
    Height = 19
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    object Panel1: TPanel
      Left = 0
      Top = 0
      Width = 1096
      Height = 2
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
    end
    object Panel4: TPanel
      Left = 337
      Top = 2
      Width = 2
      Height = 17
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 1
    end
    object Panel5: TPanel
      Left = 185
      Top = 2
      Width = 2
      Height = 17
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 2
    end
    object paStatus0: TPanel
      Left = 0
      Top = 2
      Width = 185
      Height = 17
      Align = alLeft
      Alignment = taLeftJustify
      BevelOuter = bvLowered
      TabOrder = 3
    end
    object paStatus1: TPanel
      Left = 339
      Top = 2
      Width = 757
      Height = 17
      Align = alClient
      Alignment = taLeftJustify
      BevelOuter = bvLowered
      TabOrder = 4
    end
    object Panel2: TPanel
      Left = 187
      Top = 2
      Width = 150
      Height = 17
      Align = alLeft
      Alignment = taLeftJustify
      BevelOuter = bvLowered
      TabOrder = 5
      object cgProgress: TCGauge
        Left = 1
        Top = 1
        Width = 148
        Height = 15
        Align = alClient
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Serif'
        Font.Style = []
        ForeColor = 5592405
        ParentFont = False
        Visible = False
      end
    end
  end
  object od: TOpenDialog
    DefaultExt = '*.dump'
    Filter = 'Memory Dump Files|*.dump'
    InitialDir = 'x:\'
    Left = 264
    Top = 8
  end
  object MainMenu1: TMainMenu
    Left = 296
    Top = 8
    object File1: TMenuItem
      Caption = 'File'
      object Open1: TMenuItem
        Caption = 'Open'
        OnClick = OpenClick
      end
      object Close1: TMenuItem
        Caption = 'Close'
        Enabled = False
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object ShowLog1: TMenuItem
        Caption = 'Show Log'
        OnClick = ShowLog1Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object Exit1: TMenuItem
        Caption = 'Exit'
      end
    end
    object Tools1: TMenuItem
      Caption = 'View'
      object Bypool1: TMenuItem
        Caption = 'Statistic By Pool'
        OnClick = Bypool1Click
      end
      object Bytype1: TMenuItem
        Caption = 'Statistic By Type'
        OnClick = Bytype1Click
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object AllItems1: TMenuItem
        Caption = 'View All Items'
        Enabled = False
        OnClick = AllItems1Click
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
      Enabled = False
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'main'
    UseRegistry = True
    Version = 1
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 328
    Top = 8
  end
end
