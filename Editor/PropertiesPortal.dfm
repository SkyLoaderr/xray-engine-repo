object frmPropertiesPortal: TfrmPropertiesPortal
  Left = 515
  Top = 291
  BorderStyle = bsDialog
  Caption = 'Portal properties'
  ClientHeight = 103
  ClientWidth = 193
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
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 193
    Height = 80
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object gbOrientation: TGroupBox
      Left = 0
      Top = 0
      Width = 193
      Height = 78
      Caption = ' Orientation '
      TabOrder = 0
      object RxLabel5: TRxLabel
        Left = 8
        Top = 16
        Width = 61
        Height = 13
        Caption = 'Front sector:'
      end
      object RxLabel1: TRxLabel
        Left = 8
        Top = 38
        Width = 62
        Height = 13
        Caption = 'Back sector:'
      end
      object lbBackSector: TRxLabel
        Left = 72
        Top = 38
        Width = 27
        Height = 13
        Caption = 'Back'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object lbFrontSector: TRxLabel
        Left = 72
        Top = 16
        Width = 26
        Height = 13
        Caption = 'Front'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object ebInvertOrientation: TExtBtn
        Left = 96
        Top = 56
        Width = 93
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Invert Orientation'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebInvertOrientationClick
      end
    end
  end
  object paBottom: TPanel
    Left = 0
    Top = 80
    Width = 193
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 29
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
      Left = 111
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
