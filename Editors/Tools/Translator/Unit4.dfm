object DiffFilesDialog: TDiffFilesDialog
  Left = 352
  Top = 388
  BorderStyle = bsDialog
  Caption = 'Added different files'
  ClientHeight = 158
  ClientWidth = 339
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
    Left = 64
    Top = 4
    Width = 211
    Height = 13
    Caption = 'This files was added to database from import'
  end
  object FilesList: TListBox
    Left = 3
    Top = 28
    Width = 333
    Height = 97
    ItemHeight = 13
    TabOrder = 0
  end
  object Button1: TButton
    Left = 132
    Top = 132
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
end
