object fraAIMap: TfraAIMap
  Left = 0
  Top = 0
  Width = 443
  Height = 277
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxWidth = 220
  Constraints.MinWidth = 123
  Color = 10528425
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentColor = False
  ParentFont = False
  TabOrder = 0
  object paMode: TPanel
    Left = 0
    Top = 0
    Width = 443
    Height = 83
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object Label1: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Mode'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn1: TExtBtn
      Left = 110
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
    object ebIgnoreConstraints: TExtBtn
      Left = 2
      Top = 51
      Width = 120
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 2
      Caption = 'Ignore Constraints'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      NumGlyphs = 4
      ParentFont = False
      Spacing = 3
    end
    object ebCurrentNode: TExtBtn
      Left = 2
      Top = 17
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = 'Node'
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
      Margin = 3
      NumGlyphs = 4
      ParentFont = False
      Spacing = 3
      OnClick = ebChangeCurrentClick
    end
    object ebCurrentEmitter: TExtBtn
      Left = 2
      Top = 32
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 1
      Caption = 'Emitter'
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
      Margin = 3
      NumGlyphs = 4
      ParentFont = False
      Spacing = 3
      OnClick = ebChangeCurrentClick
    end
    object ebAutoLink: TExtBtn
      Left = 2
      Top = 66
      Width = 120
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 3
      Down = True
      Caption = 'Auto Link'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      NumGlyphs = 4
      ParentFont = False
      Spacing = 3
    end
  end
  object paObjectList: TPanel
    Left = 0
    Top = 83
    Width = 443
    Height = 115
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object Label2: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Commands'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn2: TExtBtn
      Left = 110
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
    object ebFillSnapList: TExtBtn
      Left = 1
      Top = 31
      Width = 121
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Fill Snap Objects'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebFillSnapListClick
    end
    object ebGenerateMap: TExtBtn
      Left = 1
      Top = 16
      Width = 121
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Generate Map'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebGenerateMapClick
    end
    object ebDrawSnapObjects: TExtBtn
      Left = 1
      Top = 95
      Width = 121
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 3
      Down = True
      Caption = 'Draw Snap Objects'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      NumGlyphs = 4
      ParentFont = False
      Spacing = 3
      OnClick = ebDrawSnapObjectsClick
    end
    object ebSmoothNodes: TExtBtn
      Left = 1
      Top = 46
      Width = 121
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Smooth Nodes'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebSmoothNodesClick
    end
    object Bevel1: TBevel
      Left = 1
      Top = 91
      Width = 121
      Height = 2
    end
    object ebRemove0LNodes: TExtBtn
      Left = 1
      Top = 61
      Width = 121
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Remove 0-Link Nodes'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebRemove0LNodesClick
    end
    object ebRemove1LNodes: TExtBtn
      Left = 1
      Top = 76
      Width = 121
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Remove 1-Link Nodes'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebRemove1LNodesClick
    end
  end
  object paLink: TPanel
    Left = 0
    Top = 198
    Width = 123
    Height = 118
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 2
    object Label3: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Nodes'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn4: TExtBtn
      Left = 110
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
    object ebModeAppend: TExtBtn
      Left = 6
      Top = 70
      Width = 24
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = '+'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
    end
    object ebModeRemove: TExtBtn
      Left = 6
      Top = 85
      Width = 24
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      GroupIndex = 1
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
    end
    object ebUp: TExtBtn
      Tag = 1
      Left = 67
      Top = 70
      Width = 27
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'F'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
      OnClick = ebSideClick
    end
    object ebDown: TExtBtn
      Tag = 3
      Left = 67
      Top = 100
      Width = 27
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'B'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
      OnClick = ebSideClick
    end
    object ebLeft: TExtBtn
      Left = 40
      Top = 85
      Width = 27
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'L'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
      OnClick = ebSideClick
    end
    object ebRight: TExtBtn
      Tag = 2
      Left = 94
      Top = 85
      Width = 27
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'R'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
      OnClick = ebSideClick
    end
    object ebFull: TExtBtn
      Tag = 4
      Left = 67
      Top = 85
      Width = 27
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'All'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
      OnClick = ebSideClick
    end
    object ebModeInvert: TExtBtn
      Left = 6
      Top = 100
      Width = 24
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      GroupIndex = 1
      Caption = #172
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
    end
    object Label4: TLabel
      Left = 5
      Top = 37
      Width = 53
      Height = 13
      Caption = 'Brush Size:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label5: TLabel
      Left = 5
      Top = 54
      Width = 78
      Height = 13
      Caption = 'Link Commands:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentFont = False
    end
    object Label6: TLabel
      Left = 5
      Top = 19
      Width = 75
      Height = 13
      Caption = 'Visibility Radius:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object seBrushSize: TMultiObjSpinEdit
      Left = 80
      Top = 35
      Width = 40
      Height = 17
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 0
      MaxValue = 100
      MinValue = 1
      Value = 1
      AutoSize = False
      Color = 10526880
      TabOrder = 0
    end
    object seVisRadius: TMultiObjSpinEdit
      Left = 80
      Top = 17
      Width = 40
      Height = 17
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 0
      MaxValue = 250
      MinValue = 10
      Value = 30
      AutoSize = False
      Color = 10526880
      TabOrder = 1
      OnChange = seVisRadiusChange
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameAIMap'
    Options = []
    Version = 6
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paMode.Tag'
      'paObjectList.Tag'
      'paMode.Height'
      'paObjectList.Height'
      'ebCurrentEmitter.Down'
      'ebCurrentNode.Down'
      'ebIgnoreConstraints.Down'
      'ebAutoLink.Down'
      'paLink.Tag'
      'paLink.Height'
      'ebModeAppend.Down'
      'ebModeInvert.Down'
      'ebModeRemove.Down'
      'seBrushSize.Value'
      'seVisRadius.Value')
    StoredValues = <>
    Left = 65528
    Top = 65528
  end
end
