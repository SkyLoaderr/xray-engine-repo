object frmImageLib: TfrmImageLib
  Left = 659
  Top = 181
  Width = 350
  Height = 650
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSizeToolWin
  Caption = 'Image Editor'
  Color = clBtnFace
  Constraints.MinHeight = 650
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
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 174
    Top = 0
    Width = 168
    Height = 623
    Align = alRight
    BevelOuter = bvLowered
    TabOrder = 0
    object Panel2: TPanel
      Left = 1
      Top = 559
      Width = 166
      Height = 63
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 0
      object ebClose: TExtBtn
        Left = 2
        Top = 44
        Width = 162
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Close'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebCloseClick
      end
      object ebConvert: TExtBtn
        Left = 2
        Top = 20
        Width = 162
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Convert...'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebConvertClick
      end
      object Bevel1: TBevel
        Left = 3
        Top = 39
        Width = 161
        Height = 2
      end
    end
    object paImage: TPanel
      Left = 1
      Top = 1
      Width = 166
      Height = 162
      Align = alTop
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 164
        Height = 160
        Align = alClient
        Color = clGray
        ParentColor = False
        OnDblClick = pbImageDblClick
        OnPaint = pbImagePaint
      end
    end
    object gbOptions: TGroupBox
      Left = 1
      Top = 410
      Width = 166
      Height = 94
      Align = alTop
      Caption = ' Options '
      TabOrder = 2
      object cbDither: TCheckBox
        Left = 8
        Top = 15
        Width = 145
        Height = 17
        Caption = 'Dither Color'
        TabOrder = 0
        OnClick = rgChangeClick
      end
      object cbAlphaZeroBorder: TCheckBox
        Left = 8
        Top = 45
        Width = 145
        Height = 17
        Caption = 'Alpha Zero Border (1 texel)'
        TabOrder = 1
        OnClick = rgChangeClick
      end
      object cbBinaryAlpha: TCheckBox
        Left = 8
        Top = 30
        Width = 145
        Height = 17
        Caption = 'Binary Alpha'
        TabOrder = 2
        OnClick = rgChangeClick
      end
      object cbNormalMap: TCheckBox
        Left = 8
        Top = 60
        Width = 145
        Height = 17
        Caption = 'Normal Map'
        TabOrder = 3
        OnClick = rgChangeClick
      end
      object cbImplicitLighted: TCheckBox
        Left = 8
        Top = 75
        Width = 145
        Height = 17
        Caption = 'Implicit Lighted'
        TabOrder = 4
        OnClick = rgChangeClick
      end
    end
    object rgSaveFormat: TRadioGroup
      Left = 1
      Top = 194
      Width = 166
      Height = 153
      Align = alTop
      Caption = ' Save Format '
      Items.Strings = (
        'No Alpha (DXT1)'
        '1 bit Alpha (DXT1)'
        'Explicit Alpha (DXT3)'
        'Interpolated Alpha (DXT5)'
        '16 bit (4:4:4:4)'
        '16 bit (1:5:5:5)'
        '16 bit (5:6:5)'
        '-'
        '32 bit ARGB')
      TabOrder = 3
      OnClick = rgChangeClick
    end
    object GroupBox2: TGroupBox
      Left = 1
      Top = 504
      Width = 166
      Height = 56
      Align = alTop
      Caption = ' Fade To Color '
      TabOrder = 4
      object Label1: TLabel
        Left = 9
        Top = 36
        Width = 70
        Height = 13
        Caption = '# MIPs to fade'
      end
      object mcFadeColor: TMultiObjColor
        Left = 103
        Top = 15
        Width = 53
        Height = 16
        OnMouseDown = mcFadeColorMouseDown
      end
      object cbAllowFade: TCheckBox
        Left = 8
        Top = 15
        Width = 89
        Height = 17
        Caption = 'Fade To Color'
        TabOrder = 0
        OnClick = rgChangeClick
      end
      object cbNumFadeMips: TComboBox
        Left = 82
        Top = 33
        Width = 76
        Height = 19
        Style = csOwnerDrawFixed
        ItemHeight = 13
        TabOrder = 1
        OnClick = rgChangeClick
      end
    end
    object rgMIPmaps: TRadioGroup
      Left = 1
      Top = 347
      Width = 166
      Height = 63
      Align = alTop
      Caption = ' MIP maps '
      Items.Strings = (
        'Generate MIP maps'
        'Advanced MIP maps'
        'No MIP maps')
      TabOrder = 5
      OnClick = rgChangeClick
    end
    object Panel3: TPanel
      Left = 1
      Top = 163
      Width = 166
      Height = 31
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 6
      object lbFileName: TRxLabel
        Left = 33
        Top = 2
        Width = 131
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 15263976
      end
      object RxLabel2: TRxLabel
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
        ShadowColor = 15263976
      end
      object RxLabel3: TRxLabel
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
        ShadowColor = 15263976
      end
      object lbInfo: TRxLabel
        Left = 33
        Top = 18
        Width = 131
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 15263976
      end
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 174
    Height = 623
    LeftPosition = 0
    ActiveBorderType = fbtSunken
    Align = alClient
    AlwaysKeepFocus = True
    AlwaysKeepSelection = True
    AutoExpand = True
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
    DragAllowed = False
    DragTrgDrawMode = ColorRect
    DragType = dtDelphi
    DragImageMode = dimOne
    DrawFocusRect = True
    ExpandOnDblClick = True
    ExpandOnDragOver = False
    FilteredVisibility = False
    Flat = True
    FlatFocusedScrollbars = True
    FocusedSelectColor = clHighlight
    ForcedScrollBars = ssNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    HeaderActiveFilterColor = clBlack
    HeaderHeight = 22
    HeaderHotTrack = False
    HeaderInvertSortArrows = False
    HeaderSections.Data = {
      F8FFFFFF07000000E8C1E00600000000FFFFFFFF000001010100044078000000
      0000000010270000000100400000000000000440000000000000014000000000
      0000000000010000010000000001000000000000000000000000E8C1E0060000
      0000FFFFFFFF0000010101000440780000000000000010270000000100400100
      0000000004400000000000000140000000000000000000010000010000000001
      000000000000000000000000E8C1E00600000000FFFFFFFF0000010101000440
      7800000000000000102700000001004002000000000004400000000000000140
      000000000000000000010000010000000001000000000000000000000000E8C1
      E00600000000FFFFFFFF00000101010004407800000000000000102700000001
      0040030000000000044000000000000001400000000000000000000100000100
      00000001000000000000000000000000E8C1E00600000000FFFFFFFF00000101
      0100044078000000000000001027000000010040040000000000044000000000
      0000014000000000000000000001000001000000000100000000000000000000
      0000E8C1E00600000000FFFFFFFF000001010100044078000000000000001027
      0000000100400500000000000440000000000000014000000000000000000001
      0000010000000001000000000000000000000000E8C1E00600000000FFFFFFFF
      0000010101000440780000000000000010270000000100400600000000000440
      0000000000000140000000000000000000010000010000000001000000000000
      000000000000}
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
    HorzScrollBarStyles.Width = 14
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    InactiveBorderType = fbtSunkenOuter
    ItemIndent = 17
    LineHeight = 19
    LinesColor = clBtnShadow
    LinesStyle = psDot
    LineHintColor = clWindow
    LockHeaderHeight = False
    MainTreeColumn = 0
    MoveColumnOnDrag = False
    MoveFocusOnCollapse = False
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    PathSeparator = '\'
    PlusMinusTransparent = True
    RightAlignedText = False
    RightAlignedTree = False
    RowHotTrack = False
    RowSelect = True
    ScrollTracking = True
    SelectColumn = -1
    ShowButtons = True
    ShowColumns = False
    ShowCheckboxes = False
    ShowImages = True
    ShowLines = False
    ShowRoot = False
    ShowRootButtons = True
    SelectionMode = smUsual
    SortDir = sdAscend
    SortMode = smAdd
    SortSection = 0
    SortType = stText
    StickyHeaderSections = False
    TabOrder = 1
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
    VertScrollBarStyles.Width = 14
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    TextColor = clBtnText
    BkColor = clBtnShadow
    OnItemSelectedChange = tvItemsItemSelectedChange
    OnKeyPress = tvItemsKeyPress
  end
  object fsStorage: TFormStorage
    StoredValues = <>
  end
end
