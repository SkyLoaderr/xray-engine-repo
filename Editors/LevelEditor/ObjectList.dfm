object frmObjectList: TfrmObjectList
  Left = 480
  Top = 297
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
      BtnColor = 10528425
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
        CloseButton = False
        Caption = 'Refresh List'
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
        CloseButton = False
        Caption = 'Close'
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
      F4FFFFFF0700000000406A1400150000FFFFFFFF0000010101006A1478000000
      000000001027000000010064C0CA53050000000000002028000000000000016D
      0000000000000000000100000000000061746129292D3E53656C656374287472
      7565293B0D0A202020202020202055492D3E5265647261775363656E6528293B
      0D0A0D0A2020202020202020624C6F636B557064010000000001000000000000
      00000000000000406A1400150000FFFFFFFF0000010101006A14780000000000
      00001027000000010064702B49050100000000002028000000000000016D0000
      000000000000000100000000000061746129292D3E53656C6563742874727565
      293B0D0A202020202020202055492D3E5265647261775363656E6528293B0D0A
      0D0A2020202020202020624C6F636B5570640100000000010000000000000000
      0000000000406A1400150000FFFFFFFF0000010101006A147800000000000000
      1027000000010064504F150E0200000000002028000000000000016D00000000
      00000000000100000000000061746129292D3E53656C6563742874727565293B
      0D0A202020202020202055492D3E5265647261775363656E6528293B0D0A0D0A
      2020202020202020624C6F636B55706401000000000100000000000000000000
      000000406A1400150000FFFFFFFF0000010101006A1478000000000000001027
      00000001006484DF650E0300000000002028000000000000016D000000000000
      0000000100000000000061746129292D3E53656C6563742874727565293B0D0A
      202020202020202055492D3E5265647261775363656E6528293B0D0A0D0A2020
      202020202020624C6F636B557064010000000001000000000000000000000000
      00406A1400150000FFFFFFFF0000010101006A14780000000000000010270000
      00010064709814040400000000002028000000000000016D0000000000000000
      000100000000000061746129292D3E53656C6563742874727565293B0D0A2020
      20202020202055492D3E5265647261775363656E6528293B0D0A0D0A20202020
      20202020624C6F636B5570640100000000010000000000000000000000000040
      6A1400150000FFFFFFFF0000010101006A147800000000000000102700000001
      00640427B6190500000000002028000000000000016D00000000000000000001
      00000000000061746129292D3E53656C6563742874727565293B0D0A20202020
      2020202055492D3E5265647261775363656E6528293B0D0A0D0A202020202020
      2020624C6F636B55706401000000000100000000000000000000000000406A14
      00150000FFFFFFFF0000010101006A1478000000000000001027000000010064
      54C545040600000000002028000000000000016D000000000000000000010000
      0000000061746129292D3E53656C6563742874727565293B0D0A202020202020
      202055492D3E5265647261775363656E6528293B0D0A0D0A2020202020202020
      624C6F636B557064010000000001000000000000000000000000}
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
