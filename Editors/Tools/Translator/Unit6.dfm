object AddLangDialog: TAddLangDialog
  Left = 291
  Top = 382
  BorderStyle = bsDialog
  Caption = 'Add language'
  ClientHeight = 67
  ClientWidth = 188
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object RxLabel1: TRxLabel
    Left = 4
    Top = 20
    Width = 91
    Height = 13
    Caption = 'Language shortcut'
  end
  object LangShortcut: TEdit
    Left = 104
    Top = 12
    Width = 81
    Height = 21
    MaxLength = 16
    TabOrder = 0
    OnChange = LangShortcutChange
  end
  object Button1: TButton
    Left = 0
    Top = 40
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    Enabled = False
    ModalResult = 1
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 112
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
end
