object frmPropertiesLight: TfrmPropertiesLight
  Left = 735
  Top = 237
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Light properties'
  ClientHeight = 456
  ClientWidth = 301
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
  object gbType: TGroupBox
    Left = 0
    Top = 0
    Width = 301
    Height = 265
    Align = alTop
    Caption = 'Type:'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 0
    object RxLabel1: TLabel
      Left = 104
      Top = 64
      Width = 95
      Height = 13
      Caption = '<Multiple Selection>'
    end
    object pcType: TPageControl
      Left = 2
      Top = 15
      Width = 297
      Height = 248
      ActivePage = tsPoint
      Align = alClient
      Style = tsFlatButtons
      TabIndex = 1
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
          Width = 289
          Height = 82
        end
        object RxLabel13: TLabel
          Left = 1
          Top = 199
          Width = 35
          Height = 13
          Caption = 'Range:'
        end
        object RxLabel14: TLabel
          Left = 0
          Top = 86
          Width = 45
          Height = 13
          Caption = 'Constant:'
        end
        object LG: TImage
          Left = 1
          Top = 1
          Width = 287
          Height = 80
        end
        object RxLabel15: TLabel
          Left = 278
          Top = 201
          Width = 8
          Height = 13
          Caption = 'm'
        end
        object RxLabel16: TLabel
          Left = 1
          Top = 114
          Width = 32
          Height = 13
          Caption = 'Linear:'
        end
        object RxLabel17: TLabel
          Left = 1
          Top = 142
          Width = 49
          Height = 13
          Caption = 'Quadratic:'
        end
        object lbRange: TLabel
          Left = 201
          Top = 59
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
        object RxLabel3: TLabel
          Left = 1
          Top = 178
          Width = 161
          Height = 13
          Caption = 'Auto brightness at maximum range'
          WordWrap = True
        end
        object RxLabel4: TLabel
          Left = 278
          Top = 180
          Width = 8
          Height = 13
          Caption = '%'
        end
        object tbA0: TTrackBar
          Left = -3
          Top = 97
          Width = 217
          Height = 17
          Max = 100
          Orientation = trHorizontal
          PageSize = 5
          Frequency = 5
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 0
          ThumbLength = 12
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA0Change
        end
        object tbA1: TTrackBar
          Left = 16
          Top = 125
          Width = 198
          Height = 17
          Max = 100
          Orientation = trHorizontal
          PageSize = 5
          Frequency = 5
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 1
          ThumbLength = 12
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA1Change
        end
        object tbA2: TTrackBar
          Left = 16
          Top = 153
          Width = 198
          Height = 17
          Max = 100
          Orientation = trHorizontal
          PageSize = 5
          Frequency = 5
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 2
          ThumbLength = 12
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA2Change
        end
        object seAutoBMax: TMultiObjSpinEdit
          Left = 214
          Top = 176
          Width = 62
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          MaxValue = 100
          Value = 10
          AutoSize = False
          TabOrder = 3
        end
        object sePointRange: TMultiObjSpinEdit
          Left = 214
          Top = 198
          Width = 62
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
          Left = 215
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
          Left = 215
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
          Left = 215
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
    Top = 435
    Width = 301
    Height = 21
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ExtBtn1: TExtBtn
      Left = 101
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
      Left = 168
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
      Left = 235
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
  object paProps: TPanel
    Left = 0
    Top = 265
    Width = 301
    Height = 170
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 2
  end
  object tmAnimation: TTimer
    Enabled = False
    Interval = 10
    Left = 232
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 65528
    Top = 65528
  end
end
