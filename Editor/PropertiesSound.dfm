object frmPropertiesSound: TfrmPropertiesSound
  Left = 535
  Top = 375
  BorderStyle = bsDialog
  Caption = 'Sound properties'
  ClientHeight = 94
  ClientWidth = 297
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 297
    Height = 65
    Align = alTop
    Caption = 'Sound:'
    TabOrder = 0
    object RxLabel2: TRxLabel
      Left = 11
      Top = 16
      Width = 37
      Height = 13
      Caption = 'Range:'
      ShadowColor = 15263976
    end
    object Bevel2: TBevel
      Left = 79
      Top = 38
      Width = 176
      Height = 17
    end
    object RxLabel1: TRxLabel
      Left = 11
      Top = 40
      Width = 61
      Height = 13
      Caption = 'WAV Name:'
      ShadowColor = 15263976
    end
    object ebLink: TExtBtn
      Left = 256
      Top = 38
      Width = 16
      Height = 18
      Hint = 'Link picture'
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = '+'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Times New Roman'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebLinkClick
    end
    object ebUnLink: TExtBtn
      Left = 272
      Top = 38
      Width = 16
      Height = 18
      Hint = 'Unlink picture'
      Align = alNone
      BevelShow = False
      CloseButton = False
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Times New Roman'
      Font.Style = [fsBold]
      Glyph.Data = {
        DE000000424DDE00000000000000360000002800000007000000070000000100
        180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000000000000000000000000000000000000000FFFFFF000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000}
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebUnLinkClick
    end
    object seRange: TMultiObjSpinEdit
      Left = 80
      Top = 15
      Width = 105
      Height = 18
      LWSensitivity = 1
      ValueType = vtFloat
      AutoSize = False
      TabOrder = 0
    end
    object edName: TEdit
      Left = 80
      Top = 39
      Width = 174
      Height = 15
      AutoSize = False
      BorderStyle = bsNone
      Color = 15263976
      MaxLength = 32
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 65
    Width = 297
    Height = 28
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object btApply: TButton
      Left = 8
      Top = 5
      Width = 89
      Height = 20
      Caption = 'Apply'
      TabOrder = 0
      OnClick = btApplyClick
    end
    object btOk: TButton
      Left = 104
      Top = 5
      Width = 89
      Height = 20
      Caption = 'Ok'
      ModalResult = 1
      TabOrder = 1
      OnClick = btOkClick
    end
    object btCancel: TButton
      Left = 200
      Top = 5
      Width = 89
      Height = 20
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 2
    end
  end
end
