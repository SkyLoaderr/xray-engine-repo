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
    object ebShowProperties: TExtBtn
      Left = 2
      Top = 125
      Width = 95
      Height = 19
      Align = alNone
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Show Properties'
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
      F4FFFFFF0700000060D81C0C00300000FFFFFFFF000001010100202078000000
      000000001027000000010020C0CA530500000000000020660000000000000163
      00000000000000000001000000000000696E28293B5F46213D6C73742E656E64
      28293B5F462B2B290D0A20202020202020202020202020202020202020204164
      644F626A656374286E6F64652C282A5F46292D3E010000000001000000000000
      00000000000060D81C0C00300000FFFFFFFF0000010101002020780000000000
      00001027000000010020702B4905010000000000206600000000000001630000
      0000000000000001000000000000696E28293B5F46213D6C73742E656E642829
      3B5F462B2B290D0A20202020202020202020202020202020202020204164644F
      626A656374286E6F64652C282A5F46292D3E0100000000010000000000000000
      0000000060D81C0C00300000FFFFFFFF00000101010020207800000000000000
      1027000000010020504F150E0200000000002066000000000000016300000000
      000000000001000000000000696E28293B5F46213D6C73742E656E6428293B5F
      462B2B290D0A20202020202020202020202020202020202020204164644F626A
      656374286E6F64652C282A5F46292D3E01000000000100000000000000000000
      000060D81C0C00300000FFFFFFFF000001010100202078000000000000001027
      00000001002084DF650E03000000000020660000000000000163000000000000
      00000001000000000000696E28293B5F46213D6C73742E656E6428293B5F462B
      2B290D0A20202020202020202020202020202020202020204164644F626A6563
      74286E6F64652C282A5F46292D3E010000000001000000000000000000000000
      60D81C0C00300000FFFFFFFF0000010101002020780000000000000010270000
      0001002070981404040000000000206600000000000001630000000000000000
      0001000000000000696E28293B5F46213D6C73742E656E6428293B5F462B2B29
      0D0A20202020202020202020202020202020202020204164644F626A65637428
      6E6F64652C282A5F46292D3E01000000000100000000000000000000000060D8
      1C0C00300000FFFFFFFF00000101010020207800000000000000102700000001
      00200427B6190500000000002066000000000000016300000000000000000001
      000000000000696E28293B5F46213D6C73742E656E6428293B5F462B2B290D0A
      20202020202020202020202020202020202020204164644F626A656374286E6F
      64652C282A5F46292D3E01000000000100000000000000000000000060D81C0C
      00300000FFFFFFFF000001010100202078000000000000001027000000010020
      54C5450406000000000020660000000000000163000000000000000000010000
      00000000696E28293B5F46213D6C73742E656E6428293B5F462B2B290D0A2020
      2020202020202020202020202020202020204164644F626A656374286E6F6465
      2C282A5F46292D3E010000000001000000000000000000000000}
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
