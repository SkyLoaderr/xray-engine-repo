object frmPropertiesOccluder: TfrmPropertiesOccluder
  Left = 535
  Top = 375
  BorderStyle = bsDialog
  Caption = 'Occluder properties'
  ClientHeight = 81
  ClientWidth = 193
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
  object ebOk: TExtBtn
    Left = 28
    Top = 61
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
    Left = 110
    Top = 61
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
  object gbOrientation: TGroupBox
    Left = 0
    Top = 0
    Width = 193
    Height = 57
    Align = alTop
    Caption = ' Points '
    TabOrder = 0
    object RxLabel4: TRxLabel
      Left = 9
      Top = 17
      Width = 59
      Height = 13
      Caption = 'Point count:'
    end
    object RxLabel1: TRxLabel
      Left = 9
      Top = 37
      Width = 40
      Height = 13
      Caption = 'Density:'
      Enabled = False
    end
    object sePointCount: TMultiObjSpinEdit
      Left = 72
      Top = 14
      Width = 113
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      MaxValue = 6
      MinValue = 3
      Value = 3
      AutoSize = False
      TabOrder = 0
    end
    object MultiObjSpinEdit1: TMultiObjSpinEdit
      Left = 72
      Top = 34
      Width = 113
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      MaxValue = 1
      ValueType = vtFloat
      Value = 0.5
      AutoSize = False
      Enabled = False
      TabOrder = 1
    end
  end
end
