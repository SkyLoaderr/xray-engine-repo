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
      F4FFFFFF070000004403821500130000FFFFFFFF0000010101000D4078000000
      000000001027000000010040C0CA53050000000000000D400000000000000140
      00000000000000000001000000000000150A0D403E0A0D40620A0D40850A0D40
      A80A0D40CA0A0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40
      D50B0D40FA0B0D401F0C0D40440C0D40680C0D40010000000001000000000000
      0000000000004403821500130000FFFFFFFF0000010101000D40780000000000
      00001027000000010040702B49050100000000000D4000000000000001400000
      0000000000000001000000000000150A0D403E0A0D40620A0D40850A0D40A80A
      0D40CA0A0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B
      0D40FA0B0D401F0C0D40440C0D40680C0D400100000000010000000000000000
      000000004403821500130000FFFFFFFF0000010101000D407800000000000000
      1027000000010040504F150E0200000000000D40000000000000014000000000
      000000000001000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40
      CA0A0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40
      FA0B0D401F0C0D40440C0D40680C0D4001000000000100000000000000000000
      00004403821500130000FFFFFFFF0000010101000D4078000000000000001027
      00000001004084DF650E0300000000000D400000000000000140000000000000
      00000001000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A
      0D40EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B
      0D401F0C0D40440C0D40680C0D40010000000001000000000000000000000000
      4403821500130000FFFFFFFF0000010101000D40780000000000000010270000
      00010040709814040400000000000D4000000000000001400000000000000000
      0001000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A0D40
      EC0A0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B0D40
      1F0C0D40440C0D40680C0D400100000000010000000000000000000000004403
      821500130000FFFFFFFF0000010101000D407800000000000000102700000001
      00400427B6190500000000000D40000000000000014000000000000000000001
      000000000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A0D40EC0A
      0D40120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B0D401F0C
      0D40440C0D40680C0D4001000000000100000000000000000000000044038215
      00130000FFFFFFFF0000010101000D4078000000000000001027000000010040
      54C545040600000000000D400000000000000140000000000000000000010000
      00000000150A0D403E0A0D40620A0D40850A0D40A80A0D40CA0A0D40EC0A0D40
      120B0D40340B0D40620B0D408B0B0D40B00B0D40D50B0D40FA0B0D401F0C0D40
      440C0D40680C0D40010000000001000000000000000000000000}
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
