object frmPropertiesEvent: TfrmPropertiesEvent
  Left = 404
  Top = 227
  BorderStyle = bsDialog
  Caption = 'Event properties'
  ClientHeight = 115
  ClientWidth = 599
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
    Width = 599
    Height = 92
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object gbEventParams: TGroupBox
      Left = 81
      Top = 0
      Width = 518
      Height = 92
      Align = alClient
      Caption = ' Parameters '
      TabOrder = 0
      object RxLabel5: TLabel
        Left = 5
        Top = 16
        Width = 62
        Height = 13
        Caption = 'Target Class:'
      end
      object RxLabel1: TLabel
        Left = 5
        Top = 36
        Width = 45
        Height = 13
        Caption = 'On Enter:'
      end
      object RxLabel2: TLabel
        Left = 5
        Top = 55
        Width = 37
        Height = 13
        Caption = 'On Exit:'
      end
      object RxLabel3: TLabel
        Left = 5
        Top = 73
        Width = 71
        Height = 13
        Caption = 'Execute Once:'
      end
      object cbTargetClass: TComboBox
        Left = 80
        Top = 13
        Width = 434
        Height = 19
        Style = csOwnerDrawVariable
        Color = 14869218
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 13
        ParentFont = False
        Sorted = True
        TabOrder = 0
        OnChange = cbTargetClassChange
        Items.Strings = (
          'O_ACTOR'
          'O_ALIEN'
          'O_DEMO'
          'O_FLYER')
      end
      object edOnExit: TEdit
        Left = 80
        Top = 53
        Width = 434
        Height = 17
        AutoSize = False
        Color = 14869218
        TabOrder = 1
        OnChange = edOnEnterChange
      end
      object edOnEnter: TEdit
        Left = 80
        Top = 34
        Width = 434
        Height = 17
        AutoSize = False
        Color = 14869218
        TabOrder = 2
        OnChange = edOnEnterChange
      end
      object cbExecuteOnce: TMultiObjCheck
        Left = 80
        Top = 73
        Width = 12
        Height = 15
        Alignment = taLeftJustify
        BiDiMode = bdLeftToRight
        ParentBiDiMode = False
        TabOrder = 3
        OnClick = edOnEnterChange
      end
    end
    object rgType: TRadioGroup
      Left = 0
      Top = 0
      Width = 81
      Height = 92
      Align = alLeft
      Caption = ' Type '
      Enabled = False
      ItemIndex = 0
      Items.Strings = (
        'Box'
        'Sphere')
      TabOrder = 1
    end
  end
  object paBottom: TPanel
    Left = 0
    Top = 92
    Width = 599
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 433
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
      Left = 515
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
