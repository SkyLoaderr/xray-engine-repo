object frmPropertiesEObject: TfrmPropertiesEObject
  Left = 822
  Top = 456
  Width = 458
  Height = 363
  BiDiMode = bdRightToLeft
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Object Properties'
  Color = 10528425
  Constraints.MinHeight = 363
  Constraints.MinWidth = 400
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  ParentBiDiMode = False
  Scaled = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object ElPageControl1: TElPageControl
    Left = 0
    Top = 19
    Width = 450
    Height = 317
    ActiveTabColor = 10528425
    BorderWidth = 0
    Color = 10528425
    DrawFocus = False
    Flat = True
    HotTrack = False
    InactiveTabColor = 10528425
    Multiline = False
    RaggedRight = False
    ScrollOpposite = False
    Style = etsAngledTabs
    TabHeight = 15
    TabIndex = 0
    TabPosition = etpBottom
    HotTrackFont.Charset = DEFAULT_CHARSET
    HotTrackFont.Color = clBlue
    HotTrackFont.Height = -11
    HotTrackFont.Name = 'MS Sans Serif'
    HotTrackFont.Style = []
    TabBkColor = 10528425
    ActivePage = tsBasic
    FlatTabBorderColor = clBtnShadow
    Align = alClient
    ParentColor = False
    TabOrder = 0
    UseXPThemes = False
    object tsBasic: TElTabSheet
      PageControl = ElPageControl1
      ImageIndex = -1
      TabVisible = True
      Caption = 'Main Options'
      Color = 10528425
      object paBasic: TPanel
        Left = 0
        Top = 0
        Width = 446
        Height = 298
        Align = alClient
        BevelOuter = bvLowered
        Color = 10528425
        TabOrder = 0
      end
    end
    object tsSurfaces: TElTabSheet
      PageControl = ElPageControl1
      ImageIndex = -1
      TabVisible = True
      Caption = 'Surfaces'
      Color = 10528425
      Visible = False
      object paSurfaces: TPanel
        Left = 0
        Top = 0
        Width = 310
        Height = 298
        Align = alClient
        BevelOuter = bvLowered
        Color = 10528425
        TabOrder = 0
      end
      object Panel2: TPanel
        Left = 310
        Top = 0
        Width = 136
        Height = 298
        Align = alRight
        BevelOuter = bvNone
        ParentColor = True
        TabOrder = 1
        object gbTexture: TGroupBox
          Left = 0
          Top = 106
          Width = 136
          Height = 192
          Align = alClient
          Caption = ' Texture Details '
          TabOrder = 0
          object RxLabel7: TLabel
            Left = 4
            Top = 15
            Width = 31
            Height = 13
            Caption = 'Width:'
          end
          object RxLabel8: TLabel
            Left = 4
            Top = 30
            Width = 34
            Height = 13
            Caption = 'Height:'
          end
          object RxLabel9: TLabel
            Left = 4
            Top = 45
            Width = 30
            Height = 13
            Caption = 'Alpha:'
          end
          object lbWidth: TLabel
            Left = 72
            Top = 15
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbHeight: TLabel
            Left = 72
            Top = 30
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbAlpha: TLabel
            Left = 72
            Top = 45
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object paImage: TPanel
            Left = 3
            Top = 60
            Width = 130
            Height = 130
            BevelOuter = bvLowered
            Caption = '<image>'
            Color = 10528425
            TabOrder = 0
            object pbImage: TPaintBox
              Left = 1
              Top = 1
              Width = 128
              Height = 128
              Align = alClient
              OnPaint = pbImagePaint
            end
          end
        end
        object GroupBox1: TGroupBox
          Left = 0
          Top = 0
          Width = 136
          Height = 106
          Align = alTop
          Caption = ' Surface Details '
          TabOrder = 1
          object Label2: TLabel
            Left = 4
            Top = 42
            Width = 24
            Height = 13
            Caption = 'XYZ:'
          end
          object Label3: TLabel
            Left = 4
            Top = 57
            Width = 36
            Height = 13
            Caption = 'Normal:'
          end
          object Label4: TLabel
            Left = 4
            Top = 72
            Width = 36
            Height = 13
            Caption = 'Diffuse:'
          end
          object lbXYZ: TLabel
            Left = 72
            Top = 42
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbNormal: TLabel
            Left = 72
            Top = 57
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object lbDiffuse: TLabel
            Left = 72
            Top = 72
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label8: TLabel
            Left = 4
            Top = 87
            Width = 25
            Height = 13
            Caption = 'UV'#39's:'
          end
          object lbUVs: TLabel
            Left = 72
            Top = 87
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label10: TLabel
            Left = 4
            Top = 28
            Width = 67
            Height = 13
            Caption = 'Double Sided:'
          end
          object lb2Sided: TLabel
            Left = 72
            Top = 28
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
          object Label5: TLabel
            Left = 4
            Top = 14
            Width = 32
            Height = 13
            Caption = 'Faces:'
          end
          object lbSurfFaces: TLabel
            Left = 72
            Top = 14
            Width = 13
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
          end
        end
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 450
    Height = 19
    Align = alTop
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 1
    object ebSortByImage: TExtBtn
      Left = 164
      Top = 1
      Width = 45
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 1
      Caption = 'Texture'
      FlatAlwaysEdge = True
    end
    object ebSortByName: TExtBtn
      Left = 119
      Top = 1
      Width = 45
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = 'Name'
      FlatAlwaysEdge = True
    end
    object ebDropper: TExtBtn
      Left = 2
      Top = 1
      Width = 63
      Height = 17
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 2
      Caption = 'Dropper'
      Glyph.Data = {
        3E020000424D3E0200000000000036000000280000000D0000000D0000000100
        18000000000008020000120B0000120B00000000000000000000A0A6A9A0A6A9
        A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6
        A900A0A6A9A0A6A9000000A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9
        A0A6A9A0A6A9A0A6A900A0A6A9000000F7F7F7000000000000A0A6A9A0A6A9A0
        A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A900A0A6A9A0A6A9000000F7F7F7F7F7
        F7000000A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A900A0A6A9A0A6A9
        000000F7F7F7F7F7F7F7F7F7000000A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6
        A900A0A6A9A0A6A9A0A6A9000000F7F7F7F7F7F7F7F7F7000000A0A6A9000000
        A0A6A9A0A6A9A0A6A900A0A6A9A0A6A9A0A6A9A0A6A9000000F7F7F7F7F7F7F7
        F7F7000000000000A0A6A9A0A6A9A0A6A900A0A6A9A0A6A9A0A6A9A0A6A9A0A6
        A9000000F7F7F7000000000000000000000000A0A6A9A0A6A900A0A6A9A0A6A9
        A0A6A9A0A6A9A0A6A9A0A6A9000000000000000000000000000000A0A6A9A0A6
        A900A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9000000000000000000000000000000
        000000000000A0A6A900A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A900
        0000000000000000000000A0A6A9A0A6A900A0A6A9A0A6A9A0A6A9A0A6A9A0A6
        A9A0A6A9A0A6A9A0A6A9A0A6A9000000A0A6A9A0A6A9A0A6A900A0A6A9A0A6A9
        A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6A9A0A6
        A900}
      Margin = 2
      FlatAlwaysEdge = True
    end
    object ebSortByShader: TExtBtn
      Left = 209
      Top = 1
      Width = 45
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      GroupIndex = 1
      Caption = 'Shader'
      FlatAlwaysEdge = True
    end
    object Label1: TLabel
      Left = 79
      Top = 3
      Width = 37
      Height = 13
      Caption = 'Sort By:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'ElPageControl1.ActivePage')
    StoredValues = <>
    Left = 8
    Top = 24
  end
end
