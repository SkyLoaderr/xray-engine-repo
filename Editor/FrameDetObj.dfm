object fraDetailObject: TfraDetailObject
  Left = 0
  Top = 0
  Width = 123
  Height = 452
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxWidth = 154
  Constraints.MinWidth = 123
  TabOrder = 0
  object paSelect: TPanel
    Left = 0
    Top = 33
    Width = 123
    Height = 133
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 0
    Visible = False
    object ebDORandomScale: TExtBtn
      Left = 2
      Top = 101
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Random Scale'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
    end
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Selected'
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
    object ebRandomRotate: TExtBtn
      Left = 2
      Top = 116
      Width = 119
      Height = 15
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Random Rotate'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
    end
    object GroupBox2: TGroupBox
      Left = 1
      Top = 14
      Width = 121
      Height = 87
      Align = alTop
      Caption = ' Scale '
      TabOrder = 0
      object RxLabel5: TRxLabel
        Left = 5
        Top = 16
        Width = 32
        Height = 13
        Caption = 'Min Y:'
        ShadowColor = 15263976
      end
      object RxLabel6: TRxLabel
        Left = 5
        Top = 33
        Width = 35
        Height = 13
        Caption = 'Max Y:'
        ShadowColor = 15263976
      end
      object RxLabel7: TRxLabel
        Left = 5
        Top = 51
        Width = 39
        Height = 13
        Caption = 'Min XZ:'
        ShadowColor = 15263976
      end
      object RxLabel8: TRxLabel
        Left = 5
        Top = 68
        Width = 42
        Height = 13
        Caption = 'Max XZ:'
        ShadowColor = 15263976
      end
      object seScaleMinY: TMultiObjSpinEdit
        Left = 49
        Top = 13
        Width = 70
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 100
        MinValue = 0.1
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 0
      end
      object seScaleMaxY: TMultiObjSpinEdit
        Left = 49
        Top = 30
        Width = 70
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 100
        MinValue = 0.1
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 1
      end
      object seScaleMinXZ: TMultiObjSpinEdit
        Left = 49
        Top = 48
        Width = 70
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 100
        MinValue = 0.1
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 2
      end
      object seScaleMaxXZ: TMultiObjSpinEdit
        Left = 49
        Top = 65
        Width = 70
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 100
        MinValue = 0.1
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 3
      end
    end
  end
  object paCommand: TPanel
    Left = 0
    Top = 0
    Width = 123
    Height = 33
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 1
    object ebModeCluster: TExtBtn
      Left = 2
      Top = 16
      Width = 60
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = 'Cluster'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      OnClick = ebModeClusterClick
    end
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
    object ebModeObjects: TExtBtn
      Left = 62
      Top = 16
      Width = 60
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Caption = 'Objects'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      OnClick = ebModeObjectsClick
    end
  end
  object paClusterBrush: TPanel
    Left = 0
    Top = 166
    Width = 123
    Height = 39
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
      Caption = 'Cluster Brush'
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
    object RxLabel1: TRxLabel
      Left = 5
      Top = 20
      Width = 40
      Height = 13
      Caption = 'Density:'
      ShadowColor = 15263976
    end
    object seClusterDensity: TMultiObjSpinEdit
      Left = 47
      Top = 17
      Width = 73
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 1
      MaxValue = 1000
      Value = 10
      AutoSize = False
      TabOrder = 0
    end
  end
  object paObjectBrush: TPanel
    Left = 0
    Top = 205
    Width = 123
    Height = 93
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 3
    Visible = False
    object Label2: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Object Brush'
      Color = clGray
      ParentColor = False
      OnClick = ExpandClick
    end
    object ExtBtn4: TExtBtn
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
    object ebObjectName: TExtBtn
      Left = 3
      Top = 37
      Width = 37
      Height = 14
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Object:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Kind = knMinimize
      Margin = 0
      ParentFont = False
      Transparent = False
      OnClick = ebObjectNameClick
    end
    object lbObjectName: TRxLabel
      Left = 41
      Top = 38
      Width = 79
      Height = 13
      AutoSize = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clMaroon
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ShadowColor = 15263976
    end
    object RxLabel3: TRxLabel
      Left = 21
      Top = 55
      Width = 25
      Height = 13
      Caption = 'Size:'
      ShadowColor = 15263976
    end
    object RxLabel10: TRxLabel
      Left = 5
      Top = 74
      Width = 63
      Height = 13
      Caption = 'Pressure (%):'
      ShadowColor = 15263976
    end
    object ebObjectAdd: TExtBtn
      Left = 2
      Top = 16
      Width = 60
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = 'Add'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
    end
    object ebObjectClear: TExtBtn
      Left = 62
      Top = 16
      Width = 60
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Caption = 'Clear'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
    end
    object Bevel1: TBevel
      Left = 2
      Top = 34
      Width = 120
      Height = 2
    end
    object seObjectBrushSize: TMultiObjSpinEdit
      Left = 47
      Top = 52
      Width = 73
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Decimal = 1
      Increment = 0.1
      MaxValue = 100
      MinValue = 0.1
      ValueType = vtFloat
      Value = 1
      AutoSize = False
      TabOrder = 0
    end
    object cbObjectUseSize: TCheckBox
      Left = 6
      Top = 55
      Width = 13
      Height = 13
      Color = clBtnFace
      ParentColor = False
      TabOrder = 1
    end
    object seObjectPressure: TMultiObjSpinEdit
      Left = 68
      Top = 71
      Width = 52
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 1
      MaxValue = 100
      Value = 10
      AutoSize = False
      TabOrder = 2
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameAITPoint'
    Options = []
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paSelect.Height'
      'paSelect.Tag'
      'paCommand.Height'
      'paCommand.Tag'
      'ebModeCluster.Down'
      'ebModeObjects.Down'
      'seClusterDensity.Value'
      'ebObjectAdd.Down'
      'ebObjectClear.Down'
      'cbObjectUseSize.Checked'
      'seObjectBrushSize.Value'
      'seObjectPressure.Value')
    StoredValues = <>
  end
end
