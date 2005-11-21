object RemovedStringsDialog: TRemovedStringsDialog
  Left = 233
  Top = 366
  BorderStyle = bsDialog
  Caption = 'Removed strings'
  ClientHeight = 250
  ClientWidth = 383
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 4
    Top = 228
    Width = 119
    Height = 13
    Caption = 'This strings was removed'
  end
  object StringsList: TStringGrid
    Left = 0
    Top = 0
    Width = 381
    Height = 213
    ColCount = 2
    DefaultColWidth = 180
    DefaultRowHeight = 16
    FixedCols = 0
    RowCount = 1
    FixedRows = 0
    TabOrder = 0
  end
  object Button1: TButton
    Left = 308
    Top = 224
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
  end
end
