object fraPortal: TfraPortal
  Left = 0
  Top = 0
  Width = 123
  Height = 277
  VertScrollBar.ButtonSize = 11
  VertScrollBar.Range = 257
  VertScrollBar.Smooth = True
  VertScrollBar.Style = ssFlat
  VertScrollBar.Visible = False
  Align = alClient
  AutoScroll = False
  Constraints.MaxWidth = 123
  Constraints.MinWidth = 123
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentFont = False
  TabOrder = 0
  object paCompute: TPanel
    Left = 0
    Top = 90
    Width = 123
    Height = 132
    Align = alTop
    ParentColor = True
    TabOrder = 1
    Visible = False
    object ebCompute: TExtBtn
      Left = 2
      Top = 31
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Compute Sel. Portals'
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      OnClick = ebComputeClick
    end
    object RxLabel1: TRxLabel
      Left = 3
      Top = 62
      Width = 63
      Height = 13
      Caption = 'Sector Front:'
      ShadowColor = 15263976
    end
    object RxLabel2: TRxLabel
      Left = 3
      Top = 96
      Width = 64
      Height = 13
      Caption = 'Sector Back:'
      ShadowColor = 15263976
    end
    object ebPickSectors: TExtBtn
      Left = 2
      Top = 46
      Width = 120
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Caption = 'Pick F/B sectors'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        6E040000424D6E04000000000000360000002800000028000000090000000100
        18000000000038040000120B0000120B00000000000000000000FF00FF000000
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FFFF00FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF0000009999999999999999
        99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
        9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
        999999999999C4C4C4FF00FFC4C4C40000000000000000000000000000000000
        00000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
        9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
        C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
        C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
        00FFFF00FF000000999999999999999999999999999999999999C4C4C4FF00FF
        FF00FF000000999999999999999999999999999999999999C4C4C4FF00FFFF00
        FF000000999999999999999999999999999999999999C4C4C4FF00FFC4C4C499
        9999999999999999999999999999999999000000FF00FFFF00FFFF00FF000000
        999999999999999999999999999999999999C4C4C4FF00FFFF00FF0000009999
        99999999999999999999999999999999C4C4C4FF00FFFF00FF00000099999999
        9999999999999999999999999999C4C4C4FF00FFC4C4C4999999999999999999
        999999999999999999000000FF00FFFF00FFFF00FF0000009999999999999999
        99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
        9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
        999999999999C4C4C4FF00FFC4C4C49999999999999999999999999999999999
        99000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
        9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
        C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
        C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
        00FFFF00FF000000000000000000000000000000000000000000000000FF00FF
        FF00FF000000000000000000000000000000000000000000000000FF00FFFF00
        FF000000000000000000000000000000000000000000000000FF00FFC4C4C499
        9999999999999999999999999999999999000000FF00FFFF00FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC4C4C4C4C4C4C4C4C4C4C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF}
      Margin = 2
      NumGlyphs = 4
      ParentFont = False
      Spacing = 10
      Transparent = False
      OnClick = ebPickSectorsClick
    end
    object ebComputeAllPortals: TExtBtn
      Left = 2
      Top = 16
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Compute All Portals'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      OnClick = ebComputeAllPortalsClick
    end
    object APHeadLabel2: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Add'
      Color = clGray
      ParentColor = False
      OnClick = TopClick
    end
    object ExtBtn1: TExtBtn
      Left = 109
      Top = 2
      Width = 11
      Height = 11
      Align = alNone
      CloseButton = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        DE000000424DDE00000000000000360000002800000007000000070000000100
        180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
        0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
        000000000000000000000000FFFFFF0000000000000000000000000000000000
        00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000}
      ParentFont = False
      OnClick = PanelMinClick
    end
    object cbSectorFront: TComboBox
      Left = 3
      Top = 77
      Width = 118
      Height = 18
      Style = csOwnerDrawVariable
      Color = 14869218
      DropDownCount = 12
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 12
      ParentFont = False
      Sorted = True
      TabOrder = 0
      OnChange = cbSectorChange
    end
    object cbSectorBack: TComboBox
      Left = 3
      Top = 111
      Width = 118
      Height = 18
      Style = csOwnerDrawVariable
      Color = 14869218
      DropDownCount = 12
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 12
      ParentFont = False
      Sorted = True
      TabOrder = 1
      OnChange = cbSectorChange
    end
  end
  object paCommands: TPanel
    Left = 0
    Top = 0
    Width = 123
    Height = 90
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object RxLabel4: TRxLabel
      Left = 3
      Top = 15
      Width = 39
      Height = 13
      Caption = 'Current:'
      ShadowColor = 15263976
    end
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Select'
      Color = clGray
      ParentColor = False
      OnClick = TopClick
    end
    object ExtBtn2: TExtBtn
      Left = 109
      Top = 2
      Width = 11
      Height = 11
      Align = alNone
      CloseButton = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        DE000000424DDE00000000000000360000002800000007000000070000000100
        180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
        0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
        000000000000000000000000FFFFFF0000000000000000000000000000000000
        00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000}
      ParentFont = False
      OnClick = PanelMinClick
    end
    object RxLabel5: TRxLabel
      Left = 4
      Top = 48
      Width = 18
      Height = 13
      Caption = 'FS:'
    end
    object RxLabel3: TRxLabel
      Left = 4
      Top = 60
      Width = 19
      Height = 13
      Caption = 'BS:'
    end
    object lbFrontSector: TRxLabel
      Left = 24
      Top = 48
      Width = 11
      Height = 13
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clMaroon
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object lbBackSector: TRxLabel
      Left = 24
      Top = 60
      Width = 11
      Height = 13
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clMaroon
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ExtBtn3: TExtBtn
      Left = 2
      Top = 73
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Invert Orientation'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      OnClick = ExtBtn3Click
    end
    object cbItems: TComboBox
      Left = 3
      Top = 30
      Width = 118
      Height = 18
      Style = csOwnerDrawVariable
      Color = 14869218
      DropDownCount = 12
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 12
      ParentFont = False
      Sorted = True
      TabOrder = 0
      OnChange = cbItemsChange
    end
  end
end
