object frmMain: TfrmMain
  Left = 280
  Top = 289
  Width = 660
  Height = 446
  Color = clBtnFace
  Constraints.MinHeight = 446
  Constraints.MinWidth = 660
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDeactivate = FormDeactivate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paLeftBar: TPanel
    Left = 442
    Top = 0
    Width = 210
    Height = 402
    Align = alRight
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 210
    Constraints.MinWidth = 210
    TabOrder = 0
    object paTools: TPanel
      Left = 1
      Top = 1
      Width = 208
      Height = 16
      Align = alTop
      TabOrder = 0
      object APHeadLabel2: TLabel
        Left = 1
        Top = 1
        Width = 206
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Toolbar'
        Color = clGray
        ParentColor = False
        OnClick = TopClick
      end
      object sbToolsMin: TExtBtn
        Left = 195
        Top = 2
        Width = 11
        Height = 11
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          DE000000424DDE00000000000000360000002800000007000000070000000100
          180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFF000000FFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFF0000
          00000000FFFFFF000000FFFFFFFFFFFFFFFFFF000000000000000000FFFFFF00
          0000FFFFFFFFFFFF000000000000000000000000FFFFFF000000FFFFFFFFFFFF
          FFFFFF000000000000000000FFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFF0000
          00000000FFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFF00
          0000}
        OnClick = sbToolsMinClick
      end
    end
  end
  object paBottomBar: TPanel
    Left = 0
    Top = 402
    Width = 652
    Height = 17
    Align = alBottom
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 1
  end
  object paMain: TPanel
    Left = 0
    Top = 0
    Width = 442
    Height = 402
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object paTopBar: TPanel
      Left = 0
      Top = 0
      Width = 442
      Height = 18
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
    end
    object paWindow: TPanel
      Left = 0
      Top = 18
      Width = 442
      Height = 384
      Align = alClient
      BevelOuter = bvNone
      Color = clGray
      TabOrder = 1
      OnResize = paWindowResize
      object D3DWindow: TD3DWindow
        Left = 1
        Top = 1
        Width = 858
        Height = 712
        OnChangeFocus = D3DWindowChangeFocus
        BorderStyle = bsNone
        Color = 5592405
        TabOrder = 0
        OnKeyDown = D3DWindowKeyDown
        OnKeyPress = D3DWindowKeyPress
        OnKeyUp = D3DWindowKeyUp
        OnResize = D3DWindowResize
        OnPaint = D3DWindowPaint
      end
    end
  end
  object fsMainForm: TFormStorage
    IniSection = 'Main Form'
    Options = [fpState]
    RegistryRoot = prLocalMachine
    OnRestorePlacement = fsMainFormRestorePlacement
    StoredProps.Strings = (
      'paLeftBar.Tag')
    StoredValues = <>
    Left = 185
    Top = 33
  end
  object pmObjectContext: TRxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    MenuAnimation = [maTopToBottom]
    LeftMargin = 15
    Style = msBtnLowered
    Left = 157
    Top = 33
    object miVisibility: TMenuItem
      Caption = 'Visibility'
      GroupIndex = 1
      object HideSelected2: TMenuItem
        Caption = 'Hide Selected'
      end
      object HideUnselected2: TMenuItem
        Caption = 'Hide Unselected'
      end
      object HideAll2: TMenuItem
        Caption = 'Hide All'
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object UnhideAll2: TMenuItem
        Caption = 'Unhide All'
      end
    end
    object Locking1: TMenuItem
      Caption = 'Locking'
      GroupIndex = 1
      object LockSelected1: TMenuItem
        Caption = 'Lock Selected'
      end
      object LockUnselected1: TMenuItem
        Caption = 'Lock Unselected'
      end
      object LockAll1: TMenuItem
        Caption = 'Lock All'
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object UnlockSelected1: TMenuItem
        Caption = 'Unlock Selected'
      end
      object UnlockUnselected1: TMenuItem
        Caption = 'Unlock Unselected'
      end
      object UnlockAll1: TMenuItem
        Caption = 'Unlock All'
      end
    end
    object Edit1: TMenuItem
      Caption = 'Edit'
      GroupIndex = 1
      object miCopy: TMenuItem
        Caption = 'Copy (selection)'
        GroupIndex = 1
      end
      object miPaste: TMenuItem
        Caption = 'Paste (selection)'
        Enabled = False
        GroupIndex = 1
      end
      object miCut: TMenuItem
        Caption = 'Cut (selection)'
        GroupIndex = 1
      end
    end
    object Numeric1: TMenuItem
      Caption = 'Numeric Set'
      GroupIndex = 1
      object Position1: TMenuItem
        Caption = 'Position'
      end
      object Rotation1: TMenuItem
        Caption = 'Rotation'
      end
      object Scale1: TMenuItem
        Caption = 'Scale'
      end
    end
    object N4: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object miProperties: TMenuItem
      Caption = 'Properties...'
      GroupIndex = 1
      OnClick = miPropertiesClick
    end
  end
  object tmRefresh: TTimer
    Enabled = False
    OnTimer = tmRefreshTimer
    Left = 129
    Top = 33
  end
end
