object frmPropertiesPClipper: TfrmPropertiesPClipper
  Left = 418
  Top = 293
  BorderStyle = bsDialog
  Caption = 'PClipper properties'
  ClientHeight = 71
  ClientWidth = 297
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
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 297
    Height = 41
    Align = alTop
    Caption = 'PClipper:'
    TabOrder = 0
    object StaticText1: TStaticText
      Left = 8
      Top = 15
      Width = 206
      Height = 20
      Alignment = taCenter
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = 'Density:'
      TabOrder = 0
    end
    object seRange: TMultiObjSpinEdit
      Left = 216
      Top = 14
      Width = 73
      Height = 21
      Increment = 0.1
      MaxValue = 1
      ValueType = vtFloat
      Value = 0.5
      AutoSize = False
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 41
    Width = 297
    Height = 28
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object btApply: TButton
      Left = 8
      Top = 5
      Width = 89
      Height = 20
      Caption = 'Apply'
      TabOrder = 0
      OnClick = btApplyClick
    end
    object btOk: TButton
      Left = 104
      Top = 5
      Width = 89
      Height = 20
      Caption = 'Ok'
      ModalResult = 1
      TabOrder = 1
      OnClick = btOkClick
    end
    object btCancel: TButton
      Left = 200
      Top = 5
      Width = 89
      Height = 20
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 2
    end
  end
end
