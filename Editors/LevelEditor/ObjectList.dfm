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
      F4FFFFFF07000000A8C80A0100000000FFFFFFFF000001010100000078000000
      000000001027000000010000C0CA53050000000000005B000000000000000100
      0000000000000000000100000000000089E35B00010000000000000003000000
      00000000D8EB5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B008105B200
      9105B200A905B200B43B27062CC90A0190000000010000000001000000000000
      000000000000A8C80A0100000000FFFFFFFF0000010101000000780000000000
      00001027000000010000702B49050100000000005B0000000000000001000000
      000000000000000100000000000089E35B000100000000000000030000000000
      0000D8EB5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B008105B2009105
      B200A905B200B43B27062CC90A01900000000100000000010000000000000000
      00000000A8C80A0100000000FFFFFFFF00000101010000007800000000000000
      1027000000010000504F150E0200000000005B00000000000000010000000000
      00000000000100000000000089E35B0001000000000000000300000000000000
      D8EB5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B008105B2009105B200
      A905B200B43B27062CC90A019000000001000000000100000000000000000000
      0000A8C80A0100000000FFFFFFFF000001010100000078000000000000001027
      00000001000084DF650E0300000000005B000000000000000100000000000000
      0000000100000000000089E35B0001000000000000000300000000000000D8EB
      5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B008105B2009105B200A905
      B200B43B27062CC90A0190000000010000000001000000000000000000000000
      A8C80A0100000000FFFFFFFF0000010101000000780000000000000010270000
      00010000709814040400000000005B0000000000000001000000000000000000
      000100000000000089E35B0001000000000000000300000000000000D8EB5B00
      E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B008105B2009105B200A905B200
      B43B27062CC90A0190000000010000000001000000000000000000000000A8C8
      0A0100000000FFFFFFFF00000101010000007800000000000000102700000001
      00000427B6190500000000005B00000000000000010000000000000000000001
      00000000000089E35B0001000000000000000300000000000000D8EB5B00E8EB
      5B0000EC5B0010EC5B0020EC5B004CEC5B008105B2009105B200A905B200B43B
      27062CC90A0190000000010000000001000000000000000000000000A8C80A01
      00000000FFFFFFFF000001010100000078000000000000001027000000010000
      54C545040600000000005B000000000000000100000000000000000000010000
      0000000089E35B0001000000000000000300000000000000D8EB5B00E8EB5B00
      00EC5B0010EC5B0020EC5B004CEC5B008105B2009105B200A905B200B43B2706
      2CC90A0190000000010000000001000000000000000000000000}
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
  end
  object tmRefreshSelection: TTimer
    Enabled = False
    OnTimer = tmRefreshListTimer
  end
  object tmRefreshList: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmRefreshSelectionTimer
    Top = 32
  end
end
