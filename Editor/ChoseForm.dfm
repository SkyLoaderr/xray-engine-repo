object frmChoseItem: TfrmChoseItem
  Left = 463
  Top = 219
  BorderStyle = bsDialog
  Caption = 'Select Item'
  ClientHeight = 331
  ClientWidth = 346
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 208
    Top = 0
    Width = 138
    Height = 331
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object sbSelect: TExtBtn
      Left = 2
      Top = 312
      Width = 67
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbSelectClick
    end
    object sbCancel: TExtBtn
      Left = 70
      Top = 312
      Width = 67
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbCancelClick
    end
    object lbItemName: TRxLabel
      Left = 33
      Top = 141
      Width = 106
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
    object lbFileName: TRxLabel
      Left = 33
      Top = 155
      Width = 106
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
    object RxLabel1: TRxLabel
      Left = 1
      Top = 141
      Width = 32
      Height = 13
      AutoSize = False
      Caption = 'Item:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ShadowColor = 15263976
    end
    object RxLabel2: TRxLabel
      Left = 1
      Top = 155
      Width = 28
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
      Left = 1
      Top = 168
      Width = 28
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
      Top = 168
      Width = 106
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
      WordWrap = True
    end
    object paImage: TPanel
      Left = 5
      Top = 8
      Width = 130
      Height = 130
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 0
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 128
        Height = 128
        Align = alClient
        Color = clGray
        ParentColor = False
        OnPaint = pbImagePaint
      end
    end
    object paMulti: TPanel
      Left = 1
      Top = 226
      Width = 136
      Height = 83
      TabOrder = 1
      Visible = False
      object ebMultiUp: TExtBtn
        Left = 120
        Top = 3
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          F6000000424DF600000000000000360000002800000008000000080000000100
          180000000000C0000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDDDDDC000000000000DDDDDCFFFFFFFF
          FFFFFFFFFF000000000000000000000000191919000000FFFFFFFFFFFFDDDDDC
          000000000000000000000000DDDDDCFFFFFFFFFFFFFFFFFF0000000000000000
          00000000FFFFFFFFFFFFFFFFFFFFFFFFDDDDDC000000000000DDDDDCFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiUpClick
      end
      object ebMultiDown: TExtBtn
        Left = 120
        Top = 63
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          F6000000424DF600000000000000360000002800000008000000080000000100
          180000000000C0000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDDDDDC0000000000
          00DDDDDCFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000FFFFFFFF
          FFFFFFFFFFDDDDDC000000000000000000000000DDDDDCFFFFFFFFFFFF000000
          191919000000000000000000000000FFFFFFFFFFFFFFFFFFDDDDDC0000000000
          00DDDDDCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiDownClick
      end
      object ebMultiRemove: TExtBtn
        Left = 120
        Top = 23
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          56010000424D560100000000000036000000280000000A000000090000000100
          18000000000020010000120B0000120B00000000000000000000C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4000000
          000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
          000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4C8D0D4000000000000C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4000000
          000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiRemoveClick
      end
      object ebMultiClear: TExtBtn
        Left = 120
        Top = 43
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF000000000000
          FFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF0000000000000000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF00000000
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000
          000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000
          00000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000000000000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF
          000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiClearClick
      end
      object tvMulti: TElTree
        Left = 1
        Top = 1
        Width = 119
        Height = 81
        LeftPosition = 0
        ActiveBorderType = fbtSunken
        AlwaysKeepFocus = False
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
        DoInplaceEdit = True
        DraggableSections = False
        DragAllowed = True
        DragTrgDrawMode = SelColorRect
        DragType = dtDelphi
        DragImageMode = dimNever
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
          F8FFFFFF070000006CEC3C0400000000FFFFFFFF000001010100382E78000000
          0000000010270000000100640000000000006861000000000000016400000000
          00000000000100000100000000010000000000000000000000006CEC3C040000
          0000FFFFFFFF000001010100382E780000000000000010270000000100640100
          0000000068610000000000000164000000000000000000010000010000000001
          0000000000000000000000006CEC3C0400000000FFFFFFFF000001010100382E
          7800000000000000102700000001006402000000000068610000000000000164
          0000000000000000000100000100000000010000000000000000000000006CEC
          3C0400000000FFFFFFFF000001010100382E7800000000000000102700000001
          0064030000000000686100000000000001640000000000000000000100000100
          000000010000000000000000000000006CEC3C0400000000FFFFFFFF00000101
          0100382E78000000000000001027000000010064040000000000686100000000
          0000016400000000000000000001000001000000000100000000000000000000
          00006CEC3C0400000000FFFFFFFF000001010100382E78000000000000001027
          0000000100640500000000006861000000000000016400000000000000000001
          00000100000000010000000000000000000000006CEC3C0400000000FFFFFFFF
          000001010100382E780000000000000010270000000100640600000000006861
          0000000000000164000000000000000000010000010000000001000000000000
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
        ShowButtons = False
        ShowColumns = False
        ShowCheckboxes = False
        ShowImages = True
        ShowLines = False
        ShowRoot = False
        ShowRootButtons = True
        SelectionMode = smUsual
        SortDir = sdAscend
        SortMode = smNone
        SortSection = 0
        SortType = stText
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
        VertScrollBarStyles.Width = 14
        VertScrollBarStyles.ButtonSize = 14
        VertScrollBarStyles.UseSystemMetrics = False
        TextColor = clBtnText
        BkColor = clBtnShadow
        OnDragDrop = tvMultiDragDrop
        OnDragOver = tvMultiDragOver
        OnStartDrag = tvMultiStartDrag
      end
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 208
    Height = 331
    LeftPosition = 0
    ActiveBorderType = fbtSunken
    Align = alClient
    AlwaysKeepFocus = False
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
    DoInplaceEdit = True
    DraggableSections = False
    DragAllowed = False
    DragTrgDrawMode = SelColorRect
    DragType = dtDelphi
    DragImageMode = dimNever
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
      F8FFFFFF07000000D8EA3C0400000000FFFFFFFF000001010100000078000000
      0000000010270000000100000000000000000000000000000000010000000000
      0000000000010000010000000001000000000000000000000000D8EA3C040000
      0000FFFFFFFF0000010101000000780000000000000010270000000100000100
      0000000000000000000000000100000000000000000000010000010000000001
      000000000000000000000000D8EA3C0400000000FFFFFFFF0000010101000000
      7800000000000000102700000001000002000000000000000000000000000100
      000000000000000000010000010000000001000000000000000000000000D8EA
      3C0400000000FFFFFFFF00000101010000007800000000000000102700000001
      0000030000000000000000000000000001000000000000000000000100000100
      00000001000000000000000000000000D8EA3C0400000000FFFFFFFF00000101
      0100000078000000000000001027000000010000040000000000000000000000
      0000010000000000000000000001000001000000000100000000000000000000
      0000D8EA3C0400000000FFFFFFFF000001010100000078000000000000001027
      0000000100000500000000000000000000000000010000000000000000000001
      0000010000000001000000000000000000000000D8EA3C0400000000FFFFFFFF
      0000010101000000780000000000000010270000000100000600000000000000
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
    OnItemSelectedChange = tvItemsItemSelectedChange
    OnDblClick = tvItemsDblClick
    OnKeyPress = tvItemsKeyPress
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
