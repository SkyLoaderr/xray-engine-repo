object fraDetailObject: TfraDetailObject
  Left = 0
  Top = 0
  Width = 123
  Height = 277
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxWidth = 154
  Constraints.MinWidth = 123
  TabOrder = 0
  object paSelect: TPanel
    Left = 0
    Top = 78
    Width = 123
    Height = 34
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
      Caption = 'Selected Slot'
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
      OnClick = PaneMinClick
    end
    object ebReinitializeSlotObjects: TExtBtn
      Left = 2
      Top = 16
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Reinitialize Objects'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      Transparent = False
      OnClick = ebReinitializeSlotObjectsClick
    end
  end
  object paCommand: TPanel
    Left = 0
    Top = 0
    Width = 123
    Height = 78
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
      Caption = 'Commands'
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
      OnClick = PaneMinClick
    end
    object ExtBtn1: TExtBtn
      Left = 2
      Top = 32
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Initialize'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      Transparent = False
      OnClick = ExtBtn1Click
    end
    object ExtBtn5: TExtBtn
      Left = 2
      Top = 17
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Object List'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      Transparent = False
      OnClick = ExtBtn5Click
    end
    object ExtBtn6: TExtBtn
      Left = 2
      Top = 47
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Reinitialize'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      Transparent = False
      OnClick = ExtBtn6Click
    end
    object ExtBtn4: TExtBtn
      Left = 2
      Top = 61
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Update Objects'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      Transparent = False
      OnClick = ebUpdateObjectsClick
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameDO'
    Options = []
    Version = 4
    StoredProps.Strings = (
      'paSelect.Height'
      'paSelect.Tag'
      'paCommand.Height'
      'paCommand.Tag')
    StoredValues = <>
    Top = 65528
  end
end
