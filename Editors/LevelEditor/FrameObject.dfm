object fraObject: TfraObject
  Left = 0
  Top = 0
  Width = 198
  Height = 261
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
  object paSelectObject: TPanel
    Left = 0
    Top = 33
    Width = 198
    Height = 47
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object Label5: TLabel
      Left = 5
      Top = 16
      Width = 48
      Height = 26
      Caption = 'Select by reference:'
      WordWrap = True
    end
    object ebSelectByRefs: TExtBtn
      Left = 60
      Top = 15
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
      Font.Style = []
      ParentFont = False
      OnClick = ebSelectByRefsClick
    end
    object ebDeselectByRefs: TExtBtn
      Left = 93
      Top = 15
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
      Font.Style = []
      ParentFont = False
      OnClick = ebDeselectByRefsClick
    end
    object APHeadLabel3: TLabel
      Left = 1
      Top = 1
      Width = 196
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Select'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn3: TExtBtn
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
    object ebMultiSelectByRefMove: TExtBtn
      Left = 60
      Top = 29
      Width = 26
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '=%'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebMultiSelectByRefMoveClick
    end
    object Bevel1: TBevel
      Left = 124
      Top = 28
      Width = 60
      Height = 16
    end
    object Label1: TLabel
      Left = 186
      Top = 30
      Width = 8
      Height = 13
      Caption = '%'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ebMultiSelectByRefAppend: TExtBtn
      Left = 93
      Top = 29
      Width = 26
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '+%'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebMultiSelectByRefAppendClick
    end
    object seSelPercent: TMultiObjSpinEdit
      Left = 125
      Top = 30
      Width = 58
      Height = 13
      LWSensitivity = 1
      ButtonKind = bkLightWave
      MaxValue = 100
      MinValue = 1
      Value = 100
      AutoSize = False
      BorderStyle = bsNone
      Color = 12698049
      TabOrder = 0
      OnKeyPress = seSelPercentKeyPress
    end
  end
  object paCommands: TPanel
    Left = 0
    Top = 0
    Width = 198
    Height = 33
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object ebMultiAppend: TExtBtn
      Left = 2
      Top = 16
      Width = 195
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Multiple Append'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebMultiAppendClick
    end
    object APHeadLabel1: TLabel
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
      OnClick = PaneMinClick
    end
  end
  object paAddObject: TPanel
    Left = 0
    Top = 80
    Width = 198
    Height = 175
    Align = alTop
    ParentColor = True
    TabOrder = 2
    object APHeadLabel2: TLabel
      Left = 1
      Top = 1
      Width = 196
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Current Object'
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
    object Bevel2: TBevel
      Left = 1
      Top = 14
      Width = 196
      Height = 1
      Align = alTop
      Shape = bsSpacer
    end
    object paItems: TPanel
      Left = 1
      Top = 15
      Width = 196
      Height = 159
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameObject'
    Options = []
    Version = 4
    StoredProps.Strings = (
      'paCommands.Tag'
      'paCommands.Height'
      'seSelPercent.Value'
      'paSelectObject.Tag'
      'paSelectObject.Height'
      'paAddObject.Height'
      'paAddObject.Tag')
    StoredValues = <>
  end
end
