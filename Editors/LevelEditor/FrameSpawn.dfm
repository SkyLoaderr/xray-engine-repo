object fraSpawn: TfraSpawn
  Left = 339
  Top = 314
  VertScrollBar.Visible = False
  Align = alClient
  BorderStyle = bsNone
  ClientHeight = 294
  ClientWidth = 234
  Color = 10528425
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
  TextHeight = 14
  object paCurrent: TPanel
    Left = 0
    Top = 106
    Width = 234
    Height = 188
    Align = alClient
    Constraints.MinHeight = 175
    ParentColor = True
    TabOrder = 0
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 232
      Height = 14
      Align = alTop
      Alignment = taCenter
      Caption = 'Current Spawn'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object Bevel1: TBevel
      Left = 1
      Top = 15
      Width = 232
      Height = 1
      Align = alTop
      Shape = bsSpacer
    end
    object paItems: TPanel
      Left = 1
      Top = 16
      Width = 232
      Height = 171
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
    end
  end
  object paCommands: TPanel
    Left = 0
    Top = 51
    Width = 234
    Height = 55
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object Label1: TLabel
      Left = 1
      Top = 1
      Width = 232
      Height = 14
      Align = alTop
      Alignment = taCenter
      Caption = 'Commands'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn1: TExtBtn
      Left = 221
      Top = 2
      Width = 12
      Height = 12
      Align = alNone
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
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
      Top = 18
      Width = 232
      Height = 16
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      GroupIndex = 1
      Caption = 'Attach Object...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebAttachObjectClick
    end
    object evDetachObject: TExtBtn
      Left = 2
      Top = 34
      Width = 232
      Height = 17
      Align = alNone
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      Caption = 'Detach Object'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = evDetachObjectClick
    end
  end
  object paSelectObject: TPanel
    Left = 0
    Top = 0
    Width = 234
    Height = 51
    Align = alTop
    Color = 10528425
    TabOrder = 2
    object Label5: TLabel
      Left = 5
      Top = 17
      Width = 87
      Height = 14
      Caption = 'Select by Current:'
      WordWrap = True
    end
    object ebSelectByRefs: TExtBtn
      Left = 104
      Top = 16
      Width = 30
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      Caption = '+'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebSelectByRefsClick
    end
    object ebDeselectByRefs: TExtBtn
      Left = 136
      Top = 16
      Width = 29
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebDeselectByRefsClick
    end
    object APHeadLabel3: TLabel
      Left = 1
      Top = 1
      Width = 232
      Height = 14
      Align = alTop
      Alignment = taCenter
      Caption = 'Reference Select'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn3: TExtBtn
      Left = 221
      Top = 2
      Width = 12
      Height = 12
      Align = alNone
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
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
      Left = 104
      Top = 31
      Width = 30
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      Caption = '=%'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebMultiSelectByRefMoveClick
    end
    object Bevel2: TBevel
      Left = 166
      Top = 30
      Width = 48
      Height = 17
    end
    object Label2: TLabel
      Left = 216
      Top = 32
      Width = 11
      Height = 15
      Caption = '%'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ebMultiSelectByRefAppend: TExtBtn
      Left = 136
      Top = 31
      Width = 29
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      Caption = '+%'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = ebMultiSelectByRefAppendClick
    end
    object Label3: TLabel
      Left = 5
      Top = 33
      Width = 93
      Height = 14
      Caption = 'Select by Selected:'
      WordWrap = True
    end
    object seSelPercent: TMultiObjSpinEdit
      Left = 167
      Top = 32
      Width = 46
      Height = 14
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
