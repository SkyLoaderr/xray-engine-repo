object frmPropertiesPSDef: TfrmPropertiesPSDef
  Left = 478
  Top = 241
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Particle System'
  ClientHeight = 332
  ClientWidth = 203
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pcPS: TElPageControl
    Left = 0
    Top = 0
    Width = 203
    Height = 332
    ActiveTabColor = 10528425
    BorderWidth = 0
    Color = 10528425
    DrawFocus = False
    Flat = True
    HotTrack = True
    InactiveTabColor = 10528425
    Multiline = True
    RaggedRight = False
    ScrollOpposite = False
    Style = etsNetTabs
    TabIndex = 3
    TabPosition = etpTop
    TabWidth = 64
    HotTrackFont.Charset = DEFAULT_CHARSET
    HotTrackFont.Color = 15790320
    HotTrackFont.Height = -11
    HotTrackFont.Name = 'MS Sans Serif'
    HotTrackFont.Style = []
    TabBkColor = 10528425
    ActivePage = tsEmitter
    FlatTabBorderColor = clBtnShadow
    Align = alClient
    ParentColor = False
    TabOrder = 0
    object tsBase: TElTabSheet
      PageControl = pcPS
      ImageIndex = -1
      TabVisible = True
      Caption = 'Base'
      Visible = False
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 199
        Height = 286
        Align = alClient
        BevelOuter = bvNone
        Color = 10528425
        TabOrder = 0
        object RxLabel19: TMxLabel
          Left = 6
          Top = 50
          Width = 39
          Height = 13
          Caption = 'Creator:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object lbCreator: TMxLabel
          Left = 56
          Top = 50
          Width = 8
          Height = 13
          Caption = '?'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clMaroon
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object ebSelectShader: TExtBtn
          Left = 5
          Top = 12
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
          FlatAlwaysEdge = True
          OnClick = ebSelectShaderClick
        end
        object lbShader: TMxLabel
          Left = 55
          Top = 13
          Width = 139
          Height = 13
          AutoSize = False
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clNavy
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object lbTexture: TMxLabel
          Left = 55
          Top = 29
          Width = 139
          Height = 13
          AutoSize = False
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clNavy
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object ebSelectTexture: TExtBtn
          Left = 5
          Top = 28
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
          FlatAlwaysEdge = True
          OnClick = ebSelectTextureClick
        end
      end
    end
    object tsParams: TElTabSheet
      TabColor = 10528425
      PageControl = pcPS
      ImageIndex = -1
      TabVisible = True
      Caption = 'Params'
      Color = 10528425
      Visible = False
      object RxLabel31: TMxLabel
        Left = 6
        Top = 19
        Width = 36
        Height = 13
        Caption = 'Life (s):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel1: TMxLabel
        Left = 6
        Top = 72
        Width = 42
        Height = 13
        Caption = 'Size (m):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel5: TMxLabel
        Left = 6
        Top = 111
        Width = 35
        Height = 26
        Caption = 'Gravity'#13#10' (m/s2)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
        WordWrap = True
      end
      object RxLabel6: TMxLabel
        Left = 89
        Top = 38
        Width = 27
        Height = 13
        Caption = 'Start:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel7: TMxLabel
        Left = 158
        Top = 38
        Width = 24
        Height = 13
        Caption = 'End:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel8: TMxLabel
        Left = 64
        Top = 112
        Width = 9
        Height = 13
        Caption = 'X'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel9: TMxLabel
        Left = 64
        Top = 130
        Width = 9
        Height = 13
        Caption = 'Y'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel10: TMxLabel
        Left = 64
        Top = 147
        Width = 9
        Height = 13
        Caption = 'Z'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel11: TMxLabel
        Left = 6
        Top = 169
        Width = 29
        Height = 13
        Caption = 'Color:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel12: TMxLabel
        Left = 64
        Top = 170
        Width = 10
        Height = 13
        Caption = 'R'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel13: TMxLabel
        Left = 64
        Top = 188
        Width = 10
        Height = 13
        Caption = 'G'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel14: TMxLabel
        Left = 64
        Top = 205
        Width = 9
        Height = 13
        Caption = 'B'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel15: TMxLabel
        Left = 64
        Top = 222
        Width = 9
        Height = 13
        Caption = 'A'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel21: TMxLabel
        Left = 6
        Top = 90
        Width = 68
        Height = 13
        Caption = 'A.Speed (r/s):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel34: TMxLabel
        Left = 6
        Top = 53
        Width = 63
        Height = 13
        Caption = 'Speed (m/s):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object ExtBtn2: TExtBtn
        Left = 131
        Top = 69
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn2Click
      end
      object ExtBtn3: TExtBtn
        Left = 131
        Top = 87
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn3Click
      end
      object ExtBtn4: TExtBtn
        Left = 131
        Top = 109
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn4Click
      end
      object ExtBtn5: TExtBtn
        Left = 131
        Top = 127
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn5Click
      end
      object ExtBtn6: TExtBtn
        Left = 131
        Top = 145
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn6Click
      end
      object ExtBtn7: TExtBtn
        Left = 131
        Top = 167
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn7Click
      end
      object ExtBtn8: TExtBtn
        Left = 131
        Top = 185
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn8Click
      end
      object ExtBtn9: TExtBtn
        Left = 131
        Top = 203
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn9Click
      end
      object ExtBtn10: TExtBtn
        Left = 131
        Top = 221
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn10Click
      end
      object ExtBtn1: TExtBtn
        Left = 131
        Top = 51
        Width = 10
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '='
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn1Click
      end
      object RxLabel40: TMxLabel
        Left = 6
        Top = 256
        Width = 64
        Height = 13
        Caption = 'Align to path:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel41: TMxLabel
        Left = 6
        Top = 271
        Width = 100
        Height = 13
        Caption = 'Random initial angle:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object seLife: TMultiObjSpinEdit
        Left = 76
        Top = 16
        Width = 119
        Height = 18
        LWSensitivity = 1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seSizeStart: TMultiObjSpinEdit
        Left = 76
        Top = 69
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seGravityStartX: TMultiObjSpinEdit
        Left = 76
        Top = 109
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        MinValue = -10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seGravityEndX: TMultiObjSpinEdit
        Left = 141
        Top = 109
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        MinValue = -10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seGravityStartY: TMultiObjSpinEdit
        Left = 76
        Top = 127
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        MinValue = -10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seGravityEndY: TMultiObjSpinEdit
        Left = 141
        Top = 127
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        MinValue = -10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 5
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seGravityStartZ: TMultiObjSpinEdit
        Left = 76
        Top = 145
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        MinValue = -10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 6
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seGravityEndZ: TMultiObjSpinEdit
        Left = 141
        Top = 145
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        MinValue = -10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 7
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorStartR: TMultiObjSpinEdit
        Left = 76
        Top = 167
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 8
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorEndR: TMultiObjSpinEdit
        Left = 141
        Top = 167
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 9
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorStartG: TMultiObjSpinEdit
        Left = 76
        Top = 185
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 10
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorEndG: TMultiObjSpinEdit
        Left = 141
        Top = 185
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 11
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorStartB: TMultiObjSpinEdit
        Left = 76
        Top = 203
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 12
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorEndB: TMultiObjSpinEdit
        Left = 141
        Top = 203
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 13
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorStartA: TMultiObjSpinEdit
        Left = 76
        Top = 221
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 14
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seColorEndA: TMultiObjSpinEdit
        Left = 141
        Top = 221
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 255
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 15
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seSizeEnd: TMultiObjSpinEdit
        Left = 141
        Top = 69
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 16
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seASpeedStart: TMultiObjSpinEdit
        Left = 76
        Top = 87
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 17
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seASpeedEnd: TMultiObjSpinEdit
        Left = 141
        Top = 87
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 18
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object grColor: TGradient
        Left = 76
        Top = 240
        Width = 119
        Height = 15
        BeginColor = clRed
        EndColor = clBlue
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        Caption = ' '
        TextTop = 0
        TextLeft = 0
        Border = False
        BorderWidth = 1
        BorderColor = clBlack
        OnMouseDown = clColorMouseDown
      end
      object seSpeedStart: TMultiObjSpinEdit
        Left = 76
        Top = 51
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 20
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object seSpeedEnd: TMultiObjSpinEdit
        Left = 141
        Top = 51
        Width = 54
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 10000
        ValueType = vtFloat
        AutoSize = False
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 21
        OnLWChange = seLWChange
        OnExit = seExit
        OnKeyDown = seKeyDown
      end
      object cbAlignToPath: TMultiObjCheck
        Left = 109
        Top = 257
        Width = 13
        Height = 13
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 22
        OnClick = seExit
      end
      object cbRandomInitAngle: TMultiObjCheck
        Left = 109
        Top = 272
        Width = 13
        Height = 13
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 23
        OnClick = seExit
      end
    end
    object tsVary: TElTabSheet
      TabColor = 10528425
      PageControl = pcPS
      ImageIndex = -1
      TabVisible = True
      Caption = 'Variability'
      Color = 10528425
      Visible = False
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 199
        Height = 286
        Align = alClient
        BevelOuter = bvNone
        Color = 10528425
        TabOrder = 0
        object RxLabel2: TMxLabel
          Left = 6
          Top = 19
          Width = 39
          Height = 13
          Caption = 'Life (%):'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel3: TMxLabel
          Left = 6
          Top = 55
          Width = 42
          Height = 13
          Caption = 'Size (%):'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel16: TMxLabel
          Left = 6
          Top = 37
          Width = 53
          Height = 13
          Caption = 'Speed (%):'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel23: TMxLabel
          Left = 6
          Top = 94
          Width = 46
          Height = 13
          Caption = 'Color (%):'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel24: TMxLabel
          Left = 64
          Top = 95
          Width = 10
          Height = 13
          Caption = 'R'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel25: TMxLabel
          Left = 64
          Top = 113
          Width = 10
          Height = 13
          Caption = 'G'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel26: TMxLabel
          Left = 64
          Top = 130
          Width = 9
          Height = 13
          Caption = 'B'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel27: TMxLabel
          Left = 64
          Top = 147
          Width = 9
          Height = 13
          Caption = 'A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel28: TMxLabel
          Left = 6
          Top = 73
          Width = 63
          Height = 13
          Caption = 'A.Speed (%):'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object seLifeVar: TMultiObjSpinEdit
          Left = 76
          Top = 16
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seSizeVar: TMultiObjSpinEdit
          Left = 76
          Top = 52
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seSpeedVar: TMultiObjSpinEdit
          Left = 76
          Top = 34
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 2
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seColorVarR: TMultiObjSpinEdit
          Left = 76
          Top = 92
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 3
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seColorVarG: TMultiObjSpinEdit
          Left = 76
          Top = 110
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 4
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seColorVarB: TMultiObjSpinEdit
          Left = 76
          Top = 128
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 5
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seColorVarA: TMultiObjSpinEdit
          Left = 76
          Top = 146
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 6
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seASpeedVar: TMultiObjSpinEdit
          Left = 76
          Top = 70
          Width = 119
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 7
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
      end
    end
    object tsEmitter: TElTabSheet
      PageControl = pcPS
      ImageIndex = -1
      TabVisible = True
      Caption = 'Emitter'
      object Panel5: TPanel
        Left = 0
        Top = 0
        Width = 199
        Height = 286
        Align = alClient
        BevelOuter = bvNone
        Color = 10528425
        TabOrder = 0
        inline fraEmitter: TfraEmitter
          Left = 0
          Top = 0
          Width = 199
          Height = 286
          Align = alClient
          Color = 10528425
          ParentColor = False
          TabOrder = 0
          inherited ebBirthFunc: TExtBtn
            OnClick = seEmitterExit
          end
          inherited seBirthRate: TMultiObjSpinEdit
            OnLWChange = seEmitterLWChange
            OnExit = seEmitterExit
            OnKeyDown = seEmitterKeyDown
          end
          inherited seParticleLimit: TMultiObjSpinEdit
            OnLWChange = seEmitterLWChange
            OnExit = seEmitterExit
            OnKeyDown = seEmitterKeyDown
          end
          inherited cbBurst: TMultiObjCheck
            OnExit = seEmitterExit
          end
          inherited cbPlayOnce: TMultiObjCheck
            OnExit = seEmitterExit
          end
          inherited pcEmitterType: TElPageControl
            OnChange = seEmitterExit
            inherited tsCone: TElTabSheet
              inherited Panel1: TPanel
                inherited seConeAngle: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
                inherited seConeDirH: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
                inherited seConeDirP: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
                inherited seConeDirB: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
              end
            end
            inherited tsSphere: TElTabSheet
              inherited Panel5: TPanel
                inherited seSphereRadius: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
              end
            end
            inherited tsBox: TElTabSheet
              inherited Panel4: TPanel
                inherited seBoxSizeX: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
                inherited seBoxSizeY: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
                inherited seBoxSizeZ: TMultiObjSpinEdit
                  OnLWChange = seEmitterLWChange
                  OnExit = seEmitterExit
                  OnKeyDown = seEmitterKeyDown
                end
              end
            end
          end
        end
      end
    end
    object tsFrame: TElTabSheet
      PageControl = pcPS
      ImageIndex = -1
      TabVisible = True
      Caption = 'Frame'
      Visible = False
      object Panel4: TPanel
        Left = 0
        Top = 0
        Width = 199
        Height = 286
        Align = alClient
        BevelOuter = bvNone
        Color = 10528425
        TabOrder = 0
        object RxLabel29: TMxLabel
          Left = 6
          Top = 14
          Width = 44
          Height = 13
          Caption = 'Enabled:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel36: TMxLabel
          Left = 6
          Top = 203
          Width = 58
          Height = 13
          Caption = 'Speed (f/s):'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel37: TMxLabel
          Left = 6
          Top = 221
          Width = 55
          Height = 13
          Caption = 'Speed Var:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel42: TMxLabel
          Left = 6
          Top = 30
          Width = 43
          Height = 13
          Caption = 'Animate:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel43: TMxLabel
          Left = 6
          Top = 46
          Width = 100
          Height = 13
          Caption = 'Random initial frame:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel44: TMxLabel
          Left = 6
          Top = 62
          Width = 105
          Height = 13
          Caption = 'Random playback dir:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object cbAnimEnabled: TMultiObjCheck
          Left = 112
          Top = 13
          Width = 13
          Height = 17
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          OnClick = seExit
        end
        object GroupBox5: TGroupBox
          Left = 2
          Top = 75
          Width = 194
          Height = 71
          Caption = ' Frame '
          TabOrder = 1
          object RxLabel18: TMxLabel
            Left = 5
            Top = 15
            Width = 33
            Height = 13
            Caption = 'Width:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 10528425
          end
          object RxLabel22: TMxLabel
            Left = 5
            Top = 33
            Width = 36
            Height = 13
            Caption = 'Height:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 10528425
          end
          object RxLabel35: TMxLabel
            Left = 5
            Top = 51
            Width = 33
            Height = 13
            Caption = 'Count:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 10528425
          end
          object seAnimFrameWidth: TMultiObjSpinEdit
            Left = 82
            Top = 13
            Width = 108
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            MaxValue = 10000
            MinValue = 1
            Value = 1
            AutoSize = False
            Color = 10526880
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            OnLWChange = seLWChange
            OnExit = seExit
            OnKeyDown = seKeyDown
          end
          object seAnimFrameHeight: TMultiObjSpinEdit
            Left = 82
            Top = 31
            Width = 108
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            MaxValue = 10000
            MinValue = 1
            Value = 1
            AutoSize = False
            Color = 10526880
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 1
            OnLWChange = seLWChange
            OnExit = seExit
            OnKeyDown = seKeyDown
          end
          object seAnimFrameCount: TMultiObjSpinEdit
            Left = 82
            Top = 49
            Width = 108
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            MaxValue = 10000
            MinValue = 1
            Value = 1
            AutoSize = False
            Color = 10526880
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 2
            OnLWChange = seLWChange
            OnExit = seExit
            OnKeyDown = seKeyDown
          end
        end
        object seAnimSpeed: TMultiObjSpinEdit
          Left = 84
          Top = 201
          Width = 108
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1000
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 2
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seAnimSpeedVar: TMultiObjSpinEdit
          Left = 84
          Top = 219
          Width = 108
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1000
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 3
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object GroupBox6: TGroupBox
          Left = 2
          Top = 147
          Width = 194
          Height = 53
          Caption = ' Texture '
          TabOrder = 4
          object RxLabel38: TMxLabel
            Left = 5
            Top = 15
            Width = 33
            Height = 13
            Caption = 'Width:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 10528425
          end
          object RxLabel39: TMxLabel
            Left = 5
            Top = 33
            Width = 36
            Height = 13
            Caption = 'Height:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 10528425
          end
          object seAnimTexWidth: TMultiObjSpinEdit
            Left = 82
            Top = 13
            Width = 108
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            MaxValue = 10000
            MinValue = 1
            Value = 1
            AutoSize = False
            Color = 10526880
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            OnLWChange = seLWChange
            OnExit = seExit
            OnKeyDown = seKeyDown
          end
          object seAnimTexHeight: TMultiObjSpinEdit
            Left = 82
            Top = 31
            Width = 108
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            MaxValue = 10000
            MinValue = 1
            Value = 1
            AutoSize = False
            Color = 10526880
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 1
            OnLWChange = seLWChange
            OnExit = seExit
            OnKeyDown = seKeyDown
          end
        end
        object cbAnimAnimate: TMultiObjCheck
          Left = 112
          Top = 29
          Width = 13
          Height = 17
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 5
          OnClick = seExit
        end
        object cbAnimRandomInitialFrame: TMultiObjCheck
          Left = 112
          Top = 45
          Width = 13
          Height = 17
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 6
          OnClick = seExit
        end
        object cbAnimRandomPlaybackDir: TMultiObjCheck
          Left = 112
          Top = 61
          Width = 13
          Height = 17
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 7
          OnClick = seExit
        end
      end
    end
    object tsBlur: TElTabSheet
      PageControl = pcPS
      ImageIndex = -1
      TabVisible = True
      Caption = 'Blur'
      Visible = False
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 199
        Height = 286
        Align = alClient
        BevelOuter = bvNone
        Color = 10528425
        TabOrder = 0
        object RxLabel30: TMxLabel
          Left = 6
          Top = 14
          Width = 57
          Height = 13
          Caption = 'Motion blur:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel32: TMxLabel
          Left = 6
          Top = 44
          Width = 59
          Height = 13
          Caption = 'Blur time (s):'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel33: TMxLabel
          Left = 6
          Top = 61
          Width = 40
          Height = 13
          Caption = 'Blur div:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object ExtBtn11: TExtBtn
          Left = 131
          Top = 41
          Width = 10
          Height = 17
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = '='
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          FlatAlwaysEdge = True
          OnClick = ExtBtn11Click
        end
        object ExtBtn12: TExtBtn
          Left = 131
          Top = 60
          Width = 10
          Height = 17
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = '='
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          ParentFont = False
          FlatAlwaysEdge = True
          OnClick = ExtBtn12Click
        end
        object RxLabel4: TMxLabel
          Left = 92
          Top = 28
          Width = 27
          Height = 13
          Caption = 'Start:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object RxLabel20: TMxLabel
          Left = 157
          Top = 28
          Width = 24
          Height = 13
          Caption = 'End:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ShadowColor = 10528425
        end
        object cbMotionBlur: TMultiObjCheck
          Left = 76
          Top = 12
          Width = 13
          Height = 17
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          OnClick = seExit
        end
        object seBlurTimeStart: TMultiObjSpinEdit
          Left = 76
          Top = 41
          Width = 54
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          Decimal = 3
          Increment = 0.001
          MaxValue = 10
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seBlurSubdivStart: TMultiObjSpinEdit
          Left = 76
          Top = 59
          Width = 54
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          MaxValue = 1000
          MinValue = 2
          Value = 2
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 2
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seBlurTimeEnd: TMultiObjSpinEdit
          Left = 141
          Top = 41
          Width = 54
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          Decimal = 3
          Increment = 0.001
          MaxValue = 10
          ValueType = vtFloat
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 3
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
        object seBlurSubdivEnd: TMultiObjSpinEdit
          Left = 141
          Top = 59
          Width = 54
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          MaxValue = 1000
          MinValue = 2
          Value = 2
          AutoSize = False
          Color = 10526880
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 4
          OnLWChange = seLWChange
          OnExit = seExit
          OnKeyDown = seKeyDown
        end
      end
    end
  end
end
