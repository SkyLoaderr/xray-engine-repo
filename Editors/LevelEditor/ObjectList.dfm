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
    Align = alLeft
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
      F4FFFFFF070000000870580F00000000FFFFFFFF000001010100295C78000000
      000000001027000000010050C0CA5305000000000000623B0000000000000167
      0000000000000000000100000000000074696F6E733B456E67696E655C426C65
      6E646572733B456E67696E655C47616D653B523A5C426F726C616E645C436F6D
      706F6E656E7473365C616C65786D783B90000000010000000001000000000000
      0000000000000870580F00000000FFFFFFFF000001010100295C780000000000
      00001027000000010050702B4905010000000000623B00000000000001670000
      000000000000000100000000000074696F6E733B456E67696E655C426C656E64
      6572733B456E67696E655C47616D653B523A5C426F726C616E645C436F6D706F
      6E656E7473365C616C65786D783B900000000100000000010000000000000000
      000000000870580F00000000FFFFFFFF000001010100295C7800000000000000
      1027000000010050504F150E020000000000623B000000000000016700000000
      00000000000100000000000074696F6E733B456E67696E655C426C656E646572
      733B456E67696E655C47616D653B523A5C426F726C616E645C436F6D706F6E65
      6E7473365C616C65786D783B9000000001000000000100000000000000000000
      00000870580F00000000FFFFFFFF000001010100295C78000000000000001027
      00000001005084DF650E030000000000623B0000000000000167000000000000
      0000000100000000000074696F6E733B456E67696E655C426C656E646572733B
      456E67696E655C47616D653B523A5C426F726C616E645C436F6D706F6E656E74
      73365C616C65786D783B90000000010000000001000000000000000000000000
      0870580F00000000FFFFFFFF000001010100295C780000000000000010270000
      0001005070981404040000000000623B00000000000001670000000000000000
      000100000000000074696F6E733B456E67696E655C426C656E646572733B456E
      67696E655C47616D653B523A5C426F726C616E645C436F6D706F6E656E747336
      5C616C65786D783B900000000100000000010000000000000000000000000870
      580F00000000FFFFFFFF000001010100295C7800000000000000102700000001
      00500427B619050000000000623B000000000000016700000000000000000001
      00000000000074696F6E733B456E67696E655C426C656E646572733B456E6769
      6E655C47616D653B523A5C426F726C616E645C436F6D706F6E656E7473365C61
      6C65786D783B900000000100000000010000000000000000000000000870580F
      00000000FFFFFFFF000001010100295C78000000000000001027000000010050
      54C54504060000000000623B0000000000000167000000000000000000010000
      0000000074696F6E733B456E67696E655C426C656E646572733B456E67696E65
      5C47616D653B523A5C426F726C616E645C436F6D706F6E656E7473365C616C65
      786D783B90000000010000000001000000000000000000000000}
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
