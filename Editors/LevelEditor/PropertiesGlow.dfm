object frmPropertiesGlow: TfrmPropertiesGlow
  Left = 478
  Top = 367
  BorderStyle = bsDialog
  Caption = 'Glow properties'
  ClientHeight = 238
  ClientWidth = 340
  Color = 10528425
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
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 340
    Height = 216
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object paImage: TPanel
      Left = 208
      Top = 1
      Width = 132
      Height = 132
      BevelOuter = bvLowered
      Caption = '<no image>'
      ParentColor = True
      TabOrder = 0
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 130
        Height = 130
        Align = alClient
        OnPaint = pbImagePaint
      end
    end
    object paProps: TPanel
      Left = 0
      Top = 0
      Width = 207
      Height = 216
      Align = alLeft
      BevelOuter = bvLowered
      ParentColor = True
      TabOrder = 1
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 216
    Width = 340
    Height = 22
    Align = alBottom
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 1
      Top = 3
      Width = 61
      Height = 18
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Ok'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 62
      Top = 3
      Width = 61
      Height = 18
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0
      end
      item
        Name = 'EmitterDirY'
        Value = 0
      end
      item
        Name = 'EmitterDirZ'
        Value = 0
      end>
  end
end
