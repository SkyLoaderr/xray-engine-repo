object fraDetailObject: TfraDetailObject
  Left = 0
  Top = 0
  Width = 123
  Height = 401
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxWidth = 154
  Constraints.MinWidth = 123
  Color = 10528425
  ParentColor = False
  TabOrder = 0
  object paSelect: TPanel
    Left = 0
    Top = 205
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
      HotColor = 15790320
      CloseButton = False
      Caption = 'Reinitialize Objects'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebReinitializeSlotObjectsClick
    end
  end
  object paCommand: TPanel
    Left = 0
    Top = 0
    Width = 123
    Height = 141
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
      Caption = 'DO Commands'
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
      Top = 17
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'First Initialize'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ExtBtn1Click
    end
    object ExtBtn6: TExtBtn
      Left = 2
      Top = 32
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Reinitialize All'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ExtBtn6Click
    end
    object ExtBtn4: TExtBtn
      Left = 2
      Top = 47
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Reinitialize Objects'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebUpdateObjectsClick
    end
    object Label2: TLabel
      Left = 16
      Top = 122
      Width = 38
      Height = 13
      Caption = 'Density:'
    end
    object Bevel2: TBevel
      Left = 1
      Top = 117
      Width = 121
      Height = 2
    end
    object ebResetSlots: TExtBtn
      Left = 2
      Top = 87
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Clear Slots'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebResetSlotsClick
    end
    object ExtBtn7: TExtBtn
      Left = 2
      Top = 67
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Update Renderer'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebInvalidateCacheClick
    end
    object ebClearDetails: TExtBtn
      Left = 2
      Top = 102
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Clear Details'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebClearDetailsClick
    end
    object Bevel3: TBevel
      Left = 1
      Top = 83
      Width = 121
      Height = 2
    end
    object Bevel4: TBevel
      Left = 1
      Top = 63
      Width = 121
      Height = 2
    end
    object seDensity: TMultiObjSpinEdit
      Left = 56
      Top = 120
      Width = 65
      Height = 19
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 0.5
      MinValue = 0.01
      ValueType = vtFloat
      Value = 0.02
      AutoSize = False
      Color = 10526880
      TabOrder = 0
      OnLWChange = seDensityLWChange
      OnExit = seDensityExit
      OnKeyDown = seDensityKeyDown
    end
  end
  object paBaseTexture: TPanel
    Left = 0
    Top = 239
    Width = 123
    Height = 91
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 2
    Visible = False
    object Label3: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Base Texture'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn8: TExtBtn
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
    object ebBaseBlended: TExtBtn
      Left = 2
      Top = 16
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      BtnColor = 10528425
      DownColor = 10528425
      CloseButton = False
      GroupIndex = 1
      Caption = 'Render Blended'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        6E040000424D6E04000000000000360000002800000028000000090000000100
        18000000000038040000120B0000120B00000000000000000000FF00FF000000
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FFFF00FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF0000009999999999999999
        99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
        9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
        999999999999C4C4C4FF00FFC4C4C40000000000000000000000000000000000
        00000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
        9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
        C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
        C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
        00FFFF00FF000000999999999999999999999999999999999999C4C4C4FF00FF
        FF00FF000000999999999999999999999999999999999999C4C4C4FF00FFFF00
        FF0000009999999999999999999999995A699C999999C4C4C4FF00FFC4C4C499
        9999999999999999999999999999999999000000FF00FFFF00FFFF00FF000000
        999999999999999999999999999999999999C4C4C4FF00FFFF00FF0000009999
        999999995A699C999999999999999999C4C4C4FF00FFFF00FF0000009999995A
        699C999999999999999999999999C4C4C4FF00FFC4C4C4999999999999999999
        999999999999999999000000FF00FFFF00FFFF00FF0000009999999999999999
        99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
        9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
        999999999999C4C4C4FF00FFC4C4C49999999999999999999999999999999999
        99000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
        9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
        C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
        C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
        00FFFF00FF000000000000000000000000000000000000000000000000FF00FF
        FF00FF000000000000000000000000000000000000000000000000FF00FFFF00
        FF000000000000000000000000000000000000000000000000FF00FFC4C4C499
        9999999999999999999999999999999999000000FF00FFFF00FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC4C4C4C4C4C4C4C4C4C4C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF}
      Margin = 3
      NumGlyphs = 4
      ParentFont = False
      Spacing = 3
      OnClick = ebBaseBlendedClick
    end
    object RxLabel16: TLabel
      Left = 4
      Top = 54
      Width = 57
      Height = 13
      Caption = 'Brush Color:'
    end
    object mcBrushColor: TMultiObjColor
      Left = 62
      Top = 52
      Width = 58
      Height = 16
      Shape = stRoundRect
    end
    object Label4: TLabel
      Left = 4
      Top = 70
      Width = 53
      Height = 13
      Caption = 'Brush Size:'
    end
    object ebPaintMode: TExtBtn
      Left = 2
      Top = 36
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      BtnColor = 10528425
      DownColor = 10528425
      CloseButton = False
      GroupIndex = 2
      Caption = 'Paint Mode'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        6E040000424D6E04000000000000360000002800000028000000090000000100
        18000000000038040000120B0000120B00000000000000000000FF00FF000000
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FFFF00FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF0000009999999999999999
        99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
        9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
        999999999999C4C4C4FF00FFC4C4C40000000000000000000000000000000000
        00000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
        9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
        C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
        C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
        00FFFF00FF000000999999999999999999999999999999999999C4C4C4FF00FF
        FF00FF000000999999999999999999999999999999999999C4C4C4FF00FFFF00
        FF0000009999999999999999999999995A699C999999C4C4C4FF00FFC4C4C499
        9999999999999999999999999999999999000000FF00FFFF00FFFF00FF000000
        999999999999999999999999999999999999C4C4C4FF00FFFF00FF0000009999
        999999995A699C999999999999999999C4C4C4FF00FFFF00FF0000009999995A
        699C999999999999999999999999C4C4C4FF00FFC4C4C4999999999999999999
        999999999999999999000000FF00FFFF00FFFF00FF0000009999999999999999
        99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
        9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
        999999999999C4C4C4FF00FFC4C4C49999999999999999999999999999999999
        99000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
        9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
        C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
        C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
        00FFFF00FF000000000000000000000000000000000000000000000000FF00FF
        FF00FF000000000000000000000000000000000000000000000000FF00FFFF00
        FF000000000000000000000000000000000000000000000000FF00FFC4C4C499
        9999999999999999999999999999999999000000FF00FFFF00FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC4C4C4C4C4C4C4C4C4C4C4C4
        C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF}
      Margin = 3
      NumGlyphs = 4
      ParentFont = False
      Spacing = 3
      OnClick = ebBaseBlendedClick
    end
    object Bevel1: TBevel
      Left = 1
      Top = 32
      Width = 121
      Height = 2
    end
    object seBrushSize: TMultiObjSpinEdit
      Left = 61
      Top = 69
      Width = 59
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 100
      MinValue = 1
      ValueType = vtFloat
      Value = 1
      AutoSize = False
      Color = 10526880
      TabOrder = 0
      OnLWChange = seDensityLWChange
      OnExit = seDensityExit
      OnKeyDown = seDensityKeyDown
    end
  end
  object paObjects: TPanel
    Left = 0
    Top = 141
    Width = 123
    Height = 64
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 3
    object Label5: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Color Indices'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn9: TExtBtn
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
    object ExtBtn5: TExtBtn
      Left = 2
      Top = 17
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Object List'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ExtBtn5Click
    end
    object ebExportColorIndices: TExtBtn
      Left = 2
      Top = 47
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Export Indices'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebExportColorIndicesClick
    end
    object ebImportColorIndices: TExtBtn
      Left = 2
      Top = 32
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      HotColor = 15790320
      CloseButton = False
      Caption = 'Import Indices'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 13
      ParentFont = False
      OnClick = ebImportColorIndicesClick
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameDO'
    Options = []
    Version = 10
    StoredProps.Strings = (
      'paSelect.Height'
      'paSelect.Tag'
      'paCommand.Height'
      'paCommand.Tag'
      'seBrushSize.Value'
      'mcBrushColor.Brush'
      'ebPaintMode.Down'
      'paObjects.Height'
      'paObjects.Tag'
      'paBaseTexture.Tag'
      'paBaseTexture.Height')
    StoredValues = <>
    Top = 65528
  end
end
