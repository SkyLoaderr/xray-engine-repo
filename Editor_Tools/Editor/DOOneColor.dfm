object frmOneColor: TfrmOneColor
  Left = 376
  Top = 220
  Align = alTop
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 57
  ClientWidth = 143
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 0
    Top = 0
    Width = 143
    Height = 57
  end
  object Bevel2: TBevel
    Left = 1
    Top = 1
    Width = 141
    Height = 55
    Style = bsRaised
  end
  object mcColor: TMultiObjColor
    Left = 3
    Top = 15
    Width = 23
    Height = 39
    Shape = stRoundRect
    OnMouseDown = mcColorMouseDown
  end
  object ebMultiRemove: TExtBtn
    Left = 3
    Top = 2
    Width = 23
    Height = 13
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
  object tvDOList: TElTree
    Left = 27
    Top = 2
    Width = 113
    Height = 52
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderStyle = bsNone
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    DragAllowed = True
    DrawFocusRect = False
    ExplorerEditMode = False
    Flat = True
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF07000000B0D5EB0000280000FFFFFFFF000001010100EB0078000000
      00000000102700000001006DBC55640E000000000000EB000000000000000100
      0000000000000000000100000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000010000000001000000000000
      000000000000B0D5EB0000280000FFFFFFFF000001010100EB00780000000000
      0000102700000001006D5C598B0D010000000000EB0000000000000001000000
      0000000000000001000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000100000000010000000000000000
      00000000B0D5EB0000280000FFFFFFFF000001010100EB007800000000000000
      102700000001006D0CC9BA0C020000000000EB00000000000000010000000000
      0000000000010000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000001000000000100000000000000000000
      0000B0D5EB0000280000FFFFFFFF000001010100EB0078000000000000001027
      00000001006DA0727F0D030000000000EB000000000000000100000000000000
      0000000100000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000010000000001000000000000000000000000
      B0D5EB0000280000FFFFFFFF000001010100EB00780000000000000010270000
      0001006D8C58E70E040000000000EB0000000000000001000000000000000000
      0001000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000010000000001000000000000000000000000B0D5
      EB0000280000FFFFFFFF000001010100EB007800000000000000102700000001
      006DCC78BD0C050000000000EB00000000000000010000000000000000000001
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000010000000001000000000000000000000000B0D5EB00
      00280000FFFFFFFF000001010100EB007800000000000000102700000001006D
      9C6D900C060000000000EB000000000000000100000000000000000000010000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000010000000001000000000000000000000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 9
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    LinesColor = clBtnShadow
    MouseFrameSelect = True
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    ParentFont = False
    PlusMinusTransparent = True
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowButtons = False
    ShowLeafButton = False
    ShowLines = False
    ShowRootButtons = False
    StoragePath = '\Tree'
    TabOrder = 0
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    TextColor = clBtnText
    BkColor = clGray
    OnDragDrop = tvDOListDragDrop
    OnDragOver = tvDOListDragOver
    OnStartDrag = tvDOListStartDrag
  end
end
