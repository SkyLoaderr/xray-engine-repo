object fraAIMap: TfraAIMap
  Left = 0
  Top = 0
  Width = 198
  Height = 277
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxWidth = 198
  Constraints.MinWidth = 198
  Color = 10528425
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentColor = False
  ParentFont = False
  TabOrder = 0
  object paObjectList: TPanel
    Left = 0
    Top = 0
    Width = 198
    Height = 78
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object Label2: TLabel
      Left = 1
      Top = 1
      Width = 196
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Commands'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn2: TExtBtn
      Left = 185
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
    object ebGenerateMap: TExtBtn
      Left = 1
      Top = 16
      Width = 195
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
    object ebSmoothNodes: TExtBtn
      Left = 1
      Top = 31
      Width = 195
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
    object ebRemove0LNodes: TExtBtn
      Left = 1
      Top = 46
      Width = 195
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
      Top = 61
      Width = 195
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
    Top = 78
    Width = 198
    Height = 117
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object Label3: TLabel
      Left = 1
      Top = 1
      Width = 196
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Nodes'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn4: TExtBtn
      Left = 185
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
      Top = 69
      Width = 45
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
      Top = 84
      Width = 45
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
      Left = 106
      Top = 69
      Width = 45
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
      Left = 106
      Top = 99
      Width = 45
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
      Left = 61
      Top = 84
      Width = 45
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
      Left = 151
      Top = 84
      Width = 45
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
      Left = 106
      Top = 84
      Width = 45
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
      Top = 99
      Width = 45
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
    object Label5: TLabel
      Left = 5
      Top = 53
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
    object ebIgnoreConstraints: TExtBtn
      Left = 2
      Top = 16
      Width = 195
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
    object ebAutoLink: TExtBtn
      Left = 2
      Top = 31
      Width = 195
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
    object Bevel2: TBevel
      Left = 1
      Top = 48
      Width = 196
      Height = 2
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameAIMap'
    Options = []
    Version = 10
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paObjectList.Tag'
      'paObjectList.Height'
      'paLink.Tag'
      'paLink.Height'
      'ebModeAppend.Down'
      'ebModeInvert.Down'
      'ebModeRemove.Down')
    StoredValues = <>
    Left = 65528
    Top = 65528
  end
end
