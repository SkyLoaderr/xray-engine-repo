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
      Cursor = crDefault
      LeftPosition = 0
      DoubleBuffered = False
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
      ExplorerEditMode = False
      Flat = True
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
        F5FFFFFF07000000E4EC6707002A0000FFFFFFFF000001010100000078000000
        000000001027000000010041401C7A030000000000000000000000000000016D
        00000000000000000001000000000000206E6F6465203D20416464466F6C6465
        7228666F6C646572293B0D0A0972657475726E204164644974656D286E6F6465
        2C6E616D652C6F626A293B0D0A7D0D0A2F2F2D2D010000000001000000000000
        000000000000E4EC6707002A0000FFFFFFFF0000010101000000780000000000
        00001027000000010041F44106100100000000000000000000000000016D0000
        0000000000000001000000000000206E6F6465203D20416464466F6C64657228
        666F6C646572293B0D0A0972657475726E204164644974656D286E6F64652C6E
        616D652C6F626A293B0D0A7D0D0A2F2F2D2D0100000000010000000000000000
        00000000E4EC6707002A0000FFFFFFFF00000101010000007800000000000000
        1027000000010041A86C620E0200000000000000000000000000016D00000000
        000000000001000000000000206E6F6465203D20416464466F6C64657228666F
        6C646572293B0D0A0972657475726E204164644974656D286E6F64652C6E616D
        652C6F626A293B0D0A7D0D0A2F2F2D2D01000000000100000000000000000000
        0000E4EC6707002A0000FFFFFFFF000001010100000078000000000000001027
        0000000100410051320F0300000000000000000000000000016D000000000000
        00000001000000000000206E6F6465203D20416464466F6C64657228666F6C64
        6572293B0D0A0972657475726E204164644974656D286E6F64652C6E616D652C
        6F626A293B0D0A7D0D0A2F2F2D2D010000000001000000000000000000000000
        E4EC6707002A0000FFFFFFFF0000010101000000780000000000000010270000
        000100417802F7020400000000000000000000000000016D0000000000000000
        0001000000000000206E6F6465203D20416464466F6C64657228666F6C646572
        293B0D0A0972657475726E204164644974656D286E6F64652C6E616D652C6F62
        6A293B0D0A7D0D0A2F2F2D2D010000000001000000000000000000000000E4EC
        6707002A0000FFFFFFFF00000101010000007800000000000000102700000001
        0041B4E1DA040500000000000000000000000000016D00000000000000000001
        000000000000206E6F6465203D20416464466F6C64657228666F6C646572293B
        0D0A0972657475726E204164644974656D286E6F64652C6E616D652C6F626A29
        3B0D0A7D0D0A2F2F2D2D010000000001000000000000000000000000E4EC6707
        002A0000FFFFFFFF000001010100000078000000000000001027000000010041
        288C870D0600000000000000000000000000016D000000000000000000010000
        00000000206E6F6465203D20416464466F6C64657228666F6C646572293B0D0A
        0972657475726E204164644974656D286E6F64652C6E616D652C6F626A293B0D
        0A7D0D0A2F2F2D2D010000000001000000000000000000000000}
      HeaderFont.Charset = DEFAULT_CHARSET
      HeaderFont.Color = clWindowText
      HeaderFont.Height = -11
      HeaderFont.Name = 'MS Sans Serif'
      HeaderFont.Style = []
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
      LinesColor = clBlack
      MouseFrameSelect = True
      MultiSelect = False
      OwnerDrawMask = '~~@~~'
      PlusMinusTransparent = True
      ScrollbarOpposite = False
      ScrollTracking = True
      ShowButtons = False
      ShowImages = False
      ShowLeafButton = False
      ShowRoot = True
      SortMode = smAdd
      StoragePath = '\Tree'
      TabOrder = 0
      TabStop = True
      VertScrollBarStyles.ShowTrackHint = True
      VertScrollBarStyles.Width = 16
      VertScrollBarStyles.ButtonSize = 14
      VertScrollBarStyles.UseSystemMetrics = False
      VirtualityLevel = vlNone
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
      object lbItemName: TMxLabel
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
      object RxLabel1: TMxLabel
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
      object RxLabel3: TMxLabel
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
      object lbInfo: TMxLabel
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
