object frmDOShuffle: TfrmDOShuffle
  Left = 548
  Top = 171
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
        F4FFFFFF07000000C0B2370E00000000FFFFFFFF000001010100000078000000
        000000001027000000010000401C7A030000000000005B000000000000000100
        00000000000000000001000000000000D5E35B00010000000000000003000000
        0000000024EC5B0034EC5B004CEC5B005CEC5B006CEC5B0098EC5B008905B200
        9905B200B105B2009872040744B3370E90000000010000000001000000000000
        000000000000C0B2370E00000000FFFFFFFF0000010101000000780000000000
        00001027000000010000F44106100100000000005B0000000000000001000000
        0000000000000001000000000000D5E35B000100000000000000030000000000
        000024EC5B0034EC5B004CEC5B005CEC5B006CEC5B0098EC5B008905B2009905
        B200B105B2009872040744B3370E900000000100000000010000000000000000
        00000000C0B2370E00000000FFFFFFFF00000101010000007800000000000000
        1027000000010000A86C620E0200000000005B00000000000000010000000000
        000000000001000000000000D5E35B0001000000000000000300000000000000
        24EC5B0034EC5B004CEC5B005CEC5B006CEC5B0098EC5B008905B2009905B200
        B105B2009872040744B3370E9000000001000000000100000000000000000000
        0000C0B2370E00000000FFFFFFFF000001010100000078000000000000001027
        0000000100000051320F0300000000005B000000000000000100000000000000
        00000001000000000000D5E35B000100000000000000030000000000000024EC
        5B0034EC5B004CEC5B005CEC5B006CEC5B0098EC5B008905B2009905B200B105
        B2009872040744B3370E90000000010000000001000000000000000000000000
        C0B2370E00000000FFFFFFFF0000010101000000780000000000000010270000
        000100007802F7020400000000005B0000000000000001000000000000000000
        0001000000000000D5E35B000100000000000000030000000000000024EC5B00
        34EC5B004CEC5B005CEC5B006CEC5B0098EC5B008905B2009905B200B105B200
        9872040744B3370E90000000010000000001000000000000000000000000C0B2
        370E00000000FFFFFFFF00000101010000007800000000000000102700000001
        0000B4E1DA040500000000005B00000000000000010000000000000000000001
        000000000000D5E35B000100000000000000030000000000000024EC5B0034EC
        5B004CEC5B005CEC5B006CEC5B0098EC5B008905B2009905B200B105B2009872
        040744B3370E90000000010000000001000000000000000000000000C0B2370E
        00000000FFFFFFFF000001010100000078000000000000001027000000010000
        288C870D0600000000005B000000000000000100000000000000000000010000
        00000000D5E35B000100000000000000030000000000000024EC5B0034EC5B00
        4CEC5B005CEC5B006CEC5B0098EC5B008905B2009905B200B105B20098720407
        44B3370E90000000010000000001000000000000000000000000}
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
