object frmProperties: TfrmProperties
  Left = 1038
  Top = 639
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
  PixelsPerInch = 96
  TextHeight = 13
  object tvProperties: TElTree
    Left = 0
    Top = 0
    Width = 234
    Height = 330
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
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF020000000557696400680254FFFFFFFF0000010001004C577D000000
      3200000010270000000000CC14B29504000000000000746F0000000000000162
      000000000000000000010000000000006D656E74050AD7A3703D0AD7A3F83F08
      4D617856616C75650264084D696E56616C7565050AD7A3703D0AD7A3F83F0956
      616C75655479706507077674466C6F6174055661040000004B65790001000000
      0000000000000000000557696400680254FFFFFFFF0000010101004C57590000
      003200000010270000000000CCECB69504010000000000746F00000000000001
      62000000000000000000010000000000006D656E74050AD7A3703D0AD7A3F83F
      084D617856616C75650264084D696E56616C7565050AD7A3703D0AD7A3F83F09
      56616C75655479706507077674466C6F61740556610600000056616C75650001
      000000000000000000000000}
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
