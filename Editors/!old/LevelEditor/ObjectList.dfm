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
      F4FFFFFF0700000000400D0E00210000FFFFFFFF0000010101000D0E78000000
      000000001027000000010020C0CA530500000000000066200000000000000150
      000000000000000000010000000000004F626A6563742A29286E6F64652D3E44
      61746129292D3E53656C6563742874727565293B0D0A20202020202020205549
      2D3E5265647261775363656E6528293B0D0A0D0A010000000001000000000000
      00000000000000400D0E00210000FFFFFFFF0000010101000D0E780000000000
      00001027000000010020702B4905010000000000662000000000000001500000
      00000000000000010000000000004F626A6563742A29286E6F64652D3E446174
      6129292D3E53656C6563742874727565293B0D0A202020202020202055492D3E
      5265647261775363656E6528293B0D0A0D0A0100000000010000000000000000
      0000000000400D0E00210000FFFFFFFF0000010101000D0E7800000000000000
      1027000000010020504F150E0200000000006620000000000000015000000000
      0000000000010000000000004F626A6563742A29286E6F64652D3E4461746129
      292D3E53656C6563742874727565293B0D0A202020202020202055492D3E5265
      647261775363656E6528293B0D0A0D0A01000000000100000000000000000000
      000000400D0E00210000FFFFFFFF0000010101000D0E78000000000000001027
      00000001002084DF650E03000000000066200000000000000150000000000000
      000000010000000000004F626A6563742A29286E6F64652D3E4461746129292D
      3E53656C6563742874727565293B0D0A202020202020202055492D3E52656472
      61775363656E6528293B0D0A0D0A010000000001000000000000000000000000
      00400D0E00210000FFFFFFFF0000010101000D0E780000000000000010270000
      0001002070981404040000000000662000000000000001500000000000000000
      00010000000000004F626A6563742A29286E6F64652D3E4461746129292D3E53
      656C6563742874727565293B0D0A202020202020202055492D3E526564726177
      5363656E6528293B0D0A0D0A0100000000010000000000000000000000000040
      0D0E00210000FFFFFFFF0000010101000D0E7800000000000000102700000001
      00200427B6190500000000006620000000000000015000000000000000000001
      0000000000004F626A6563742A29286E6F64652D3E4461746129292D3E53656C
      6563742874727565293B0D0A202020202020202055492D3E5265647261775363
      656E6528293B0D0A0D0A01000000000100000000000000000000000000400D0E
      00210000FFFFFFFF0000010101000D0E78000000000000001027000000010020
      54C5450406000000000066200000000000000150000000000000000000010000
      000000004F626A6563742A29286E6F64652D3E4461746129292D3E53656C6563
      742874727565293B0D0A202020202020202055492D3E5265647261775363656E
      6528293B0D0A0D0A010000000001000000000000000000000000}
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
