object frmEditParticles: TfrmEditParticles
  Left = 414
  Top = 329
  Width = 314
  Height = 425
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSizeToolWin
  Caption = 'Particles'
  Color = clBtnFace
  Constraints.MinHeight = 425
  Constraints.MinWidth = 314
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
    Left = 173
    Top = 0
    Width = 133
    Height = 398
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object GroupBox1: TGroupBox
      Left = 0
      Top = 0
      Width = 133
      Height = 104
      Align = alTop
      Caption = ' Command '
      TabOrder = 0
      object ebNewPS: TExtBtn
        Left = 3
        Top = 15
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'New'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebNewPSClick
      end
      object ebRemovePS: TExtBtn
        Left = 3
        Top = 51
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Remove Selected'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebRemovePSClick
      end
      object ebPropertiesPS: TExtBtn
        Left = 3
        Top = 82
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Properties'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebPropertiesPSClick
      end
      object ebClonePS: TExtBtn
        Left = 3
        Top = 33
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Clone Selected'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebClonePSClick
      end
    end
    object Panel2: TPanel
      Left = 0
      Top = 320
      Width = 133
      Height = 78
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      object ebCancel: TExtBtn
        Left = 2
        Top = 60
        Width = 131
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Close'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebCancelClick
      end
      object ebSave: TExtBtn
        Left = 2
        Top = 43
        Width = 131
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Save'
        Enabled = False
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebSaveClick
      end
      object ebReloadPS: TExtBtn
        Left = 2
        Top = 19
        Width = 131
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Reload'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebReloadPSClick
      end
      object ebMerge: TExtBtn
        Left = 2
        Top = 2
        Width = 131
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Merge...'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebMergeClick
      end
    end
    object gbCurrentPS: TGroupBox
      Left = 0
      Top = 104
      Width = 133
      Height = 60
      Align = alTop
      Caption = ' Current PS '
      TabOrder = 2
      Visible = False
      object ExtBtn2: TExtBtn
        Left = 3
        Top = 15
        Width = 63
        Height = 16
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFBABABA0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFBABABA0000000000000000004F4F4F7B7B7BFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000000000000000000008080828
          2828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA000000000000000000000000
          0000000000001212121F1F1FC6C6C6FFFFFFFFFFFFBABABA0000000000000000
          00000000080808282828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000
          00000000004F4F4F7B7B7BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
          0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn2Click
      end
      object ExtBtn5: TExtBtn
        Left = 67
        Top = 15
        Width = 63
        Height = 16
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          9E010000424D9E0100000000000036000000280000000C0000000A0000000100
          18000000000068010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFDDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFF
          FFFFFFFFFFFFFFFF969696000000000000000000000000000000000000969696
          FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000000000000000000000000000
          00969696FFFFFFFFFFFFFFFFFFFFFFFF96969600000000000000000000000000
          0000000000969696FFFFFFFFFFFFFFFFFFFFFFFF969696000000000000000000
          000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000
          00000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF96969600
          0000000000000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF
          DDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn5Click
      end
      object lbCurState: TRxLabel
        Left = 47
        Top = 32
        Width = 84
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object lbParticleCount: TRxLabel
        Left = 47
        Top = 45
        Width = 84
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel1: TRxLabel
        Left = 2
        Top = 32
        Width = 30
        Height = 13
        Caption = 'State:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel2: TRxLabel
        Left = 2
        Top = 45
        Width = 45
        Height = 13
        Caption = 'Particles:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 173
    Height = 398
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
    DragAllowed = True
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
      F8FFFFFF0700000024750041003C0000FFFFFFFF000001010100004178000000
      0000000010270000000100790000000000000041000000000000010000000000
      000000000001000001000000000100000000000000000000000024750041003C
      0000FFFFFFFF0000010101000041780000000000000010270000000100790100
      0000000000410000000000000100000000000000000000010000010000000001
      00000000000000000000000024750041003C0000FFFFFFFF0000010101000041
      7800000000000000102700000001007902000000000000410000000000000100
      0000000000000000000100000100000000010000000000000000000000002475
      0041003C0000FFFFFFFF00000101010000417800000000000000102700000001
      0079030000000000004100000000000001000000000000000000000100000100
      0000000100000000000000000000000024750041003C0000FFFFFFFF00000101
      0100004178000000000000001027000000010079040000000000004100000000
      0000010000000000000000000001000001000000000100000000000000000000
      000024750041003C0000FFFFFFFF000001010100004178000000000000001027
      0000000100790500000000000041000000000000010000000000000000000001
      000001000000000100000000000000000000000024750041003C0000FFFFFFFF
      0000010101000041780000000000000010270000000100790600000000000041
      0000000000000100000000000000000000010000010000000001000000000000
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
    PopupMenu = pmShaders
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
    OnItemChange = tvItemsItemChange
    OnTryEdit = tvItemsTryEdit
    OnItemSelectedChange = tvItemsItemSelectedChange
    OnDragDrop = tvItemsDragDrop
    OnDragOver = tvItemsDragOver
    OnStartDrag = tvItemsStartDrag
    OnDblClick = tvItemsDblClick
    OnKeyPress = tvItemsKeyPress
  end
  object pmShaders: TPopupMenu
    object miNewFolder: TMenuItem
      Caption = 'New Folder...'
      GroupIndex = 1
      OnClick = miNewFolderClick
    end
    object miEditFolder: TMenuItem
      Caption = 'Edit Folder'
      GroupIndex = 1
      OnClick = miEditFolderClick
    end
    object miDeleteFolder: TMenuItem
      Caption = 'Delete Folder'
      GroupIndex = 1
      OnClick = ebRemovePSClick
    end
    object N1: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object Object1: TMenuItem
      Caption = 'PS'
      GroupIndex = 1
      object LoadObject1: TMenuItem
        Caption = 'Append'
        GroupIndex = 1
        OnClick = ebNewPSClick
      end
      object DeleteObject1: TMenuItem
        Caption = 'Delete'
        GroupIndex = 1
        OnClick = ebRemovePSClick
      end
      object N2: TMenuItem
        Caption = '-'
        GroupIndex = 1
      end
      object miObjectProperties: TMenuItem
        Caption = 'Properties'
        GroupIndex = 1
        OnClick = ebPropertiesPSClick
      end
    end
  end
  object fsStorage: TFormStorage
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0s
      end
      item
        Name = 'EmitterDirY'
        Value = 0s
      end
      item
        Name = 'EmitterDirZ'
        Value = 0s
      end>
    Left = 8
    Top = 8
  end
end
