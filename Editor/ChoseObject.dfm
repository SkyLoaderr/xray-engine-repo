object frmChoseObject: TfrmChoseObject
  Left = 250
  Top = 241
  BorderStyle = bsDialog
  Caption = 'Select Object'
  ClientHeight = 280
  ClientWidth = 466
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 225
    Top = 0
    Width = 241
    Height = 280
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object sbSelect: TExtBtn
      Left = -1
      Top = 260
      Width = 121
      Height = 20
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Select'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbSelectClick
    end
    object sbCancel: TExtBtn
      Left = 120
      Top = 260
      Width = 121
      Height = 20
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbCancelClick
    end
    object ebAddFolder: TExtBtn
      Left = 0
      Top = 2
      Width = 121
      Height = 20
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Add Folder'
      Transparent = False
      FlatAlwaysEdge = True
    end
    object ebDeleteFolder: TExtBtn
      Left = 121
      Top = 2
      Width = 121
      Height = 20
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Delete Folder'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbCancelClick
    end
    object paPreview: TPanel
      Left = 2
      Top = 24
      Width = 239
      Height = 234
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 0
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 225
    Height = 280
    LeftPosition = 0
    ActiveBorderType = fbtSunken
    Align = alLeft
    AlwaysKeepFocus = False
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
      F8FFFFFF0700000068C2980300000000FFFFFFFF000001010100BB0A78000000
      000000001027000000010008000000000000C50A000000000000010800000000
      000000000001000001000000000100000000000000000000000068C298030000
      0000FFFFFFFF000001010100BB0A780000000000000010270000000100080100
      00000000C50A0000000000000108000000000000000000010000010000000001
      00000000000000000000000068C2980300000000FFFFFFFF000001010100BB0A
      78000000000000001027000000010008020000000000C50A0000000000000108
      00000000000000000001000001000000000100000000000000000000000068C2
      980300000000FFFFFFFF000001010100BB0A7800000000000000102700000001
      0008030000000000C50A00000000000001080000000000000000000100000100
      0000000100000000000000000000000068C2980300000000FFFFFFFF00000101
      0100BB0A78000000000000001027000000010008040000000000C50A00000000
      0000010800000000000000000001000001000000000100000000000000000000
      000068C2980300000000FFFFFFFF000001010100BB0A78000000000000001027
      000000010008050000000000C50A000000000000010800000000000000000001
      000001000000000100000000000000000000000068C2980300000000FFFFFFFF
      000001010100BB0A78000000000000001027000000010008060000000000C50A
      0000000000000108000000000000000000010000010000000001000000000000
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
    SortMode = smNone
    SortSection = 0
    SortType = stCustom
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
    OnDblClick = tvObjectsDblClick
  end
end
