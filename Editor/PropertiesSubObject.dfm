object frmPropertiesSubObject: TfrmPropertiesSubObject
  Left = 549
  Top = 168
  BorderStyle = bsDialog
  Caption = 'Mesh properties'
  ClientHeight = 249
  ClientWidth = 376
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
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
    Top = 226
    Width = 376
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 208
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 290
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
  object pcMesh: TPageControl
    Left = 0
    Top = 0
    Width = 376
    Height = 226
    ActivePage = tsLayers
    Align = alClient
    TabOrder = 1
    object tsInfo: TTabSheet
      Caption = 'Information'
      ImageIndex = 1
      OnShow = tsInfoShow
      object RxLabel1: TRxLabel
        Left = 3
        Top = 8
        Width = 43
        Height = 13
        Caption = 'Vertices:'
        ShadowColor = 15263976
      end
      object RxLabel3: TRxLabel
        Left = 3
        Top = 24
        Width = 34
        Height = 13
        Caption = 'Faces:'
        ShadowColor = 15263976
      end
      object lbVertices: TRxLabel
        Left = 83
        Top = 8
        Width = 15
        Height = 13
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        ShadowColor = 15263976
      end
      object lbFaces: TRxLabel
        Left = 83
        Top = 24
        Width = 15
        Height = 13
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        ShadowColor = 15263976
      end
      object RxLabel4: TRxLabel
        Left = 3
        Top = 40
        Width = 71
        Height = 13
        Caption = 'Texture layers:'
        ShadowColor = 15263976
      end
      object lbTLayers: TRxLabel
        Left = 83
        Top = 40
        Width = 15
        Height = 13
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        ShadowColor = 15263976
      end
    end
    object tsLayers: TTabSheet
      Caption = 'Texture Layers'
      ImageIndex = 3
      OnShow = tsLayersShow
      object RxLabel2: TRxLabel
        Left = 187
        Top = 8
        Width = 62
        Height = 13
        Caption = 'Layer Name:'
        ShadowColor = 15263976
      end
      object Bevel2: TBevel
        Left = 259
        Top = 6
        Width = 106
        Height = 17
      end
      object RxLabel5: TRxLabel
        Left = 187
        Top = 29
        Width = 63
        Height = 13
        Caption = 'Script Name:'
        ShadowColor = 15263976
      end
      object Bevel1: TBevel
        Left = 259
        Top = 27
        Width = 106
        Height = 17
      end
      object RxLabel7: TRxLabel
        Left = 187
        Top = 50
        Width = 44
        Height = 13
        Caption = 'Class ID:'
        ShadowColor = 15263976
      end
      object Bevel5: TBevel
        Left = 259
        Top = 73
        Width = 75
        Height = 17
      end
      object RxLabel8: TRxLabel
        Left = 187
        Top = 76
        Width = 41
        Height = 13
        Caption = 'Texture:'
        ShadowColor = 15263976
      end
      object ebTexture: TExtBtn
        Left = 334
        Top = 73
        Width = 16
        Height = 18
        Hint = 'Link picture'
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = '+'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Times New Roman'
        Font.Style = [fsBold]
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebTextureClick
      end
      object ebDestroyTex: TExtBtn
        Left = 350
        Top = 73
        Width = 16
        Height = 18
        Hint = 'Unlink picture'
        Align = alNone
        BevelShow = False
        CloseButton = False
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Times New Roman'
        Font.Style = [fsBold]
        Glyph.Data = {
          DE000000424DDE00000000000000360000002800000007000000070000000100
          180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000000000000000000000000000000000000000FFFFFF000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebDestroyTexClick
      end
      object ebFaceList: TExtBtn
        Left = 190
        Top = 180
        Width = 176
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Face list'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebFaceListClick
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 185
        Height = 198
        Align = alLeft
        BevelOuter = bvNone
        TabOrder = 0
        object ebAddLayer: TExtBtn
          Left = 0
          Top = 180
          Width = 92
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Add'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebAddLayerClick
        end
        object ebDeleteLayer: TExtBtn
          Left = 92
          Top = 180
          Width = 93
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Delete'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebDeleteLayerClick
        end
        object tvTLayers: TElTree
          Left = 0
          Top = 0
          Width = 185
          Height = 181
          LeftPosition = 0
          ActiveBorderType = fbtSunken
          Align = alTop
          AlwaysKeepFocus = True
          AlwaysKeepSelection = True
          AutoExpand = False
          AutoLookup = False
          DockOrientation = doNoOrient
          DoubleBuffered = False
          AutoLineHeight = True
          BarStyle = False
          BarStyleVerticalLines = False
          BorderStyle = bsSingle
          ChangeStateImage = False
          CustomCheckboxes = False
          CustomPlusMinus = False
          DeselectChildrenOnCollapse = False
          DivLinesColor = clBtnFace
          DoInplaceEdit = False
          DraggableSections = False
          DragAllowed = True
          DragTrgDrawMode = ColorRect
          DragType = dtDelphi
          DragImageMode = dimOne
          DrawFocusRect = True
          ExpandOnDblClick = True
          ExpandOnDragOver = False
          FilteredVisibility = False
          Flat = False
          FlatFocusedScrollbars = True
          FocusedSelectColor = clHighlight
          ForcedScrollBars = ssNone
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          FullRowSelect = True
          HeaderActiveFilterColor = clBlack
          HeaderHeight = 19
          HeaderHotTrack = False
          HeaderInvertSortArrows = False
          HeaderSections.Data = {F8FFFFFF00000000}
          HeaderFilterColor = clBtnText
          HeaderFlat = False
          HideFocusRect = False
          HideHintOnTimer = False
          HideHintOnMove = True
          HideSelectColor = clBtnFace
          HideSelection = False
          HorizontalLines = False
          HideHorzScrollBar = False
          HideVertScrollBar = False
          HorzScrollBarStyles.ActiveFlat = False
          HorzScrollBarStyles.BlendBackground = True
          HorzScrollBarStyles.Color = clBtnFace
          HorzScrollBarStyles.Flat = True
          HorzScrollBarStyles.MinThumbSize = 4
          HorzScrollBarStyles.NoDisableButtons = False
          HorzScrollBarStyles.NoSunkenThumb = False
          HorzScrollBarStyles.OwnerDraw = False
          HorzScrollBarStyles.SecondaryButtons = False
          HorzScrollBarStyles.SecondBtnKind = sbkOpposite
          HorzScrollBarStyles.ShowLeftArrows = True
          HorzScrollBarStyles.ShowRightArrows = True
          HorzScrollBarStyles.ShowTrackHint = False
          HorzScrollBarStyles.ThumbMode = etmAuto
          HorzScrollBarStyles.ThumbSize = 0
          HorzScrollBarStyles.Width = 16
          HorzScrollBarStyles.ButtonSize = 16
          HorzScrollBarStyles.UseSystemMetrics = False
          InactiveBorderType = fbtSunkenOuter
          ItemIndent = 17
          LineHeight = 19
          LinesColor = clBtnFace
          LinesStyle = psDot
          LineHintColor = clWindow
          LockHeaderHeight = False
          MainTreeColumn = 0
          MoveColumnOnDrag = False
          MoveFocusOnCollapse = False
          MultiSelect = False
          OwnerDrawMask = '~~@~~'
          PathSeparator = '\'
          PlusMinusTransparent = False
          RightAlignedText = False
          RightAlignedTree = False
          RowHotTrack = False
          RowSelect = True
          ScrollTracking = False
          SelectColumn = -1
          ShowButtons = True
          ShowColumns = False
          ShowCheckboxes = False
          ShowImages = True
          ShowLines = False
          ShowRoot = False
          ShowRootButtons = False
          SelectionMode = smUsual
          SortDir = sdAscend
          SortMode = smNone
          SortSection = 0
          SortType = stCustom
          StickyHeaderSections = False
          TabOrder = 0
          TabStop = True
          Tracking = True
          TrackColor = clHighlight
          UnderlineTracked = True
          VerticalLines = False
          VertScrollBarStyles.ActiveFlat = False
          VertScrollBarStyles.BlendBackground = True
          VertScrollBarStyles.Color = clBtnFace
          VertScrollBarStyles.Flat = True
          VertScrollBarStyles.MinThumbSize = 4
          VertScrollBarStyles.NoDisableButtons = False
          VertScrollBarStyles.NoSunkenThumb = False
          VertScrollBarStyles.OwnerDraw = False
          VertScrollBarStyles.SecondaryButtons = False
          VertScrollBarStyles.SecondBtnKind = sbkOpposite
          VertScrollBarStyles.ShowLeftArrows = True
          VertScrollBarStyles.ShowRightArrows = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.ThumbMode = etmAuto
          VertScrollBarStyles.ThumbSize = 0
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          TextColor = clWindowText
          BkColor = clBtnShadow
          OnItemSelectedChange = tvTLayersItemSelectedChange
          OnDragDrop = tvTLayersDragDrop
          OnDragOver = tvTLayersDragOver
          OnStartDrag = tvTLayersStartDrag
          OnDblClick = tvTLayersDblClick
        end
      end
      object edName: TEdit
        Left = 260
        Top = 7
        Width = 104
        Height = 15
        AutoSize = False
        BorderStyle = bsNone
        Color = 15263976
        MaxLength = 32
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnChange = edNameChange
      end
      object edScript: TEdit
        Left = 261
        Top = 28
        Width = 104
        Height = 15
        AutoSize = False
        BorderStyle = bsNone
        Color = 15263976
        MaxLength = 32
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnChange = edScriptChange
      end
      object edTextureName: TEdit
        Left = 260
        Top = 74
        Width = 73
        Height = 15
        AutoSize = False
        BorderStyle = bsNone
        Color = 13816530
        MaxLength = 32
        ParentShowHint = False
        ReadOnly = True
        ShowHint = True
        TabOrder = 3
      end
      object paImage: TPanel
        Left = 287
        Top = 98
        Width = 80
        Height = 80
        BevelOuter = bvLowered
        Caption = '<no image>'
        TabOrder = 4
        object pbImage: TPaintBox
          Left = 1
          Top = 1
          Width = 78
          Height = 78
          Align = alClient
          OnPaint = pbImagePaint
        end
      end
      object rbShader: TRadioGroup
        Left = 190
        Top = 93
        Width = 91
        Height = 85
        Caption = 'Shader OP:'
        Items.Strings = (
          'Blend'
          'Multiply'
          'Add')
        TabOrder = 5
        OnClick = rbShaderClick
      end
      object cbClassID: TComboBox
        Left = 259
        Top = 48
        Width = 108
        Height = 21
        Style = csDropDownList
        Color = 15263976
        ItemHeight = 13
        TabOrder = 6
        OnChange = cbClassIDChange
        Items.Strings = (
          'STD'
          'NULL'
          'FIRE'
          'REFL'
          'LMAP'
          'AVI'
          'SP_ENV')
      end
    end
  end
  object fsMeshProp: TFormStorage
    IniSection = 'Mesh Properties'
    Options = []
    RegistryRoot = prLocalMachine
    StoredProps.Strings = (
      'cbClassID.Items')
    StoredValues = <>
    Left = 4
    Top = 2
  end
  object dldFaces: TDualListDialog
    Sorted = True
    Title = 'Edit texture layer face list'
    Label1Caption = '&Source'
    Label2Caption = '&Destination'
    HelpContext = 0
    Left = 33
    Top = 2
  end
end
