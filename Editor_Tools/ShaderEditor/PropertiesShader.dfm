object frmShaderProperties: TfrmShaderProperties
  Left = 468
  Top = 497
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
      F5FFFFFF02000000C0E4641000000000FFFFFFFF00000100010028427D000000
      32000000102700000000006C14B29504000000000000645C000000000000016E
      000000000000000000010000000000006C616E645C436F6D706F6E656E74735C
      456C646F735C436F64653B523A5C426F726C616E645C436F6D706F6E656E7473
      5C456C646F735C436F64655C536F757290000000040000004B65790001000000
      000000000000000000C0E4641000000000FFFFFFFF0000010101002842590000
      0032000000102700000000006CECB69504010000000000645C00000000000001
      6E000000000000000000010000000000006C616E645C436F6D706F6E656E7473
      5C456C646F735C436F64653B523A5C426F726C616E645C436F6D706F6E656E74
      735C456C646F735C436F64655C536F7572900000000600000056616C75650001
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
  object InplaceEdit: TElTreeInplaceEdit
    Tree = tvProperties
    Types = [sftText]
    OnAfterOperation = InplaceEditAfterOperation
    Left = 8
    Top = 152
  end
  object InplaceCustom1: TElTreeInplaceButtonEdit
    Types = [sftCustom]
    Left = 8
    Top = 184
  end
  object InplaceFloat: TElTreeInplaceFloatSpinEdit
    Tree = tvProperties
    Types = [sftFloating]
    DefaultValueAsText = '0'
    OnBeforeOperation = InplaceFloatBeforeOperation
    OnAfterOperation = InplaceFloatAfterOperation
    Left = 72
    Top = 152
  end
  object InplaceNumber: TElTreeInplaceSpinEdit
    Tree = tvProperties
    Types = [sftNumber]
    DefaultValueAsText = '0'
    OnBeforeOperation = InplaceNumberBeforeOperation
    OnAfterOperation = InplaceNumberAfterOperation
    Left = 104
    Top = 152
  end
  object InplaceEnum: TElTreeInplaceModalEdit
    Tree = tvProperties
    Types = [sftEnum]
    Left = 136
    Top = 152
  end
  object pmEnum: TMxPopupMenu
    Alignment = paCenter
    TrackButton = tbLeftButton
    MarginStartColor = 10921638
    MarginEndColor = 2763306
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 7368816
    SepLColor = 10921638
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 136
    Top = 184
  end
  object fsStorage: TFormStorage
    StoredValues = <>
    Left = 8
    Top = 8
  end
end
