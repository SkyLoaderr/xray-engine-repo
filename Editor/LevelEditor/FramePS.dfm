object fraPS: TfraPS
  Left = 0
  Top = 0
  Width = 443
  Height = 277
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxWidth = 123
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
  object paSelectObject: TPanel
    Left = 0
    Top = 0
    Width = 443
    Height = 32
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object Label5: TLabel
      Left = 2
      Top = 16
      Width = 58
      Height = 13
      Caption = 'Ref'#39's select:'
    end
    object ebSelectByRefs: TExtBtn
      Left = 65
      Top = 16
      Width = 26
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '+'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = ebSelectByRefsClick
    end
    object ebDeselectByRefs: TExtBtn
      Left = 91
      Top = 16
      Width = 26
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = ebDeselectByRefsClick
    end
    object Label: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Select'
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
  end
  object Panel1: TPanel
    Left = 0
    Top = 32
    Width = 443
    Height = 32
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object ebCurObj: TExtBtn
      Left = 2
      Top = 16
      Width = 120
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentFont = False
      OnClick = ebCurObjClick
    end
    object APHeadLabel2: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Current PS'
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
      OnClick = PaneMinClick
    end
  end
end
