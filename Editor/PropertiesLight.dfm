object frmPropertiesLight: TfrmPropertiesLight
  Left = 473
  Top = 272
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Light properties'
  ClientHeight = 439
  ClientWidth = 341
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
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object gbColor: TGroupBox
    Left = 0
    Top = 261
    Width = 341
    Height = 56
    Align = alTop
    Caption = ' Color: '
    TabOrder = 1
    object mcDiffuse: TMultiObjColor
      Left = 50
      Top = 13
      Width = 52
      Height = 16
      Shape = stRoundRect
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
      Width = 115
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
    Top = 317
    Width = 341
    Height = 100
    Align = alTop
    Caption = ' Addition: '
    TabOrder = 2
    object MxLabel1: TMxLabel
      Left = 6
      Top = 16
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
      Left = 203
      Top = 30
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
      Top = 78
      Width = 76
      Height = 17
      BiDiMode = bdLeftToRight
      Caption = 'Use in D3D'
      ParentBiDiMode = False
      TabOrder = 1
    end
    object edName: TEdit
      Left = 56
      Top = 13
      Width = 280
      Height = 18
      AutoSize = False
      Color = 14671839
      TabOrder = 2
    end
  end
  object gbType: TGroupBox
    Left = 0
    Top = 0
    Width = 341
    Height = 261
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
      Width = 337
      Height = 244
      ActivePage = tsPoint
      Align = alClient
      Style = tsFlatButtons
      TabOrder = 0
      object tsSun: TTabSheet
        Caption = 'Sun'
      end
      object tsPoint: TTabSheet
        Tag = 1
        Caption = 'Point'
        ImageIndex = 1
        object Bevel2: TBevel
          Left = 0
          Top = 0
          Width = 329
          Height = 82
        end
        object RxLabel13: TMxLabel
          Left = -1
          Top = 197
          Width = 37
          Height = 13
          Caption = 'Range:'
        end
        object RxLabel14: TMxLabel
          Left = 0
          Top = 86
          Width = 47
          Height = 13
          Caption = 'Constant:'
        end
        object LG: TImage
          Left = 1
          Top = 1
          Width = 327
          Height = 80
        end
        object RxLabel15: TMxLabel
          Left = 218
          Top = 199
          Width = 10
          Height = 13
          Caption = 'm'
        end
        object RxLabel16: TMxLabel
          Left = 1
          Top = 114
          Width = 34
          Height = 13
          Caption = 'Linear:'
        end
        object RxLabel17: TMxLabel
          Left = 1
          Top = 142
          Width = 51
          Height = 13
          Caption = 'Quadratic:'
        end
        object lbRange: TLabel
          Left = 233
          Top = 67
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
          Top = 127
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
          Top = 155
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
          Top = 177
          Width = 163
          Height = 13
          Caption = 'Auto brightness at maximum range'
        end
        object RxLabel4: TMxLabel
          Left = 217
          Top = 178
          Width = 10
          Height = 13
          Caption = '%'
        end
        object tbA0: TTrackBar
          Left = -3
          Top = 97
          Width = 146
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
          Top = 125
          Width = 127
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
          Top = 153
          Width = 127
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
          Top = 174
          Width = 48
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          MaxValue = 100
          Value = 10
          AutoSize = False
          TabOrder = 3
        end
        object sePointRange: TMultiObjSpinEdit
          Left = 158
          Top = 195
          Width = 58
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
          Left = 141
          Top = 96
          Width = 75
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
          Left = 141
          Top = 124
          Width = 75
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
          Left = 141
          Top = 153
          Width = 75
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
  object Panel1: TPanel
    Left = 0
    Top = 418
    Width = 341
    Height = 21
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 3
    object ebAdjustScene: TExtBtn
      Left = 1
      Top = 2
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
    object ExtBtn1: TExtBtn
      Left = 139
      Top = 2
      Width = 66
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Apply'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = btApplyClick
    end
    object ebOk: TExtBtn
      Left = 206
      Top = 2
      Width = 66
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
      Left = 273
      Top = 2
      Width = 66
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = btCancelClick
    end
  end
  object tmAnimation: TTimer
    Enabled = False
    Interval = 10
    Left = 232
  end
end
