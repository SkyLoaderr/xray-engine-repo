object fraPortal: TfraPortal
  Left = 0
  Top = 0
  Width = 198
  Height = 277
  VertScrollBar.ButtonSize = 11
  VertScrollBar.Range = 257
  VertScrollBar.Smooth = True
  VertScrollBar.Style = ssFlat
  VertScrollBar.Visible = False
  Align = alClient
  AutoScroll = False
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
  object paCommands: TPanel
    Left = 0
    Top = 0
    Width = 198
    Height = 68
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 196
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Command'
      Color = clGray
      ParentColor = False
      OnClick = TopClick
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
    object ebInvertOrient: TExtBtn
      Left = 2
      Top = 18
      Width = 195
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Invert Orientation'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebInvertOrientClick
    end
    object ebComputeAllPortals: TExtBtn
      Left = 2
      Top = 34
      Width = 195
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Compute All Portals'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebComputeAllPortalsClick
    end
    object ebComputeSelPortals: TExtBtn
      Left = 2
      Top = 50
      Width = 195
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Compute Sel. Portals'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebComputeClick
    end
  end
end
