object frmPropertiesLight: TfrmPropertiesLight
  Left = 418
  Top = 232
  BorderStyle = bsDialog
  Caption = 'Light properties'
  ClientHeight = 266
  ClientWidth = 380
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
  object Bevel1: TBevel
    Left = 0
    Top = 216
    Width = 380
    Height = 2
    Align = alTop
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 133
    Width = 380
    Height = 42
    Align = alTop
    Caption = 'Color:'
    TabOrder = 1
    object mcAmbient: TMultiObjColor
      Left = 56
      Top = 16
      Width = 41
      Height = 17
      OnMouseDown = mcColorMouseDown
    end
    object mcDiffuse: TMultiObjColor
      Left = 144
      Top = 16
      Width = 41
      Height = 17
      OnMouseDown = mcColorMouseDown
    end
    object mcSpecular: TMultiObjColor
      Left = 240
      Top = 16
      Width = 41
      Height = 17
      OnMouseDown = mcColorMouseDown
    end
    object RxLabel5: TRxLabel
      Left = 8
      Top = 18
      Width = 43
      Height = 13
      Caption = 'Ambient:'
    end
    object RxLabel6: TRxLabel
      Left = 104
      Top = 18
      Width = 38
      Height = 13
      Caption = 'Diffuse:'
    end
    object RxLabel7: TRxLabel
      Left = 192
      Top = 18
      Width = 47
      Height = 13
      Caption = 'Specular:'
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 175
    Width = 380
    Height = 41
    Align = alTop
    Caption = 'Shading:'
    TabOrder = 2
    object cbCastShadows: TMultiObjCheck
      Left = 8
      Top = 16
      Width = 97
      Height = 17
      Caption = 'cbCastShadows'
      TabOrder = 0
    end
  end
  object gbType: TGroupBox
    Left = 0
    Top = 0
    Width = 380
    Height = 133
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
      Width = 376
      Height = 116
      ActivePage = tsPoint
      Align = alClient
      Style = tsFlatButtons
      TabOrder = 0
      OnChange = pcTypeChange
      object tsSun: TTabSheet
        Caption = 'Sun'
        object RxLabel9: TRxLabel
          Left = 0
          Top = 3
          Width = 47
          Height = 13
          Caption = 'Direction:'
        end
        object RxLabel10: TRxLabel
          Left = 68
          Top = 3
          Width = 12
          Height = 13
          Caption = 'X:'
        end
        object RxLabel11: TRxLabel
          Left = 68
          Top = 27
          Width = 12
          Height = 13
          Caption = 'Y:'
        end
        object RxLabel12: TRxLabel
          Left = 68
          Top = 51
          Width = 12
          Height = 13
          Caption = 'Z:'
        end
        object RxLabel2: TRxLabel
          Left = 168
          Top = 5
          Width = 45
          Height = 13
          Caption = 'Sun size:'
        end
        object RxLabel3: TRxLabel
          Left = 168
          Top = 24
          Width = 35
          Height = 13
          Caption = 'Power:'
        end
        object RxLabel4: TRxLabel
          Left = 168
          Top = 64
          Width = 45
          Height = 13
          Caption = 'Gradient:'
        end
        object RxLabel8: TRxLabel
          Left = 168
          Top = 45
          Width = 33
          Height = 13
          Caption = 'Flares:'
        end
        object seDirectionalDirX: TRxSpinEdit
          Left = 80
          Top = 0
          Width = 57
          Height = 18
          Increment = 0.1
          MaxValue = 10000
          MinValue = -10000
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 0
          OnChange = seDirectionalDirChange
        end
        object seDirectionalDirY: TRxSpinEdit
          Left = 80
          Top = 24
          Width = 57
          Height = 18
          Increment = 0.1
          MaxValue = 10000
          MinValue = -10000
          ValueType = vtFloat
          Value = -1
          AutoSize = False
          TabOrder = 1
          OnChange = seDirectionalDirChange
        end
        object seDirectionalDirZ: TRxSpinEdit
          Left = 80
          Top = 48
          Width = 57
          Height = 18
          Increment = 0.1
          MaxValue = 10000
          MinValue = -10000
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 2
          OnChange = seDirectionalDirChange
        end
        object cbFlares: TCheckBox
          Left = 224
          Top = 43
          Width = 17
          Height = 17
          Checked = True
          State = cbChecked
          TabOrder = 3
        end
        object cbGradient: TCheckBox
          Left = 224
          Top = 63
          Width = 17
          Height = 17
          Checked = True
          State = cbChecked
          TabOrder = 4
        end
        object seSunSize: TRxSpinEdit
          Left = 224
          Top = 3
          Width = 57
          Height = 18
          Increment = 0.1
          MaxValue = 10
          ValueType = vtFloat
          Value = 0.15
          AutoSize = False
          TabOrder = 5
        end
        object sePower: TRxSpinEdit
          Left = 224
          Top = 22
          Width = 57
          Height = 18
          Increment = 0.1
          MaxValue = 1
          ValueType = vtFloat
          Value = 0.6
          AutoSize = False
          TabOrder = 6
        end
      end
      object tsPoint: TTabSheet
        Tag = 1
        Caption = 'Point'
        ImageIndex = 1
        object Bevel2: TBevel
          Left = 245
          Top = 1
          Width = 121
          Height = 82
        end
        object RxLabel13: TRxLabel
          Left = 151
          Top = 53
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
          Left = 246
          Top = 2
          Width = 118
          Height = 80
        end
        object RxLabel15: TRxLabel
          Left = 225
          Top = 68
          Width = 10
          Height = 13
          Caption = 'm'
        end
        object RxLabel16: TRxLabel
          Left = 0
          Top = 26
          Width = 34
          Height = 13
          Caption = 'Linear:'
        end
        object RxLabel17: TRxLabel
          Left = 0
          Top = 54
          Width = 51
          Height = 13
          Caption = 'Quadratic:'
        end
        object RxLabel18: TRxLabel
          Left = 150
          Top = -2
          Width = 54
          Height = 13
          Caption = 'Brightness:'
        end
        object lbMaxA: TLabel
          Left = 276
          Top = 1
          Width = 85
          Height = 13
          AutoSize = False
          Caption = 'Max A = '
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clNavy
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          Transparent = True
        end
        object lbRange: TLabel
          Left = 276
          Top = 68
          Width = 85
          Height = 13
          AutoSize = False
          Caption = 'Range = '
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clNavy
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          Transparent = True
        end
        object sePointRange: TRxSpinEdit
          Left = 152
          Top = 66
          Width = 73
          Height = 19
          MaxValue = 1000
          ValueType = vtFloat
          Value = 5
          AutoSize = False
          TabOrder = 0
          OnChange = sePointRangeChange
        end
        object tbBrightness: TTrackBar
          Left = 147
          Top = 9
          Width = 92
          Height = 17
          Max = 200
          Min = -200
          Orientation = trHorizontal
          Frequency = 40
          Position = 100
          SelEnd = 0
          SelStart = 0
          TabOrder = 1
          ThumbLength = 10
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbBrightnessChange
        end
        object tbA0: TTrackBar
          Left = -3
          Top = 9
          Width = 92
          Height = 17
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
          OnChange = tbA0Change
        end
        object tbA1: TTrackBar
          Left = -3
          Top = 37
          Width = 92
          Height = 17
          Max = 100
          Orientation = trHorizontal
          Frequency = 10
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 3
          ThumbLength = 10
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA1Change
        end
        object tbA2: TTrackBar
          Left = -3
          Top = 65
          Width = 92
          Height = 16
          Max = 100
          Orientation = trHorizontal
          Frequency = 10
          Position = 0
          SelEnd = 0
          SelStart = 0
          TabOrder = 4
          ThumbLength = 10
          TickMarks = tmBottomRight
          TickStyle = tsAuto
          OnChange = tbA2Change
        end
        object seA0: TMultiObjSpinEdit
          Left = 88
          Top = 7
          Width = 51
          Height = 18
          Decimal = 3
          Increment = 0.001
          MaxValue = 1
          MinValue = 0.0001
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 5
          OnChange = seA0Change
        end
        object seA1: TMultiObjSpinEdit
          Left = 88
          Top = 35
          Width = 51
          Height = 18
          Decimal = 3
          Increment = 0.001
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 6
          OnChange = seA1Change
        end
        object seA2: TMultiObjSpinEdit
          Left = 88
          Top = 63
          Width = 51
          Height = 18
          Decimal = 3
          Increment = 0.001
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 7
          OnChange = seA2Change
        end
        object seBrightness: TMultiObjSpinEdit
          Left = 152
          Top = 31
          Width = 89
          Height = 18
          Decimal = 3
          Increment = 0.01
          MaxValue = 2
          MinValue = -2
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          TabOrder = 8
          OnChange = seBrightnessChange
        end
      end
    end
  end
  object btApply: TButton
    Left = 88
    Top = 242
    Width = 89
    Height = 20
    Caption = 'Apply'
    TabOrder = 3
    OnClick = btApplyClick
  end
  object btOk: TButton
    Left = 184
    Top = 242
    Width = 89
    Height = 20
    Caption = 'Ok'
    ModalResult = 1
    TabOrder = 4
    OnClick = btOkClick
  end
  object btCancel: TButton
    Left = 280
    Top = 242
    Width = 89
    Height = 20
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 5
  end
  object cbAutoApply: TCheckBox
    Left = 8
    Top = 222
    Width = 193
    Height = 14
    Caption = 'Auto apply on parameters changed'
    Checked = True
    State = cbChecked
    TabOrder = 6
    OnClick = cbAutoApplyClick
  end
  object cdColor: TColorDialog
    Ctl3D = True
    Left = 264
  end
end
