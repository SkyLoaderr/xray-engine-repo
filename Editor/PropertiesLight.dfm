object frmPropertiesLight: TfrmPropertiesLight
  Left = 840
  Top = 536
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Light properties'
  ClientHeight = 350
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
    Top = 330
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
    Top = 330
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
    Top = 330
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
    Top = 330
    Width = 82
    Height = 18
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Adjust Scene'
    Transparent = False
    FlatAlwaysEdge = True
    Visible = False
    OnClick = btAdjustSceneClick
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
    object RxLabel6: TRxLabel
      Left = 8
      Top = 14
      Width = 38
      Height = 13
      Caption = 'Diffuse:'
    end
    object RxLabel18: TRxLabel
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
    Height = 137
    Align = alTop
    Caption = ' Addition: '
    TabOrder = 2
    object RxLabel2: TRxLabel
      Left = 4
      Top = 83
      Width = 86
      Height = 13
      Caption = 'Shadowed Scale:'
    end
    object cbCastShadows: TMultiObjCheck
      Left = 3
      Top = 100
      Width = 132
      Height = 17
      Alignment = taLeftJustify
      BiDiMode = bdLeftToRight
      Caption = 'Cast Shadows'
      ParentBiDiMode = False
      TabOrder = 1
    end
    object seShadowedScale: TMultiObjSpinEdit
      Left = 91
      Top = 82
      Width = 47
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 1
      ValueType = vtFloat
      Value = 0.05
      AutoSize = False
      TabOrder = 2
    end
    object gbLightType: TGroupBox
      Left = 6
      Top = 13
      Width = 133
      Height = 66
      Caption = ' Light Type '
      TabOrder = 3
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
    object paHideAnimKeys: TPanel
      Left = 139
      Top = 12
      Width = 239
      Height = 106
      BevelOuter = bvNone
      Caption = '<Animation Disabled>'
      TabOrder = 0
    end
    object gbProceduralKeys: TGroupBox
      Left = 143
      Top = 13
      Width = 259
      Height = 120
      Caption = ' Animated Keys '
      TabOrder = 4
      object RxLabel8: TRxLabel
        Left = 133
        Top = 100
        Width = 23
        Height = 13
        Caption = 'Key:'
      end
      object lbAnimKeyNum: TRxLabel
        Left = 157
        Top = 100
        Width = 8
        Height = 13
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowPos = spRightBottom
      end
      object lbMinAnimKey: TRxLabel
        Left = 5
        Top = 99
        Width = 8
        Height = 13
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowPos = spRightBottom
      end
      object lbMaxAnimKey: TRxLabel
        Left = 244
        Top = 99
        Width = 8
        Height = 13
        Alignment = taRightJustify
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowPos = spRightBottom
      end
      object ebAnimPlay: TExtBtn
        Left = 88
        Top = 100
        Width = 20
        Height = 13
        Align = alNone
        BevelShow = False
        CloseButton = False
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
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAnimPlayClick
      end
      object ebAnimStop: TExtBtn
        Left = 108
        Top = 100
        Width = 20
        Height = 13
        Align = alNone
        BevelShow = False
        CloseButton = False
        Enabled = False
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
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAnimStopClick
      end
      object ebAnimStart: TExtBtn
        Left = 25
        Top = 100
        Width = 20
        Height = 13
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFF6F6F6E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFCBCBCB454545FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD2D2D2989898
          F2F2F2FFFFFFFFFFFFCBCBCB454545FFFFFFFFFFFFFFFFFFEAEAEA7272723838
          38000000DADADAFFFFFFFFFFFFCBCBCB454545FFFFFFFFFFFF5C5C5C30303000
          0000000000000000DADADAFFFFFFFFFFFFCBCBCB585858ACACAC1A1A1A060606
          000000000000000000000000DADADAFFFFFFFFFFFFCBCBCB454545FFFFFFFFFF
          FF5C5C5C303030000000000000000000DADADAFFFFFFFFFFFFCBCBCB454545FF
          FFFFFFFFFFFFFFFFEAEAEA727272383838000000DADADAFFFFFFFFFFFFCBCBCB
          454545FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD2D2D2989898F2F2F2FFFFFFFFFF
          FFF6F6F6E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAnimStartClick
      end
      object ebAnimEnd: TExtBtn
        Left = 212
        Top = 100
        Width = 20
        Height = 13
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE0E0E0F6F6F6FF
          FFFFFFFFFFF2F2F2989898D2D2D2FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF454545
          CBCBCBFFFFFFFFFFFFDADADA000000383838727272EAEAEAFFFFFFFFFFFFFFFF
          FF454545CBCBCBFFFFFFFFFFFFDADADA0000000000000000003030305C5C5CFF
          FFFFFFFFFF454545CBCBCBFFFFFFFFFFFFDADADA000000000000000000000000
          0606061A1A1AACACAC585858CBCBCBFFFFFFFFFFFFDADADA0000000000000000
          003030305C5C5CFFFFFFFFFFFF454545CBCBCBFFFFFFFFFFFFDADADA00000038
          3838727272EAEAEAFFFFFFFFFFFFFFFFFF454545CBCBCBFFFFFFFFFFFFF2F2F2
          989898D2D2D2FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF454545CBCBCBFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE0E0E0F6F6F6FF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAnimEndClick
      end
      object ebAnimForward: TExtBtn
        Left = 192
        Top = 100
        Width = 20
        Height = 13
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          16020000424D16020000000000003600000028000000100000000A0000000100
          180000000000E0010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE9E9E9FFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFE6E6E6FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD3D3D3
          292929C8C8C8FFFFFFFFFFFFFFFFFFFFFFFF222222C8C8C8FFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFE0E0E0000000000000505050ADADADFDFDFDFF
          FFFF0B0B0B000000343434ADADADFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDFDFDF
          0000000000000000000000005656567878780606060000000000000000004545
          45777777FFFFFFFFFFFFFFFFFFDFDFDF00000000000000000000000056565678
          7878060606000000000000000000454545777777FFFFFFFFFFFFFFFFFFE0E0E0
          000000000000505050ADADADFDFDFDFFFFFF0B0B0B000000343434ADADADFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFD3D3D3292929C8C8C8FFFFFFFFFFFFFFFFFFFF
          FFFF222222C8C8C8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          E9E9E9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE6E6E6FFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAnimForwardClick
      end
      object ebAnimRewind: TExtBtn
        Left = 45
        Top = 100
        Width = 20
        Height = 13
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          16020000424D16020000000000003600000028000000100000000A0000000100
          180000000000E0010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE6
          E6E6FCFCFCFFFFFFFFFFFFFFFFFFFFFFFFE9E9E9FFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFC8C8C8222222FFFFFFFFFFFFFFFFFFFFFFFFC8C8
          C8292929D3D3D3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFADADAD3434340000000B
          0B0BFFFFFFFDFDFDADADAD505050000000000000E0E0E0FFFFFFFFFFFFFFFFFF
          7777774545450000000000000000000606067878785656560000000000000000
          00000000DFDFDFFFFFFFFFFFFFFFFFFF77777745454500000000000000000006
          0606787878565656000000000000000000000000DFDFDFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFADADAD3434340000000B0B0BFFFFFFFDFDFDADADAD5050500000
          00000000E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC8C8C822
          2222FFFFFFFFFFFFFFFFFFFFFFFFC8C8C8292929D3D3D3FFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE6E6E6FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFE9E9E9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAnimRewindClick
      end
      object gbKeyActions: TGroupBox
        Left = 7
        Top = 16
        Width = 248
        Height = 60
        Caption = ' Key actions '
        TabOrder = 0
        object ebAppendKey: TExtBtn
          Left = 4
          Top = 16
          Width = 60
          Height = 15
          Hint = 'Copy (Ctrl+C)'
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Append'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebAppendKeyClick
        end
        object ebInsertKey: TExtBtn
          Left = 64
          Top = 16
          Width = 60
          Height = 15
          Hint = 'Copy (Ctrl+C)'
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Insert'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebInsertKeyClick
        end
        object ebRemoveKey: TExtBtn
          Left = 124
          Top = 16
          Width = 60
          Height = 15
          Hint = 'Copy (Ctrl+C)'
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Remove'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebRemoveKeyClick
        end
        object ebRemoveAllKeys: TExtBtn
          Left = 184
          Top = 16
          Width = 60
          Height = 15
          Hint = 'Copy (Ctrl+C)'
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Remove All'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebRemoveAllKeysClick
        end
        object RxLabel20: TRxLabel
          Left = 4
          Top = 38
          Width = 133
          Height = 13
          Alignment = taCenter
          Caption = 'Animation Speed, (key/sec)'
        end
        object seAnimSpeed: TMultiObjSpinEdit
          Left = 161
          Top = 35
          Width = 83
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          Increment = 0.1
          MaxValue = 1000
          ValueType = vtFloat
          Value = 0.5
          AutoSize = False
          TabOrder = 0
        end
      end
      object tbAnimKeys: TTrackBar
        Left = 2
        Top = 83
        Width = 254
        Height = 16
        Max = 32
        Orientation = trHorizontal
        Frequency = 1
        Position = 0
        SelEnd = 0
        SelStart = 0
        TabOrder = 1
        ThumbLength = 9
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = tbAnimKeysChange
      end
    end
    object cbUseInD3D: TMultiObjCheck
      Left = 3
      Top = 116
      Width = 132
      Height = 17
      Alignment = taLeftJustify
      BiDiMode = bdLeftToRight
      Caption = 'Use in D3D'
      ParentBiDiMode = False
      TabOrder = 5
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
    object RxLabel1: TRxLabel
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
      ActivePage = tsSun
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
        object RxLabel13: TRxLabel
          Left = 255
          Top = 89
          Width = 37
          Height = 13
          Caption = 'Range:'
        end
        object RxLabel14: TRxLabel
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
        object RxLabel15: TRxLabel
          Left = 381
          Top = 90
          Width = 10
          Height = 13
          Caption = 'm'
        end
        object RxLabel16: TRxLabel
          Left = 1
          Top = 26
          Width = 34
          Height = 13
          Caption = 'Linear:'
        end
        object RxLabel17: TRxLabel
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
        object RxLabel3: TRxLabel
          Left = 1
          Top = 89
          Width = 163
          Height = 13
          Caption = 'Auto brightness at maximum range'
        end
        object RxLabel4: TRxLabel
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
    OnTimer = tmAnimationTimer
    Left = 232
  end
end
