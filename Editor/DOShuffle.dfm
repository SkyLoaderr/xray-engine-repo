object frmDOShuffle: TfrmDOShuffle
  Left = 664
  Top = 389
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
      Top = 178
      Width = 158
      Height = 212
      LeftPosition = 0
      ActiveBorderType = fbtSunken
      Align = alClient
      AlwaysKeepFocus = True
      AlwaysKeepSelection = True
      AutoExpand = True
      AutoLookup = False
      DockOrientation = doNoOrient
      DoubleBuffered = False
      AutoLineHeight = True
      BarStyle = False
      BarStyleVerticalLines = False
      BorderStyle = bsSingle
      ChangeStateImage = False
      Color = clGray
      CustomCheckboxes = False
      CustomPlusMinus = False
      DeselectChildrenOnCollapse = False
      DivLinesColor = clBtnFace
      DoInplaceEdit = True
      DraggableSections = False
      DragAllowed = True
      DragTrgDrawMode = SelColorRect
      DragType = dtDelphi
      DragImageMode = dimNever
      DrawFocusRect = True
      ExpandOnDblClick = True
      ExpandOnDragOver = False
      FilteredVisibility = False
      Flat = True
      FlatFocusedScrollbars = True
      FocusedSelectColor = clHighlight
      ForcedScrollBars = ssNone
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FullRowSelect = False
      HeaderActiveFilterColor = clBlack
      HeaderHeight = 22
      HeaderHotTrack = False
      HeaderInvertSortArrows = False
      HeaderSections.Data = {
        F8FFFFFF0700000074EC080500000000FFFFFFFF000001010100206678000000
        0000000010270000000100650000000000007420000000000000017200000000
        000000000001000001000000000100000000000000000000000074EC08050000
        0000FFFFFFFF0000010101002066780000000000000010270000000100650100
        0000000074200000000000000172000000000000000000010000010000000001
        00000000000000000000000074EC080500000000FFFFFFFF0000010101002066
        7800000000000000102700000001006502000000000074200000000000000172
        00000000000000000001000001000000000100000000000000000000000074EC
        080500000000FFFFFFFF00000101010020667800000000000000102700000001
        0065030000000000742000000000000001720000000000000000000100000100
        0000000100000000000000000000000074EC080500000000FFFFFFFF00000101
        0100206678000000000000001027000000010065040000000000742000000000
        0000017200000000000000000001000001000000000100000000000000000000
        000074EC080500000000FFFFFFFF000001010100206678000000000000001027
        0000000100650500000000007420000000000000017200000000000000000001
        000001000000000100000000000000000000000074EC080500000000FFFFFFFF
        0000010101002066780000000000000010270000000100650600000000007420
        0000000000000172000000000000000000010000010000000001000000000000
        000000000000}
      HeaderFilterColor = clBtnText
      HeaderFlat = False
      HideFocusRect = False
      HideHintOnTimer = False
      HideHintOnMove = True
      HideSelectColor = clBtnFace
      HideSelection = False
      HorizontalLines = False
      HideHorzScrollBar = False
      HideVertScrollBar = False
      HorzScrollBarStyles.ActiveFlat = False
      HorzScrollBarStyles.BlendBackground = True
      HorzScrollBarStyles.Color = clBtnFace
      HorzScrollBarStyles.Flat = True
      HorzScrollBarStyles.MinThumbSize = 4
      HorzScrollBarStyles.NoDisableButtons = False
      HorzScrollBarStyles.NoSunkenThumb = False
      HorzScrollBarStyles.OwnerDraw = False
      HorzScrollBarStyles.SecondaryButtons = False
      HorzScrollBarStyles.SecondBtnKind = sbkOpposite
      HorzScrollBarStyles.ShowLeftArrows = True
      HorzScrollBarStyles.ShowRightArrows = True
      HorzScrollBarStyles.ShowTrackHint = False
      HorzScrollBarStyles.ThumbMode = etmAuto
      HorzScrollBarStyles.ThumbSize = 0
      HorzScrollBarStyles.Width = 14
      HorzScrollBarStyles.ButtonSize = 14
      HorzScrollBarStyles.UseSystemMetrics = False
      InactiveBorderType = fbtSunkenOuter
      ItemIndent = 17
      LineHeight = 19
      LinesColor = clBlack
      LinesStyle = psDot
      LineHintColor = clWindow
      LockHeaderHeight = False
      MainTreeColumn = 0
      MoveColumnOnDrag = False
      MoveFocusOnCollapse = False
      MultiSelect = False
      OwnerDrawMask = '~~@~~'
      PathSeparator = '\'
      PlusMinusTransparent = True
      RightAlignedText = False
      RightAlignedTree = False
      RowHotTrack = False
      RowSelect = True
      ScrollTracking = True
      SelectColumn = -1
      ShowButtons = False
      ShowColumns = False
      ShowCheckboxes = False
      ShowImages = False
      ShowLines = True
      ShowRoot = True
      ShowRootButtons = True
      SelectionMode = smUsual
      SortDir = sdAscend
      SortMode = smAdd
      SortSection = 0
      SortType = stText
      StickyHeaderSections = False
      TabOrder = 0
      TabStop = True
      Tracking = True
      TrackColor = clHighlight
      UnderlineTracked = True
      VerticalLines = False
      VertScrollBarStyles.ActiveFlat = False
      VertScrollBarStyles.BlendBackground = True
      VertScrollBarStyles.Color = clBtnFace
      VertScrollBarStyles.Flat = True
      VertScrollBarStyles.MinThumbSize = 4
      VertScrollBarStyles.NoDisableButtons = False
      VertScrollBarStyles.NoSunkenThumb = False
      VertScrollBarStyles.OwnerDraw = False
      VertScrollBarStyles.SecondaryButtons = False
      VertScrollBarStyles.SecondBtnKind = sbkOpposite
      VertScrollBarStyles.ShowLeftArrows = True
      VertScrollBarStyles.ShowRightArrows = True
      VertScrollBarStyles.ShowTrackHint = True
      VertScrollBarStyles.ThumbMode = etmAuto
      VertScrollBarStyles.ThumbSize = 0
      VertScrollBarStyles.Width = 14
      VertScrollBarStyles.ButtonSize = 14
      VertScrollBarStyles.UseSystemMetrics = False
      TextColor = clBtnText
      BkColor = clBtnShadow
      OnItemSelectedChange = tvItemsItemSelectedChange
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
      Height = 178
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
      object lbItemName: TRxLabel
        Left = 38
        Top = 133
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
        ShadowColor = 15263976
      end
      object RxLabel1: TRxLabel
        Left = 2
        Top = 133
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
        ShadowColor = 15263976
      end
      object RxLabel3: TRxLabel
        Left = 2
        Top = 148
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
        ShadowColor = 15263976
      end
      object lbInfo: TRxLabel
        Left = 38
        Top = 148
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
        ShadowColor = 15263976
        WordWrap = True
      end
      object paImage: TPanel
        Left = 1
        Top = 0
        Width = 156
        Height = 130
        BevelOuter = bvLowered
        Caption = '<no image>'
        Color = clGray
        TabOrder = 0
        object pbImage: TPaintBox
          Left = 1
          Top = 1
          Width = 154
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
