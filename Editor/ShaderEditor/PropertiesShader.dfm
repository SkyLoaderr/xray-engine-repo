object frmShaderProperties: TfrmShaderProperties
  Left = 749
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
  OldCreateOrder = False
  ParentBiDiMode = False
  Scaled = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object tvProperties: TElTree
    Left = 0
    Top = 0
    Width = 234
    Height = 311
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
    HeaderColor = 7368816
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF0200000048436F6C00720419FFFFFFFF0000010001006C6F7D000000
      32000000102700000000006914B295040000000000000B6D0000000000000172
      0000000000000000000100000000000009544D656E754974656D06506C616E65
      310743617074696F6E0605506C616E65074F6E436C69636B070C507265766965
      77436C69636B000009544D656E754974656D0442040000004B65790001000000
      00000000000000000048436F6C00720419FFFFFFFF0000010101006C6F590000
      00320000001027000000000069ECB695040100000000000B6D00000000000001
      720000000000000000000100000000000009544D656E754974656D06506C616E
      65310743617074696F6E0605506C616E65074F6E436C69636B070C5072657669
      6577436C69636B000009544D656E754974656D04420600000056616C75650001
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
    OnClick = tvPropertiesClick
    OnMouseDown = tvPropertiesMouseDown
  end
  object Panel1: TPanel
    Left = 0
    Top = 311
    Width = 234
    Height = 19
    Align = alBottom
    Color = 10528425
    TabOrder = 1
    Visible = False
  end
  object seNumber: TMultiObjSpinEdit
    Left = 0
    Top = 16
    Width = 76
    Height = 20
    LWSensitivity = 0.01
    ButtonKind = bkLightWave
    ButtonWidth = 16
    ValueType = vtFloat
    AutoSize = False
    BorderStyle = bsNone
    Color = 10526880
    Ctl3D = True
    ParentCtl3D = False
    TabOrder = 2
    Visible = False
    OnExit = seNumberExit
    OnKeyDown = seNumberKeyDown
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
    Top = 32
  end
end
