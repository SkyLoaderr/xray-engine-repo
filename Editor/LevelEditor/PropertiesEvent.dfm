object frmPropertiesEvent: TfrmPropertiesEvent
  Left = 419
  Top = 352
  BorderStyle = bsDialog
  Caption = 'Event properties'
  ClientHeight = 377
  ClientWidth = 309
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
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paProps: TPanel
    Left = 0
    Top = 0
    Width = 309
    Height = 354
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
  end
  object paBottom: TPanel
    Left = 0
    Top = 354
    Width = 309
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 145
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 227
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
end
