object frmMain: TfrmMain
  Left = 253
  Top = 146
  Width = 700
  Height = 686
  Caption = 'Memory Dump'
  Color = 11581113
  Constraints.MinHeight = 550
  Constraints.MinWidth = 700
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
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
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 260
    Height = 621
    Align = alLeft
    BevelInner = bvLowered
    TabOrder = 1
    object Panel3: TPanel
      Left = 2
      Top = 362
      Width = 256
      Height = 1
      Align = alBottom
      BevelOuter = bvNone
      Color = clGray
      TabOrder = 0
    end
    object Panel4: TPanel
      Left = 2
      Top = 2
      Width = 256
      Height = 176
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
      object MxLabel2: TMxLabel
        Left = 8
        Top = 23
        Width = 40
        Height = 13
        Caption = 'End Ptr:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object lbEndPtr: TMxLabel
        Left = 59
        Top = 23
        Width = 8
        Height = 14
        Caption = '0'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object MxLabel4: TMxLabel
        Left = 8
        Top = 5
        Width = 48
        Height = 13
        Caption = 'Begin Ptr:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object lbBeginPtr: TMxLabel
        Left = 59
        Top = 5
        Width = 8
        Height = 14
        Caption = '0'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object MxLabel3: TMxLabel
        Left = 8
        Top = 41
        Width = 51
        Height = 13
        Caption = 'Mem Size:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object lbMemSize: TMxLabel
        Left = 59
        Top = 41
        Width = 8
        Height = 14
        Caption = '0'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object MxLabel5: TMxLabel
        Left = 126
        Top = 23
        Width = 55
        Height = 13
        Caption = 'Max Block:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object lbMaxBlock: TMxLabel
        Left = 180
        Top = 23
        Width = 8
        Height = 14
        Caption = '0'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object MxLabel7: TMxLabel
        Left = 126
        Top = 5
        Width = 52
        Height = 13
        Caption = 'Min Block:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object lbMinBlock: TMxLabel
        Left = 180
        Top = 5
        Width = 8
        Height = 14
        Caption = '0'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object rgCellSize: TRadioGroup
        Left = 8
        Top = 61
        Width = 241
        Height = 111
        Caption = ' Cell Size (bytes) '
        Columns = 2
        ItemIndex = 6
        Items.Strings = (
          '8'
          '16'
          '32'
          '64'
          '128'
          '256'
          '512'
          '1024'
          '2048'
          '4096'
          '8192'
          '16384')
        TabOrder = 0
        OnClick = rgCellSizeClick
      end
    end
    object lbDetDesc: TListBox
      Left = 2
      Top = 201
      Width = 256
      Height = 161
      Align = alClient
      BorderStyle = bsNone
      Color = clWhite
      ItemHeight = 13
      TabOrder = 2
    end
    object Panel8: TPanel
      Left = 2
      Top = 178
      Width = 256
      Height = 23
      Align = alTop
      BevelInner = bvLowered
      TabOrder = 3
      object MxLabel1: TMxLabel
        Left = 8
        Top = 5
        Width = 65
        Height = 13
        Caption = 'Current Mem:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
      object lbCurrentCell: TMxLabel
        Left = 77
        Top = 5
        Width = 8
        Height = 14
        Caption = '0'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        ShadowSize = 0
      end
    end
    object paDetMem: TMxPanel
      Left = 2
      Top = 363
      Width = 256
      Height = 256
      Cursor = crCross
      Align = alBottom
      BevelOuter = bvNone
      Color = 15790320
      TabOrder = 4
      OnPaint = paDetMemPaint
    end
  end
  object sbMem: TScrollBar
    Left = 676
    Top = 0
    Width = 16
    Height = 621
    Align = alRight
    Enabled = False
    Kind = sbVertical
    PageSize = 0
    TabOrder = 2
    OnChange = sbMemChange
  end
  object Panel2: TPanel
    Left = 675
    Top = 0
    Width = 1
    Height = 621
    Align = alRight
    BevelOuter = bvNone
    Caption = 'Panel2'
    Color = clGray
    TabOrder = 3
  end
  object Panel6: TPanel
    Left = 260
    Top = 0
    Width = 415
    Height = 621
    Align = alClient
    BevelOuter = bvNone
    Color = 11581113
    TabOrder = 4
    object paMemBase: TPanel
      Left = 0
      Top = 23
      Width = 415
      Height = 598
      Align = alClient
      BevelInner = bvLowered
      TabOrder = 0
      OnResize = paMemBaseResize
      object paMem: TMxPanel
        Left = 10
        Top = 2
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
    object Panel7: TPanel
      Left = 0
      Top = 0
      Width = 415
      Height = 23
      Align = alTop
      BevelInner = bvLowered
      TabOrder = 1
    end
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
end
