object frmPropertiesLight: TfrmPropertiesLight
  Left = 440
  Top = 282
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Light properties'
  ClientHeight = 285
  ClientWidth = 541
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
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object gbType: TGroupBox
    Left = 239
    Top = 0
    Width = 302
    Height = 264
    Align = alRight
    Caption = 'Type:'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 0
    object RxLabel1: TLabel
      Left = 104
      Top = 64
      Width = 95
      Height = 13
      Caption = '<Multiple Selection>'
    end
    object pcType: TPageControl
      Left = 2
      Top = 15
      Width = 298
      Height = 247
      ActivePage = tsPoint
      Align = alClient
      Style = tsFlatButtons
      TabIndex = 1
      TabOrder = 0
      object tsSun: TTabSheet
        Caption = 'Sun'
      end
      object tsPoint: TTabSheet
        Tag = 1
        Caption = 'Point'
        ImageIndex = 1
        object Bevel2: TBevel
          Left = 0
          Top = 0
          Width = 289
          Height = 82
        end
        object LG: TImage
          Left = 1
          Top = 1
          Width = 287
          Height = 80
        end
        object lbRange: TLabel
          Left = 201
          Top = 59
          Width = 89
          Height = 13
          AutoSize = False
          Caption = 'Range = '
          Color = 26112
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clYellow
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          Transparent = True
        end
        object ebALauto: TExtBtn
          Left = 199
          Top = 199
          Width = 45
          Height = 17
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Auto L'
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = [fsBold]
          Margin = 2
          ParentFont = False
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebALautoClick
        end
        object ebQLauto: TExtBtn
          Left = 245
          Top = 199
          Width = 45
          Height = 17
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Auto Q'
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = [fsBold]
          Margin = 2
          ParentFont = False
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebQLautoClick
        end
        object RxLabel3: TLabel
          Left = 1
          Top = 201
          Width = 137
          Height = 13
          Caption = 'Brightness at maximum range'
          WordWrap = True
        end
        object RxLabel4: TLabel
          Left = 184
          Top = 201
          Width = 8
          Height = 13
          Caption = '%'
        end
        object seAutoBMax: TMultiObjSpinEdit
          Left = 139
          Top = 199
          Width = 44
          Height = 18
          LWSensitivity = 1
          ButtonKind = bkLightWave
          MaxValue = 100
          Value = 10
          AutoSize = False
          TabOrder = 0
        end
        object paPointProps: TPanel
          Left = 0
          Top = 83
          Width = 289
          Height = 109
          BevelOuter = bvNone
          TabOrder = 1
        end
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 264
    Width = 541
    Height = 21
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ExtBtn1: TExtBtn
      Left = 1
      Top = 2
      Width = 66
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Apply'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = btApplyClick
    end
    object ebOk: TExtBtn
      Left = 68
      Top = 2
      Width = 66
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = btOkClick
    end
    object ebCancel: TExtBtn
      Left = 135
      Top = 2
      Width = 66
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = btCancelClick
    end
  end
  object paProps: TPanel
    Left = 0
    Top = 0
    Width = 239
    Height = 264
    Align = alClient
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 2
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 65528
    Top = 65528
  end
end
