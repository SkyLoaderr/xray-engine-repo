object frmSceneProperties: TfrmSceneProperties
  Left = 603
  Top = 207
  BorderStyle = bsDialog
  Caption = 'Build...'
  ClientHeight = 228
  ClientWidth = 348
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
    Left = 0
    Top = 0
    Width = 348
    Height = 201
    ActivePage = tsBuildStages
    Align = alTop
    HotTrack = True
    MultiLine = True
    TabOrder = 0
    object tsBuildStages: TTabSheet
      Caption = 'Build stages'
      ImageIndex = 4
      object cbTesselation: TCheckBox
        Left = 0
        Top = 8
        Width = 137
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Tesselation'
        TabOrder = 0
        OnClick = cbTesselationClick
      end
      object cbProgressive: TCheckBox
        Left = 0
        Top = 24
        Width = 137
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Convert to progressive'
        TabOrder = 1
        OnClick = cbProgressiveClick
      end
      object cbLightmaps: TCheckBox
        Left = 0
        Top = 40
        Width = 137
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Calculate lightmaps'
        TabOrder = 2
        OnClick = cbLightmapsClick
      end
    end
    object tsMainOptions: TTabSheet
      Caption = 'Main Options'
      object RxLabel1: TRxLabel
        Left = 4
        Top = 12
        Width = 60
        Height = 13
        Caption = 'Level name:'
      end
      object RxLabel2: TRxLabel
        Left = 5
        Top = 34
        Width = 55
        Height = 13
        Caption = 'Level path:'
      end
      object RxLabel3: TRxLabel
        Left = 5
        Top = 58
        Width = 95
        Height = 13
        Caption = 'Adititional level text:'
      end
      object edLevelName: TEdit
        Left = 72
        Top = 8
        Width = 263
        Height = 21
        TabOrder = 0
      end
      object deLevelPath: TDirectoryEdit
        Left = 72
        Top = 30
        Width = 264
        Height = 21
        InitialDir = 'game\data\level'
        NumGlyphs = 1
        TabOrder = 1
        Text = 'level'
      end
      object mmText: TRichEdit
        Left = 0
        Top = 80
        Width = 340
        Height = 75
        Align = alBottom
        TabOrder = 2
      end
    end
    object tsOptimizing: TTabSheet
      Caption = 'Optimizing'
      ImageIndex = 1
    end
    object tsTesselation: TTabSheet
      Caption = 'Tesselation'
      ImageIndex = 2
      object RxLabel4: TRxLabel
        Left = 2
        Top = 10
        Width = 66
        Height = 13
        Caption = 'Max edge (m)'
      end
      object seMaxEdge: TMultiObjSpinEdit
        Left = 72
        Top = 8
        Width = 121
        Height = 18
        Decimal = 1
        Increment = 0.05
        MaxValue = 1000
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        TabOrder = 0
      end
    end
    object tsLightmaps: TTabSheet
      Caption = 'Light maps'
      ImageIndex = 4
      object RxLabel5: TRxLabel
        Left = 2
        Top = 10
        Width = 76
        Height = 13
        Caption = 'Pixels per meter'
      end
      object sePixelsPerMeter: TMultiObjSpinEdit
        Left = 88
        Top = 8
        Width = 121
        Height = 18
        Decimal = 1
        MaxValue = 10000
        ValueType = vtFloat
        Value = 20
        AutoSize = False
        TabOrder = 0
      end
    end
    object tsProgressive: TTabSheet
      Caption = 'Progressive'
      ImageIndex = 5
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
        Top = 31
        Width = 78
        Height = 13
        Caption = 'VB max vertices'
      end
      object seVBMaxSize: TMultiObjSpinEdit
        Left = 88
        Top = 8
        Width = 121
        Height = 18
        MaxValue = 1000000
        Value = 64
        AutoSize = False
        TabOrder = 0
      end
      object seVBMaxVertices: TMultiObjSpinEdit
        Left = 88
        Top = 29
        Width = 121
        Height = 18
        MaxValue = 100000
        Value = 4096
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
      object seMaxSize: TMultiObjSpinEdit
        Left = 96
        Top = 8
        Width = 121
        Height = 18
        Decimal = 1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 16
        AutoSize = False
        TabOrder = 0
      end
    end
    object tsVisibility: TTabSheet
      Caption = 'Visibility'
      ImageIndex = 8
      object RxLabel9: TRxLabel
        Left = 2
        Top = 10
        Width = 76
        Height = 13
        Caption = 'VIS slot size (m)'
      end
      object RxLabel10: TRxLabel
        Left = 2
        Top = 31
        Width = 85
        Height = 13
        Caption = 'View distance (m)'
      end
      object seRelevance: TMultiObjSpinEdit
        Left = 96
        Top = 8
        Width = 121
        Height = 18
        Decimal = 1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 10
        AutoSize = False
        TabOrder = 0
      end
      object seViewDist: TMultiObjSpinEdit
        Left = 96
        Top = 29
        Width = 121
        Height = 18
        Decimal = 1
        MaxValue = 1000
        ValueType = vtFloat
        Value = 100
        AutoSize = False
        TabOrder = 1
      end
    end
  end
  object btOk: TButton
    Left = 160
    Top = 205
    Width = 89
    Height = 20
    Caption = 'Ok'
    ModalResult = 1
    TabOrder = 1
    OnClick = btContinueClick
  end
  object btCancel: TButton
    Left = 256
    Top = 205
    Width = 89
    Height = 20
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object fsSceneProps: TFormStorage
    IniSection = 'Build Options'
    Options = []
    RegistryRoot = prLocalMachine
    StoredProps.Strings = (
      'cbLightmaps.Checked'
      'cbProgressive.Checked'
      'cbTesselation.Checked'
      'seMaxEdge.Value'
      'seMaxSize.Value'
      'sePixelsPerMeter.Value'
      'seRelevance.Value'
      'seVBMaxSize.Value'
      'seVBMaxVertices.Value'
      'seViewDist.Value')
    StoredValues = <>
    Left = 4
    Top = 2
  end
end
