object frmImageLib: TfrmImageLib
  Left = 481
  Top = 276
  Width = 390
  Height = 459
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Image Editor'
  Color = 10528425
  Constraints.MinHeight = 400
  Constraints.MinWidth = 390
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 194
    Top = 0
    Width = 2
    Height = 432
    Cursor = crHSplit
    Color = 3026478
    ParentColor = False
  end
  object paRight: TPanel
    Left = 196
    Top = 0
    Width = 186
    Height = 432
    Align = alClient
    BevelOuter = bvLowered
    Color = 10528425
    Constraints.MinWidth = 172
    TabOrder = 0
    object Bevel2: TBevel
      Left = 1
      Top = 337
      Width = 184
      Height = 2
      Align = alBottom
      Shape = bsBottomLine
    end
    object paCommand: TPanel
      Left = 1
      Top = 339
      Width = 184
      Height = 92
      Align = alBottom
      BevelInner = bvLowered
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 0
      object ebOk: TExtBtn
        Left = 1
        Top = 56
        Width = 182
        Height = 17
        Align = alTop
        BevelShow = False
        Caption = 'Ok'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebOkClick
      end
      object Bevel1: TBevel
        Left = 1
        Top = 18
        Width = 182
        Height = 2
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
      object ebCancel: TExtBtn
        Left = 1
        Top = 73
        Width = 182
        Height = 17
        Align = alTop
        BevelShow = False
        Caption = 'Cancel'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebCancelClick
      end
      object ebRemoveTexture: TExtBtn
        Left = 1
        Top = 1
        Width = 182
        Height = 17
        Align = alTop
        BevelShow = False
        Caption = 'Remove Texture'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebRemoveTextureClick
      end
      object ebRebuildAssociation: TExtBtn
        Left = 1
        Top = 37
        Width = 182
        Height = 17
        Align = alTop
        BevelShow = False
        Caption = 'Rebuild Association'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebRebuildAssociationClick
      end
      object Bevel5: TBevel
        Left = 1
        Top = 54
        Width = 182
        Height = 2
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
      object ebSyncTypes: TExtBtn
        Left = 1
        Top = 20
        Width = 182
        Height = 17
        Align = alTop
        BevelShow = False
        Caption = 'Sync Types'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebSyncTypesClick
      end
    end
    object paProperties: TPanel
      Left = 1
      Top = 133
      Width = 184
      Height = 204
      Align = alClient
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 1
    end
    object Panel1: TPanel
      Left = 1
      Top = 1
      Width = 184
      Height = 132
      Align = alTop
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 2
      object paImage: TMxPanel
        Left = 1
        Top = 1
        Width = 130
        Height = 130
        BevelOuter = bvLowered
        Caption = '<no image>'
        ParentColor = True
        TabOrder = 0
        OnPaint = paImagePaint
      end
    end
  end
  object paItems: TPanel
    Left = 0
    Top = 0
    Width = 194
    Height = 432
    Align = alLeft
    BevelOuter = bvNone
    Constraints.MinWidth = 194
    ParentColor = True
    TabOrder = 1
    object paFilter: TPanel
      Left = 0
      Top = 0
      Width = 194
      Height = 21
      Align = alTop
      BevelInner = bvLowered
      Constraints.MinWidth = 168
      ParentColor = True
      TabOrder = 0
      object ttImage: TExtBtn
        Left = 2
        Top = 2
        Width = 38
        Height = 17
        Align = alNone
        AllowAllUp = True
        BevelShow = False
        GroupIndex = 1
        Down = True
        Caption = 'Image'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ttImageClick
      end
      object ttBumpMap: TExtBtn
        Left = 78
        Top = 2
        Width = 38
        Height = 17
        Align = alNone
        AllowAllUp = True
        BevelShow = False
        GroupIndex = 3
        Down = True
        Caption = 'Bump'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ttImageClick
      end
      object ttNormalMap: TExtBtn
        Left = 116
        Top = 2
        Width = 38
        Height = 17
        Align = alNone
        AllowAllUp = True
        BevelShow = False
        GroupIndex = 4
        Down = True
        Caption = 'Normal'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ttImageClick
      end
      object ttCubeMap: TExtBtn
        Left = 40
        Top = 2
        Width = 38
        Height = 17
        Align = alNone
        AllowAllUp = True
        BevelShow = False
        GroupIndex = 2
        Down = True
        Caption = 'Cube'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ttImageClick
      end
      object ttTerrain: TExtBtn
        Left = 154
        Top = 2
        Width = 38
        Height = 17
        Align = alNone
        AllowAllUp = True
        BevelShow = False
        GroupIndex = 5
        Down = True
        Caption = 'Terrain'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ttImageClick
      end
    end
  end
  object fsStorage: TFormStorage
    Version = 1
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paRight.Width'
      'paItems.Width')
    StoredValues = <>
    Top = 40
  end
  object ImageList: TImageList
    Height = 10
    Width = 11
    Left = 32
    Top = 40
    Bitmap = {
      494C01010200040004000B000A00FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      00000000000036000000280000002C0000000A0000000100200000000000E006
      0000000000000000000000000000000000000000000000000000000000000000
      00000C0C57000C0C570000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000C0C57000C0C5700000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000C0C57000C0C570000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000002A2A57000C0C57000C0C57002A2A57000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000002A2A
      57000C0C57000C0C57002A2A5700000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000002A2A57000C0C57000C0C57002A2A57000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000002A2A
      57000C0C57000C0C57002A2A5700000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000C0C57000C0C5700000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      280000002C0000000A0000000100010000000000500000000000000000000000
      000000000000000000000000FFFFFF00F3FFFC0000000000F3FDFC0000000000
      FFF8FC0000000000FFF07C0000000000F3E33C0000000000E1E79C0000000000
      E1FFCC0000000000E1FFE40000000000E1FFF40000000000F3FFFC0000000000
      00000000000000000000000000000000000000000000}
  end
end
