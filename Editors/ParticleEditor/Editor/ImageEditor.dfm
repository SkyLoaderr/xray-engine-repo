object frmImageLib: TfrmImageLib
  Left = 509
  Top = 234
  Width = 350
  Height = 455
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Image Editor'
  Color = 10528425
  Constraints.MinHeight = 400
  Constraints.MinWidth = 350
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
    Left = 168
    Top = 0
    Width = 2
    Height = 428
    Cursor = crHSplit
    Align = alRight
    Color = 3026478
    ParentColor = False
  end
  object paRight: TPanel
    Left = 170
    Top = 0
    Width = 172
    Height = 428
    Align = alRight
    BevelOuter = bvLowered
    Color = 10528425
    Constraints.MinWidth = 172
    TabOrder = 0
    object Bevel2: TBevel
      Left = 1
      Top = 302
      Width = 170
      Height = 2
      Align = alBottom
      Shape = bsBottomLine
    end
    object paCommand: TPanel
      Left = 1
      Top = 304
      Width = 170
      Height = 123
      Align = alBottom
      BevelInner = bvLowered
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 0
      object ebOk: TExtBtn
        Left = 1
        Top = 87
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Ok'
        FlatAlwaysEdge = True
        OnClick = ebOkClick
      end
      object Bevel1: TBevel
        Left = 1
        Top = 58
        Width = 168
        Height = 6
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
      object ebCheckAllCompliance: TExtBtn
        Left = 1
        Top = 41
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Check All Compliance'
        FlatAlwaysEdge = True
        OnClick = ebCheckAllComplianceClick
      end
      object ebCheckSelCompliance: TExtBtn
        Left = 1
        Top = 24
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Check Sel Compliance'
        FlatAlwaysEdge = True
        OnClick = ebCheckSelComplianceClick
      end
      object ebCancel: TExtBtn
        Left = 1
        Top = 104
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Cancel'
        FlatAlwaysEdge = True
        OnClick = ebCancelClick
      end
      object Bevel3: TBevel
        Left = 1
        Top = 18
        Width = 168
        Height = 6
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
      object ebRemoveTexture: TExtBtn
        Left = 1
        Top = 1
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Remove Texture'
        FlatAlwaysEdge = True
        OnClick = ebRemoveTextureClick
      end
      object ebRebuildAssociation: TExtBtn
        Left = 1
        Top = 64
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Rebuild Association'
        FlatAlwaysEdge = True
        OnClick = ebRebuildAssociationClick
      end
      object Bevel5: TBevel
        Left = 1
        Top = 81
        Width = 168
        Height = 6
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
    end
    object Panel3: TPanel
      Left = 1
      Top = 171
      Width = 170
      Height = 34
      Align = alTop
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 1
      object lbFileName: TLabel
        Left = 33
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
        Width = 29
        Height = 13
        AutoSize = False
        Caption = 'File:'
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
        Width = 29
        Height = 13
        AutoSize = False
        Caption = 'Info:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbInfo: TLabel
        Left = 33
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
    object paProperties: TPanel
      Left = 1
      Top = 205
      Width = 170
      Height = 97
      Align = alClient
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 2
    end
    object paImage: TMxPanel
      Left = 1
      Top = 1
      Width = 170
      Height = 170
      Align = alTop
      BevelOuter = bvLowered
      Caption = '<no image>'
      ParentColor = True
      TabOrder = 3
      OnResize = paImageResize
      OnPaint = paImagePaint
    end
  end
  object paItems: TPanel
    Left = 0
    Top = 0
    Width = 168
    Height = 428
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
  end
  object fsStorage: TFormStorage
    Version = 1
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paRight.Width')
    StoredValues = <>
  end
  object ImageList: TImageList
    Height = 10
    Width = 11
    Left = 32
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
