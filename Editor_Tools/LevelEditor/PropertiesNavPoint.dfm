object frmPropertiesNavPoint: TfrmPropertiesNavPoint
  Left = 418
  Top = 293
  BorderStyle = bsDialog
  Caption = 'Navigation point properties'
  ClientHeight = 70
  ClientWidth = 233
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
  object btApply: TButton
    Left = 10
    Top = 46
    Width = 74
    Height = 20
    Caption = 'Apply'
    TabOrder = 0
    OnClick = btApplyClick
  end
  object btOk: TButton
    Left = 84
    Top = 46
    Width = 74
    Height = 20
    Caption = 'Ok'
    ModalResult = 1
    TabOrder = 1
    OnClick = btOkClick
  end
  object btCancel: TButton
    Left = 159
    Top = 46
    Width = 74
    Height = 20
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 233
    Height = 41
    Align = alTop
    Caption = 'Type:'
    TabOrder = 3
    object cbTypeStandart: TMultiObjCheck
      Left = 8
      Top = 16
      Width = 97
      Height = 17
      Caption = 'Standart'
      Checked = True
      State = cbChecked
      TabOrder = 0
      OnClick = cbTypeStandartClick
    end
    object cbTypeItem: TMultiObjCheck
      Left = 120
      Top = 16
      Width = 97
      Height = 17
      Caption = 'Item'
      TabOrder = 1
      OnClick = cbTypeItemClick
    end
  end
end
