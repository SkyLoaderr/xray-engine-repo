object frmDOShuffle: TfrmDOShuffle
  Left = 664
  Top = 390
  Width = 320
  Height = 454
  BorderStyle = bsSizeToolWin
  Caption = 'Detail objects'
  Color = clBtnFace
  Constraints.MaxWidth = 320
  Constraints.MinHeight = 454
  Constraints.MinWidth = 320
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
  object paTools: TPanel
    Left = 158
    Top = 0
    Width = 154
    Height = 390
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 154
      Height = 19
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
      object ebAppendIndex: TExtBtn
        Left = 28
        Top = 0
        Width = 125
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Append Color Index'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAppendIndexClick
      end
      object ebMultiClear: TExtBtn
        Left = 1
        Top = 0
        Width = 26
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
        FlatAlwaysEdge = True
        OnClick = ebMultiClearClick
      end
    end
    object sbDO: TScrollBox
      Left = 0
      Top = 19
      Width = 154
      Height = 371
      VertScrollBar.ButtonSize = 11
      VertScrollBar.Increment = 10
      VertScrollBar.Size = 11
      VertScrollBar.Style = ssHotTrack
      VertScrollBar.Tracking = True
      Align = alClient
      BorderStyle = bsNone
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 390
    Width = 312
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebAddObject: TExtBtn
      Left = 1
      Top = 18
      Width = 78
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Add Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebAddObjectClick
    end
    object ebDelObject: TExtBtn
      Left = 79
      Top = 18
      Width = 78
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Del Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebDelObjectClick
    end
    object ebOk: TExtBtn
      Left = 158
      Top = 18
      Width = 77
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 235
      Top = 18
      Width = 77
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
    object ExtBtn2: TExtBtn
      Left = 158
      Top = 0
      Width = 77
      Height = 18
      Align = alNone
      CloseButton = False
      Caption = 'Load'
      Enabled = False
      Spacing = 0
      Transparent = False
      FlatAlwaysEdge = True
    end
    object ExtBtn1: TExtBtn
      Left = 235
      Top = 0
      Width = 77
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
    end
    object ebDOProperties: TExtBtn
      Left = 1
      Top = 0
      Width = 156
      Height = 18
      Align = alNone
      CloseButton = False
      Caption = 'Object Properties'
      Spacing = 0
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebDOPropertiesClick
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 158
    Height = 390
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object tvItems: TElTree
      Left = 0
      Top = 176
      Width = 158
      Height = 214
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
      Color = clGray
      DragAllowed = True
      DrawFocusRect = False
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
        F4FFFFFF0700000054860D0500240000FFFFFFFF000001010100000078000000
        000000001027000000010041401C7A030000000000000000000000000000016D
        000000000000000000010000000000006A656374202A536F757263652C20696E
        7420582C20696E7420592C20544472616753746174652053746174652C20626F
        6F6C2026416363657074290D0A7B0D0A20202020010000000001000000000000
        00000000000054860D0500240000FFFFFFFF0000010101000000780000000000
        00001027000000010041F44106100100000000000000000000000000016D0000
        00000000000000010000000000006A656374202A536F757263652C20696E7420
        582C20696E7420592C20544472616753746174652053746174652C20626F6F6C
        2026416363657074290D0A7B0D0A202020200100000000010000000000000000
        0000000054860D0500240000FFFFFFFF00000101010000007800000000000000
        1027000000010041A86C620E0200000000000000000000000000016D00000000
        0000000000010000000000006A656374202A536F757263652C20696E7420582C
        20696E7420592C20544472616753746174652053746174652C20626F6F6C2026
        416363657074290D0A7B0D0A2020202001000000000100000000000000000000
        000054860D0500240000FFFFFFFF000001010100000078000000000000001027
        0000000100410051320F0300000000000000000000000000016D000000000000
        000000010000000000006A656374202A536F757263652C20696E7420582C2069
        6E7420592C20544472616753746174652053746174652C20626F6F6C20264163
        63657074290D0A7B0D0A20202020010000000001000000000000000000000000
        54860D0500240000FFFFFFFF0000010101000000780000000000000010270000
        000100417802F7020400000000000000000000000000016D0000000000000000
        00010000000000006A656374202A536F757263652C20696E7420582C20696E74
        20592C20544472616753746174652053746174652C20626F6F6C202641636365
        7074290D0A7B0D0A202020200100000000010000000000000000000000005486
        0D0500240000FFFFFFFF00000101010000007800000000000000102700000001
        0041B4E1DA040500000000000000000000000000016D00000000000000000001
        0000000000006A656374202A536F757263652C20696E7420582C20696E742059
        2C20544472616753746174652053746174652C20626F6F6C2026416363657074
        290D0A7B0D0A2020202001000000000100000000000000000000000054860D05
        00240000FFFFFFFF000001010100000078000000000000001027000000010041
        288C870D0600000000000000000000000000016D000000000000000000010000
        000000006A656374202A536F757263652C20696E7420582C20696E7420592C20
        544472616753746174652053746174652C20626F6F6C2026416363657074290D
        0A7B0D0A20202020010000000001000000000000000000000000}
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
      LinesColor = clBlack
      MouseFrameSelect = True
      MultiSelect = False
      OwnerDrawMask = '~~@~~'
      ParentFont = False
      PlusMinusTransparent = True
      ScrollbarOpposite = False
      ScrollTracking = True
      ShowButtons = False
      ShowImages = False
      ShowLeafButton = False
      ShowLines = False
      ShowRootButtons = False
      SortMode = smAdd
      StoragePath = '\Tree'
      TabOrder = 0
      TabStop = True
      Tracking = False
      TrackColor = clBlack
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
      OnDragDrop = tvItemsDragDrop
      OnDragOver = tvItemsDragOver
      OnStartDrag = tvItemsStartDrag
      OnDblClick = tvItemsDblClick
      OnKeyPress = tvItemsKeyPress
    end
    object Panel4: TPanel
      Left = 0
      Top = 0
      Width = 158
      Height = 176
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
      object lbItemName: TLabel
        Left = 38
        Top = 130
        Width = 120
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
      object RxLabel1: TLabel
        Left = 2
        Top = 130
        Width = 33
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
      object RxLabel3: TLabel
        Left = 2
        Top = 145
        Width = 33
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
        Left = 38
        Top = 145
        Width = 120
        Height = 29
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object paImage: TPanel
        Left = 0
        Top = 0
        Width = 158
        Height = 130
        Align = alTop
        BevelOuter = bvLowered
        Caption = '<no image>'
        Color = clGray
        TabOrder = 0
        object pbImage: TPaintBox
          Left = 1
          Top = 1
          Width = 156
          Height = 128
          Align = alClient
          Color = clGray
          ParentColor = False
          OnPaint = pbImagePaint
        end
      end
    end
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
  end
end
