object frmBuildProgress: TfrmBuildProgress
  Left = 433
  Top = 188
  BorderStyle = bsDialog
  Caption = 'Build Progress'
  ClientHeight = 93
  ClientWidth = 384
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 31
    Height = 13
    Caption = 'Stage:'
  end
  object lbStage: TLabel
    Left = 48
    Top = 8
    Width = 329
    Height = 13
    AutoSize = False
    Caption = 'lbStage'
  end
  object btStop: TButton
    Left = 302
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Stop'
    ModalResult = 2
    TabOrder = 0
  end
  object pb: TProgressBar
    Left = 8
    Top = 32
    Width = 369
    Height = 25
    Min = 0
    Max = 100
    Position = 12
    Smooth = True
    TabOrder = 1
  end
end
