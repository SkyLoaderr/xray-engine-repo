object frmSceneProperties: TfrmSceneProperties
  Left = 414
  Top = 473
  BorderStyle = bsDialog
  Caption = 'Build options'
  ClientHeight = 300
  ClientWidth = 496
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
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pcBuildOptions: TPageControl
    Left = 134
    Top = 0
    Width = 362
    Height = 279
    ActivePage = tsLevelOptions
    Align = alClient
    HotTrack = True
    TabIndex = 1
    TabOrder = 1
    object tsBuildOptions: TTabSheet
      Caption = 'Build Options'
      ImageIndex = 8
      object RxLabel25: TLabel
        Left = 107
        Top = 106
        Width = 126
        Height = 16
        Caption = 'Editor Build Options...'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
    object tsLevelOptions: TTabSheet
      Caption = 'Level options'
      OnShow = tsLevelOptionsShow
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 354
        Height = 80
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object RxLabel1: TLabel
          Left = 3
          Top = 10
          Width = 55
          Height = 13
          Caption = 'Level name'
        end
        object RxLabel2: TLabel
          Left = 3
          Top = 32
          Width = 50
          Height = 13
          Caption = 'Level path'
        end
        object Label1: TLabel
          Left = 2
          Top = 59
          Width = 57
          Height = 13
          Caption = 'HOM object'
        end
        object ebChooseHOM: TExtBtn
          Left = 265
          Top = 56
          Width = 42
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Select'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebChooseHOMClick
        end
        object ebClearHOM: TExtBtn
          Left = 309
          Top = 56
          Width = 42
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Clear'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebClearHOMClick
        end
        object edLevelName: TEdit
          Left = 98
          Top = 9
          Width = 254
          Height = 18
          AutoSize = False
          TabOrder = 0
        end
        object edLevelPath: TEdit
          Left = 98
          Top = 29
          Width = 254
          Height = 18
          AutoSize = False
          TabOrder = 1
        end
        object edHOMObjectName: TEdit
          Left = 98
          Top = 56
          Width = 164
          Height = 18
          AutoSize = False
          ReadOnly = True
          TabOrder = 2
        end
      end
    end
    object tsLevelEnvironment: TTabSheet
      Caption = 'Level environment'
      ImageIndex = 10
      OnShow = tsLevelEnvironmentShow
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 354
        Height = 50
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object RxLabel3: TLabel
          Left = 2
          Top = 12
          Width = 76
          Height = 13
          Caption = 'Skydome object'
        end
        object ebChooseSkydome: TExtBtn
          Left = 265
          Top = 9
          Width = 42
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Select'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebChooseSkydomeClick
        end
        object ebClearSkydome: TExtBtn
          Left = 309
          Top = 9
          Width = 42
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Clear'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebClearSkydomeClick
        end
        object RxLabel10: TLabel
          Left = 2
          Top = 31
          Width = 89
          Height = 13
          Caption = 'Environment count'
        end
        object RxLabel29: TLabel
          Left = 181
          Top = 31
          Width = 95
          Height = 13
          Caption = 'Current environment'
        end
        object edSkydomeObjectName: TEdit
          Left = 98
          Top = 9
          Width = 164
          Height = 18
          AutoSize = False
          ReadOnly = True
          TabOrder = 0
        end
        object seEnvCount: TMultiObjSpinEdit
          Left = 98
          Top = 29
          Width = 70
          Height = 18
          LWSensitivity = 0.001
          ButtonKind = bkDiagonal
          Decimal = 1
          EditorEnabled = False
          MaxValue = 32
          MinValue = 1
          Value = 1
          AutoSize = False
          TabOrder = 1
          OnChange = seEnvCountChange
        end
        object seCurEnv: TMultiObjSpinEdit
          Left = 281
          Top = 29
          Width = 70
          Height = 18
          LWSensitivity = 0.001
          ButtonKind = bkDiagonal
          Decimal = 1
          EditorEnabled = False
          MaxValue = 32
          AutoSize = False
          TabOrder = 2
        end
      end
      object sbEnvs: TScrollBox
        Left = 0
        Top = 50
        Width = 354
        Height = 201
        HorzScrollBar.Visible = False
        VertScrollBar.Increment = 10
        VertScrollBar.Style = ssHotTrack
        VertScrollBar.Tracking = True
        Align = alClient
        BorderStyle = bsNone
        TabOrder = 1
      end
    end
    object tsLevelScript: TTabSheet
      Caption = 'Level script'
      ImageIndex = 9
      object mmText: TRichEdit
        Left = 0
        Top = 0
        Width = 354
        Height = 251
        Align = alClient
        ScrollBars = ssVertical
        TabOrder = 0
      end
    end
    object tsOptimizing: TTabSheet
      Caption = 'Optimizing'
      ImageIndex = 1
      object RxLabel11: TLabel
        Left = 2
        Top = 10
        Width = 99
        Height = 13
        Caption = 'Normal smooth angle'
      end
      object RxLabel20: TLabel
        Left = 3
        Top = 30
        Width = 85
        Height = 13
        Caption = 'Weld distance (m)'
      end
      object seSMAngle: TMultiObjSpinEdit
        Left = 115
        Top = 8
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 89
        AutoSize = False
        TabOrder = 0
      end
      object seWeldDistance: TMultiObjSpinEdit
        Left = 115
        Top = 28
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 3
        Increment = 0.001
        MaxValue = 10
        ValueType = vtFloat
        Value = 0.001
        AutoSize = False
        TabOrder = 1
      end
    end
    object tsTesselation: TTabSheet
      Caption = 'Tesselation'
      ImageIndex = 2
      object RxLabel4: TLabel
        Left = 2
        Top = 28
        Width = 64
        Height = 13
        Caption = 'Max edge (m)'
      end
      object seMaxEdge: TMultiObjSpinEdit
        Left = 114
        Top = 26
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.05
        MaxValue = 1000
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 0
      end
      object cbTesselation: TCheckBox
        Left = 2
        Top = 8
        Width = 125
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Tesselation'
        TabOrder = 1
      end
    end
    object tsLightmaps: TTabSheet
      Caption = 'Lightmaps'
      ImageIndex = 4
      object RxLabel5: TLabel
        Left = 2
        Top = 27
        Width = 74
        Height = 13
        Caption = 'Pixels per meter'
      end
      object RxLabel12: TLabel
        Left = 2
        Top = 47
        Width = 98
        Height = 13
        Caption = 'Deflectors split angle'
      end
      object RxLabel32: TLabel
        Left = 2
        Top = 66
        Width = 111
        Height = 13
        Caption = 'Error (for LM collapsing)'
      end
      object MxLabel1: TLabel
        Left = 2
        Top = 86
        Width = 84
        Height = 13
        Caption = 'Error (for LM zero)'
        WordWrap = True
      end
      object seLMPixelsPerMeter: TMultiObjSpinEdit
        Left = 114
        Top = 25
        Width = 78
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 20
        ValueType = vtFloat
        Value = 5
        AutoSize = False
        TabOrder = 0
      end
      object seLMDeflSplitAngle: TMultiObjSpinEdit
        Left = 114
        Top = 45
        Width = 78
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 179
        ValueType = vtFloat
        Value = 89
        AutoSize = False
        TabOrder = 1
      end
      object cbLMLightmaps: TCheckBox
        Left = 0
        Top = 8
        Width = 127
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Calculate lightmaps'
        Checked = True
        State = cbChecked
        TabOrder = 2
      end
      object seLMRMS: TMultiObjSpinEdit
        Left = 114
        Top = 65
        Width = 78
        Height = 18
        LWSensitivity = 0.01
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 8
        AutoSize = False
        TabOrder = 3
      end
      object GroupBox2: TGroupBox
        Left = 195
        Top = 3
        Width = 159
        Height = 106
        Caption = ' Hemisphere lighting '
        TabOrder = 4
        object RxLabel34: TLabel
          Left = 5
          Top = 16
          Width = 76
          Height = 13
          Caption = 'Dispersion (deg)'
        end
        object RxLabel36: TLabel
          Left = 5
          Top = 36
          Width = 95
          Height = 13
          Caption = 'Summary energy (%)'
        end
        object Bevel2: TBevel
          Left = 87
          Top = 54
          Width = 67
          Height = 17
        end
        object mcLMAreaColor: TMultiObjColor
          Left = 89
          Top = 56
          Width = 64
          Height = 14
          Pen.Style = psClear
          OnMouseDown = mcLMAmbientMouseDown
        end
        object RxLabel24: TLabel
          Left = 5
          Top = 55
          Width = 24
          Height = 13
          Caption = 'Color'
        end
        object RxLabel39: TLabel
          Left = 5
          Top = 83
          Width = 32
          Height = 13
          Caption = 'Quality'
        end
        object seLMAreaDispersion: TMultiObjSpinEdit
          Left = 88
          Top = 14
          Width = 66
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Decimal = 1
          Increment = 0.1
          MaxValue = 360
          ValueType = vtFloat
          Value = 7.5
          AutoSize = False
          TabOrder = 0
        end
        object seLMAreaSummaryEnergy: TMultiObjSpinEdit
          Left = 104
          Top = 34
          Width = 50
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          Decimal = 0
          MaxValue = 1000
          ValueType = vtFloat
          Value = 100
          AutoSize = False
          TabOrder = 1
        end
        object rgAreaQuality: TRadioGroup
          Left = 56
          Top = 71
          Width = 98
          Height = 30
          Columns = 3
          ItemIndex = 1
          Items.Strings = (
            '0'
            '1'
            '2')
          TabOrder = 2
        end
      end
      object GroupBox3: TGroupBox
        Left = 0
        Top = 162
        Width = 192
        Height = 86
        Caption = ' Jittering '
        TabOrder = 5
        object RxLabel9: TLabel
          Left = 5
          Top = 62
          Width = 43
          Height = 13
          Caption = 'Samples '
        end
        object RxLabel13: TLabel
          Left = 5
          Top = 17
          Width = 28
          Height = 13
          Caption = 'Dither'
        end
        object RxLabel14: TLabel
          Left = 5
          Top = 36
          Width = 44
          Height = 13
          Caption = 'Distortion'
        end
        object rgLMJitterSamples: TRadioGroup
          Left = 58
          Top = 51
          Width = 127
          Height = 30
          Columns = 3
          ItemIndex = 1
          Items.Strings = (
            '1'
            '4'
            '9')
          TabOrder = 0
        end
        object seLMDither: TMultiObjSpinEdit
          Left = 115
          Top = 15
          Width = 71
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 1
          ValueType = vtFloat
          Value = 0.02
          AutoSize = False
          TabOrder = 1
        end
        object seLMJitter: TMultiObjSpinEdit
          Left = 115
          Top = 35
          Width = 71
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 0.5
          ValueType = vtFloat
          Value = 0.49
          AutoSize = False
          TabOrder = 2
        end
      end
      object GroupBox4: TGroupBox
        Left = 0
        Top = 104
        Width = 192
        Height = 57
        Caption = ' Ambient '
        TabOrder = 6
        object Bevel1: TBevel
          Left = 115
          Top = 13
          Width = 72
          Height = 17
        end
        object mcLMAmbient: TMultiObjColor
          Left = 117
          Top = 15
          Width = 69
          Height = 14
          Pen.Style = psClear
          OnMouseDown = mcLMAmbientMouseDown
        end
        object RxLabel22: TLabel
          Left = 5
          Top = 15
          Width = 24
          Height = 13
          Caption = 'Color'
        end
        object RxLabel23: TLabel
          Left = 5
          Top = 35
          Width = 57
          Height = 13
          Caption = 'Fogness (%)'
        end
        object seLMAmbFogness: TMultiObjSpinEdit
          Left = 115
          Top = 33
          Width = 72
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          Decimal = 0
          MaxValue = 100
          MinValue = 1
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          TabOrder = 0
        end
      end
      object GroupBox5: TGroupBox
        Left = 195
        Top = 109
        Width = 159
        Height = 139
        Caption = ' Fuzzy point light '
        TabOrder = 7
        object RxLabel35: TLabel
          Left = 5
          Top = 33
          Width = 40
          Height = 13
          Caption = 'Samples'
        end
        object RxLabel40: TLabel
          Left = 5
          Top = 52
          Width = 34
          Height = 13
          Caption = 'Min (m)'
        end
        object RxLabel41: TLabel
          Left = 5
          Top = 72
          Width = 37
          Height = 13
          Caption = 'Max (m)'
        end
        object cbLMFuzzyEnabled: TCheckBox
          Left = 4
          Top = 14
          Width = 95
          Height = 17
          Alignment = taLeftJustify
          Caption = 'Enabled'
          Checked = True
          State = cbChecked
          TabOrder = 0
        end
        object seLMFuzzySamples: TMultiObjSpinEdit
          Left = 86
          Top = 31
          Width = 71
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Decimal = 0
          Increment = 0.01
          MaxValue = 256
          ValueType = vtFloat
          Value = 16
          AutoSize = False
          TabOrder = 1
        end
        object seLMFuzzyMin: TMultiObjSpinEdit
          Left = 86
          Top = 51
          Width = 71
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 10
          ValueType = vtFloat
          Value = 0.3
          AutoSize = False
          TabOrder = 2
          OnLWChange = seLMFuzzyMinLWChange
          OnExit = seLMFuzzyMinExit
          OnKeyDown = seLMFuzzyMinKeyDown
        end
        object seLMFuzzyMax: TMultiObjSpinEdit
          Left = 86
          Top = 71
          Width = 71
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 100
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          TabOrder = 3
        end
      end
      object seLMRMSZero: TMultiObjSpinEdit
        Left = 114
        Top = 85
        Width = 78
        Height = 18
        LWSensitivity = 0.01
        ButtonKind = bkLightWave
        MaxValue = 255
        Value = 8
        AutoSize = False
        TabOrder = 8
      end
    end
    object tsProgressive: TTabSheet
      Caption = 'Progressive'
      ImageIndex = 5
      object RxLabel15: TLabel
        Left = 2
        Top = 28
        Width = 49
        Height = 13
        Caption = 'UV weight'
      end
      object RxLabel16: TLabel
        Left = 2
        Top = 47
        Width = 71
        Height = 13
        Caption = 'Position weight'
      end
      object RxLabel17: TLabel
        Left = 2
        Top = 67
        Width = 80
        Height = 13
        Caption = 'Curvature weight'
      end
      object RxLabel26: TLabel
        Left = 2
        Top = 88
        Width = 93
        Height = 13
        Caption = 'Higher border angle'
      end
      object RxLabel27: TLabel
        Left = 2
        Top = 107
        Width = 107
        Height = 13
        Caption = 'Higher border distance'
      end
      object RxLabel28: TLabel
        Left = 2
        Top = 127
        Width = 175
        Height = 16
        AutoSize = False
        Caption = 'Heuristic (Stripify vs Progressive)'
        WordWrap = True
      end
      object sePMUV: TMultiObjSpinEdit
        Left = 162
        Top = 26
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 0.5
        AutoSize = False
        TabOrder = 0
      end
      object sePMPos: TMultiObjSpinEdit
        Left = 162
        Top = 46
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 1
      end
      object sePMCurv: TMultiObjSpinEdit
        Left = 162
        Top = 66
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 2
      end
      object cbProgressive: TCheckBox
        Left = 1
        Top = 8
        Width = 174
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Convert to progressive'
        TabOrder = 3
      end
      object sePM_borderH_angle: TMultiObjSpinEdit
        Left = 162
        Top = 86
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        ValueType = vtFloat
        Value = 150
        AutoSize = False
        TabOrder = 4
      end
      object sePM_borderH_distance: TMultiObjSpinEdit
        Left = 162
        Top = 106
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 100
        ValueType = vtFloat
        Value = 0.05
        AutoSize = False
        TabOrder = 5
      end
      object sePM_heuristic: TMultiObjSpinEdit
        Left = 162
        Top = 126
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1
        ValueType = vtFloat
        Value = 0.85
        AutoSize = False
        TabOrder = 6
      end
    end
    object tsStrippifier: TTabSheet
      Caption = 'Strippifier'
      ImageIndex = 8
      object RxLabel18: TLabel
        Left = 2
        Top = 27
        Width = 52
        Height = 13
        Caption = 'Cache size'
      end
      object RxLabel37: TLabel
        Left = 130
        Top = 43
        Width = 217
        Height = 26
        Caption = 
          '15-17 ge1, ge2, ge2MX, radeon1, radeon7xxx'#13#10'23-25 geforce3, rade' +
          'on8xxx'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object cbStrippify: TCheckBox
        Left = 0
        Top = 7
        Width = 141
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Convert to strips'
        TabOrder = 0
      end
      object seStripCacheSize: TMultiObjSpinEdit
        Left = 128
        Top = 25
        Width = 81
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        MaxValue = 64
        MinValue = 8
        Value = 16
        AutoSize = False
        TabOrder = 1
      end
    end
    object tsVertexBuffers: TTabSheet
      Caption = 'Vertex buffers'
      ImageIndex = 6
      object RxLabel6: TLabel
        Left = 2
        Top = 10
        Width = 79
        Height = 13
        Caption = 'VB max size (kB)'
      end
      object RxLabel7: TLabel
        Left = 2
        Top = 30
        Width = 76
        Height = 13
        Caption = 'VB max vertices'
      end
      object seVBMaxSize: TMultiObjSpinEdit
        Left = 114
        Top = 8
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 1000000
        Value = 8192
        AutoSize = False
        TabOrder = 0
      end
      object seVBMaxVertices: TMultiObjSpinEdit
        Left = 114
        Top = 28
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 100000
        Value = 65535
        AutoSize = False
        TabOrder = 1
      end
    end
    object tsSubdivision: TTabSheet
      Caption = 'Subdivision'
      ImageIndex = 7
      object RxLabel8: TLabel
        Left = 2
        Top = 10
        Width = 90
        Height = 13
        Caption = 'Max object size (m)'
      end
      object RxLabel19: TLabel
        Left = 2
        Top = 30
        Width = 121
        Height = 13
        Caption = 'Perform addititional merge'
      end
      object RxLabel21: TLabel
        Left = 2
        Top = 50
        Width = 159
        Height = 13
        Caption = 'Spatial subdivisions low limit (poly)'
      end
      object RxLabel30: TLabel
        Left = 2
        Top = 70
        Width = 166
        Height = 13
        Caption = 'Spatial subdivisions hight limit (poly)'
      end
      object RxLabel31: TLabel
        Left = 2
        Top = 84
        Width = 175
        Height = 27
        AutoSize = False
        Caption = 'Dedicate to cached pipeline if less than # faces'
        WordWrap = True
      end
      object seSSMaxSize: TMultiObjSpinEdit
        Left = 178
        Top = 8
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 48
        AutoSize = False
        TabOrder = 0
      end
      object seSSMergeCoef: TMultiObjSpinEdit
        Left = 178
        Top = 28
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1
        ValueType = vtFloat
        Value = 0.5
        AutoSize = False
        TabOrder = 1
      end
      object seSSLowLimit: TMultiObjSpinEdit
        Left = 178
        Top = 48
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        MaxValue = 2024
        MinValue = 12
        Value = 12
        AutoSize = False
        TabOrder = 2
        OnChange = seSSLowLimitChange
      end
      object seSSHighLimit: TMultiObjSpinEdit
        Left = 178
        Top = 68
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        MaxValue = 2048
        MinValue = 36
        Value = 2048
        AutoSize = False
        TabOrder = 3
        OnChange = seSSHighLimitChange
      end
      object seSSDedicatedCashed: TMultiObjSpinEdit
        Left = 178
        Top = 88
        Width = 121
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        MaxValue = 1000000
        Value = 32
        AutoSize = False
        TabOrder = 4
      end
    end
  end
  object tvOptions: TElTree
    Left = 0
    Top = 0
    Width = 134
    Height = 279
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alLeft
    AlwaysKeepFocus = True
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    CustomPlusMinus = True
    DoInplaceEdit = False
    DrawFocusRect = False
    DragImageMode = dimOne
    DragTrgDrawMode = ColorRect
    ExplorerEditMode = False
    Flat = True
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF0700000064671901001D0000FFFFFFFF000001010100104078000000
      00000000102700000001004020E1C60500000000000001050000000000000105
      00000000000000000001000000000000133301053B3301055F33010582330105
      A5330105C9330105E93301050E34010531340105573401057734010597340105
      BC340105DF340105013501052035010542350105010000000001000000000000
      00000000000064671901001D0000FFFFFFFF0000010101001040780000000000
      00001027000000010040E8E5C605010000000000010500000000000001050000
      0000000000000001000000000000133301053B3301055F33010582330105A533
      0105C9330105E93301050E34010531340105573401057734010597340105BC34
      0105DF3401050135010520350105423501050100000000010000000000000000
      0000000064671901001D0000FFFFFFFF00000101010010407800000000000000
      1027000000010040ACE6C6050200000000000105000000000000010500000000
      000000000001000000000000133301053B3301055F33010582330105A5330105
      C9330105E93301050E34010531340105573401057734010597340105BC340105
      DF34010501350105203501054235010501000000000100000000000000000000
      000064671901001D0000FFFFFFFF000001010100104078000000000000001027
      00000001004070E7C60503000000000001050000000000000105000000000000
      00000001000000000000133301053B3301055F33010582330105A5330105C933
      0105E93301050E34010531340105573401057734010597340105BC340105DF34
      0105013501052035010542350105010000000001000000000000000000000000
      64671901001D0000FFFFFFFF0000010101001040780000000000000010270000
      0001004034E8C605040000000000010500000000000001050000000000000000
      0001000000000000133301053B3301055F33010582330105A5330105C9330105
      E93301050E34010531340105573401057734010597340105BC340105DF340105
      0135010520350105423501050100000000010000000000000000000000006467
      1901001D0000FFFFFFFF00000101010010407800000000000000102700000001
      0040F8E8C6050500000000000105000000000000010500000000000000000001
      000000000000133301053B3301055F33010582330105A5330105C9330105E933
      01050E34010531340105573401057734010597340105BC340105DF3401050135
      0105203501054235010501000000000100000000000000000000000064671901
      001D0000FFFFFFFF000001010100104078000000000000001027000000010040
      BCE9C60506000000000001050000000000000105000000000000000000010000
      00000000133301053B3301055F33010582330105A5330105C9330105E9330105
      0E34010531340105573401057734010597340105BC340105DF34010501350105
      2035010542350105010000000001000000000000000000000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    LinesColor = 5329233
    MinusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      808080808080808080808080808080808080808080808080808080E0E0E08080
      80808080808080808080808080808080808080E0E0E080808080808080808080
      8080808080808080E0E0E0E0E0E0E0E0E0808080808080808080808080808080
      E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0E0E0E0808080808080808080E0E0E0E0E0E0E0E0E0E0E0E0E0E0E080808080
      8080808080808080808080808080808080808080808080808080}
    MouseFrameSelect = True
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    PlusMinusTransparent = True
    PlusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      8080808080808080808080808080808080808080808080808080808080808080
      80808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080E0E0E0E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0
      E0E0E0E0E0E0E0E0E0E0E0E0E0E0E0808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080808080808080808080808080808080808080808080}
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowCheckboxes = True
    ShowLeafButton = False
    ShowLines = False
    StoragePath = '\Tree'
    TabOrder = 0
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertDivLinesColor = 7368816
    VerticalLines = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    TextColor = clBtnText
    BkColor = clGray
    OnItemSelectedChange = tvOptionsItemSelectedChange
  end
  object Panel1: TPanel
    Left = 0
    Top = 279
    Width = 496
    Height = 21
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    object ebOk: TExtBtn
      Left = 335
      Top = 1
      Width = 80
      Height = 20
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = btContinueClick
    end
    object ebCancel: TExtBtn
      Left = 415
      Top = 1
      Width = 80
      Height = 20
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object fsSceneProps: TFormStorage
    IniSection = 'Build Options'
    Options = [fpPosition]
    RegistryRoot = prLocalMachine
    OnRestorePlacement = fsScenePropsRestorePlacement
    StoredProps.Strings = (
      'pcBuildOptions.ActivePage')
    StoredValues = <>
    Left = 4
    Top = 2
  end
end
