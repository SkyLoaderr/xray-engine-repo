object frmEditLibrary: TfrmEditLibrary
  Left = 366
  Top = 270
  BorderStyle = bsDialog
  Caption = 'Edit Library'
  ClientHeight = 276
  ClientWidth = 491
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 225
    Top = 0
    Width = 266
    Height = 276
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object sbCancel: TExtBtn
      Left = 134
      Top = 258
      Width = 132
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Close'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbCancelClick
    end
    object sbNewObject: TExtBtn
      Left = 2
      Top = 1
      Width = 89
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Add New Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbNewObjectClick
    end
    object sbDeleteObject: TExtBtn
      Left = 185
      Top = 1
      Width = 81
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Delete Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbDeleteObjectClick
    end
    object sbEdit: TExtBtn
      Left = 91
      Top = 1
      Width = 94
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Edit Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbEditClick
    end
    object sbSave: TExtBtn
      Left = 2
      Top = 258
      Width = 132
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbSaveClick
    end
    object pcObjects: TPageControl
      Left = 2
      Top = 21
      Width = 265
      Height = 236
      ActivePage = tsImage
      TabOrder = 0
      object tsImage: TTabSheet
        Caption = 'Preview'
        object paPreview: TPanel
          Left = 0
          Top = 0
          Width = 257
          Height = 208
          Align = alClient
          BevelOuter = bvLowered
          Caption = '<no image>'
          TabOrder = 0
        end
      end
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 225
    Height = 276
    LeftPosition = 0
    ActiveBorderType = fbtSunken
    Align = alLeft
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
      F8FFFFFF070000003023050600000000FFFFFFFF000001010100000078000000
      0000000010270000000100000000000000000000000000000000010000000000
      0000000000010000010000000001000000000000000000000000302305060000
      0000FFFFFFFF0000010101000000780000000000000010270000000100000100
      0000000000000000000000000100000000000000000000010000010000000001
      0000000000000000000000003023050600000000FFFFFFFF0000010101000000
      7800000000000000102700000001000002000000000000000000000000000100
      0000000000000000000100000100000000010000000000000000000000003023
      050600000000FFFFFFFF00000101010000007800000000000000102700000001
      0000030000000000000000000000000001000000000000000000000100000100
      000000010000000000000000000000003023050600000000FFFFFFFF00000101
      0100000078000000000000001027000000010000040000000000000000000000
      0000010000000000000000000001000001000000000100000000000000000000
      00003023050600000000FFFFFFFF000001010100000078000000000000001027
      0000000100000500000000000000000000000000010000000000000000000001
      00000100000000010000000000000000000000003023050600000000FFFFFFFF
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
    OnItemSelectedChange = tvObjectsItemSelectedChange
    OnDblClick = tvObjectsDblClick
  end
end
