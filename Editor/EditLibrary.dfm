object frmEditLibrary: TfrmEditLibrary
  Left = 504
  Top = 408
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Edit Library'
  ClientHeight = 309
  ClientWidth = 308
  Color = clBtnFace
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
    Left = 192
    Top = 0
    Width = 116
    Height = 309
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object ebCancel: TExtBtn
      Left = 4
      Top = 290
      Width = 109
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
      Left = 4
      Top = 273
      Width = 109
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
    object ebValidate: TExtBtn
      Left = 4
      Top = 249
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Validate Library'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebValidateClick
    end
    object ebMakeThm: TExtBtn
      Left = 4
      Top = 232
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Make Thumbnail'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMakeThmClick
    end
    object ebUnload: TExtBtn
      Left = 4
      Top = 208
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Unload Objects'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebUnloadClick
    end
    object ebLoadObject: TExtBtn
      Left = 4
      Top = 5
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Append'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebLoadObjectClick
    end
    object ebDeleteObject: TExtBtn
      Left = 4
      Top = 43
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Delete'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebDeleteObjectClick
    end
    object ebProperties: TExtBtn
      Left = 4
      Top = 66
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Properties'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebPropertiesClick
    end
    object ebSaveObjectOGF: TExtBtn
      Left = 4
      Top = 109
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save OGF'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveObjectOGFClick
    end
    object ebSaveObjectVCF: TExtBtn
      Left = 4
      Top = 91
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save Collision Form'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveObjectVCFClick
    end
    object ebSaveObjectSkeletonOGF: TExtBtn
      Left = 4
      Top = 127
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save Skeleton'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveObjectSkeletonOGFClick
    end
    object ebReloadObject: TExtBtn
      Left = 4
      Top = 23
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Reload'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebReloadObjectClick
    end
    object ebSaveObjectDO: TExtBtn
      Left = 4
      Top = 145
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save DO'
      Transparent = False
      FlatAlwaysEdge = True
      Visible = False
    end
    object cbPreview: TCheckBox
      Left = 4
      Top = 168
      Width = 108
      Height = 17
      Caption = 'Preview'
      TabOrder = 0
      OnClick = cbPreviewClick
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 309
    LeftPosition = 0
    ActiveBorderType = fbtSunken
    Align = alLeft
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
      F8FFFFFF0700000050CDF60D00000000FFFFFFFF0000010101006C6178000000
      00000000102700000001006C0000000000006F64000000000000016500000000
      000000000001000001000000000100000000000000000000000050CDF60D0000
      0000FFFFFFFF0000010101006C617800000000000000102700000001006C0100
      000000006F640000000000000165000000000000000000010000010000000001
      00000000000000000000000050CDF60D00000000FFFFFFFF0000010101006C61
      7800000000000000102700000001006C0200000000006F640000000000000165
      00000000000000000001000001000000000100000000000000000000000050CD
      F60D00000000FFFFFFFF0000010101006C617800000000000000102700000001
      006C0300000000006F6400000000000001650000000000000000000100000100
      0000000100000000000000000000000050CDF60D00000000FFFFFFFF00000101
      01006C617800000000000000102700000001006C0400000000006F6400000000
      0000016500000000000000000001000001000000000100000000000000000000
      000050CDF60D00000000FFFFFFFF0000010101006C6178000000000000001027
      00000001006C0500000000006F64000000000000016500000000000000000001
      000001000000000100000000000000000000000050CDF60D00000000FFFFFFFF
      0000010101006C617800000000000000102700000001006C0600000000006F64
      0000000000000165000000000000000000010000010000000001000000000000
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
    PopupMenu = pmObjects
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
    OnItemChange = tvObjectsItemChange
    OnTryEdit = tvObjectsTryEdit
    OnItemSelectedChange = tvObjectsItemSelectedChange
    OnDragDrop = tvObjectsDragDrop
    OnDragOver = tvObjectsDragOver
    OnStartDrag = tvObjectsStartDrag
    OnDblClick = tvObjectsDblClick
    OnKeyPress = tvObjectsKeyPress
  end
  object pmObjects: TPopupMenu
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
      OnClick = miDeleteFolderClick
    end
    object Folder1: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object LoadObject1: TMenuItem
      Caption = 'Append Object'
      GroupIndex = 1
      OnClick = ebLoadObjectClick
    end
    object DeleteObject1: TMenuItem
      Caption = 'Delete Object'
      GroupIndex = 1
      OnClick = ebDeleteObjectClick
    end
    object miObjectProperties: TMenuItem
      Caption = 'Object Properties'
      GroupIndex = 1
      OnClick = ebPropertiesClick
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
