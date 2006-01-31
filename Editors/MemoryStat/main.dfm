object frmMain: TfrmMain
  Left = 480
  Top = 235
  Width = 700
  Height = 686
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
  object sbStatus: TStatusBar
    Left = 0
    Top = 621
    Width = 692
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
  object paDesc: TPanel
    Left = 436
    Top = 0
    Width = 256
    Height = 621
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    object paDetMem: TMxPanel
      Left = 0
      Top = 365
      Width = 256
      Height = 256
      Cursor = crCross
      Align = alBottom
      BevelOuter = bvNone
      Color = 15790320
      TabOrder = 0
      OnPaint = paDetMemPaint
    end
    object paInfo: TPanel
      Left = 0
      Top = 18
      Width = 256
      Height = 329
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
    end
    object Panel3: TPanel
      Left = 0
      Top = 347
      Width = 256
      Height = 18
      Align = alBottom
      Alignment = taLeftJustify
      BevelInner = bvLowered
      Caption = ' Detailed current cell (16384 bytes)'
      Color = clGray
      TabOrder = 2
    end
    object Panel9: TPanel
      Left = 0
      Top = 0
      Width = 256
      Height = 18
      Align = alTop
      Alignment = taLeftJustify
      BevelInner = bvLowered
      Caption = ' Description'
      Color = clGray
      TabOrder = 3
    end
  end
  object Panel6: TPanel
    Left = 0
    Top = 0
    Width = 435
    Height = 621
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object paMemBase: TPanel
      Left = 0
      Top = 18
      Width = 419
      Height = 603
      Align = alClient
      BevelInner = bvLowered
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
    object sbMem: TScrollBar
      Left = 419
      Top = 18
      Width = 16
      Height = 603
      Align = alRight
      Enabled = False
      Kind = sbVertical
      PageSize = 0
      TabOrder = 1
      OnChange = sbMemChange
    end
    object Panel4: TPanel
      Left = 0
      Top = 0
      Width = 435
      Height = 18
      Align = alTop
      Alignment = taLeftJustify
      BevelInner = bvLowered
      Caption = ' Memory'
      Color = clGray
      TabOrder = 2
    end
  end
  object Panel2: TPanel
    Left = 435
    Top = 0
    Width = 1
    Height = 621
    Align = alRight
    BevelOuter = bvNone
    Color = clWhite
    TabOrder = 3
  end
  object od: TOpenDialog
    DefaultExt = '*.txt'
    Filter = '$memory_dump$|*.txt'
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
      object N1: TMenuItem
        Caption = '-'
      end
      object Exit1: TMenuItem
        Caption = 'Exit'
      end
    end
    object Tools1: TMenuItem
      Caption = 'Tools'
    end
    object Help1: TMenuItem
      Caption = 'Help'
    end
  end
  object fsStorage: TFormStorage
    Version = 1
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 328
    Top = 8
  end
end
