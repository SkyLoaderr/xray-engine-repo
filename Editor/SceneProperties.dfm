object frmSceneProperties: TfrmSceneProperties
  Left = 462
  Top = 481
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
    ActivePage = tsLightmaps
    Align = alClient
    HotTrack = True
    TabOrder = 1
    object tsBuildOptions: TTabSheet
      Caption = 'Build Options'
      ImageIndex = 8
      object RxLabel25: TRxLabel
        Left = 107
        Top = 106
        Width = 128
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
        Height = 51
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object RxLabel1: TRxLabel
          Left = 3
          Top = 10
          Width = 57
          Height = 13
          Caption = 'Level name'
        end
        object RxLabel2: TRxLabel
          Left = 4
          Top = 32
          Width = 52
          Height = 13
          Caption = 'Level path'
        end
        object edLevelName: TEdit
          Left = 98
          Top = 9
          Width = 254
          Height = 18
          AutoSize = False
          TabOrder = 0
        end
        object deLevelPath: TDirectoryEdit
          Left = 98
          Top = 30
          Width = 254
          Height = 18
          InitialDir = 'game\data\level'
          NumGlyphs = 1
          ButtonWidth = 20
          TabOrder = 1
          Text = 'level'
        end
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 51
        Width = 354
        Height = 38
        Align = alTop
        Caption = ' Detail objects '
        TabOrder = 1
        object RxLabel33: TRxLabel
          Left = 4
          Top = 15
          Width = 72
          Height = 13
          Caption = 'Cluster size (m)'
        end
        object seDOClusterSize: TMultiObjSpinEdit
          Left = 98
          Top = 13
          Width = 79
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          Decimal = 1
          Increment = 0.1
          MaxValue = 128
          MinValue = 1
          ValueType = vtFloat
          Value = 4
          AutoSize = False
          TabOrder = 0
          OnChange = seEnvCountChange
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
        Height = 51
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object RxLabel3: TRxLabel
          Left = 2
          Top = 12
          Width = 78
          Height = 13
          Caption = 'Skydome object'
        end
        object ebChooseSkydome: TExtBtn
          Left = 267
          Top = 9
          Width = 42
          Height = 19
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
          Height = 19
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Clear'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebClearSkydomeClick
        end
        object RxLabel10: TRxLabel
          Left = 2
          Top = 34
          Width = 91
          Height = 13
          Caption = 'Environment count'
        end
        object RxLabel29: TRxLabel
          Left = 181
          Top = 34
          Width = 97
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
          Top = 31
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
          Top = 31
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
        Top = 51
        Width = 354
        Height = 200
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
      object RxLabel11: TRxLabel
        Left = 2
        Top = 10
        Width = 101
        Height = 13
        Caption = 'Normal smooth angle'
      end
      object RxLabel20: TRxLabel
        Left = 3
        Top = 30
        Width = 87
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
      object RxLabel4: TRxLabel
        Left = 2
        Top = 28
        Width = 66
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
      object RxLabel5: TRxLabel
        Left = 2
        Top = 44
        Width = 76
        Height = 13
        Caption = 'Pixels per meter'
      end
      object RxLabel12: TRxLabel
        Left = 2
        Top = 64
        Width = 100
        Height = 13
        Caption = 'Deflectors split angle'
      end
      object RxLabel32: TRxLabel
        Left = 2
        Top = 77
        Width = 88
        Height = 26
        Caption = 'Error metrics '#13#10'(for LM collapsing)'
        WordWrap = True
      end
      object seLMPixelsPerMeter: TMultiObjSpinEdit
        Left = 114
        Top = 42
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
        Top = 62
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
      object cbLMRadiosity: TCheckBox
        Left = 0
        Top = 24
        Width = 127
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Radiosity'
        TabOrder = 3
      end
      object seLMRMS: TMultiObjSpinEdit
        Left = 115
        Top = 82
        Width = 77
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Increment = 0.01
        MaxValue = 1
        ValueType = vtFloat
        Value = 0.02
        AutoSize = False
        TabOrder = 4
      end
      object GroupBox2: TGroupBox
        Left = 195
        Top = 3
        Width = 159
        Height = 106
        Caption = ' Hemisphere lighting '
        TabOrder = 5
        object RxLabel34: TRxLabel
          Left = 5
          Top = 16
          Width = 78
          Height = 13
          Caption = 'Dispersion (deg)'
        end
        object RxLabel36: TRxLabel
          Left = 5
          Top = 36
          Width = 97
          Height = 13
          Caption = 'Summary energy (%)'
        end
        object Bevel2: TBevel
          Left = 87
          Top = 54
          Width = 67
          Height = 17
        end
        object mcLMAreaDark: TMultiObjColor
          Left = 89
          Top = 56
          Width = 64
          Height = 14
          Pen.Style = psClear
          OnMouseDown = mcLMAmbientMouseDown
        end
        object RxLabel24: TRxLabel
          Left = 5
          Top = 55
          Width = 26
          Height = 13
          Caption = 'Color'
        end
        object RxLabel39: TRxLabel
          Left = 5
          Top = 83
          Width = 34
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
        TabOrder = 6
        object RxLabel9: TRxLabel
          Left = 5
          Top = 62
          Width = 45
          Height = 13
          Caption = 'Samples '
        end
        object RxLabel13: TRxLabel
          Left = 5
          Top = 17
          Width = 30
          Height = 13
          Caption = 'Dither'
        end
        object RxLabel14: TRxLabel
          Left = 5
          Top = 36
          Width = 46
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
        TabOrder = 7
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
        object RxLabel22: TRxLabel
          Left = 5
          Top = 15
          Width = 26
          Height = 13
          Caption = 'Color'
        end
        object RxLabel23: TRxLabel
          Left = 5
          Top = 35
          Width = 59
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
        TabOrder = 8
        object RxLabel35: TRxLabel
          Left = 5
          Top = 33
          Width = 42
          Height = 13
          Caption = 'Samples'
        end
        object RxLabel40: TRxLabel
          Left = 5
          Top = 52
          Width = 36
          Height = 13
          Caption = 'Min (m)'
        end
        object RxLabel41: TRxLabel
          Left = 5
          Top = 72
          Width = 39
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
    end
    object tsProgressive: TTabSheet
      Caption = 'Progressive'
      ImageIndex = 5
      object RxLabel15: TRxLabel
        Left = 2
        Top = 28
        Width = 51
        Height = 13
        Caption = 'UV weight'
      end
      object RxLabel16: TRxLabel
        Left = 2
        Top = 47
        Width = 73
        Height = 13
        Caption = 'Position weight'
      end
      object RxLabel17: TRxLabel
        Left = 2
        Top = 67
        Width = 82
        Height = 13
        Caption = 'Curvature weight'
      end
      object RxLabel26: TRxLabel
        Left = 2
        Top = 88
        Width = 95
        Height = 13
        Caption = 'Higher border angle'
      end
      object RxLabel27: TRxLabel
        Left = 2
        Top = 107
        Width = 109
        Height = 13
        Caption = 'Higher border distance'
      end
      object RxLabel28: TRxLabel
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
      object RxLabel18: TRxLabel
        Left = 2
        Top = 27
        Width = 54
        Height = 13
        Caption = 'Cache size'
      end
      object RxLabel37: TRxLabel
        Left = 130
        Top = 43
        Width = 219
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
        ShadowPos = spRightTop
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
      object RxLabel6: TRxLabel
        Left = 2
        Top = 10
        Width = 81
        Height = 13
        Caption = 'VB max size (kB)'
      end
      object RxLabel7: TRxLabel
        Left = 2
        Top = 30
        Width = 78
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
      object RxLabel8: TRxLabel
        Left = 2
        Top = 10
        Width = 92
        Height = 13
        Caption = 'Max object size (m)'
      end
      object RxLabel19: TRxLabel
        Left = 2
        Top = 30
        Width = 123
        Height = 13
        Caption = 'Perform addititional merge'
      end
      object RxLabel21: TRxLabel
        Left = 2
        Top = 50
        Width = 161
        Height = 13
        Caption = 'Spatial subdivisions low limit (poly)'
      end
      object RxLabel30: TRxLabel
        Left = 2
        Top = 70
        Width = 168
        Height = 13
        Caption = 'Spatial subdivisions hight limit (poly)'
      end
      object RxLabel31: TRxLabel
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
    LeftPosition = 0
    ActiveBorderType = fbtSunken
    Align = alLeft
    AlwaysKeepFocus = True
    AlwaysKeepSelection = True
    AutoExpand = True
    AutoLookup = False
    DockOrientation = doNoOrient
    DoubleBuffered = False
    AutoLineHeight = True
    BarStyle = False
    BarStyleVerticalLines = False
    BorderStyle = bsSingle
    ChangeStateImage = False
    CustomCheckboxes = False
    CustomPlusMinus = False
    DeselectChildrenOnCollapse = False
    DivLinesColor = clBtnFace
    DoInplaceEdit = False
    DraggableSections = False
    DragAllowed = False
    DragTrgDrawMode = ColorRect
    DragType = dtDelphi
    DragImageMode = dimOne
    DrawFocusRect = True
    ExpandOnDblClick = True
    ExpandOnDragOver = False
    FilteredVisibility = False
    Flat = True
    FlatFocusedScrollbars = True
    FocusedSelectColor = clHighlight
    ForcedScrollBars = ssNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    HeaderActiveFilterColor = clBlack
    HeaderHeight = 22
    HeaderHotTrack = False
    HeaderInvertSortArrows = False
    HeaderSections.Data = {
      F8FFFFFF07000000F894260D00000000FFFFFFFF000001010100706978000000
      0000000010270000000100000000000000000000000000000000016200000000
      0000000000010000010000000001000000000000000000000000F894260D0000
      0000FFFFFFFF0000010101007069780000000000000010270000000100000100
      0000000000000000000000000162000000000000000000010000010000000001
      000000000000000000000000F894260D00000000FFFFFFFF0000010101007069
      7800000000000000102700000001000002000000000000000000000000000162
      000000000000000000010000010000000001000000000000000000000000F894
      260D00000000FFFFFFFF00000101010070697800000000000000102700000001
      0000030000000000000000000000000001620000000000000000000100000100
      00000001000000000000000000000000F894260D00000000FFFFFFFF00000101
      0100706978000000000000001027000000010000040000000000000000000000
      0000016200000000000000000001000001000000000100000000000000000000
      0000F894260D00000000FFFFFFFF000001010100706978000000000000001027
      0000000100000500000000000000000000000000016200000000000000000001
      0000010000000001000000000000000000000000F894260D00000000FFFFFFFF
      0000010101007069780000000000000010270000000100000600000000000000
      0000000000000162000000000000000000010000010000000001000000000000
      000000000000}
    HeaderFilterColor = clBtnText
    HeaderFlat = False
    HideFocusRect = False
    HideHintOnTimer = False
    HideHintOnMove = True
    HideSelectColor = clBtnFace
    HideSelection = False
    HorizontalLines = False
    HideHorzScrollBar = False
    HideVertScrollBar = False
    HorzScrollBarStyles.ActiveFlat = False
    HorzScrollBarStyles.BlendBackground = True
    HorzScrollBarStyles.Color = clBtnFace
    HorzScrollBarStyles.Flat = True
    HorzScrollBarStyles.MinThumbSize = 4
    HorzScrollBarStyles.NoDisableButtons = False
    HorzScrollBarStyles.NoSunkenThumb = False
    HorzScrollBarStyles.OwnerDraw = False
    HorzScrollBarStyles.SecondaryButtons = False
    HorzScrollBarStyles.SecondBtnKind = sbkOpposite
    HorzScrollBarStyles.ShowLeftArrows = True
    HorzScrollBarStyles.ShowRightArrows = True
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.ThumbMode = etmAuto
    HorzScrollBarStyles.ThumbSize = 0
    HorzScrollBarStyles.Width = 14
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    InactiveBorderType = fbtSunkenOuter
    ItemIndent = 17
    LineHeight = 19
    LinesColor = 5329233
    LinesStyle = psDot
    LineHintColor = clWindow
    LockHeaderHeight = False
    MainTreeColumn = 0
    MoveColumnOnDrag = False
    MoveFocusOnCollapse = False
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    PathSeparator = '\'
    PlusMinusTransparent = True
    RightAlignedText = False
    RightAlignedTree = False
    RowHotTrack = False
    RowSelect = True
    ScrollTracking = True
    SelectColumn = -1
    ShowButtons = True
    ShowColumns = False
    ShowCheckboxes = True
    ShowImages = True
    ShowLines = True
    ShowRoot = False
    ShowRootButtons = True
    SelectionMode = smUsual
    SortDir = sdAscend
    SortMode = smNone
    SortSection = 0
    SortType = stText
    StickyHeaderSections = False
    TabOrder = 0
    TabStop = True
    Tracking = True
    TrackColor = clHighlight
    UnderlineTracked = True
    VerticalLines = False
    VertScrollBarStyles.ActiveFlat = False
    VertScrollBarStyles.BlendBackground = True
    VertScrollBarStyles.Color = clBtnFace
    VertScrollBarStyles.Flat = True
    VertScrollBarStyles.MinThumbSize = 4
    VertScrollBarStyles.NoDisableButtons = False
    VertScrollBarStyles.NoSunkenThumb = False
    VertScrollBarStyles.OwnerDraw = False
    VertScrollBarStyles.SecondaryButtons = False
    VertScrollBarStyles.SecondBtnKind = sbkOpposite
    VertScrollBarStyles.ShowLeftArrows = True
    VertScrollBarStyles.ShowRightArrows = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.ThumbMode = etmAuto
    VertScrollBarStyles.ThumbSize = 0
    VertScrollBarStyles.Width = 14
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    TextColor = clBtnText
    BkColor = clBtnShadow
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
    Options = []
    RegistryRoot = prLocalMachine
    OnRestorePlacement = fsScenePropsRestorePlacement
    StoredProps.Strings = (
      'seMaxEdge.Value'
      'seSSMaxSize.Value'
      'seLMPixelsPerMeter.Value'
      'seVBMaxSize.Value'
      'seVBMaxVertices.Value'
      'seLMDeflSplitAngle.Value'
      'sePMCurv.Value'
      'sePMPos.Value'
      'sePMUV.Value'
      'seSMAngle.Value'
      'seStripCacheSize.Value'
      'cbStrippify.Checked'
      'cbLMLightmaps.Checked'
      'cbProgressive.Checked'
      'cbTesselation.Checked'
      'pcBuildOptions.ActivePage'
      'seWeldDistance.Value'
      'cbLMRadiosity.Checked'
      'sePM_borderH_angle.Value'
      'sePM_borderH_distance.Value'
      'sePM_heuristic.Value'
      'seSSHighLimit.Value'
      'seSSLowLimit.Value'
      'seSSMergeCoef.Value'
      'seSSDedicatedCashed.Value'
      'seDOClusterSize.Value'
      'seLMAmbFogness.Value'
      'seLMAreaDispersion.Value'
      'seLMAreaSummaryEnergy.Value'
      'seLMDither.Value'
      'seLMJitter.Value'
      'seLMRMS.Value'
      'rgLMJitterSamples.ItemIndex'
      'cbLMFuzzyEnabled.Checked'
      'seLMFuzzyMax.Value'
      'seLMFuzzyMin.Value'
      'seLMFuzzySamples.Value'
      'rgAreaQuality.ItemIndex'
      'mcLMAreaDark.Brush'
      'mcLMAmbient.Brush')
    StoredValues = <>
    Left = 4
    Top = 18
  end
end
