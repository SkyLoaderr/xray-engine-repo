object frmPropertiesDO: TfrmPropertiesDO
  Left = 486
  Top = 307
  BorderStyle = bsDialog
  Caption = 'Detail properties'
  ClientHeight = 112
  ClientWidth = 177
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
  object paBottom: TPanel
    Left = 0
    Top = 93
    Width = 177
    Height = 19
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 12
      Top = 0
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
      Left = 94
      Top = 0
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
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 177
    Height = 93
    Align = alClient
    Caption = ' Properties '
    TabOrder = 1
    object RxLabel5: TLabel
      Left = 5
      Top = 16
      Width = 49
      Height = 13
      Caption = 'Scale min:'
    end
    object RxLabel6: TLabel
      Left = 5
      Top = 36
      Width = 52
      Height = 13
      Caption = 'Scele max:'
    end
    object RxLabel1: TLabel
      Left = 5
      Top = 56
      Width = 68
      Height = 13
      Caption = 'Density factor:'
    end
    object seScaleMinY: TMultiObjSpinEdit
      Left = 81
      Top = 13
      Width = 84
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 100
      MinValue = 0.01
      ValueType = vtFloat
      Value = 1
      AutoSize = False
      TabOrder = 0
      OnChange = OnModified
    end
    object seScaleMaxY: TMultiObjSpinEdit
      Left = 81
      Top = 33
      Width = 84
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
      OnChange = OnModified
    end
    object seDensityFactor: TMultiObjSpinEdit
      Left = 81
      Top = 53
      Width = 84
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 1
      MinValue = 0.1
      ValueType = vtFloat
      Value = 1
      AutoSize = False
      TabOrder = 2
      OnChange = OnModified
    end
    object cbNoWaving: TMultiObjCheck
      Left = 3
      Top = 72
      Width = 91
      Height = 17
      Alignment = taLeftJustify
      Caption = 'No waving:'
      TabOrder = 3
      OnClick = OnModified
    end
  end
end
