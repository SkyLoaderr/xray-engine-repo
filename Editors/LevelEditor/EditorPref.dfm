object frmEditPrefs: TfrmEditPrefs
  Left = 390
  Top = 445
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
  OnShow = FormShow
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
      ActivePage = tsCamera
      Align = alClient
      TabIndex = 0
      TabOrder = 0
      object tsCamera: TTabSheet
        Caption = 'Camera'
        ImageIndex = 5
        object GroupBox8: TGroupBox
          Left = 196
          Top = 75
          Width = 192
          Height = 61
          Caption = ' Sensitivity '
          TabOrder = 0
          object RxLabel19: TLabel
            Left = 10
            Top = 18
            Width = 46
            Height = 13
            Caption = 'Rotate, %'
          end
          object RxLabel20: TLabel
            Left = 10
            Top = 37
            Width = 41
            Height = 13
            Caption = 'Move, %'
          end
          object seCameraSR: TMultiObjSpinEdit
            Left = 117
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
            Left = 117
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
          Width = 192
          Height = 75
          Caption = ' Viewport '
          TabOrder = 1
          object RxLabel11: TLabel
            Left = 6
            Top = 16
            Width = 66
            Height = 13
            Caption = 'Near plane, m'
          end
          object RxLabel1: TLabel
            Left = 6
            Top = 35
            Width = 58
            Height = 13
            Caption = 'Far plane, m'
          end
          object RxLabel22: TLabel
            Left = 6
            Top = 54
            Width = 104
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
            Increment = 0.05
            MaxValue = 10
            MinValue = 0.01
            ValueType = vtFloat
            Value = 0.1
            AutoSize = False
            TabOrder = 0
          end
          object seZFar: TMultiObjSpinEdit
            Left = 117
            Top = 33
            Width = 65
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            MaxValue = 10000
            MinValue = 10
            ValueType = vtFloat
            Value = 1500
            AutoSize = False
            TabOrder = 1
          end
          object seFOV: TMultiObjSpinEdit
            Left = 117
            Top = 52
            Width = 65
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 1
            MaxValue = 170
            MinValue = 0.1
            ValueType = vtFloat
            Value = 60
            AutoSize = False
            TabOrder = 2
          end
        end
        object GroupBox5: TGroupBox
          Left = 0
          Top = 75
          Width = 192
          Height = 61
          Caption = ' Fly Style '
          TabOrder = 2
          object RxLabel21: TLabel
            Left = 10
            Top = 18
            Width = 70
            Height = 13
            Caption = 'Fly speed, m/c'
          end
          object RxLabel15: TLabel
            Left = 10
            Top = 37
            Width = 64
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
        object GroupBox6: TGroupBox
          Left = 196
          Top = 0
          Width = 192
          Height = 75
          Caption = ' Fog '
          TabOrder = 3
          object RxLabel30: TLabel
            Left = 6
            Top = 35
            Width = 40
            Height = 13
            Caption = 'Fogness'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
          end
          object RxLabel32: TLabel
            Left = 6
            Top = 16
            Width = 24
            Height = 13
            Caption = 'Color'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
          end
          object mcFogColor: TMultiObjColor
            Left = 117
            Top = 16
            Width = 65
            Height = 16
            Brush.Color = 5592405
            OnMouseDown = mcColorDialogClick
          end
          object seFogness: TMultiObjSpinEdit
            Left = 117
            Top = 33
            Width = 65
            Height = 18
            LWSensitivity = 0.01
            ButtonKind = bkLightWave
            Decimal = 1
            Increment = 0.1
            MaxValue = 100
            ValueType = vtFloat
            Value = 0.9
            AutoSize = False
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
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
          object RxLabel8: TLabel
            Left = 10
            Top = 56
            Width = 41
            Height = 13
            Caption = 'Scale, %'
          end
          object RxLabel2: TLabel
            Left = 10
            Top = 17
            Width = 46
            Height = 13
            Caption = 'Rotate, %'
          end
          object RxLabel4: TLabel
            Left = 10
            Top = 36
            Width = 41
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
          object RxLabel6: TLabel
            Left = 10
            Top = 22
            Width = 71
            Height = 13
            AutoSize = False
            Caption = 'Segment'
          end
          object RxLabel7: TLabel
            Left = 10
            Top = 41
            Width = 20
            Height = 13
            Caption = 'Size'
          end
          object RxLabel9: TLabel
            Left = 10
            Top = 60
            Width = 46
            Height = 13
            Caption = 'Up Depth'
          end
          object RxLabel10: TLabel
            Left = 10
            Top = 79
            Width = 46
            Height = 13
            Caption = 'Dn Depth'
          end
          object RxLabel16: TLabel
            Left = 10
            Top = 98
            Width = 24
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
        object RxLabel17: TLabel
          Left = 189
          Top = 3
          Width = 60
          Height = 13
          Caption = 'Undo Levels'
        end
        object Label2: TLabel
          Left = 189
          Top = 22
          Width = 90
          Height = 13
          Caption = 'Recent Files Count'
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
          object RxLabel18: TLabel
            Left = 5
            Top = 33
            Width = 97
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
          Height = 70
          Caption = ' Snap '
          TabOrder = 2
          object RxLabel28: TLabel
            Left = 4
            Top = 15
            Width = 60
            Height = 13
            Caption = 'Angle (grad):'
          end
          object RxLabel29: TLabel
            Left = 4
            Top = 33
            Width = 47
            Height = 13
            Caption = 'Move (m):'
          end
          object Label1: TLabel
            Left = 4
            Top = 51
            Width = 63
            Height = 13
            Caption = 'Move To (m):'
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
          object seSnapMoveTo: TMultiObjSpinEdit
            Left = 109
            Top = 49
            Width = 69
            Height = 18
            LWSensitivity = 0.1
            ButtonKind = bkLightWave
            Increment = 0.01
            MaxValue = 1000
            MinValue = 0.01
            ValueType = vtFloat
            Value = 0.5
            AutoSize = False
            TabOrder = 2
          end
        end
        object GroupBox3: TGroupBox
          Left = 0
          Top = 154
          Width = 181
          Height = 53
          Caption = ' Grid '
          TabOrder = 3
          object MxLabel1: TLabel
            Left = 4
            Top = 15
            Width = 77
            Height = 13
            Caption = 'Square Size (m):'
          end
          object MxLabel2: TLabel
            Left = 4
            Top = 33
            Width = 77
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
        object seRecentFilesCount: TMultiObjSpinEdit
          Left = 283
          Top = 20
          Width = 103
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Decimal = 1
          MaxValue = 20
          Value = 10
          AutoSize = False
          TabOrder = 4
        end
        object cbLeaveEmptyRef: TCheckBox
          Left = 187
          Top = 41
          Width = 198
          Height = 17
          Alignment = taLeftJustify
          Caption = 'Leave Empty Refs Editable Object'
          Checked = True
          State = cbChecked
          TabOrder = 5
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
      'seBrushSegment.Value'
      'seFOV.Value'
      'seSM.Value'
      'seSR.Value'
      'seSS.Value'
      'seCameraSM.Value'
      'seCameraSR.Value'
      'pcObjects.ActivePage'
      'cbBoxPickFaceCulling.Checked'
      'cbBoxPickLimitedDepth.Checked'
      'seBoxPickDepthTolerance.Value'
      'seUndoLevels.Value'
      'seSnapAngle.Value'
      'seSnapMove.Value'
      'seCameraFlyAltitude.Value'
      'seCameraFlySpeed.Value'
      'seSnapMoveTo.Value'
      'seRecentFilesCount.Value'
      'cbLeaveEmptyRef.Checked'
      'seFogness.Value')
    StoredValues = <>
    Left = 273
    Top = 9
  end
end
