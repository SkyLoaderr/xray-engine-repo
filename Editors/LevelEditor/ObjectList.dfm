object frmObjectList: TfrmObjectList
  Left = 480
  Top = 297
  Width = 297
  Height = 359
  Caption = 'Object List'
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
    Width = 97
    Height = 332
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object sbClose: TExtBtn
      Left = 1
      Top = 312
      Width = 95
      Height = 19
      Align = alNone
      CloseButton = False
      Caption = 'Close'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbCloseClick
    end
    object sbRefreshList: TExtBtn
      Left = 1
      Top = 292
      Width = 95
      Height = 19
      Align = alNone
      CloseButton = False
      Caption = 'Refresh List'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbRefreshListClick
    end
    object ebShowSel: TExtBtn
      Left = 1
      Top = 84
      Width = 95
      Height = 19
      Align = alNone
      CloseButton = False
      Caption = 'Show Selected'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebShowSelClick
    end
    object ebHideSel: TExtBtn
      Left = 1
      Top = 104
      Width = 95
      Height = 19
      Align = alNone
      CloseButton = False
      Caption = 'Hide Selected'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebHideSelClick
    end
    object rgSO: TRadioGroup
      Left = 0
      Top = 0
      Width = 97
      Height = 81
      Align = alTop
      Caption = ' Show options '
      ItemIndex = 1
      Items.Strings = (
        'All'
        'Visible Only'
        'Invisible Only')
      TabOrder = 0
      OnClick = rgSOClick
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 332
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alClient
    AlwaysKeepSelection = False
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    CustomPlusMinus = True
    DoInplaceEdit = False
    DrawFocusRect = False
    DragImageMode = dimOne
    DragTrgDrawMode = ColorRect
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
      F4FFFFFF070000008487AC0500000000FFFFFFFF000001010100737478000000
      000000001027000000010003C0CA530500000000000000000000000000000100
      000000000000000000010000000000000000000000000000A05B8C0418B7FC04
      3C0000000000000000000000000000000000000027000000703A5C6524FDAD05
      24FDAD0518000000503A5C7872436F7290000000010000000001000000000000
      0000000000008487AC0500000000FFFFFFFF0000010101007374780000000000
      00001027000000010003702B4905010000000000000000000000000001000000
      00000000000000010000000000000000000000000000A05B8C0418B7FC043C00
      00000000000000000000000000000000000027000000703A5C6524FDAD0524FD
      AD0518000000503A5C7872436F72900000000100000000010000000000000000
      000000008487AC0500000000FFFFFFFF00000101010073747800000000000000
      1027000000010003504F150E0200000000000000000000000000010000000000
      0000000000010000000000000000000000000000A05B8C0418B7FC043C000000
      0000000000000000000000000000000027000000703A5C6524FDAD0524FDAD05
      18000000503A5C7872436F729000000001000000000100000000000000000000
      00008487AC0500000000FFFFFFFF000001010100737478000000000000001027
      00000001000384DF650E03000000000000000000000000000100000000000000
      000000010000000000000000000000000000A05B8C0418B7FC043C0000000000
      000000000000000000000000000027000000703A5C6524FDAD0524FDAD051800
      0000503A5C7872436F7290000000010000000001000000000000000000000000
      8487AC0500000000FFFFFFFF0000010101007374780000000000000010270000
      0001000370981404040000000000000000000000000001000000000000000000
      00010000000000000000000000000000A05B8C0418B7FC043C00000000000000
      00000000000000000000000027000000703A5C6524FDAD0524FDAD0518000000
      503A5C7872436F72900000000100000000010000000000000000000000008487
      AC0500000000FFFFFFFF00000101010073747800000000000000102700000001
      00030427B6190500000000000000000000000000010000000000000000000001
      0000000000000000000000000000A05B8C0418B7FC043C000000000000000000
      0000000000000000000027000000703A5C6524FDAD0524FDAD0518000000503A
      5C7872436F72900000000100000000010000000000000000000000008487AC05
      00000000FFFFFFFF000001010100737478000000000000001027000000010003
      54C5450406000000000000000000000000000100000000000000000000010000
      000000000000000000000000A05B8C0418B7FC043C0000000000000000000000
      000000000000000027000000703A5C6524FDAD0524FDAD0518000000503A5C78
      72436F7290000000010000000001000000000000000000000000}
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
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    LinesColor = clBtnShadow
    MinusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      808080808080808080808080808080808080808080808080808080E0E0E08080
      80808080808080808080808080808080808080E0E0E080808080808080808080
      8080808080808080E0E0E0E0E0E0E0E0E0808080808080808080808080808080
      E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0E0E0E0808080808080808080E0E0E0E0E0E0E0E0E0E0E0E0E0E0E080808080
      8080808080808080808080808080808080808080808080808080}
    MouseFrameSelect = True
    OwnerDrawMask = '~~@~~'
    ParentFont = False
    PlusMinusTransparent = True
    PlusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      8080808080808080808080808080808080808080808080808080808080808080
      80808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080E0E0E0E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0
      E0E0E0E0E0E0E0E0E0E0E0E0E0E0E0808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080808080808080808080808080808080808080808080}
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowLeafButton = False
    ShowLines = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 1
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertDivLinesColor = 7368816
    VerticalLines = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    TextColor = clBtnText
    BkColor = clGray
    OnAfterSelectionChange = tvItemsAfterSelectionChange
    OnItemFocused = tvItemsItemFocused
    OnKeyPress = tvItemsKeyPress
  end
  object fsStorage: TFormStorage
    IniSection = 'Object List'
    StoredValues = <>
  end
end
