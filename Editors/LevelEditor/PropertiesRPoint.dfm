object frmPropertiesSpawnPoint: TfrmPropertiesSpawnPoint
  Left = 578
  Top = 384
  Width = 253
  Height = 319
  Caption = 'Spawn properties'
  Color = 10528425
  Constraints.MinHeight = 319
  Constraints.MinWidth = 253
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
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paBottom: TPanel
    Left = 0
    Top = 270
    Width = 245
    Height = 22
    Align = alBottom
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
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
  object paProps: TPanel
    Left = 0
    Top = 0
    Width = 245
    Height = 270
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 65528
    Top = 65528
  end
end
