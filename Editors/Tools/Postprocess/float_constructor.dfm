object Form8: TForm8
  Left = 350
  Top = 648
  Width = 313
  Height = 160
  BorderIcons = [biSystemMenu]
  Caption = 'Single constructor'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 4
    Top = 108
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 0
  end
  object Button2: TButton
    Left = 208
    Top = 108
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'scon'
    Filter = 'Single constructor files (*.scon)|*.scon|All files (*.*)|*.*'
    FilterIndex = 0
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 52
    Top = 28
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'scon'
    Filter = 'Single constructor files (*.scon)|*.scon|All files (*.*)|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofEnableSizing]
    Left = 116
    Top = 28
  end
end
