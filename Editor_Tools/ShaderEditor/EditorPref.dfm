object frmEditorPreferences: TfrmEditorPreferences
  Left = 380
  Top = 272
  BorderStyle = bsDialog
  Caption = 'Editor preferences'
  ClientHeight = 266
  ClientWidth = 396
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
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 396
    Height = 243
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object pcObjects: TPageControl
      Left = 0
      Top = 0
      Width = 396
      Height = 243
      ActivePage = tsScene
      Align = alClient
      TabOrder = 0
      object tsCamera: TTabSheet
        Caption = 'Camera'
        ImageIndex = 5
        object GroupBox8: TGroupBox
          Left = 196
          Top = 56
          Width = 192
          Height = 61
          Caption = ' Sensitivity '
          TabOrder = 0
          object RxLabel19: TMxLabel
            Left = 10
            Top = 18
            Width = 48
            Height = 13
            Caption = 'Rotate, %'
          end
          object RxLabel20: TMxLabel
            Left = 10
            Top = 37
            Width = 43
            Height = 13
            Caption = 'Move, %'
          end
          object seCameraSR: TMultiObjSpinEdit
            Left = 93
            Top = 18
            Width = 65
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 0
            MaxValue = 100
            MinValue = 1
            ValueType = vtFloat
            Value = 60
            AutoSize = False
            TabOrder = 0
          end
          object seCameraSM: TMultiObjSpinEdit
            Left = 93
            Top = 37
            Width = 65
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 0
            MaxValue = 100
            MinValue = 1
            ValueType = vtFloat
            Value = 60
            AutoSize = False
            TabOrder = 1
          end
        end
        object GroupBox4: TGroupBox
          Left = 0
          Top = 0
          Width = 388
          Height = 56
          Align = alTop
          Caption = ' Viewport '
          TabOrder = 1
          object RxLabel11: TMxLabel
            Left = 6
            Top = 16
            Width = 68
            Height = 13
            Caption = 'Near plane, m'
          end
          object RxLabel1: TMxLabel
            Left = 206
            Top = 16
            Width = 60
            Height = 13
            Caption = 'Far plane, m'
          end
          object RxLabel22: TMxLabel
            Left = 6
            Top = 34
            Width = 106
            Height = 13
            Caption = 'Perspective FOV, deg'
          end
          object seZNear: TMultiObjSpinEdit
            Left = 117
            Top = 14
            Width = 65
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.05
            MaxValue = 10
            ValueType = vtFloat
            Value = 0.1
            AutoSize = False
            TabOrder = 0
          end
          object seZFar: TMultiObjSpinEdit
            Left = 289
            Top = 14
            Width = 65
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Decimal = 1
            MaxValue = 10000
            MinValue = 10
            ValueType = vtFloat
            Value = 1500
            AutoSize = False
            TabOrder = 1
          end
          object seFOV: TMultiObjSpinEdit
            Left = 117
            Top = 32
            Width = 65
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 0
            MaxValue = 170
            MinValue = 3
            ValueType = vtFloat
            Value = 60
            AutoSize = False
            TabOrder = 2
          end
        end
        object GroupBox5: TGroupBox
          Left = 0
          Top = 56
          Width = 192
          Height = 61
          Caption = ' Fly Style '
          TabOrder = 2
          object RxLabel21: TMxLabel
            Left = 10
            Top = 18
            Width = 72
            Height = 13
            Caption = 'Fly speed, m/c'
          end
          object RxLabel15: TMxLabel
            Left = 10
            Top = 37
            Width = 66
            Height = 13
            Caption = 'Fly altitude, m'
          end
          object seCameraFlySpeed: TMultiObjSpinEdit
            Left = 117
            Top = 18
            Width = 65
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Increment = 0.01
            MaxValue = 100
            MinValue = 0.01
            ValueType = vtFloat
            Value = 5
            AutoSize = False
            TabOrder = 0
          end
          object seCameraFlyAltitude: TMultiObjSpinEdit
            Left = 117
            Top = 37
            Width = 65
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Increment = 0.01
            MaxValue = 1000
            ValueType = vtFloat
            Value = 1.8
            AutoSize = False
            TabOrder = 1
          end
        end
      end
      object tsMouse: TTabSheet
        Caption = 'Mouse '
        ImageIndex = 1
        object GroupBox1: TGroupBox
          Left = 0
          Top = 0
          Width = 169
          Height = 77
          Caption = ' Sensitivity '
          TabOrder = 0
          object RxLabel8: TMxLabel
            Left = 10
            Top = 56
            Width = 43
            Height = 13
            Caption = 'Scale, %'
          end
          object RxLabel2: TMxLabel
            Left = 10
            Top = 17
            Width = 48
            Height = 13
            Caption = 'Rotate, %'
          end
          object RxLabel4: TMxLabel
            Left = 10
            Top = 36
            Width = 43
            Height = 13
            Caption = 'Move, %'
          end
          object seSM: TMultiObjSpinEdit
            Left = 79
            Top = 35
            Width = 87
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 0
            MaxValue = 100
            MinValue = 1
            ValueType = vtFloat
            Value = 60
            AutoSize = False
            TabOrder = 1
          end
          object seSR: TMultiObjSpinEdit
            Left = 79
            Top = 17
            Width = 87
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 0
            MaxValue = 100
            MinValue = 1
            ValueType = vtFloat
            Value = 60
            AutoSize = False
            TabOrder = 0
          end
          object seSS: TMultiObjSpinEdit
            Left = 79
            Top = 53
            Width = 87
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 0
            MaxValue = 100
            MinValue = 1
            ValueType = vtFloat
            Value = 60
            AutoSize = False
            TabOrder = 2
          end
        end
        object GroupBox2: TGroupBox
          Left = 0
          Top = 80
          Width = 169
          Height = 117
          Caption = ' 3D Cursor '
          TabOrder = 1
          object RxLabel6: TMxLabel
            Left = 10
            Top = 22
            Width = 71
            Height = 13
            AutoSize = False
            Caption = 'Segment'
          end
          object RxLabel7: TMxLabel
            Left = 10
            Top = 41
            Width = 22
            Height = 13
            Caption = 'Size'
          end
          object RxLabel9: TMxLabel
            Left = 10
            Top = 60
            Width = 48
            Height = 13
            Caption = 'Up Depth'
          end
          object RxLabel10: TMxLabel
            Left = 10
            Top = 79
            Width = 48
            Height = 13
            Caption = 'Dn Depth'
          end
          object RxLabel16: TMxLabel
            Left = 10
            Top = 98
            Width = 26
            Height = 13
            Caption = 'Color'
          end
          object mc3DCursorColor: TMultiObjColor
            Left = 79
            Top = 97
            Width = 86
            Height = 16
            Brush.Color = clLime
            OnMouseDown = mcColorDialogClick
          end
          object seBrushSize: TMultiObjSpinEdit
            Left = 79
            Top = 39
            Width = 87
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.1
            MaxValue = 100
            MinValue = 0.05
            ValueType = vtFloat
            Value = 1
            AutoSize = False
            TabOrder = 1
          end
          object seBrushUpDepth: TMultiObjSpinEdit
            Left = 79
            Top = 58
            Width = 87
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.1
            MaxValue = 100
            MinValue = 0.05
            ValueType = vtFloat
            Value = 2
            AutoSize = False
            TabOrder = 2
          end
          object seBrushDnDepth: TMultiObjSpinEdit
            Left = 79
            Top = 77
            Width = 87
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.1
            MaxValue = 100
            MinValue = 0.05
            ValueType = vtFloat
            Value = 20
            AutoSize = False
            TabOrder = 3
          end
          object seBrushSegment: TMultiObjSpinEdit
            Left = 79
            Top = 20
            Width = 87
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 0
            MaxValue = 100
            MinValue = 3
            ValueType = vtFloat
            Value = 3
            AutoSize = False
            TabOrder = 0
          end
        end
      end
      object tsScene: TTabSheet
        Caption = 'Scene'
        ImageIndex = 2
        object RxLabel17: TMxLabel
          Left = 189
          Top = 3
          Width = 62
          Height = 13
          Caption = 'Undo Levels'
        end
        object seUndoLevels: TMultiObjSpinEdit
          Left = 283
          Top = 1
          Width = 103
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Decimal = 1
          MaxValue = 10000
          Value = 125
          AutoSize = False
          TabOrder = 0
        end
        object GroupBox7: TGroupBox
          Left = 0
          Top = 0
          Width = 181
          Height = 77
          Caption = ' Box Pick '
          TabOrder = 1
          object RxLabel18: TMxLabel
            Left = 5
            Top = 33
            Width = 99
            Height = 13
            Caption = 'Depth Tolerance (m)'
          end
          object cbBoxPickLimitedDepth: TCheckBox
            Left = 4
            Top = 15
            Width = 118
            Height = 17
            Alignment = taLeftJustify
            Caption = 'Limited Depth'
            Checked = True
            State = cbChecked
            TabOrder = 0
          end
          object seBoxPickDepthTolerance: TMultiObjSpinEdit
            Left = 109
            Top = 32
            Width = 70
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Increment = 0.01
            MaxValue = 10000
            ValueType = vtFloat
            Value = 0.1
            AutoSize = False
            TabOrder = 1
          end
          object cbBoxPickFaceCulling: TCheckBox
            Left = 4
            Top = 50
            Width = 118
            Height = 17
            Alignment = taLeftJustify
            Caption = 'Back Face Culling'
            Checked = True
            State = cbChecked
            TabOrder = 2
          end
        end
        object GroupBox13: TGroupBox
          Left = 0
          Top = 77
          Width = 181
          Height = 53
          Caption = ' Snap '
          TabOrder = 2
          object RxLabel28: TMxLabel
            Left = 4
            Top = 15
            Width = 62
            Height = 13
            Caption = 'Angle (grad):'
          end
          object RxLabel29: TMxLabel
            Left = 4
            Top = 33
            Width = 49
            Height = 13
            Caption = 'Move (m):'
          end
          object seSnapMove: TMultiObjSpinEdit
            Left = 109
            Top = 31
            Width = 69
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Increment = 0.01
            MaxValue = 1000
            MinValue = 0.01
            ValueType = vtFloat
            Value = 0.1
            AutoSize = False
            TabOrder = 1
          end
          object seSnapAngle: TMultiObjSpinEdit
            Left = 109
            Top = 13
            Width = 69
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.1
            MaxValue = 360
            ValueType = vtFloat
            Value = 5
            AutoSize = False
            TabOrder = 0
          end
        end
        object GroupBox6: TGroupBox
          Left = 185
          Top = 20
          Width = 203
          Height = 57
          Caption = ' Detail Objects '
          TabOrder = 3
          object RxLabel12: TMxLabel
            Left = 5
            Top = 17
            Width = 69
            Height = 13
            Caption = 'Hide Distance'
          end
          object RxLabel13: TMxLabel
            Left = 5
            Top = 36
            Width = 96
            Height = 13
            Caption = 'Ray Pick Height (m)'
          end
          object seDOHideDistance: TMultiObjSpinEdit
            Left = 98
            Top = 16
            Width = 103
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.1
            MaxValue = 1000
            ValueType = vtFloat
            Value = 50
            AutoSize = False
            TabOrder = 0
          end
          object seDORayPickHeight: TMultiObjSpinEdit
            Left = 98
            Top = 35
            Width = 103
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.1
            MaxValue = 1000
            ValueType = vtFloat
            Value = 5
            AutoSize = False
            TabOrder = 1
          end
        end
        object GroupBox3: TGroupBox
          Left = 0
          Top = 130
          Width = 181
          Height = 53
          Caption = ' Grid '
          TabOrder = 4
          object MxLabel1: TMxLabel
            Left = 4
            Top = 15
            Width = 79
            Height = 13
            Caption = 'Square Size (m):'
          end
          object MxLabel2: TMxLabel
            Left = 4
            Top = 33
            Width = 79
            Height = 13
            Caption = 'Number of Cells:'
          end
          object seGridNumberOfCells: TMultiObjSpinEdit
            Left = 109
            Top = 31
            Width = 69
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Increment = 10
            MaxValue = 1000
            MinValue = 10
            Value = 100
            AutoSize = False
            TabOrder = 1
          end
          object seGridSquareSize: TMultiObjSpinEdit
            Left = 109
            Top = 13
            Width = 69
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 1
            MaxValue = 10
            MinValue = 0.1
            ValueType = vtFloat
            Value = 1
            AutoSize = False
            TabOrder = 0
          end
        end
      end
      object tsEdit: TTabSheet
        Caption = 'Edit'
        ImageIndex = 3
        object GroupBox9: TGroupBox
          Left = 0
          Top = 0
          Width = 388
          Height = 215
          Align = alClient
          Caption = ' Append Object '
          TabOrder = 0
          object GroupBox10: TGroupBox
            Left = 2
            Top = 12
            Width = 233
            Height = 66
            Caption = 'Random Scale'
            TabOrder = 0
            object RxLabel3: TMxLabel
              Left = 8
              Top = 29
              Width = 52
              Height = 13
              Caption = 'Min (XYZ):'
            end
            object RxLabel5: TMxLabel
              Left = 8
              Top = 47
              Width = 55
              Height = 13
              Caption = 'Max (XYZ):'
            end
            object cbRandomScale: TCheckBox
              Left = 6
              Top = 12
              Width = 70
              Height = 17
              Alignment = taLeftJustify
              Caption = 'Enabled'
              Checked = True
              State = cbChecked
              TabOrder = 0
            end
            object seRandomScaleMinX: TMultiObjSpinEdit
              Left = 63
              Top = 27
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 1
            end
            object seRandomScaleMinY: TMultiObjSpinEdit
              Left = 119
              Top = 27
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 2
            end
            object seRandomScaleMinZ: TMultiObjSpinEdit
              Left = 175
              Top = 27
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 3
            end
            object seRandomScaleMaxX: TMultiObjSpinEdit
              Left = 63
              Top = 45
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 4
            end
            object seRandomScaleMaxY: TMultiObjSpinEdit
              Left = 119
              Top = 45
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 5
            end
            object seRandomScaleMaxZ: TMultiObjSpinEdit
              Left = 175
              Top = 45
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 6
            end
          end
          object GroupBox11: TGroupBox
            Left = 2
            Top = 78
            Width = 233
            Height = 66
            Caption = 'Random Rotation'
            TabOrder = 1
            object RxLabel23: TMxLabel
              Left = 8
              Top = 29
              Width = 52
              Height = 13
              Caption = 'Min (XYZ):'
            end
            object RxLabel24: TMxLabel
              Left = 8
              Top = 47
              Width = 55
              Height = 13
              Caption = 'Max (XYZ):'
            end
            object cbRandomRotation: TCheckBox
              Left = 6
              Top = 12
              Width = 70
              Height = 17
              Alignment = taLeftJustify
              Caption = 'Enabled'
              Checked = True
              State = cbChecked
              TabOrder = 0
            end
            object seRandomRotateMinX: TMultiObjSpinEdit
              Left = 63
              Top = 27
              Width = 56
              Height = 18
              LWSensitivity = 0.01
              ButtonKind = bkLightWave
              MaxValue = 180
              MinValue = -180
              AutoSize = False
              TabOrder = 1
            end
            object seRandomRotateMinY: TMultiObjSpinEdit
              Left = 119
              Top = 27
              Width = 56
              Height = 18
              LWSensitivity = 0.01
              ButtonKind = bkLightWave
              MaxValue = 180
              MinValue = -180
              AutoSize = False
              TabOrder = 2
            end
            object seRandomRotateMinZ: TMultiObjSpinEdit
              Left = 175
              Top = 27
              Width = 56
              Height = 18
              LWSensitivity = 0.01
              ButtonKind = bkLightWave
              MaxValue = 180
              MinValue = -180
              AutoSize = False
              TabOrder = 3
            end
            object seRandomRotateMaxX: TMultiObjSpinEdit
              Left = 63
              Top = 45
              Width = 56
              Height = 18
              LWSensitivity = 0.01
              ButtonKind = bkLightWave
              MaxValue = 180
              MinValue = -180
              AutoSize = False
              TabOrder = 4
            end
            object seRandomRotateMaxY: TMultiObjSpinEdit
              Left = 119
              Top = 45
              Width = 56
              Height = 18
              LWSensitivity = 0.01
              ButtonKind = bkLightWave
              MaxValue = 180
              MinValue = -180
              AutoSize = False
              TabOrder = 5
            end
            object seRandomRotateMaxZ: TMultiObjSpinEdit
              Left = 175
              Top = 45
              Width = 56
              Height = 18
              LWSensitivity = 0.01
              ButtonKind = bkLightWave
              MaxValue = 180
              MinValue = -180
              AutoSize = False
              TabOrder = 6
            end
          end
          object GroupBox12: TGroupBox
            Left = 4
            Top = 145
            Width = 119
            Height = 66
            Caption = 'Random Size'
            TabOrder = 2
            object RxLabel25: TMxLabel
              Left = 8
              Top = 29
              Width = 22
              Height = 13
              Caption = 'Min:'
            end
            object RxLabel26: TMxLabel
              Left = 8
              Top = 47
              Width = 25
              Height = 13
              Caption = 'Max:'
            end
            object cbRandomSize: TCheckBox
              Left = 6
              Top = 12
              Width = 68
              Height = 17
              Alignment = taLeftJustify
              Caption = 'Enabled'
              Checked = True
              State = cbChecked
              TabOrder = 0
            end
            object seRandomSizeMax: TMultiObjSpinEdit
              Left = 61
              Top = 45
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 2
            end
            object seRandomSizeMin: TMultiObjSpinEdit
              Left = 61
              Top = 27
              Width = 56
              Height = 18
              LWSensitivity = 0.1
              ButtonKind = bkLightWave
              Increment = 0.01
              MaxValue = 100
              MinValue = 0.01
              ValueType = vtFloat
              Value = 1
              AutoSize = False
              TabOrder = 1
            end
          end
        end
      end
    end
  end
  object paBottom: TPanel
    Left = 0
    Top = 243
    Width = 396
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 228
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 310
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Main Form'
    Options = []
    RegistryRoot = prLocalMachine
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'seBrushSize.Value'
      'seBrushDnDepth.Value'
      'seBrushUpDepth.Value'
      'mc3DCursorColor.Brush'
      'seZFar.Value'
      'seZNear.Value'
      'seRandomRotateMaxX.Value'
      'seRandomRotateMaxY.Value'
      'seRandomRotateMaxZ.Value'
      'seRandomRotateMinX.Value'
      'seRandomRotateMinY.Value'
      'seRandomRotateMinZ.Value'
      'seRandomScaleMaxX.Value'
      'seRandomScaleMaxY.Value'
      'seRandomScaleMaxZ.Value'
      'seRandomScaleMinX.Value'
      'seRandomScaleMinY.Value'
      'seRandomScaleMinZ.Value'
      'cbRandomRotation.Checked'
      'cbRandomScale.Checked'
      'cbRandomSize.Checked'
      'seRandomSizeMax.Value'
      'seRandomSizeMin.Value'
      'seBrushSegment.Value'
      'seFOV.Value'
      'seSM.Value'
      'seSR.Value'
      'seSS.Value'
      'seCameraSM.Value'
      'seCameraSR.Value'
      'pcObjects.ActivePage'
      'seDOHideDistance.Value'
      'seDORayPickHeight.Value'
      'cbBoxPickFaceCulling.Checked'
      'cbBoxPickLimitedDepth.Checked'
      'seBoxPickDepthTolerance.Value'
      'seUndoLevels.Value'
      'seSnapAngle.Value'
      'seSnapMove.Value'
      'seCameraFlyAltitude.Value'
      'seCameraFlySpeed.Value')
    StoredValues = <>
    Left = 273
    Top = 9
  end
end
