object fraSpawnPoint: TfraSpawnPoint
  Left = 0
  Top = 0
  Width = 443
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
  object paCurrent: TPanel
    Left = 0
    Top = 51
    Width = 443
    Height = 226
    Align = alClient
    Constraints.MinHeight = 175
    ParentColor = True
    TabOrder = 0
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Current Spawn'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object Bevel1: TBevel
      Left = 1
      Top = 14
      Width = 441
      Height = 1
      Align = alTop
      Shape = bsSpacer
    end
    object paItems: TPanel
      Left = 1
      Top = 15
      Width = 441
      Height = 210
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
    end
  end
  object paCommands: TPanel
    Left = 0
    Top = 0
    Width = 443
    Height = 51
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object Label1: TLabel
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
    object ExtBtn1: TExtBtn
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
      OnClick = PaneMinClick
    end
    object ebAttachObject: TExtBtn
      Left = 2
      Top = 17
      Width = 195
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      GroupIndex = 1
      Caption = 'Attach Object...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebAttachObjectClick
    end
    object evDetachObject: TExtBtn
      Left = 2
      Top = 32
      Width = 195
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Detach Object'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = evDetachObjectClick
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameRPoint'
    Options = []
    Version = 3
    StoredProps.Strings = (
      'paCurrent.Tag'
      'paCurrent.Height'
      'paCommands.Tag'
      'paCommands.Height')
    StoredValues = <>
  end
end
