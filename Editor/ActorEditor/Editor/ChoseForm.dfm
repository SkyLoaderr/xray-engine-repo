object frmChoseItem: TfrmChoseItem
  Left = 933
  Top = 526
  Width = 347
  Height = 452
  Caption = 'Select Item'
  Color = clBtnFace
  Constraints.MinHeight = 360
  Constraints.MinWidth = 300
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 201
    Top = 0
    Width = 138
    Height = 425
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object paMulti: TPanel
      Left = 0
      Top = 184
      Width = 138
      Height = 223
      Align = alClient
      TabOrder = 0
      Visible = False
      object ebMultiUp: TExtBtn
        Left = 122
        Top = 3
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          F6000000424DF600000000000000360000002800000008000000080000000100
          180000000000C0000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDDDDDC000000000000DDDDDCFFFFFFFF
          FFFFFFFFFF000000000000000000000000191919000000FFFFFFFFFFFFDDDDDC
          000000000000000000000000DDDDDCFFFFFFFFFFFFFFFFFF0000000000000000
          00000000FFFFFFFFFFFFFFFFFFFFFFFFDDDDDC000000000000DDDDDCFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiUpClick
      end
      object ebMultiDown: TExtBtn
        Left = 122
        Top = 63
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          F6000000424DF600000000000000360000002800000008000000080000000100
          180000000000C0000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDDDDDC0000000000
          00DDDDDCFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000FFFFFFFF
          FFFFFFFFFFDDDDDC000000000000000000000000DDDDDCFFFFFFFFFFFF000000
          191919000000000000000000000000FFFFFFFFFFFFFFFFFFDDDDDC0000000000
          00DDDDDCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiDownClick
      end
      object ebMultiRemove: TExtBtn
        Left = 122
        Top = 23
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          56010000424D560100000000000036000000280000000A000000090000000100
          18000000000020010000120B0000120B00000000000000000000C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4000000
          000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
          000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4C8D0D4000000000000C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4000000
          000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiRemoveClick
      end
      object ebMultiClear: TExtBtn
        Left = 122
        Top = 43
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF000000000000
          FFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF0000000000000000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF00000000
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000
          000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000
          00000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000000000000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF
          000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiClearClick
      end
      object tvMulti: TElTree
        Left = 1
        Top = 1
        Width = 120
        Height = 221
        Cursor = crDefault
        LeftPosition = 0
        DoubleBuffered = False
        DragCursor = crDrag
        Align = alLeft
        AutoCollapse = False
        AutoExpand = True
        DockOrientation = doNoOrient
        DefaultSectionWidth = 120
        BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
        DragAllowed = True
        ExplorerEditMode = False
        Flat = True
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
          F4FFFFFF0700000094A1590500280000FFFFFFFF000001010100000078000000
          000000001027000000010041648977060000000000000000000000000000016D
          00000000000000000001000000000000656D3A3A65624D756C74695570436C69
          636B28544F626A656374202A53656E646572290D0A7B0D0A2020202069662028
          74764D756C74692D3E53656C6563746564262674010000000001000000000000
          00000000000094A1590500280000FFFFFFFF0000010101000000780000000000
          00001027000000010041BC0834040100000000000000000000000000016D0000
          0000000000000001000000000000656D3A3A65624D756C74695570436C69636B
          28544F626A656374202A53656E646572290D0A7B0D0A20202020696620287476
          4D756C74692D3E53656C65637465642626740100000000010000000000000000
          0000000094A1590500280000FFFFFFFF00000101010000007800000000000000
          1027000000010041DC3D26040200000000000000000000000000016D00000000
          000000000001000000000000656D3A3A65624D756C74695570436C69636B2854
          4F626A656374202A53656E646572290D0A7B0D0A202020206966202874764D75
          6C74692D3E53656C656374656426267401000000000100000000000000000000
          000094A1590500280000FFFFFFFF000001010100000078000000000000001027
          0000000100414022300D0300000000000000000000000000016D000000000000
          00000001000000000000656D3A3A65624D756C74695570436C69636B28544F62
          6A656374202A53656E646572290D0A7B0D0A202020206966202874764D756C74
          692D3E53656C6563746564262674010000000001000000000000000000000000
          94A1590500280000FFFFFFFF0000010101000000780000000000000010270000
          00010041FCCF2A040400000000000000000000000000016D0000000000000000
          0001000000000000656D3A3A65624D756C74695570436C69636B28544F626A65
          6374202A53656E646572290D0A7B0D0A202020206966202874764D756C74692D
          3E53656C656374656426267401000000000100000000000000000000000094A1
          590500280000FFFFFFFF00000101010000007800000000000000102700000001
          00411C0E440D0500000000000000000000000000016D00000000000000000001
          000000000000656D3A3A65624D756C74695570436C69636B28544F626A656374
          202A53656E646572290D0A7B0D0A202020206966202874764D756C74692D3E53
          656C656374656426267401000000000100000000000000000000000094A15905
          00280000FFFFFFFF000001010100000078000000000000001027000000010041
          28886A000600000000000000000000000000016D000000000000000000010000
          00000000656D3A3A65624D756C74695570436C69636B28544F626A656374202A
          53656E646572290D0A7B0D0A202020206966202874764D756C74692D3E53656C
          6563746564262674010000000001000000000000000000000000}
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
        IgnoreEnabled = False
        IncrementalSearch = False
        KeepSelectionWithinLevel = False
        LineBorderActiveColor = clBlack
        LineBorderInactiveColor = clBlack
        LineHeight = 17
        LinesColor = clBtnShadow
        MouseFrameSelect = True
        MultiSelect = False
        OwnerDrawMask = '~~@~~'
        PlusMinusTransparent = True
        ScrollbarOpposite = False
        ScrollTracking = True
        ShowButtons = False
        ShowLeafButton = False
        ShowLines = False
        StoragePath = '\Tree'
        TabOrder = 0
        TabStop = True
        TrackColor = 10526880
        VertDivLinesColor = 7368816
        VertScrollBarStyles.ShowTrackHint = True
        VertScrollBarStyles.Width = 16
        VertScrollBarStyles.ButtonSize = 14
        VertScrollBarStyles.UseSystemMetrics = False
        VirtualityLevel = vlNone
        TextColor = clBtnText
        BkColor = clGray
        OnDragDrop = tvMultiDragDrop
        OnDragOver = tvMultiDragOver
        OnStartDrag = tvMultiStartDrag
      end
    end
    object Panel2: TPanel
      Left = 0
      Top = 407
      Width = 138
      Height = 18
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      object sbSelect: TExtBtn
        Left = 2
        Top = 0
        Width = 68
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Ok'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = sbSelectClick
      end
      object sbCancel: TExtBtn
        Left = 70
        Top = 0
        Width = 68
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Cancel'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = sbCancelClick
      end
    end
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 138
      Height = 184
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 2
      object lbItemName: TLabel
        Left = 32
        Top = 141
        Width = 106
        Height = 13
        AutoSize = False
        Caption = '-'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object lbFileName: TLabel
        Left = 32
        Top = 155
        Width = 106
        Height = 13
        AutoSize = False
        Caption = '-'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object mxLabel1: TLabel
        Left = 1
        Top = 141
        Width = 32
        Height = 13
        AutoSize = False
        Caption = 'Item:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object mxLabel2: TLabel
        Left = 1
        Top = 155
        Width = 28
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
      object mxLabel3: TLabel
        Left = 1
        Top = 168
        Width = 28
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
        Left = 32
        Top = 168
        Width = 106
        Height = 13
        AutoSize = False
        Caption = '-'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object paImage: TPanel
        Left = 3
        Top = 6
        Width = 132
        Height = 132
        BevelOuter = bvLowered
        Caption = '<no image>'
        TabOrder = 0
        object pbImage: TPaintBox
          Left = 1
          Top = 1
          Width = 130
          Height = 130
          Align = alClient
          Color = clGray
          ParentColor = False
          OnPaint = pbImagePaint
        end
      end
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 201
    Height = 425
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alClient
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    Color = clWhite
    CustomPlusMinus = True
    DrawFocusRect = False
    ExplorerEditMode = False
    Flat = True
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderColor = 7368816
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF0700000094A1590500280000FFFFFFFF000001010100590578000000
      00000000102700000001006D206F2C0400000000000059050000000000000141
      0000000000000000000100000000000094A15905D82700006572466F6E742E4E
      616D65002475004194A15905C027000070436C69636B28544F626A656374202A
      53656E646572290D0A7B0D0A2020202069662028010000000001000000000000
      00000000000094A1590500280000FFFFFFFF0000010101005905780000000000
      0000102700000001006D18452904010000000000590500000000000001410000
      000000000000000100000000000094A15905D82700006572466F6E742E4E616D
      65002475004194A15905C027000070436C69636B28544F626A656374202A5365
      6E646572290D0A7B0D0A20202020696620280100000000010000000000000000
      0000000094A1590500280000FFFFFFFF00000101010059057800000000000000
      102700000001006DAC2976060200000000005905000000000000014100000000
      00000000000100000000000094A15905D82700006572466F6E742E4E616D6500
      2475004194A15905C027000070436C69636B28544F626A656374202A53656E64
      6572290D0A7B0D0A202020206966202801000000000100000000000000000000
      000094A1590500280000FFFFFFFF000001010100590578000000000000001027
      00000001006DD8CF160C03000000000059050000000000000141000000000000
      0000000100000000000094A15905D82700006572466F6E742E4E616D65002475
      004194A15905C027000070436C69636B28544F626A656374202A53656E646572
      290D0A7B0D0A2020202069662028010000000001000000000000000000000000
      94A1590500280000FFFFFFFF0000010101005905780000000000000010270000
      0001006DF0A92304040000000000590500000000000001410000000000000000
      000100000000000094A15905D82700006572466F6E742E4E616D650024750041
      94A15905C027000070436C69636B28544F626A656374202A53656E646572290D
      0A7B0D0A202020206966202801000000000100000000000000000000000094A1
      590500280000FFFFFFFF00000101010059057800000000000000102700000001
      006D106126040500000000005905000000000000014100000000000000000001
      00000000000094A15905D82700006572466F6E742E4E616D65002475004194A1
      5905C027000070436C69636B28544F626A656374202A53656E646572290D0A7B
      0D0A202020206966202801000000000100000000000000000000000094A15905
      00280000FFFFFFFF00000101010059057800000000000000102700000001006D
      1065720006000000000059050000000000000141000000000000000000010000
      0000000094A15905D82700006572466F6E742E4E616D65002475004194A15905
      C027000070436C69636B28544F626A656374202A53656E646572290D0A7B0D0A
      2020202069662028010000000001000000000000000000000000}
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
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 18
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
    ShowCheckboxes = True
    ShowLeafButton = False
    ShowLines = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 1
    TabStop = True
    Tracking = False
    TrackColor = 10526880
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
    OnItemChange = tvItemsItemChange
    OnItemFocused = tvItemsItemFocused
    OnDblClick = tvItemsDblClick
    OnKeyPress = tvItemsKeyPress
  end
  object fsStorage: TFormStorage
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0s
      end
      item
        Name = 'EmitterDirY'
        Value = 0s
      end
      item
        Name = 'EmitterDirZ'
        Value = 0s
      end>
    Left = 8
    Top = 8
  end
end
