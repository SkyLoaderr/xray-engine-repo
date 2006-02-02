object frmMain: TfrmMain
  Left = 303
  Top = 98
  Width = 831
  Height = 833
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
    Left = 561
    Top = 0
    Width = 2
    Height = 768
    Align = alRight
    Shape = bsLeftLine
    Style = bsRaised
  end
  object sbStatus: TStatusBar
    Left = 0
    Top = 768
    Width = 823
    Height = 19
    Panels = <
      item
        Width = 150
      end
      item
        Width = 4096
      end>
    SimplePanel = False
  end
  object paMainDesc: TPanel
    Left = 563
    Top = 0
    Width = 260
    Height = 768
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    object paInfo: TPanel
      Left = 0
      Top = 18
      Width = 260
      Height = 474
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
      Top = 492
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
    Width = 561
    Height = 768
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object Bevel2: TBevel
      Left = 543
      Top = 18
      Width = 2
      Height = 748
      Align = alRight
      Shape = bsRightLine
      Style = bsRaised
    end
    object Bevel4: TBevel
      Left = 0
      Top = 766
      Width = 561
      Height = 2
      Align = alBottom
      Shape = bsBottomLine
      Style = bsRaised
    end
    object Bevel1: TBevel
      Left = 0
      Top = 18
      Width = 2
      Height = 748
      Align = alLeft
      Shape = bsLeftLine
      Style = bsRaised
    end
    object paMemBase: TPanel
      Left = 2
      Top = 18
      Width = 541
      Height = 748
      Align = alClient
      BevelOuter = bvNone
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
      Width = 561
      Height = 18
      Align = alTop
      Alignment = taLeftJustify
      BevelInner = bvLowered
      Caption = ' Memory'
      Color = clGray
      TabOrder = 1
    end
    object sbMem: TScrollBar
      Left = 545
      Top = 18
      Width = 16
      Height = 748
      Align = alRight
      Ctl3D = True
      Kind = sbVertical
      PageSize = 0
      ParentCtl3D = False
      TabOrder = 2
      OnChange = sbMemChange
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
      Caption = 'Tools'
      object Statistic1: TMenuItem
        Caption = 'Show statistic'
        object Bytype1: TMenuItem
          Caption = 'By type'
          OnClick = Bytype1Click
        end
        object Bypool1: TMenuItem
          Caption = 'By pool'
          OnClick = Bypool1Click
        end
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
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
