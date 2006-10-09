object SingleParam: TSingleParam
  Left = 228
  Top = 288
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 160
  ClientWidth = 686
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object RxLabel1: TMxLabel
    Left = 0
    Top = 0
    Width = 41
    Height = 13
    Caption = 'Point list'
  end
  object PointList: TListBox
    Left = 0
    Top = 15
    Width = 121
    Height = 145
    ItemHeight = 13
    TabOrder = 0
    OnClick = PointListClick
  end
  object GroupBox1: TGroupBox
    Left = 124
    Top = 12
    Width = 185
    Height = 121
    Caption = 'Node data'
    TabOrder = 1
    object RxLabel2: TMxLabel
      Left = 12
      Top = 28
      Width = 29
      Height = 13
      Caption = 'Value'
    end
    object RxLabel3: TMxLabel
      Left = 12
      Top = 52
      Width = 40
      Height = 13
      Caption = 'Tension'
    end
    object RxLabel4: TMxLabel
      Left = 12
      Top = 76
      Width = 48
      Height = 13
      Caption = 'Continuity'
    end
    object RxLabel5: TMxLabel
      Left = 12
      Top = 100
      Width = 22
      Height = 13
      Caption = 'Bias'
    end
    object Value: TMultiObjSpinEdit
      Left = 88
      Top = 20
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.01
      ValueType = vtFloat
      TabOrder = 0
      OnChange = ChangeParam
    end
    object Tension: TMultiObjSpinEdit
      Left = 88
      Top = 44
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 1
      OnChange = ChangeParam
    end
    object Contin: TMultiObjSpinEdit
      Left = 88
      Top = 68
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 2
      OnChange = ChangeParam
    end
    object Bias: TMultiObjSpinEdit
      Left = 88
      Top = 92
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 3
      OnChange = ChangeParam
    end
  end
  object Button1: TButton
    Left = 124
    Top = 135
    Width = 75
    Height = 25
    Caption = 'Constructor'
    TabOrder = 2
    OnClick = Button1Click
  end
end
