object frmImageLib: TfrmImageLib
  Left = 469
  Top = 373
  Width = 350
  Height = 499
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSizeToolWin
  Caption = 'Image Editor'
  Color = clBtnFace
  Constraints.MinHeight = 400
  Constraints.MinWidth = 350
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
    Left = 174
    Top = 0
    Width = 168
    Height = 472
    Align = alRight
    BevelOuter = bvLowered
    Color = 10528425
    TabOrder = 0
    object Panel2: TPanel
      Left = 1
      Top = 427
      Width = 166
      Height = 44
      Align = alBottom
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 0
      object ebClose: TExtBtn
        Left = 2
        Top = 26
        Width = 162
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Close'
        FlatAlwaysEdge = True
        OnClick = ebCloseClick
      end
      object ebConvert: TExtBtn
        Left = 2
        Top = 2
        Width = 162
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Convert...'
        Enabled = False
        FlatAlwaysEdge = True
        OnClick = ebConvertClick
      end
      object Bevel1: TBevel
        Left = 3
        Top = 21
        Width = 161
        Height = 2
      end
    end
    object paImage: TPanel
      Left = 1
      Top = 1
      Width = 166
      Height = 162
      Align = alTop
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 164
        Height = 160
        Align = alClient
        Color = clGray
        ParentColor = False
        OnPaint = pbImagePaint
      end
    end
    object Panel3: TPanel
      Left = 1
      Top = 163
      Width = 166
      Height = 34
      Align = alTop
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 2
      object lbFileName: TLabel
        Left = 33
        Top = 2
        Width = 131
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel2: TLabel
        Left = 4
        Top = 2
        Width = 29
        Height = 13
        AutoSize = False
        Caption = 'File:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object RxLabel3: TLabel
        Left = 4
        Top = 18
        Width = 29
        Height = 13
        AutoSize = False
        Caption = 'Info:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbInfo: TLabel
        Left = 33
        Top = 18
        Width = 131
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
    object paProperties: TPanel
      Left = 1
      Top = 197
      Width = 166
      Height = 230
      Align = alClient
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 3
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 174
    Height = 472
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alClient
    AlwaysKeepFocus = True
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
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF070000003CA2400400130000FFFFFFFF000001010100004178000000
      000000001027000000010000E8211B0700000000000000000000000000000105
      0000000000000000000100000000000000000000070000005F45444928000000
      1F000000000000000D0000005F4C4556454C5F454449544F7800000017000000
      00000000060000005F4445425547000024750041010000000001000000000000
      0000000000003CA2400400130000FFFFFFFF0000010101000041780000000000
      00001027000000010000B0261B07010000000000000000000000000001050000
      000000000000000100000000000000000000070000005F454449280000001F00
      0000000000000D0000005F4C4556454C5F454449544F78000000170000000000
      0000060000005F44454255470000247500410100000000010000000000000000
      000000003CA2400400130000FFFFFFFF00000101010000417800000000000000
      102700000001000074271B070200000000000000000000000000010500000000
      00000000000100000000000000000000070000005F454449280000001F000000
      000000000D0000005F4C4556454C5F454449544F780000001700000000000000
      060000005F444542554700002475004101000000000100000000000000000000
      00003CA2400400130000FFFFFFFF000001010100004178000000000000001027
      00000001000038281B0703000000000000000000000000000105000000000000
      0000000100000000000000000000070000005F454449280000001F0000000000
      00000D0000005F4C4556454C5F454449544F7800000017000000000000000600
      00005F4445425547000024750041010000000001000000000000000000000000
      3CA2400400130000FFFFFFFF0000010101000041780000000000000010270000
      00010000FC281B07040000000000000000000000000001050000000000000000
      000100000000000000000000070000005F454449280000001F00000000000000
      0D0000005F4C4556454C5F454449544F78000000170000000000000006000000
      5F44454255470000247500410100000000010000000000000000000000003CA2
      400400130000FFFFFFFF00000101010000417800000000000000102700000001
      0000C0291B070500000000000000000000000000010500000000000000000001
      00000000000000000000070000005F454449280000001F000000000000000D00
      00005F4C4556454C5F454449544F780000001700000000000000060000005F44
      454255470000247500410100000000010000000000000000000000003CA24004
      00130000FFFFFFFF000001010100004178000000000000001027000000010000
      842A1B0706000000000000000000000000000105000000000000000000010000
      0000000000000000070000005F454449280000001F000000000000000D000000
      5F4C4556454C5F454449544F780000001700000000000000060000005F444542
      5547000024750041010000000001000000000000000000000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 17
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
    OnItemFocused = tvItemsItemFocused
    OnKeyPress = tvItemsKeyPress
  end
  object fsStorage: TFormStorage
    StoredValues = <>
  end
end
