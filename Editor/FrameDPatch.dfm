object fraDPatch: TfraDPatch
  Left = 0
  Top = 0
  Width = 443
  Height = 365
  VertScrollBar.Visible = False
  Align = alTop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentFont = False
  TabOrder = 0
  object paAddDPatch: TPanel
    Left = 0
    Top = 0
    Width = 123
    Height = 186
    Align = alTop
    Constraints.MaxWidth = 123
    Constraints.MinWidth = 123
    ParentColor = True
    TabOrder = 0
    object RxLabel4: TRxLabel
      Left = 2
      Top = 18
      Width = 41
      Height = 13
      Caption = 'Amount:'
      ShadowColor = 15263976
    end
    object RxLabel1: TRxLabel
      Left = 2
      Top = 38
      Width = 62
      Height = 13
      Caption = 'DPatch size:'
      ShadowColor = 15263976
      WordWrap = True
    end
    object RxLabel3: TRxLabel
      Left = 12
      Top = 55
      Width = 39
      Height = 13
      Caption = 'Min (m):'
      ShadowColor = 15263976
    end
    object RxLabel5: TRxLabel
      Left = 12
      Top = 74
      Width = 42
      Height = 13
      Caption = 'Max (m):'
      ShadowColor = 15263976
    end
    object APHeadLabel1: TLabel
      Left = 1
      Top = 1
      Width = 121
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Detail Patches'
      Color = clGray
      ParentColor = False
      OnClick = TopClick
    end
    object ExtBtn2: TExtBtn
      Left = 109
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
      OnClick = PanelMinClick
    end
    object ebSelectShader: TExtBtn
      Left = 3
      Top = 150
      Width = 40
      Height = 16
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Shader'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSelectShaderClick
    end
    object lbDPShader: TRxLabel
      Left = 4
      Top = 167
      Width = 116
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ebSelectTexture: TExtBtn
      Left = 4
      Top = 115
      Width = 40
      Height = 16
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Texture'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSelectTextureClick
    end
    object lbDPTexture: TRxLabel
      Left = 4
      Top = 132
      Width = 116
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object paImage: TPanel
      Left = 89
      Top = 98
      Width = 32
      Height = 32
      BevelOuter = bvLowered
      Caption = '...'
      TabOrder = 0
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 30
        Height = 30
        Align = alClient
        OnPaint = pbImagePaint
      end
    end
    object seBrushDPatchAmount: TMultiObjSpinEdit
      Left = 56
      Top = 17
      Width = 65
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      MaxValue = 10000
      MinValue = 1
      Value = 1000
      AutoSize = False
      Color = 15263976
      Constraints.MaxHeight = 18
      Constraints.MinHeight = 18
      TabOrder = 1
    end
    object seDPatchMin: TMultiObjSpinEdit
      Left = 55
      Top = 53
      Width = 66
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.05
      MaxValue = 1000
      MinValue = 0.01
      ValueType = vtFloat
      Value = 0.1
      AutoSize = False
      Color = 15263976
      Constraints.MaxHeight = 18
      Constraints.MinHeight = 18
      TabOrder = 2
    end
    object seDPatchMax: TMultiObjSpinEdit
      Left = 55
      Top = 72
      Width = 66
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.05
      MaxValue = 1000
      MinValue = 0.01
      ValueType = vtFloat
      Value = 0.2
      AutoSize = False
      Color = 15263976
      Constraints.MaxHeight = 18
      Constraints.MinHeight = 18
      TabOrder = 3
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'FrameDPatch'
    Options = []
    StoredProps.Strings = (
      'seBrushDPatchAmount.Value'
      'seDPatchMax.Value'
      'seDPatchMin.Value'
      'lbDPShader.Caption'
      'lbDPTexture.Caption'
      'paAddDPatch.Tag'
      'paAddDPatch.Height')
    StoredValues = <>
  end
end
