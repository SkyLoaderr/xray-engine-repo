object frmEditLightAnim: TfrmEditLightAnim
  Left = 469
  Top = 804
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsToolWindow
  Caption = 'Light Animation Library'
  ClientHeight = 154
  ClientWidth = 525
  Color = 10528425
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
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paItemProps: TPanel
    Left = 164
    Top = 0
    Width = 361
    Height = 154
    Align = alClient
    BevelOuter = bvNone
    Color = 10528425
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    Visible = False
    object Bevel1: TBevel
      Left = 230
      Top = 1
      Width = 128
      Height = 73
    end
    object pbG: TPaintBox
      Left = 3
      Top = 102
      Width = 357
      Height = 30
      Color = clBlack
      ParentColor = False
      OnMouseDown = pbGMouseDown
      OnMouseMove = pbGMouseMove
      OnMouseUp = pbGMouseUp
      OnPaint = pbGPaint
    end
    object ebDeleteKey: TExtBtn
      Left = 75
      Top = 135
      Width = 67
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Delete Key'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebDeleteKeyClick
    end
    object ebCreateKey: TExtBtn
      Left = 3
      Top = 135
      Width = 67
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Create Key'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCreateKeyClick
    end
    object ebPrevKey: TExtBtn
      Left = 121
      Top = 82
      Width = 21
      Height = 15
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '<'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebPrevKeyClick
    end
    object ebFirstKey: TExtBtn
      Left = 100
      Top = 82
      Width = 21
      Height = 15
      Hint = 'First Key'
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '|<<'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebFirstKeyClick
    end
    object ebNextKey: TExtBtn
      Left = 222
      Top = 82
      Width = 21
      Height = 15
      Hint = 'Next Key'
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '>'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebNextKeyClick
    end
    object ebLastKey: TExtBtn
      Left = 243
      Top = 82
      Width = 21
      Height = 15
      Hint = 'Last Key'
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '>>|'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebLastKeyClick
    end
    object ebMoveKeyLeft: TExtBtn
      Left = 76
      Top = 82
      Width = 21
      Height = 15
      Hint = 'Move Key Left'
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '<-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMoveKeyLeftClick
    end
    object ebMoveKeyRight: TExtBtn
      Left = 267
      Top = 82
      Width = 21
      Height = 15
      Hint = 'Move Key Right'
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '->'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMoveKeyRightClick
    end
    object ebFirstFrame: TExtBtn
      Left = 3
      Top = 82
      Width = 21
      Height = 15
      Hint = 'First Frame'
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '|<<'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebFirstFrameClick
    end
    object ebLastFrame: TExtBtn
      Left = 338
      Top = 82
      Width = 21
      Height = 15
      Hint = 'Last Frame'
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = '>>|'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebLastFrameClick
    end
    object paColor: TPanel
      Left = 232
      Top = 2
      Width = 125
      Height = 70
      BevelOuter = bvNone
      Color = 10526880
      TabOrder = 1
      object lbCurFrame: TMxLabel
        Left = 114
        Top = 1
        Width = 8
        Height = 13
        Alignment = taRightJustify
        Caption = '7'
        Color = clWhite
        Font.Charset = DEFAULT_CHARSET
        Font.Color = 15790320
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentColor = False
        ParentFont = False
        ShadowColor = 5460819
        ShadowPos = spRightBottom
        Transparent = True
      end
    end
    object sePointer: TMultiObjSpinEdit
      Left = 142
      Top = 80
      Width = 79
      Height = 17
      Hint = 'Current Frame'
      LWSensitivity = 0.01
      ButtonKind = bkDiagonal
      ButtonWidth = 16
      MaxValue = 100000
      AutoSize = False
      Color = 12582911
      Ctl3D = True
      ParentCtl3D = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnBottomClick = sePointerExit
      OnTopClick = sePointerExit
      OnChange = sePointerChange
      OnExit = sePointerExit
      OnKeyDown = sePointerKeyDown
    end
    object stStartFrame: TStaticText
      Left = 25
      Top = 82
      Width = 38
      Height = 15
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = '0'
      Color = 10526880
      ParentColor = False
      TabOrder = 2
    end
    object stEndFrame: TStaticText
      Left = 300
      Top = 82
      Width = 38
      Height = 15
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = '0'
      Color = 10526880
      ParentColor = False
      TabOrder = 3
    end
    object paProps: TPanel
      Left = 0
      Top = 1
      Width = 229
      Height = 73
      BevelOuter = bvLowered
      ParentColor = True
      TabOrder = 4
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 164
    Height = 154
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 1
    object tvItems: TElTree
      Left = 0
      Top = 0
      Width = 164
      Height = 116
      Cursor = crDefault
      LeftPosition = 0
      DragCursor = crDrag
      Align = alClient
      AlwaysKeepFocus = True
      AlwaysKeepSelection = False
      AutoCollapse = False
      AutoExpand = True
      DockOrientation = doNoOrient
      DefaultSectionWidth = 120
      BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
      CustomPlusMinus = True
      DragAllowed = True
      DrawFocusRect = False
      DragImageMode = dimOne
      DragTrgDrawMode = dtdDownColorLine
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
        F4FFFFFF0700000000806807001A0000FFFFFFFF0000010101002D2D78000000
        00000000102700000001002DC0CA53050000000000002D2D000000000000010D
        000000000000000000010000000000002072702E546F7009092D3D313B0D0A20
        2020202020202072702E426F74746F6D092B3D313B0D0A0920202020422D3E43
        616E7661732D3E42727573682D3E436F6C6F7209010000000001000000000000
        00000000000000806807001A0000FFFFFFFF0000010101002D2D780000000000
        0000102700000001002D702B49050100000000002D2D000000000000010D0000
        00000000000000010000000000002072702E546F7009092D3D313B0D0A202020
        202020202072702E426F74746F6D092B3D313B0D0A0920202020422D3E43616E
        7661732D3E42727573682D3E436F6C6F72090100000000010000000000000000
        0000000000806807001A0000FFFFFFFF0000010101002D2D7800000000000000
        102700000001002D504F150E0200000000002D2D000000000000010D00000000
        0000000000010000000000002072702E546F7009092D3D313B0D0A2020202020
        20202072702E426F74746F6D092B3D313B0D0A0920202020422D3E43616E7661
        732D3E42727573682D3E436F6C6F720901000000000100000000000000000000
        000000806807001A0000FFFFFFFF0000010101002D2D78000000000000001027
        00000001002D84DF650E0300000000002D2D000000000000010D000000000000
        000000010000000000002072702E546F7009092D3D313B0D0A20202020202020
        2072702E426F74746F6D092B3D313B0D0A0920202020422D3E43616E7661732D
        3E42727573682D3E436F6C6F7209010000000001000000000000000000000000
        00806807001A0000FFFFFFFF0000010101002D2D780000000000000010270000
        0001002D709814040400000000002D2D000000000000010D0000000000000000
        00010000000000002072702E546F7009092D3D313B0D0A202020202020202072
        702E426F74746F6D092B3D313B0D0A0920202020422D3E43616E7661732D3E42
        727573682D3E436F6C6F72090100000000010000000000000000000000000080
        6807001A0000FFFFFFFF0000010101002D2D7800000000000000102700000001
        002D0427B6190500000000002D2D000000000000010D00000000000000000001
        0000000000002072702E546F7009092D3D313B0D0A202020202020202072702E
        426F74746F6D092B3D313B0D0A0920202020422D3E43616E7661732D3E427275
        73682D3E436F6C6F720901000000000100000000000000000000000000806807
        001A0000FFFFFFFF0000010101002D2D7800000000000000102700000001002D
        54C545040600000000002D2D000000000000010D000000000000000000010000
        000000002072702E546F7009092D3D313B0D0A202020202020202072702E426F
        74746F6D092B3D313B0D0A0920202020422D3E43616E7661732D3E4272757368
        2D3E436F6C6F7209010000000001000000000000000000000000}
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
      MultiSelect = False
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
      OnItemFocused = tvItemsItemFocused
      OnDragDrop = tvItemsDragDrop
      OnDragOver = tvItemsDragOver
      OnStartDrag = tvItemsStartDrag
      OnMouseDown = tvItemsMouseDown
      OnKeyPress = tvItemsKeyPress
    end
    object Panel2: TPanel
      Left = 0
      Top = 116
      Width = 164
      Height = 38
      Align = alBottom
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 1
      object ebAddAnim: TExtBtn
        Left = 1
        Top = 1
        Width = 81
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Add'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAddAnimClick
      end
      object ebDeleteAnim: TExtBtn
        Left = 83
        Top = 1
        Width = 81
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Delete'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebDeleteAnimClick
      end
      object ebSave: TExtBtn
        Left = 1
        Top = 19
        Width = 81
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Save'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebSaveClick
      end
      object ebReload: TExtBtn
        Left = 83
        Top = 19
        Width = 81
        Height = 17
        Align = alNone
        BevelShow = False
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Reload'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebReloadClick
      end
    end
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
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
  object pmActionList: TMxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    MarginStartColor = 13158600
    MarginEndColor = 1644825
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Top = 30
    object N2: TMenuItem
      Caption = '-'
    end
    object Rename1: TMenuItem
      Caption = 'Rename'
      OnClick = Rename1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object CreateFolder1: TMenuItem
      Caption = 'Create Folder'
      OnClick = CreateFolder1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object ExpandAll1: TMenuItem
      Caption = 'Expand All'
      OnClick = ExpandAll1Click
    end
    object CollapseAll1: TMenuItem
      Caption = 'Collapse All'
      OnClick = CollapseAll1Click
    end
  end
  object InplaceTextEdit: TElTreeInplaceAdvancedEdit
    Types = [sftText]
    OnValidateResult = InplaceTextEditValidateResult
    Left = 30
    Top = 30
  end
end
