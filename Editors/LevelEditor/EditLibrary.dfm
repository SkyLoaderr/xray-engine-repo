object frmEditLibrary: TfrmEditLibrary
  Left = 365
  Top = 245
  Width = 332
  Height = 559
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Object Library'
  Color = 10528425
  Constraints.MinHeight = 405
  Constraints.MinWidth = 332
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 192
    Top = 0
    Width = 132
    Height = 532
    Align = alRight
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object ebMakeThm: TExtBtn
      Left = 2
      Top = 188
      Width = 129
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Make Thumbnail'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMakeThmClick
    end
    object ebProperties: TExtBtn
      Left = 2
      Top = 170
      Width = 129
      Height = 18
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Properties'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebPropertiesClick
    end
    object ebMakeLOD: TExtBtn
      Left = 2
      Top = 205
      Width = 129
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Make LOD'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMakeLODClick
    end
    object Bevel4: TBevel
      Left = 0
      Top = 166
      Width = 132
      Height = 2
      Align = alTop
      Shape = bsBottomLine
    end
    object cbPreview: TCheckBox
      Left = 2
      Top = 223
      Width = 128
      Height = 17
      Caption = 'Preview'
      TabOrder = 0
      OnClick = cbPreviewClick
    end
    object paImage: TPanel
      Left = 0
      Top = 0
      Width = 132
      Height = 132
      Align = alTop
      BevelOuter = bvLowered
      Caption = '<no image>'
      ParentColor = True
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 130
        Height = 130
        Align = alClient
        OnPaint = pbImagePaint
      end
    end
    object Panel3: TPanel
      Left = 0
      Top = 132
      Width = 132
      Height = 34
      Align = alTop
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 2
      object lbFaces: TLabel
        Left = 81
        Top = 2
        Width = 9
        Height = 13
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel2: TLabel
        Left = 4
        Top = 2
        Width = 69
        Height = 13
        Caption = 'Face count:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object RxLabel3: TLabel
        Left = 4
        Top = 18
        Width = 77
        Height = 13
        Caption = 'Vertex count:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbVertices: TLabel
        Left = 81
        Top = 18
        Width = 9
        Height = 13
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
    object Panel2: TPanel
      Left = 0
      Top = 452
      Width = 132
      Height = 80
      Align = alBottom
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 3
      object ebImport: TExtBtn
        Left = 2
        Top = 0
        Width = 129
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Import Object'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebImportClick
      end
      object ebMakeLWO: TExtBtn
        Left = 2
        Top = 17
        Width = 129
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Export LWO'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebMakeLWOClick
      end
      object ebSave: TExtBtn
        Left = 2
        Top = 45
        Width = 129
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Save'
        Enabled = False
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebSaveClick
      end
      object ebCancel: TExtBtn
        Left = 2
        Top = 62
        Width = 129
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Close'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebCancelClick
      end
    end
  end
  object paItems: TPanel
    Left = 0
    Top = 0
    Width = 192
    Height = 532
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
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
