object fraObject: TfraObject
  Left = 414
  Top = 352
  VertScrollBar.Visible = False
  Align = alClient
  BorderStyle = bsNone
  ClientHeight = 267
  ClientWidth = 198
  Color = 10528425
  Constraints.MaxWidth = 206
  Constraints.MinWidth = 206
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label5: TLabel
    Left = 5
    Top = 16
    Width = 84
    Height = 13
    Caption = 'Select by Current:'
    WordWrap = True
  end
  object ebSelectByRefs: TExtBtn
    Left = 97
    Top = 15
    Width = 22
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
    Left = 121
    Top = 15
    Width = 22
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
  object ebMultiSelectByRefMove: TExtBtn
    Left = 97
    Top = 29
    Width = 22
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
    Left = 144
    Top = 28
    Width = 40
    Height = 16
  end
  object Label2: TLabel
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
    Left = 121
    Top = 29
    Width = 22
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
  object Label3: TLabel
    Left = 5
    Top = 31
    Width = 92
    Height = 13
    Caption = 'Select by Selected:'
    WordWrap = True
  end
  object paCommands: TPanel
    Left = 0
    Top = 47
    Width = 198
    Height = 33
    Align = alTop
    Color = 10528425
    TabOrder = 0
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
  object paCurrentObject: TPanel
    Left = 0
    Top = 80
    Width = 198
    Height = 187
    Align = alClient
    Color = 10528425
    Constraints.MinHeight = 175
    TabOrder = 1
    OnResize = paCurrentObjectResize
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
    object Bevel2: TBevel
      Left = 1
      Top = 37
      Width = 196
      Height = 1
      Align = alTop
      Shape = bsBottomLine
    end
    object paItems: TPanel
      Left = 1
      Top = 38
      Width = 196
      Height = 148
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
    end
    object Panel1: TPanel
      Left = 1
      Top = 14
      Width = 196
      Height = 23
      Align = alTop
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 1
      object ExtBtn4: TExtBtn
        Left = 1
        Top = 5
        Width = 195
        Height = 15
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        CloseButton = False
        Caption = 'Select ...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = ExtBtn4Click
      end
    end
  end
  object seSelPercent: TMultiObjSpinEdit
    Left = 145
    Top = 30
    Width = 38
    Height = 13
    LWSensitivity = 1
    ButtonKind = bkLightWave
    MaxValue = 100
    MinValue = 1
    Value = 100
    AutoSize = False
    BorderStyle = bsNone
    Color = 12698049
    TabOrder = 2
    OnKeyPress = seSelPercentKeyPress
  end
  object paSelectObject: TPanel
    Left = 0
    Top = 0
    Width = 198
    Height = 47
    Align = alTop
    Color = 10528425
    TabOrder = 3
    object Label1: TLabel
      Left = 5
      Top = 16
      Width = 84
      Height = 13
      Caption = 'Select by Current:'
      WordWrap = True
    end
    object ExtBtn1: TExtBtn
      Left = 97
      Top = 15
      Width = 22
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
    object ExtBtn3: TExtBtn
      Left = 121
      Top = 15
      Width = 22
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
      Caption = 'Reference Select'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn5: TExtBtn
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
    object ExtBtn6: TExtBtn
      Left = 97
      Top = 29
      Width = 22
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
    object Bevel3: TBevel
      Left = 144
      Top = 28
      Width = 40
      Height = 16
    end
    object Label4: TLabel
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
    object ExtBtn7: TExtBtn
      Left = 121
      Top = 29
      Width = 22
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
    object Label6: TLabel
      Left = 5
      Top = 31
      Width = 92
      Height = 13
      Caption = 'Select by Selected:'
      WordWrap = True
    end
    object MultiObjSpinEdit1: TMultiObjSpinEdit
      Left = 145
      Top = 30
      Width = 38
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
  object fsStorage: TFormStorage
    IniSection = 'FrameObject'
    Options = []
    Version = 1
    StoredProps.Strings = (
      'paCommands.Height'
      'paCommands.Tag')
    StoredValues = <>
  end
end
