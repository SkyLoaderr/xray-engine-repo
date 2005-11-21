object StatsDialog: TStatsDialog
  Left = 237
  Top = 347
  BorderStyle = bsDialog
  Caption = 'Statistic'
  ClientHeight = 115
  ClientWidth = 425
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
    Width = 209
    Height = 85
    Caption = 'Source language'
    TabOrder = 0
    object Panel1: TPanel
      Left = 4
      Top = 20
      Width = 97
      Height = 17
      Caption = 'Files'
      TabOrder = 0
    end
    object Panel2: TPanel
      Left = 4
      Top = 40
      Width = 97
      Height = 17
      Caption = 'Strings'
      TabOrder = 1
    end
    object Panel3: TPanel
      Left = 4
      Top = 60
      Width = 97
      Height = 17
      Caption = 'Words'
      TabOrder = 2
    end
    object SrcFiles: TPanel
      Left = 104
      Top = 20
      Width = 97
      Height = 17
      Caption = '0'
      TabOrder = 3
    end
    object SrcStrings: TPanel
      Left = 104
      Top = 40
      Width = 97
      Height = 17
      Caption = '0'
      TabOrder = 4
    end
    object SrcWords: TPanel
      Left = 104
      Top = 60
      Width = 97
      Height = 17
      Caption = '0'
      TabOrder = 5
    end
  end
  object GroupBox2: TGroupBox
    Left = 216
    Top = 0
    Width = 209
    Height = 85
    Caption = 'Destination language'
    TabOrder = 1
    object Panel4: TPanel
      Left = 4
      Top = 20
      Width = 97
      Height = 17
      Caption = 'Files'
      TabOrder = 0
    end
    object Panel5: TPanel
      Left = 4
      Top = 40
      Width = 97
      Height = 17
      Caption = 'Strings'
      TabOrder = 1
    end
    object Panel6: TPanel
      Left = 4
      Top = 60
      Width = 97
      Height = 17
      Caption = 'Words'
      TabOrder = 2
    end
    object DestFiles: TPanel
      Left = 104
      Top = 20
      Width = 97
      Height = 17
      Caption = '0'
      TabOrder = 3
    end
    object DestStrings: TPanel
      Left = 104
      Top = 40
      Width = 97
      Height = 17
      Caption = '0'
      TabOrder = 4
    end
    object DestWords: TPanel
      Left = 104
      Top = 60
      Width = 97
      Height = 17
      Caption = '0'
      TabOrder = 5
    end
  end
  object Button1: TButton
    Left = 175
    Top = 88
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
end
