object Form6: TForm6
  Left = 310
  Top = 533
  Width = 275
  Height = 332
  BorderIcons = [biSystemMenu]
  Caption = 'Constructor'
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
    Tag = 1000
    Left = 0
    Top = 280
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 0
  end
  object Button2: TButton
    Tag = 1000
    Left = 170
    Top = 280
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object ColorDialog: TColorDialog
    Ctl3D = True
    Options = [cdFullOpen, cdSolidColor]
    Left = 36
    Top = 60
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'con'
    Filter = 'Constructor files (*.con)|*.con|All files (*.*)|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofFileMustExist, ofEnableSizing]
    Left = 68
    Top = 60
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'con'
    Filter = 'Constructor files (*.con)|*.con|All files (*.*)|*.*'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 100
    Top = 60
  end
end
