object frmPropertiesLight: TfrmPropertiesLight
  Left = 497
  Top = 253
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Light properties'
  ClientHeight = 290
  ClientWidth = 406
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnKeyDown = FormKeyDown
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object ebOk: TExtBtn
    Left = 241
    Top = 270
    Width = 82
    Height = 18
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Ok'
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = btOkClick
  end
  object ebCancel: TExtBtn
    Left = 323
    Top = 270
    Width = 82
    Height = 18
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Cancel'
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = btCancelClick
  end
  object ExtBtn1: TExtBtn
    Left = 159
    Top = 270
    Width = 82
    Height = 18
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Apply'
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = btApplyClick
  end
  object ebAdjustScene: TExtBtn
    Left = 1
    Top = 270
    Width = 82
    Height = 18
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Adjust Scene'
    Transparent = False
    FlatAlwaysEdge = True
    Visible = False
  end
  object gbColor: TGroupBox
    Left = 0
    Top = 152
    Width = 406
    Height = 38
    Align = alTop
    Caption = ' Color: '
    TabOrder = 1
    object mcDiffuse: TMultiObjColor
      Left = 48
      Top = 14
      Width = 49
      Height = 16
      OnMouseDown = mcColorMouseDown
    end
    object RxLabel6: TMxLabel
      Left = 8
      Top = 14
      Width = 38
      Height = 13
      Caption = 'Diffuse:'
    end
    object RxLabel18: TMxLabel
      Left = 104
      Top = 14
      Width = 54
      Height = 13
      Caption = 'Brightness:'
    end
    object tbBrightness: TTrackBar
      Left = 224
      Top = 13
      Width = 177
      Height = 17
      Max = 150
      Min = -150
      Orientation = trHorizontal
      Frequency = 25
      Position = 100
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      ThumbLength = 10
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = tbBrightnessChange
    end
    object seBrightness: TMultiObjSpinEdit
      Left = 159
      Top = 12
      Width = 66
      Height = 18
      LWSensitivity = 1
      ButtonKind = bkLightWave
      Increment = 0.1
      MaxValue = 1.5
      MinValue = -1.5
      ValueType = vtFloat
      AutoSize = False
      TabOrder = 1
      OnBottomClick = seBrightnessChange
      OnTopClick = seBrightnessChange
      OnChange = seBrightnessChange
      OnExit = seBrightnessChange
    end
  end
  object gbAddition: TGroupBox
    Left = 0
    Top = 190
    Width = 406
    Height = 77
    Align = alTop
    Caption = ' Addition: '
    TabOrder = 2
    object MxLabel1: TMxLabel
      Left = 6
      Top = 14
      Width = 33
      Height = 13
      Caption = 'Name:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object gbLightType: TGroupBox
      Left = 269
      Top = 8
      Width = 133
      Height = 66
      Caption = ' Light Type '
      TabOrder = 0
      object cbTargetLM: TMultiObjCheck
        Left = 7
        Top = 14
        Width = 122
        Height = 17
        Alignment = taLeftJustify
        BiDiMode = bdLeftToRight
        Caption = 'Lightmap'
        ParentBiDiMode = False
        TabOrder = 0
        OnClick = cbTargetLMClick
      end
      object cbTargetDynamic: TMultiObjCheck
        Left = 7
        Top = 30
        Width = 122
        Height = 17
        Alignment = taLeftJustify
        BiDiMode = bdLeftToRight
        Caption = 'Dynamic models'
        ParentBiDiMode = False
        TabOrder = 1
      end
      object cbTargetAnimated: TMultiObjCheck
        Left = 7
        Top = 46
        Width = 122
        Height = 17
        Alignment = taLeftJustify
        BiDiMode = bdLeftToRight
        Caption = 'Animated'
        ParentBiDiMode = False
        TabOrder = 2
        OnClick = cbTargetAnimatedClick
      end
    end
    object cbUseInD3D: TMultiObjCheck
      Left = 8
      Top = 58
      Width = 76
      Height = 17
      BiDiMode = bdLeftToRight
      Caption = 'Use in D3D'
      ParentBiDiMode = False
      TabOrder = 1
    end
    object edName: TEdit
      Left = 56
      Top = 11
      Width = 209
      Height = 18
      AutoSize = False
      Color = 14671839
      TabOrder = 2
    end
  end
  object gbType: TGroupBox
    Left = 0
    Top = 0
    Width = 406
    Height = 152
    Align = alTop
    Caption = 'Type:'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 0
    object RxLabel1: TMxLabel
      Left = 104
      Top = 64
      Width = 97
      Height = 13
      Caption = '<Multiple Selection>'
    end
    object pcType: TPageControl
      Left = 2
      Top = 15
      Width = 402
      Height = 135
      ActivePage = tsPoint
      Align = alClient
      Style = tsFlatButtons
      TabOrder = 0
      object tsSun: TTabSheet
        Caption = 'Sun'
        object cbFlares: TMultiObjCheck
          Left = 0
          Top = 0
          Width = 57
          Height = 17
          Caption = 'Flares'
          Checked = True
          State = cbChecked
          TabOrder = 0
          OnClick = cbFlaresClick
        end
        object mmFlares: TMemo
          Left = 0
          Top = 16
          Width = 394
          Height = 88
          ScrollBars = ssVertical
          TabOrder = 1
          WantTabs = True
          WordWrap = False
        end
      end
      object tsPoint: TTabSheet
        Tag = 1
        Caption = 'Point'
        ImageIndex = 1
        object Bevel2: TBevel
          Left = 168
          Top = 1
          Width = 226
          Height = 82
        end
        object RxLabel13: TMxLabel
          Left = 255
          Top = 89
          Width = 37
          Height = 13
          Caption = 'Range:'
        end
        object RxLabel14: TMxLabel
          Left = 0
          Top = -2
          Width = 47
          Height = 13
          Caption = 'Constant:'
        end
        object LG: TImage
          Left = 169
          Top = 2
          Width = 224
          Height = 80
        end
        object RxLabel15: TMxLabel
          Left = 381
          Top = 90
          Width = 10
          Height = 13
          Caption = 'm'
        end
        object RxLabel16: TMxLabel
          Left = 1
          Top = 26
          Width = 34
          Height = 13
          Caption = 'Linear:'
        end
        object RxLabel17: TMxLabel
          Left = 1
          Top = 54
          Width = 51
          Height = 13
          Caption = 'Quadratic:'
        end
        object lbRange: TLabel
          Left = 306
          Top = 68
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'Range = '
          Color = 26112
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clYellow
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          Transparent = True
        end
        object ebALauto: TExtBtn
          Left = 1
          Top = 39
          Width = 15
          Height = 15
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'A'
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = [fsBold]
          Margin = 2
          ParentFont = False
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebALautoClick
        end
        object ebQLauto: TExtBtn
          Left = 1
          Top = 67
          Width = 15
          Height = 15
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'A'
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = [fsBold]
          Margin = 2
          ParentFont = False
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebQLautoClick
        end
        object RxLabel3: TMxLabel
          Left = 1
          Top = 89
          Width = 163
          Height = 13
          Caption = 'Auto brightness at maximum range'
        end
        object RxLabel4: TMxLabel
          Left = 217
          Top = 90
          Width = 10
          Height = 13
          Caption = '%'
        end
        object tbA0: TTrackBar
          Left = -3
          Top = 9
          Width = 101
          Height = 17
          Max = 100
          Orientation = trHorizontal
          Frequency = 10
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 0
          ThumbLength = 10
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA0Change
        end
        object tbA1: TTrackBar
          Left = 16
          Top = 37
          Width = 82
          Height = 17
          Max = 100
          Orientation = trHorizontal
          Frequency = 10
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 1
          ThumbLength = 10
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA1Change
        end
        object tbA2: TTrackBar
          Left = 16
          Top = 65
          Width = 82
          Height = 16
          Max = 100
          Orientation = trHorizontal
          Frequency = 10
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 2
          ThumbLength = 10
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA2Change
        end
        object seAutoBMax: TMultiObjSpinEdit
          Left = 168
          Top = 86
          Width = 49
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          MaxValue = 100
          Value = 10
          AutoSize = False
          TabOrder = 3
        end
        object sePointRange: TMultiObjSpinEdit
          Left = 304
          Top = 86
          Width = 73
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          Increment = 0.1
          MaxValue = 1000
          ValueType = vtFloat
          Value = 5
          AutoSize = False
          TabOrder = 4
          OnBottomClick = sePointRangeChange
          OnTopClick = sePointRangeChange
          OnChange = sePointRangeChange
          OnExit = sePointRangeChange
        end
        object seA0: TMultiObjSpinEdit
          Left = 95
          Top = 8
          Width = 71
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          Decimal = 6
          EditorEnabled = False
          Increment = 0.0001
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 5
          OnBottomClick = seA0Change
          OnTopClick = seA0Change
          OnChange = seA0Change
          OnExit = seA0Change
        end
        object seA1: TMultiObjSpinEdit
          Left = 95
          Top = 36
          Width = 71
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          Decimal = 6
          EditorEnabled = False
          Increment = 0.0001
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 6
          OnBottomClick = seA1Change
          OnTopClick = seA2Change
          OnChange = seA1Change
          OnExit = seA1Change
        end
        object seA2: TMultiObjSpinEdit
          Left = 95
          Top = 65
          Width = 71
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          Decimal = 6
          EditorEnabled = False
          Increment = 0.0001
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 7
          OnBottomClick = seA2Change
          OnTopClick = seA2Change
          OnChange = seA2Change
          OnExit = seA2Change
        end
      end
    end
  end
  object tmAnimation: TTimer
    Enabled = False
    Interval = 10
    Left = 232
  end
end
