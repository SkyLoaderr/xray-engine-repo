object frmDOShuffle: TfrmDOShuffle
  Left = 856
  Top = 304
  Width = 325
  Height = 454
  BorderIcons = [biSystemMenu]
  Caption = 'Detail objects'
  Color = 10528425
  Constraints.MaxWidth = 325
  Constraints.MinHeight = 454
  Constraints.MinWidth = 325
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paTools: TPanel
    Left = 158
    Top = 0
    Width = 159
    Height = 427
    Align = alRight
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 159
      Height = 19
      Align = alTop
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
      object ebAppendIndex: TExtBtn
        Left = 28
        Top = 0
        Width = 130
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Append Color Index'
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
        FlatAlwaysEdge = True
        OnClick = ebMultiClearClick
      end
    end
    object sbDO: TScrollBox
      Left = 0
      Top = 19
      Width = 159
      Height = 408
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
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 158
    Height = 427
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object Splitter1: TSplitter
      Left = 0
      Top = 241
      Width = 158
      Height = 3
      Cursor = crVSplit
      Align = alTop
      Beveled = True
      ResizeStyle = rsUpdate
    end
    object tvItems: TElTree
      Left = 0
      Top = 244
      Width = 158
      Height = 164
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
        F4FFFFFF07000000BC60610C00000000FFFFFFFF000001010100104078000000
        000000001027000000010040401C7A030000000000000D400000000000000140
        000000000000000000010000000000001B250D4041250D4063250D4089250D40
        AB250D401C61610C1C61610C680000004255494C447E325C6C69625C00007236
        3861610C3861610C180000006D656D6D67722E6C010000000001000000000000
        000000000000BC60610C00000000FFFFFFFF0000010101001040780000000000
        00001027000000010040F44106100100000000000D4000000000000001400000
        00000000000000010000000000001B250D4041250D4063250D4089250D40AB25
        0D401C61610C1C61610C680000004255494C447E325C6C69625C000072363861
        610C3861610C180000006D656D6D67722E6C0100000000010000000000000000
        00000000BC60610C00000000FFFFFFFF00000101010010407800000000000000
        1027000000010040A86C620E0200000000000D40000000000000014000000000
        0000000000010000000000001B250D4041250D4063250D4089250D40AB250D40
        1C61610C1C61610C680000004255494C447E325C6C69625C000072363861610C
        3861610C180000006D656D6D67722E6C01000000000100000000000000000000
        0000BC60610C00000000FFFFFFFF000001010100104078000000000000001027
        0000000100400051320F0300000000000D400000000000000140000000000000
        000000010000000000001B250D4041250D4063250D4089250D40AB250D401C61
        610C1C61610C680000004255494C447E325C6C69625C000072363861610C3861
        610C180000006D656D6D67722E6C010000000001000000000000000000000000
        BC60610C00000000FFFFFFFF0000010101001040780000000000000010270000
        000100407802F7020400000000000D4000000000000001400000000000000000
        00010000000000001B250D4041250D4063250D4089250D40AB250D401C61610C
        1C61610C680000004255494C447E325C6C69625C000072363861610C3861610C
        180000006D656D6D67722E6C010000000001000000000000000000000000BC60
        610C00000000FFFFFFFF00000101010010407800000000000000102700000001
        0040B4E1DA040500000000000D40000000000000014000000000000000000001
        0000000000001B250D4041250D4063250D4089250D40AB250D401C61610C1C61
        610C680000004255494C447E325C6C69625C000072363861610C3861610C1800
        00006D656D6D67722E6C010000000001000000000000000000000000BC60610C
        00000000FFFFFFFF000001010100104078000000000000001027000000010040
        288C870D0600000000000D400000000000000140000000000000000000010000
        000000001B250D4041250D4063250D4089250D40AB250D401C61610C1C61610C
        680000004255494C447E325C6C69625C000072363861610C3861610C18000000
        6D656D6D67722E6C010000000001000000000000000000000000}
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
      OnKeyPress = tvItemsKeyPress
    end
    object paObject: TPanel
      Left = 0
      Top = 0
      Width = 158
      Height = 241
      Align = alTop
      BevelOuter = bvNone
      Constraints.MinHeight = 240
      ParentColor = True
      TabOrder = 1
      object paImage: TPanel
        Left = 0
        Top = 0
        Width = 158
        Height = 158
        Align = alTop
        BevelOuter = bvLowered
        Caption = '<no image>'
        Color = clGray
        TabOrder = 0
        object pbImage: TPaintBox
          Left = 1
          Top = 1
          Width = 156
          Height = 156
          Align = alClient
          Color = clGray
          ParentColor = False
          OnPaint = pbImagePaint
        end
      end
      object paObjectProps: TPanel
        Left = 0
        Top = 158
        Width = 158
        Height = 83
        Align = alClient
        BevelOuter = bvNone
        ParentColor = True
        TabOrder = 1
      end
    end
    object Panel1: TPanel
      Left = 0
      Top = 408
      Width = 158
      Height = 19
      Align = alBottom
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 2
      object ebAddObject: TExtBtn
        Left = 1
        Top = 1
        Width = 26
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          AE000000424DAE00000000000000360000002800000006000000060000000100
          18000000000078000000120B0000120B00000000000000000000FFFFFFFFFFFF
          1C1C1C1C1C1CFFFFFFFFFFFF0000FFFFFFFFFFFF191919191919FFFFFFFFFFFF
          00001B1B1B1717170303030303031717171B1B1B00001B1B1B17171703030303
          03031717171B1B1B0000FFFFFFFFFFFF191919191919FFFFFFFFFFFF0000FFFF
          FFFFFFFF1C1C1C1C1C1CFFFFFFFFFFFF0000}
        FlatAlwaysEdge = True
        OnClick = ebAddObjectClick
      end
      object ebDelObject: TExtBtn
        Left = 27
        Top = 1
        Width = 26
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          AE000000424DAE00000000000000360000002800000006000000060000000100
          18000000000078000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          00000B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B00000B0B0B0B0B0B0B0B0B0B
          0B0B0B0B0B0B0B0B0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
        FlatAlwaysEdge = True
        OnClick = ebDelObjectClick
      end
      object ebLoadList: TExtBtn
        Left = 79
        Top = 1
        Width = 39
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Load...'
        FlatAlwaysEdge = True
        OnClick = ebLoadListClick
      end
      object ebSaveList: TExtBtn
        Left = 118
        Top = 1
        Width = 39
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Save...'
        FlatAlwaysEdge = True
        OnClick = ebSaveListClick
      end
      object ebClearList: TExtBtn
        Left = 53
        Top = 1
        Width = 26
        Height = 17
        Align = alNone
        BevelShow = False
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
        FlatAlwaysEdge = True
        OnClick = ebClearListClick
      end
    end
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paObject.Height')
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
  end
end
