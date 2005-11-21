object LangDialog: TLangDialog
  Left = 299
  Top = 507
  BorderStyle = bsDialog
  Caption = 'Language selection'
  ClientHeight = 163
  ClientWidth = 270
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 133
    Height = 129
    Caption = 'Source'
    TabOrder = 0
    object SourceList: TListBox
      Left = 4
      Top = 16
      Width = 125
      Height = 109
      ItemHeight = 13
      TabOrder = 0
      OnClick = SourceListClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 136
    Top = 0
    Width = 133
    Height = 129
    Caption = 'Destination'
    TabOrder = 1
    object DestList: TListBox
      Left = 4
      Top = 16
      Width = 125
      Height = 109
      ItemHeight = 13
      TabOrder = 0
      OnClick = SourceListClick
    end
  end
  object Button1: TButton
    Left = 0
    Top = 136
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object Button2: TButton
    Left = 192
    Top = 136
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
end
