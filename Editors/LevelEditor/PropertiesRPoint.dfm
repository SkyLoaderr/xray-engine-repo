object frmPropertiesRPoint: TfrmPropertiesRPoint
  Left = 347
  Top = 514
  BorderStyle = bsDialog
  Caption = 'Spawn properties'
  ClientHeight = 172
  ClientWidth = 435
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
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paBottom: TPanel
    Left = 0
    Top = 150
    Width = 435
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
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 435
    Height = 150
    Align = alClient
    TabOrder = 1
    object RxLabel1: TLabel
      Left = 6
      Top = 14
      Width = 31
      Height = 13
      Caption = 'Name:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object edName: TEdit
      Left = 44
      Top = 11
      Width = 152
      Height = 18
      AutoSize = False
      Color = 10526880
      TabOrder = 0
      OnChange = OnModified
    end
    object paEntity: TPanel
      Left = 2
      Top = 29
      Width = 196
      Height = 40
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 1
      object Label3: TLabel
        Left = 3
        Top = 22
        Width = 29
        Height = 13
        Caption = 'Entity:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object ebEntityRefs: TExtBtn
        Left = 41
        Top = 20
        Width = 153
        Height = 18
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = '<none>'
        Margin = 1
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebEntityRefsClick
      end
      object cbActive: TMultiObjCheck
        Left = 2
        Top = 2
        Width = 52
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Active:'
        TabOrder = 0
        OnClick = OnModified
      end
    end
  end
end
