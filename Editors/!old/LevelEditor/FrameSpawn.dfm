object fraSpawn: TfraSpawn
  Left = 466
  Top = 421
  VertScrollBar.Visible = False
  Align = alClient
  BorderStyle = bsNone
  ClientHeight = 273
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
  object paCurrent: TPanel
    Left = 0
    Top = 98
    Width = 198
    Height = 175
    Align = alClient
    Constraints.MinHeight = 175
    ParentColor = True
    TabOrder = 0
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 196
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
  object paCommands: TPanel
    Left = 0
    Top = 47
    Width = 198
    Height = 51
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object Label1: TLabel
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
  object paSelectObject: TPanel
    Left = 0
    Top = 0
    Width = 198
    Height = 47
    Align = alTop
    Color = 10528425
    TabOrder = 2
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
    object Bevel2: TBevel
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
      TabOrder = 0
      OnKeyPress = seSelPercentKeyPress
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
