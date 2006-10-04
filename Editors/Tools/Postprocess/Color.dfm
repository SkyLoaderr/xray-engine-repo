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
  object RxLabel1: TRxLabel
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
    object RxLabel2: TRxLabel
      Left = 12
      Top = 28
      Width = 29
      Height = 13
      Caption = 'Value'
    end
    object RxLabel3: TRxLabel
      Left = 12
      Top = 52
      Width = 40
      Height = 13
      Caption = 'Tension'
    end
    object RxLabel4: TRxLabel
      Left = 12
      Top = 76
      Width = 48
      Height = 13
      Caption = 'Continuity'
    end
    object RxLabel5: TRxLabel
      Left = 12
      Top = 100
      Width = 22
      Height = 13
      Caption = 'Bias'
    end
    object RedValue: TRxSpinEdit
      Left = 88
      Top = 20
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.01
      ValueType = vtFloat
      TabOrder = 0
      OnChange = CnahgeRedParam
    end
    object RedTension: TRxSpinEdit
      Left = 88
      Top = 44
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 1
      OnChange = CnahgeRedParam
    end
    object RedContin: TRxSpinEdit
      Left = 88
      Top = 68
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 2
      OnChange = CnahgeRedParam
    end
    object RedBias: TRxSpinEdit
      Left = 88
      Top = 92
      Width = 93
      Height = 21
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
    object RxLabel6: TRxLabel
      Left = 12
      Top = 28
      Width = 29
      Height = 13
      Caption = 'Value'
    end
    object RxLabel7: TRxLabel
      Left = 12
      Top = 52
      Width = 40
      Height = 13
      Caption = 'Tension'
    end
    object RxLabel8: TRxLabel
      Left = 12
      Top = 76
      Width = 48
      Height = 13
      Caption = 'Continuity'
    end
    object RxLabel9: TRxLabel
      Left = 12
      Top = 100
      Width = 22
      Height = 13
      Caption = 'Bias'
    end
    object GreenValue: TRxSpinEdit
      Left = 88
      Top = 20
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.01
      ValueType = vtFloat
      TabOrder = 0
      OnChange = ChangeGreenParam
    end
    object GreenTension: TRxSpinEdit
      Left = 88
      Top = 44
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 1
      OnChange = ChangeGreenParam
    end
    object GreenContin: TRxSpinEdit
      Left = 88
      Top = 68
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 2
      OnChange = ChangeGreenParam
    end
    object GreenBias: TRxSpinEdit
      Left = 88
      Top = 92
      Width = 93
      Height = 21
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
    object RxLabel10: TRxLabel
      Left = 12
      Top = 28
      Width = 29
      Height = 13
      Caption = 'Value'
    end
    object RxLabel11: TRxLabel
      Left = 12
      Top = 52
      Width = 40
      Height = 13
      Caption = 'Tension'
    end
    object RxLabel12: TRxLabel
      Left = 12
      Top = 76
      Width = 48
      Height = 13
      Caption = 'Continuity'
    end
    object RxLabel13: TRxLabel
      Left = 12
      Top = 100
      Width = 22
      Height = 13
      Caption = 'Bias'
    end
    object BlueValue: TRxSpinEdit
      Left = 88
      Top = 20
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.01
      ValueType = vtFloat
      TabOrder = 0
      OnChange = ChangeBlueParam
    end
    object BlueTension: TRxSpinEdit
      Left = 88
      Top = 44
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 1
      OnChange = ChangeBlueParam
    end
    object BlueContin: TRxSpinEdit
      Left = 88
      Top = 68
      Width = 93
      Height = 21
      Alignment = taRightJustify
      Increment = 0.1
      ValueType = vtFloat
      TabOrder = 2
      OnChange = ChangeBlueParam
    end
    object BlueBias: TRxSpinEdit
      Left = 88
      Top = 92
      Width = 93
      Height = 21
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
