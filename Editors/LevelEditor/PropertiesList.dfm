object Properties: TProperties
  Left = 836
  Top = 301
  Width = 242
  Height = 357
  BiDiMode = bdRightToLeft
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Shader Properties'
  Color = clBtnFace
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
  OnClose = FormClose
  OnDeactivate = FormDeactivate
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 0
    Top = 0
    Width = 234
    Height = 1
    Align = alTop
    Shape = bsFrame
  end
  object tvProperties: TElTree
    Left = 0
    Top = 1
    Width = 234
    Height = 329
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alClient
    AlwaysKeepSelection = False
    AutoCollapse = False
    AutoLineHeight = False
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    AdjustMultilineHeight = False
    BorderStyle = bsNone
    BorderSides = []
    Color = clWhite
    CustomPlusMinus = True
    DrawFocusRect = False
    ExpandOnDblClick = False
    ExplorerEditMode = True
    FilteredVisibility = True
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderColor = 7368816
    HeaderHeight = 17
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF020000004845CD0D00000000FFFFFFFF0000010001005C417D000000
      32000000102700000000007214B2950400000000000064790000000000000165
      000000000000000000010000000000000000000039000000503A5C456469746F
      72735C4163746F72456469746F725C496E7465726D6564696174655C536B656C
      65746F6E43616C63756C6174652E6F6290000000040000004B65790001000000
      0000000000000000004845CD0D00000000FFFFFFFF0000010101005C41590000
      00320000001027000000000072ECB69504010000000000647900000000000001
      65000000000000000000010000000000000000000039000000503A5C45646974
      6F72735C4163746F72456469746F725C496E7465726D6564696174655C536B65
      6C65746F6E43616C63756C6174652E6F62900000000600000056616C75650001
      000000000000000000000000}
    HeaderFlat = True
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 16
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 17
    LinesColor = clBtnShadow
    LinesStyle = psSolid
    MinusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C000000000000000000000000000000000000000808080808080
      808080808080808080808080808080808080808080808080808080E0E0E08080
      80808080808080808080808080808080808080E0E0E080808080808080808080
      8080808080808080E0E0E0E0E0E0E0E0E0808080808080808080808080808080
      E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0E0E0E0808080808080808080E0E0E0E0E0E0E0E0E0E0E0E0E0E0E080808080
      8080808080808080808080808080808080808080808080808080}
    MouseFrameSelect = True
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    ParentFont = False
    ParentShowHint = False
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
    QuickEditMode = True
    ScrollbarOpposite = False
    ShowColumns = True
    ShowHint = False
    ShowImages = False
    ShowLeafButton = False
    ShowLines = False
    SortType = stCustom
    StoragePath = '\Tree'
    TabOrder = 0
    TabStop = True
    Tracking = False
    TrackColor = 10526880
    VertDivLinesColor = 7368816
    VerticalLines = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 16
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    TextColor = clBtnText
    BkColor = clGray
    OnHeaderColumnResize = tvPropertiesHeaderColumnResize
    OnItemDraw = tvPropertiesItemDraw
    OnItemFocused = tvPropertiesItemFocused
    OnClick = tvPropertiesClick
    OnMouseDown = tvPropertiesMouseDown
  end
  object seNumber: TMultiObjSpinEdit
    Left = 0
    Top = 40
    Width = 76
    Height = 17
    LWSensitivity = 0.01
    ButtonKind = bkLightWave
    ButtonWidth = 16
    ValueType = vtFloat
    AutoSize = False
    BorderStyle = bsNone
    Color = 10526880
    Ctl3D = True
    ParentCtl3D = False
    TabOrder = 1
    Visible = False
    OnExit = seNumberExit
    OnKeyDown = seNumberKeyDown
  end
  object edText: TEdit
    Left = 0
    Top = 64
    Width = 121
    Height = 15
    BorderStyle = bsNone
    Color = 10526880
    TabOrder = 2
    Visible = False
    OnDblClick = edTextDblClick
    OnExit = edTextExit
    OnKeyDown = edTextKeyDown
  end
  object pmEnum: TMxPopupMenu
    Alignment = paCenter
    TrackButton = tbLeftButton
    MarginStartColor = 10921638
    MarginEndColor = 2763306
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 8
    Top = 152
  end
  object fsStorage: TFormStorage
    StoredValues = <>
    Left = 8
    Top = 8
  end
end
