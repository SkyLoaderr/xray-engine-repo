object ColorForm: TColorForm
  Left = 282
  Top = 428
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
    Top = 16
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
    Caption = 'Red component'
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
    object RedValue: TMultiObjSpinEdit
      Left = 88
      Top = 20
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.01
      ValueType = vtFloat
      TabOrder = 0
      OnChange = CnahgeRedParam
    end
    object RedTension: TMultiObjSpinEdit
      Left = 88
      Top = 44
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 1
      OnChange = CnahgeRedParam
    end
    object RedContin: TMultiObjSpinEdit
      Left = 88
      Top = 68
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 2
      OnChange = CnahgeRedParam
    end
    object RedBias: TMultiObjSpinEdit
      Left = 88
      Top = 92
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 3
      OnChange = CnahgeRedParam
    end
  end
  object GroupBox2: TGroupBox
    Left = 312
    Top = 12
    Width = 185
    Height = 121
    Caption = 'Green component'
    TabOrder = 2
    object RxLabel6: TMxLabel
      Left = 12
      Top = 28
      Width = 29
      Height = 13
      Caption = 'Value'
    end
    object RxLabel7: TMxLabel
      Left = 12
      Top = 52
      Width = 40
      Height = 13
      Caption = 'Tension'
    end
    object RxLabel8: TMxLabel
      Left = 12
      Top = 76
      Width = 48
      Height = 13
      Caption = 'Continuity'
    end
    object RxLabel9: TMxLabel
      Left = 12
      Top = 100
      Width = 22
      Height = 13
      Caption = 'Bias'
    end
    object GreenValue: TMultiObjSpinEdit
      Left = 88
      Top = 20
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.01
      ValueType = vtFloat
      TabOrder = 0
      OnChange = ChangeGreenParam
    end
    object GreenTension: TMultiObjSpinEdit
      Left = 88
      Top = 44
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 1
      OnChange = ChangeGreenParam
    end
    object GreenContin: TMultiObjSpinEdit
      Left = 88
      Top = 68
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 2
      OnChange = ChangeGreenParam
    end
    object GreenBias: TMultiObjSpinEdit
      Left = 88
      Top = 92
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 3
      OnChange = ChangeGreenParam
    end
  end
  object GroupBox3: TGroupBox
    Left = 500
    Top = 12
    Width = 185
    Height = 121
    Caption = 'Blue component'
    TabOrder = 3
    object RxLabel10: TMxLabel
      Left = 12
      Top = 28
      Width = 29
      Height = 13
      Caption = 'Value'
    end
    object RxLabel11: TMxLabel
      Left = 12
      Top = 52
      Width = 40
      Height = 13
      Caption = 'Tension'
    end
    object RxLabel12: TMxLabel
      Left = 12
      Top = 76
      Width = 48
      Height = 13
      Caption = 'Continuity'
    end
    object RxLabel13: TMxLabel
      Left = 12
      Top = 100
      Width = 22
      Height = 13
      Caption = 'Bias'
    end
    object BlueValue: TMultiObjSpinEdit
      Left = 88
      Top = 20
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.01
      ValueType = vtFloat
      TabOrder = 0
      OnChange = ChangeBlueParam
    end
    object BlueTension: TMultiObjSpinEdit
      Left = 88
      Top = 44
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 1
      OnChange = ChangeBlueParam
    end
    object BlueContin: TMultiObjSpinEdit
      Left = 88
      Top = 68
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 2
      OnChange = ChangeBlueParam
    end
    object BlueBias: TMultiObjSpinEdit
      Left = 88
      Top = 92
      Width = 93
      Height = 21
      LWSensitivity = 1
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 3
      OnChange = ChangeBlueParam
    end
  end
  object Button1: TButton
    Left = 608
    Top = 140
    Width = 75
    Height = 21
    Caption = 'Constructor'
    TabOrder = 4
    OnClick = Button1Click
  end
  object Color: TPanel
    Left = 124
    Top = 136
    Width = 37
    Height = 21
    TabOrder = 5
    OnClick = ColorClick
  end
  object ColorDialog: TColorDialog
    Ctl3D = True
    Options = [cdFullOpen, cdSolidColor]
    Left = 192
    Top = 132
  end
end
