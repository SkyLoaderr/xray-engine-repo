object fraEmitter: TfraEmitter
  Left = 0
  Top = 0
  Width = 232
  Height = 209
  TabOrder = 0
  object RxLabel20: TRxLabel
    Left = 4
    Top = 103
    Width = 47
    Height = 13
    Caption = 'Birth rate:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object RxLabel22: TRxLabel
    Left = 4
    Top = 121
    Width = 60
    Height = 13
    Caption = 'Particle limit:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object ebBirthFunc: TExtBtn
    Left = 171
    Top = 100
    Width = 17
    Height = 17
    Align = alNone
    AllowAllUp = True
    BevelShow = False
    CloseButton = False
    GroupIndex = 1
    Caption = 'E'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = False
    FlatAlwaysEdge = True
  end
  object RxLabel1: TRxLabel
    Left = 4
    Top = 138
    Width = 29
    Height = 13
    Caption = 'Burst:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object RxLabel2: TRxLabel
    Left = -1
    Top = 3
    Width = 52
    Height = 13
    Caption = 'Size X (m):'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object RxLabel3: TRxLabel
    Left = 0
    Top = 21
    Width = 52
    Height = 13
    Caption = 'Size Y (m):'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object RxLabel5: TRxLabel
    Left = -1
    Top = 40
    Width = 52
    Height = 13
    Caption = 'Size Z (m):'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object RxLabel6: TRxLabel
    Left = 4
    Top = 153
    Width = 52
    Height = 13
    Caption = 'Play once:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object seBirthRate: TMultiObjSpinEdit
    Left = 84
    Top = 100
    Width = 88
    Height = 18
    LWSensitivity = 0.01
    ButtonKind = bkLightWave
    Increment = 0.01
    MaxValue = 10000
    ValueType = vtFloat
    AutoSize = False
    Color = 14671839
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
  object seParticleLimit: TMultiObjSpinEdit
    Left = 84
    Top = 118
    Width = 105
    Height = 18
    LWSensitivity = 1
    ButtonKind = bkLightWave
    MaxValue = 1000000
    AutoSize = False
    Color = 14671839
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
  end
  object pcEmitterType: TPageControl
    Left = 0
    Top = 0
    Width = 232
    Height = 100
    ActivePage = tsCone
    Align = alTop
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    MultiLine = True
    ParentFont = False
    Style = tsFlatButtons
    TabHeight = 18
    TabOrder = 2
    TabWidth = 39
    object tsPoint: TTabSheet
      Caption = 'Point'
      object RxLabel42: TRxLabel
        Left = 47
        Top = 27
        Width = 65
        Height = 13
        Caption = '<No params>'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
    object tsCone: TTabSheet
      Caption = 'Cone'
      ImageIndex = 1
      object RxLabel4: TRxLabel
        Left = -1
        Top = 3
        Width = 45
        Height = 13
        Caption = 'Angle (°):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel35: TRxLabel
        Left = -1
        Top = 21
        Width = 62
        Height = 13
        Caption = 'Heading (Y°)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel36: TRxLabel
        Left = 0
        Top = 39
        Width = 46
        Height = 13
        Caption = 'Pitch (X°)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel37: TRxLabel
        Left = -1
        Top = 58
        Width = 47
        Height = 13
        Caption = 'Bank (Z°)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object seConeAngle: TMultiObjSpinEdit
        Left = 81
        Top = 0
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 180
        ValueType = vtFloat
        Value = 30
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
      end
      object seConeDirH: TMultiObjSpinEdit
        Left = 81
        Top = 18
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        MinValue = -360
        ValueType = vtFloat
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
      end
      object seConeDirP: TMultiObjSpinEdit
        Left = 81
        Top = 36
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        MinValue = -360
        ValueType = vtFloat
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
      end
      object seConeDirB: TMultiObjSpinEdit
        Left = 81
        Top = 54
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        MinValue = -360
        ValueType = vtFloat
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
      end
    end
    object tsSphere: TTabSheet
      Caption = 'Sphere'
      ImageIndex = 2
      object RxLabel38: TRxLabel
        Left = -1
        Top = 3
        Width = 55
        Height = 13
        Caption = 'Radius (m):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object seSphereRadius: TMultiObjSpinEdit
        Left = 81
        Top = 0
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 10000
        ValueType = vtFloat
        Value = 1
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
      end
    end
    object tsBox: TTabSheet
      Caption = 'Box'
      ImageIndex = 3
      object RxLabel39: TRxLabel
        Left = -1
        Top = 3
        Width = 52
        Height = 13
        Caption = 'Size X (m):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel40: TRxLabel
        Left = 0
        Top = 21
        Width = 52
        Height = 13
        Caption = 'Size Y (m):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel41: TRxLabel
        Left = -1
        Top = 40
        Width = 52
        Height = 13
        Caption = 'Size Z (m):'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object seBoxSizeX: TMultiObjSpinEdit
        Left = 81
        Top = 0
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        MinValue = -360
        ValueType = vtFloat
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
      end
      object seBoxSizeY: TMultiObjSpinEdit
        Left = 81
        Top = 18
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        MinValue = -360
        ValueType = vtFloat
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
      end
      object seBoxSizeZ: TMultiObjSpinEdit
        Left = 81
        Top = 36
        Width = 105
        Height = 18
        LWSensitivity = 0.1
        ButtonKind = bkLightWave
        Decimal = 1
        Increment = 0.1
        MaxValue = 360
        MinValue = -360
        ValueType = vtFloat
        AutoSize = False
        Color = 14671839
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
      end
    end
  end
  object cbBurst: TMultiObjCheck
    Left = 84
    Top = 138
    Width = 13
    Height = 13
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
  end
  object cbPlayOnce: TMultiObjCheck
    Left = 84
    Top = 153
    Width = 13
    Height = 13
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
  end
end
