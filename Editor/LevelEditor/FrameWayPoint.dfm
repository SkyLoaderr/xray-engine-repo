object fraWayPoint: TfraWayPoint
  Left = 0
  Top = 0
  Width = 123
  Height = 374
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxWidth = 154
  Constraints.MinWidth = 123
  Color = 10528425
  ParentColor = False
  TabOrder = 0
  object paCommands: TPanel
    Left = 0
    Top = 0
    Width = 123
    Height = 54
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 0
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 121
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
      Glyph.Data = {
        DE000000424DDE00000000000000360000002800000007000000070000000100
        180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
        0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
        000000000000000000000000FFFFFF0000000000000000000000000000000000
        00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000}
      OnClick = PanelMinClick
    end
    object ebAutoLink: TExtBtn
      Left = 62
      Top = 17
      Width = 59
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = 'Auto link'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ebRemoveSelected: TExtBtn
      Left = 2
      Top = 37
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Remove Point'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebRemoveSelectedClick
    end
    object Bevel2: TBevel
      Left = 2
      Top = 34
      Width = 120
      Height = 2
    end
    object ebPointMode: TExtBtn
      Left = 2
      Top = 17
      Width = 59
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      GroupIndex = 2
      Caption = 'Point Mode'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      FlatAlwaysEdge = True
    end
  end
  object paLink: TPanel
    Left = 0
    Top = 54
    Width = 123
    Height = 79
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 1
    object Label1: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Link Command'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn3: TExtBtn
      Left = 110
      Top = 2
      Width = 11
      Height = 11
      Align = alNone
      CloseButton = False
      Glyph.Data = {
        DE000000424DDE00000000000000360000002800000007000000070000000100
        180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
        0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
        000000000000000000000000FFFFFF0000000000000000000000000000000000
        00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000}
      OnClick = PanelMinClick
    end
    object ebInvertLink: TExtBtn
      Left = 2
      Top = 46
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Invert Link'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebInvertLinkClick
    end
    object ebAdd1Link: TExtBtn
      Left = 2
      Top = 16
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Make 1-Link'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebAdd1LinksClick
    end
    object ebRemoveLinks: TExtBtn
      Left = 2
      Top = 61
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Remove Link'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebRemoveLinksClick
    end
    object ebAdd2Link: TExtBtn
      Left = 2
      Top = 31
      Width = 120
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Make 2-Link'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebAdd2LinkClick
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameAITPoint'
    Options = []
    Version = 3
    StoredProps.Strings = (
      'ebAutoLink.Down'
      'paCommands.Height'
      'paCommands.Tag'
      'paLink.Tag'
      'paLink.Height')
    StoredValues = <>
    Left = 65528
    Top = 65528
  end
end
