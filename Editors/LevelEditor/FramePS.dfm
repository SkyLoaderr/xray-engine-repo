object fraPS: TfraPS
  Left = 0
  Top = 0
  Width = 123
  Height = 243
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
    Width = 123
    Height = 50
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
      Height = 15
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
      FlatAlwaysEdge = True
      OnClick = ebSelectByRefsClick
    end
    object ebDeselectByRefs: TExtBtn
      Left = 91
      Top = 16
      Width = 26
      Height = 15
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
      FlatAlwaysEdge = True
      OnClick = ebDeselectByRefsClick
    end
    object Label: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'PS Commands'
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
    object ebCurrentPSPlay: TExtBtn
      Left = 65
      Top = 32
      Width = 26
      Height = 15
      Align = alNone
      BevelShow = False
      CloseButton = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        C2010000424DC20100000000000036000000280000000C0000000B0000000100
        1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFBABABA0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFBABABA0000000000000000004F4F4F7B7B7BFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000000000000000000008080828
        2828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA000000000000000000000000
        0000000000001212121F1F1FC6C6C6FFFFFFFFFFFFBABABA0000000000000000
        00000000080808282828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000
        00000000004F4F4F7B7B7BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
        0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFF}
      ParentFont = False
      FlatAlwaysEdge = True
      OnClick = ebCurrentPSPlayClick
    end
    object ebCurrentPSStop: TExtBtn
      Left = 91
      Top = 32
      Width = 26
      Height = 15
      Align = alNone
      BevelShow = False
      CloseButton = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        9E010000424D9E0100000000000036000000280000000C0000000A0000000100
        18000000000068010000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFDDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFF
        FFFFFFFFFFFFFFFF969696000000000000000000000000000000000000969696
        FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000000000000000000000000000
        00969696FFFFFFFFFFFFFFFFFFFFFFFF96969600000000000000000000000000
        0000000000969696FFFFFFFFFFFFFFFFFFFFFFFF969696000000000000000000
        000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000
        00000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF96969600
        0000000000000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF
        DDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFF}
      ParentFont = False
      FlatAlwaysEdge = True
      OnClick = ebCurrentPSStopClick
    end
    object Label1: TLabel
      Left = 2
      Top = 32
      Width = 45
      Height = 13
      Caption = 'Selected:'
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 50
    Width = 123
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
      Width = 121
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
