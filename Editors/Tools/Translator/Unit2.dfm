object ConfigDialog: TConfigDialog
  Left = 242
  Top = 363
  BorderStyle = bsDialog
  Caption = 'Configuration'
  ClientHeight = 169
  ClientWidth = 426
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    426
    169)
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = 4
    Width = 425
    Height = 57
    Caption = 'Adding languages'
    TabOrder = 0
    object Label1: TLabel
      Left = 8
      Top = 32
      Width = 107
      Height = 13
      Caption = 'First language shortcut'
    end
    object Label2: TLabel
      Left = 216
      Top = 32
      Width = 125
      Height = 13
      Caption = 'Second language shortcut'
    end
    object Bevel1: TBevel
      Left = 208
      Top = 6
      Width = 6
      Height = 72
      Shape = bsRightLine
    end
    object Lang1: TEdit
      Left = 140
      Top = 24
      Width = 69
      Height = 21
      MaxLength = 16
      TabOrder = 0
      OnChange = TextChange
    end
    object Lang2: TEdit
      Left = 348
      Top = 24
      Width = 69
      Height = 21
      MaxLength = 16
      TabOrder = 1
      OnChange = TextChange
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 64
    Width = 425
    Height = 76
    Anchors = [akTop, akBottom]
    Caption = 'Path designating'
    TabOrder = 1
    DesignSize = (
      425
      76)
    object Label3: TLabel
      Left = 8
      Top = 28
      Width = 77
      Height = 36
      Anchors = [akTop, akBottom]
      Caption = 'String base path'
    end
    object Label4: TLabel
      Left = 8
      Top = 52
      Width = 82
      Height = 13
      Caption = 'String import path'
    end
    object Path1: TDirectoryEdit
      Left = 112
      Top = 24
      Width = 305
      Height = 21
      DialogKind = dkWin32
      NumGlyphs = 1
      Anchors = [akTop, akBottom]
      TabOrder = 0
      OnChange = TextChange
    end
    object ImportPath: TDirectoryEdit
      Left = 112
      Top = 48
      Width = 305
      Height = 21
      DialogKind = dkWin32
      NumGlyphs = 1
      TabOrder = 1
      OnChange = TextChange
    end
  end
  object OkButton: TButton
    Left = 1
    Top = 144
    Width = 75
    Height = 24
    Anchors = [akTop, akBottom]
    Caption = 'OK'
    Enabled = False
    ModalResult = 1
    TabOrder = 2
    OnClick = OkButtonClick
  end
  object Button2: TButton
    Left = 350
    Top = 144
    Width = 75
    Height = 24
    Anchors = [akTop, akBottom]
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
end
