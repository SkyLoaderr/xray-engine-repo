object frmMain: TfrmMain
  Left = 291
  Top = 132
  BorderStyle = bsSingle
  ClientHeight = 602
  ClientWidth = 804
  Color = clBtnFace
  Constraints.MinHeight = 480
  Constraints.MinWidth = 640
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paLeftBar: TPanel
    Left = 664
    Top = 0
    Width = 140
    Height = 585
    Align = alRight
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 140
    Constraints.MinWidth = 140
    TabOrder = 0
    object paTools: TPanel
      Left = 1
      Top = 1
      Width = 138
      Height = 16
      Align = alTop
      TabOrder = 0
      object APHeadLabel2: TLabel
        Left = 1
        Top = 1
        Width = 136
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Toolbar'
        Color = clGray
        ParentColor = False
        OnClick = TopClick
      end
      object sbToolsMin: TExtBtn
        Left = 124
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
    Top = 585
    Width = 804
    Height = 17
    Align = alBottom
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 1
  end
  object paMain: TPanel
    Left = 0
    Top = 0
    Width = 664
    Height = 585
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object paTopBar: TPanel
      Left = 0
      Top = 0
      Width = 664
      Height = 18
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
    end
    object paWindow: TPanel
      Left = 0
      Top = 18
      Width = 664
      Height = 567
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
        OnMouseDown = D3DWindowMouseDown
        OnMouseMove = D3DWindowMouseMove
        OnMouseUp = D3DWindowMouseUp
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
      'paLeftBar.Align'
      'paLeftBar.Height'
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
        OnClick = miHideSelectedClick
      end
      object HideUnselected2: TMenuItem
        Caption = 'Hide Unselected'
        OnClick = HideUnselected2Click
      end
      object HideAll2: TMenuItem
        Caption = 'Hide All'
        OnClick = miHideAllClick
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object UnhideAll2: TMenuItem
        Caption = 'Unhide All'
        OnClick = miUnhideAllClick
      end
    end
    object Locking1: TMenuItem
      Caption = 'Locking'
      GroupIndex = 1
      object LockSelected1: TMenuItem
        Caption = 'Lock Selected'
        OnClick = LockSelected1Click
      end
      object LockUnselected1: TMenuItem
        Caption = 'Lock Unselected'
        OnClick = LockUnselected1Click
      end
      object LockAll1: TMenuItem
        Caption = 'Lock All'
        OnClick = LockAll1Click
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object UnlockSelected1: TMenuItem
        Caption = 'Unlock Selected'
        OnClick = UnlockSelected1Click
      end
      object UnlockUnselected1: TMenuItem
        Caption = 'Unlock Unselected'
        OnClick = UnlockUnselected1Click
      end
      object UnlockAll1: TMenuItem
        Caption = 'Unlock All'
        OnClick = UnlockAll1Click
      end
    end
    object Edit1: TMenuItem
      Caption = 'Edit'
      GroupIndex = 1
      object miCopy: TMenuItem
        Caption = 'Copy (selection)'
        GroupIndex = 1
        OnClick = miCopyClick
      end
      object miPaste: TMenuItem
        Caption = 'Paste (selection)'
        Enabled = False
        GroupIndex = 1
        OnClick = miPasteClick
      end
      object miCut: TMenuItem
        Caption = 'Cut (selection)'
        GroupIndex = 1
        OnClick = miCutClick
      end
    end
    object Numeric1: TMenuItem
      Caption = 'Numeric Set'
      GroupIndex = 1
      object Position1: TMenuItem
        Caption = 'Position'
        OnClick = Position1Click
      end
      object Rotation1: TMenuItem
        Caption = 'Rotation'
        OnClick = Rotation1Click
      end
      object Scale1: TMenuItem
        Caption = 'Scale'
        OnClick = Scale1Click
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
