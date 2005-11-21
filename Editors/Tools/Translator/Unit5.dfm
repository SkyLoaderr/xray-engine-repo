object DiffStrDialog: TDiffStrDialog
  Left = 297
  Top = 347
  BorderStyle = bsDialog
  Caption = 'Added different strings'
  ClientHeight = 327
  ClientWidth = 544
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
    Left = 161
    Top = 4
    Width = 223
    Height = 13
    Caption = 'This strings was added from import to database'
  end
  object RxLabel2: TRxLabel
    Left = 4
    Top = 36
    Width = 23
    Height = 13
    Caption = 'Files'
  end
  object RxLabel3: TRxLabel
    Left = 268
    Top = 36
    Width = 34
    Height = 13
    Caption = 'Strings'
  end
  object FilesList: TListBox
    Left = 4
    Top = 52
    Width = 261
    Height = 241
    ItemHeight = 13
    TabOrder = 0
    OnClick = FilesListClick
  end
  object StringsList: TListBox
    Left = 268
    Top = 52
    Width = 273
    Height = 241
    ItemHeight = 13
    TabOrder = 1
    OnClick = GotoButtonClick
  end
  object Button1: TButton
    Left = 3
    Top = 300
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 2
    TabOrder = 2
  end
  object GotoButton: TButton
    Left = 468
    Top = 300
    Width = 75
    Height = 25
    Caption = 'Goto'
    ModalResult = 1
    TabOrder = 3
    OnClick = GotoButtonClick
  end
end
