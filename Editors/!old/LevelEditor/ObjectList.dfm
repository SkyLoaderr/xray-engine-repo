object frmObjectList: TfrmObjectList
  Left = 443
  Top = 295
  Width = 297
  Height = 360
  Caption = 'Object List'
  Color = clBtnFace
  Constraints.MinHeight = 360
  Constraints.MinWidth = 297
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
    Height = 333
    Align = alRight
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 0
    object ebShowSel: TExtBtn
      Left = 1
      Top = 84
      Width = 95
      Height = 19
      Align = alNone
      BtnColor = 10528425
      Caption = 'Show Selected'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
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
      BtnColor = 10528425
      Caption = 'Hide Selected'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebHideSelClick
    end
    object ebShowProperties: TExtBtn
      Left = 2
      Top = 125
      Width = 95
      Height = 19
      Align = alNone
      BtnColor = 10528425
      Caption = 'Show Properties'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebShowPropertiesClick
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
    object Panel2: TPanel
      Left = 0
      Top = 293
      Width = 97
      Height = 40
      Align = alBottom
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 1
      object sbRefreshList: TExtBtn
        Left = 1
        Top = 0
        Width = 95
        Height = 19
        Align = alNone
        BtnColor = 10528425
        Caption = 'Refresh List'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = sbRefreshListClick
      end
      object sbClose: TExtBtn
        Left = 1
        Top = 20
        Width = 95
        Height = 19
        Align = alNone
        BtnColor = 10528425
        Caption = 'Close'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = sbCloseClick
      end
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 333
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
      F4FFFFFF0700000070476A0600270000FFFFFFFF000001010100696F78000000
      000000001027000000010006C0CA5305000000000000466F0000000000000154
      000000000000000000010000000000000D0A20202020766F6964205F5F666173
      7463616C6C2065624869646553656C436C69636B28544F626A656374202A5365
      6E646572293B0D0A20202020766F6964205F5F66010000000001000000000000
      00000000000070476A0600270000FFFFFFFF000001010100696F780000000000
      00001027000000010006702B4905010000000000466F00000000000001540000
      00000000000000010000000000000D0A20202020766F6964205F5F6661737463
      616C6C2065624869646553656C436C69636B28544F626A656374202A53656E64
      6572293B0D0A20202020766F6964205F5F660100000000010000000000000000
      0000000070476A0600270000FFFFFFFF000001010100696F7800000000000000
      1027000000010006504F150E020000000000466F000000000000015400000000
      0000000000010000000000000D0A20202020766F6964205F5F6661737463616C
      6C2065624869646553656C436C69636B28544F626A656374202A53656E646572
      293B0D0A20202020766F6964205F5F6601000000000100000000000000000000
      000070476A0600270000FFFFFFFF000001010100696F78000000000000001027
      00000001000684DF650E030000000000466F0000000000000154000000000000
      000000010000000000000D0A20202020766F6964205F5F6661737463616C6C20
      65624869646553656C436C69636B28544F626A656374202A53656E646572293B
      0D0A20202020766F6964205F5F66010000000001000000000000000000000000
      70476A0600270000FFFFFFFF000001010100696F780000000000000010270000
      0001000670981404040000000000466F00000000000001540000000000000000
      00010000000000000D0A20202020766F6964205F5F6661737463616C6C206562
      4869646553656C436C69636B28544F626A656374202A53656E646572293B0D0A
      20202020766F6964205F5F660100000000010000000000000000000000007047
      6A0600270000FFFFFFFF000001010100696F7800000000000000102700000001
      00060427B619050000000000466F000000000000015400000000000000000001
      0000000000000D0A20202020766F6964205F5F6661737463616C6C2065624869
      646553656C436C69636B28544F626A656374202A53656E646572293B0D0A2020
      2020766F6964205F5F6601000000000100000000000000000000000070476A06
      00270000FFFFFFFF000001010100696F78000000000000001027000000010006
      54C54504060000000000466F0000000000000154000000000000000000010000
      000000000D0A20202020766F6964205F5F6661737463616C6C20656248696465
      53656C436C69636B28544F626A656374202A53656E646572293B0D0A20202020
      766F6964205F5F66010000000001000000000000000000000000}
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
