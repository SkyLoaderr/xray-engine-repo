object frmChoseItem: TfrmChoseItem
  Left = 525
  Top = 336
  Width = 347
  Height = 452
  Caption = 'Select Item'
  Color = 10528425
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
    ParentColor = True
    TabOrder = 0
    object paMulti: TPanel
      Left = 0
      Top = 197
      Width = 138
      Height = 210
      Align = alClient
      ParentColor = True
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
        OnClick = ebMultiClearClick
      end
      object tvMulti: TElTree
        Left = 1
        Top = 1
        Width = 120
        Height = 208
        Cursor = crDefault
        LeftPosition = 0
        DragCursor = crDrag
        Align = alLeft
        AutoCollapse = False
        AutoExpand = True
        DockOrientation = doNoOrient
        DefaultSectionWidth = 120
        BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
        DragAllowed = True
        DrawFocusRect = False
        DragTrgDrawMode = dtdDownColorLine
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
          F4FFFFFF070000006174610000000000FFFFFFFF000001010100000078000000
          0000000010270000000100006489770600000000000000000000000000000100
          0000000000000000000100000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000010000000001000000000000
          0000000000006174610000000000FFFFFFFF0000010101000000780000000000
          00001027000000010000BC083404010000000000000000000000000001000000
          0000000000000001000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000100000000010000000000000000
          000000006174610000000000FFFFFFFF00000101010000007800000000000000
          1027000000010000DC3D26040200000000000000000000000000010000000000
          0000000000010000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000001000000000100000000000000000000
          00006174610000000000FFFFFFFF000001010100000078000000000000001027
          0000000100004022300D03000000000000000000000000000100000000000000
          0000000100000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000010000000001000000000000000000000000
          6174610000000000FFFFFFFF0000010101000000780000000000000010270000
          00010000FCCF2A04040000000000000000000000000001000000000000000000
          0001000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000100000000010000000000000000000000006174
          610000000000FFFFFFFF00000101010000007800000000000000102700000001
          00001C0E440D0500000000000000000000000000010000000000000000000001
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000001000000000100000000000000000000000061746100
          00000000FFFFFFFF000001010100000078000000000000001027000000010000
          28886A0006000000000000000000000000000100000000000000000000010000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000010000000001000000000000000000000000}
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
        ItemIndent = 14
        KeepSelectionWithinLevel = False
        LineBorderActiveColor = clBlack
        LineBorderInactiveColor = clBlack
        LineHeight = 15
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
        StripedOddColor = clGray
        StripedEvenColor = 8158332
        StripedItems = True
        TabOrder = 0
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
      ParentColor = True
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
        FlatAlwaysEdge = True
        OnClick = sbCancelClick
      end
    end
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 138
      Height = 197
      Align = alTop
      BevelOuter = bvNone
      ParentColor = True
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
        Height = 26
        AutoSize = False
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
        ParentColor = True
        TabOrder = 0
        object pbImage: TPaintBox
          Left = 1
          Top = 1
          Width = 130
          Height = 130
          Align = alClient
          OnPaint = pbImagePaint
        end
      end
    end
  end
  object Panel4: TPanel
    Left = 0
    Top = 0
    Width = 201
    Height = 425
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object tvItems: TElTree
      Left = 0
      Top = 0
      Width = 201
      Height = 409
      Cursor = crDefault
      LeftPosition = 0
      DragCursor = crDrag
      Align = alClient
      AutoCollapse = False
      AutoExpand = True
      DockOrientation = doNoOrient
      DefaultSectionWidth = 120
      BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
      CheckBoxGlyph.Data = {
        D6050000424DD60500000000000036000000280000003C000000080000000100
        180000000000A0050000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF535353
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF535353FFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000FFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFF535353535353535353FFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFF000000000000000000FFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFF535353535353535353FFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FF000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF53
        5353535353535353535353535353FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000
        000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5353535353
        53535353535353535353FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        535353FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000FFFFFFFFFF
        FF000000000000FFFFFFFFFFFFFFFFFF535353535353535353FFFFFFFFFFFF53
        5353535353FFFFFFFFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000
        000000FFFFFFFFFFFFFFFFFF535353535353535353FFFFFFFFFFFF5353535353
        53FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000FFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF535353535353535353FFFFFFFFFFFF
        FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFF
        FFFFFFFF535353535353FFFFFFFFFFFFFFFFFFFFFFFF535353535353FFFFFFFF
        FFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF
        535353535353FFFFFFFFFFFFFFFFFFFFFFFF535353535353FFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFF535353FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFF535353535353FFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFF535353535353FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFF535353535353FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        535353535353FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF535353FFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF535353}
      Color = clWhite
      CustomCheckboxes = True
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
        F4FFFFFF070000006174610000000000FFFFFFFF000001010100000078000000
        000000001027000000010000206F2C0400000000000000000000000000000100
        0000000000000000000100000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000010000000001000000000000
        0000000000006174610000000000FFFFFFFF0000010101000000780000000000
        0000102700000001000018452904010000000000000000000000000001000000
        0000000000000001000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000100000000010000000000000000
        000000006174610000000000FFFFFFFF00000101010000007800000000000000
        1027000000010000AC2976060200000000000000000000000000010000000000
        0000000000010000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000001000000000100000000000000000000
        00006174610000000000FFFFFFFF000001010100000078000000000000001027
        000000010000D8CF160C03000000000000000000000000000100000000000000
        0000000100000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000010000000001000000000000000000000000
        6174610000000000FFFFFFFF0000010101000000780000000000000010270000
        00010000F0A92304040000000000000000000000000001000000000000000000
        0001000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000100000000010000000000000000000000006174
        610000000000FFFFFFFF00000101010000007800000000000000102700000001
        0000106126040500000000000000000000000000010000000000000000000001
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000001000000000100000000000000000000000061746100
        00000000FFFFFFFF000001010100000078000000000000001027000000010000
        1065720006000000000000000000000000000100000000000000000000010000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000010000000001000000000000000000000000}
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
      SortUseCase = False
      StripedOddColor = clGray
      StripedEvenColor = 8158332
      StripedItems = True
      TabOrder = 0
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
    object paItemsCount: TPanel
      Left = 0
      Top = 409
      Width = 201
      Height = 16
      Align = alBottom
      Alignment = taLeftJustify
      BevelOuter = bvNone
      Caption = 'Items: 123'
      Color = 10528425
      TabOrder = 1
    end
  end
  object fsStorage: TFormStorage
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0
      end
      item
        Name = 'EmitterDirY'
        Value = 0
      end
      item
        Name = 'EmitterDirZ'
        Value = 0
      end>
    Top = 120
  end
end
