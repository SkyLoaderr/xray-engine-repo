object frmMain: TfrmMain
  Left = 265
  Top = 199
  Width = 770
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
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paLeftBar: TPanel
    Left = 544
    Top = 0
    Width = 218
    Height = 402
    Align = alRight
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 218
    Constraints.MinWidth = 218
    TabOrder = 0
    object paTools: TPanel
      Left = 1
      Top = 1
      Width = 216
      Height = 16
      Align = alTop
      TabOrder = 0
      object APHeadLabel2: TLabel
        Left = 1
        Top = 1
        Width = 214
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Toolbar'
        Color = clGray
        ParentColor = False
        OnClick = TopClick
      end
      object sbToolsMin: TExtBtn
        Left = 203
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
      object ebAllMin: TExtBtn
        Left = 3
        Top = 2
        Width = 11
        Height = 11
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          AE000000424DAE00000000000000360000002800000006000000060000000100
          18000000000078000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          00000B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B00000B0B0B0B0B0B0B0B0B0B
          0B0B0B0B0B0B0B0B0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        OnClick = ebAllMinClick
      end
      object ebAllMax: TExtBtn
        Left = 15
        Top = 2
        Width = 11
        Height = 11
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          AE000000424DAE00000000000000360000002800000006000000060000000100
          18000000000078000000120B0000120B00000000000000000000FFFFFFFFFFFF
          1C1C1C1C1C1CFFFFFFFFFFFF0000FFFFFFFFFFFF191919191919FFFFFFFFFFFF
          00001B1B1B1717170303030303031717171B1B1B00001B1B1B17171703030303
          03031717171B1B1B0000FFFFFFFFFFFF191919191919FFFFFFFFFFFF0000FFFF
          FFFFFFFF1C1C1C1C1C1CFFFFFFFFFFFF0000}
        OnClick = ebAllMaxClick
      end
    end
  end
  object paBottomBar: TPanel
    Left = 0
    Top = 402
    Width = 762
    Height = 17
    Align = alBottom
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 1
  end
  object paMain: TPanel
    Left = 0
    Top = 0
    Width = 544
    Height = 402
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object paTopBar: TPanel
      Left = 0
      Top = 0
      Width = 544
      Height = 18
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
    end
    object paWindow: TPanel
      Left = 0
      Top = 18
      Width = 544
      Height = 384
      Align = alClient
      BevelOuter = bvNone
      Color = 4868682
      TabOrder = 1
      OnResize = paWindowResize
      object D3DWindow: TD3DWindow
        Left = 22
        Top = 22
        Width = 339
        Height = 259
        FocusedColor = 9502719
        UnfocusedColor = 9211020
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
  object fsStorage: TFormStorage
    IniSection = 'Main Form'
    RegistryRoot = prLocalMachine
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paLeftBar.Tag')
    StoredValues = <>
    Left = 161
    Top = 33
  end
  object tmRefresh: TTimer
    Enabled = False
    OnTimer = tmRefreshTimer
    Left = 129
    Top = 33
  end
end
