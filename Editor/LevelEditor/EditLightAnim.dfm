object frmEditLightAnim: TfrmEditLightAnim
  Left = 245
  Top = 499
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Light Animation Library'
  ClientHeight = 151
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
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paItemProps: TPanel
    Left = 164
    Top = 0
    Width = 361
    Height = 151
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
      Left = 231
      Top = 7
      Width = 128
      Height = 62
    end
    object Label1: TLabel
      Left = 3
      Top = 8
      Width = 31
      Height = 13
      Caption = 'Name:'
    end
    object Label2: TLabel
      Left = 3
      Top = 30
      Width = 91
      Height = 13
      Caption = 'Frame Per Second:'
    end
    object Label3: TLabel
      Left = 3
      Top = 52
      Width = 62
      Height = 13
      Caption = 'Frame count:'
    end
    object pbG: TPaintBox
      Left = 3
      Top = 102
      Width = 357
      Height = 25
      Color = clBlack
      ParentColor = False
      OnMouseDown = pbGMouseDown
      OnMouseMove = pbGMouseMove
      OnMouseUp = pbGMouseUp
      OnPaint = pbGPaint
    end
    object ebDeleteKey: TExtBtn
      Left = 75
      Top = 132
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
      Top = 132
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
      Top = 84
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebPrevKeyClick
    end
    object ebFirstKey: TExtBtn
      Left = 100
      Top = 84
      Width = 21
      Height = 15
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebFirstKeyClick
    end
    object ebNextKey: TExtBtn
      Left = 222
      Top = 84
      Width = 21
      Height = 15
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebNextKeyClick
    end
    object ebLastKey: TExtBtn
      Left = 243
      Top = 84
      Width = 21
      Height = 15
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebLastKeyClick
    end
    object ebMoveKeyLeft: TExtBtn
      Left = 76
      Top = 84
      Width = 21
      Height = 15
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMoveKeyLeftClick
    end
    object ebMoveKeyRight: TExtBtn
      Left = 267
      Top = 84
      Width = 21
      Height = 15
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMoveKeyRightClick
    end
    object ebFirstFrame: TExtBtn
      Left = 3
      Top = 84
      Width = 21
      Height = 15
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebFirstFrameClick
    end
    object ebLastFrame: TExtBtn
      Left = 338
      Top = 84
      Width = 21
      Height = 15
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
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebLastFrameClick
    end
    object paColor: TPanel
      Left = 233
      Top = 8
      Width = 125
      Height = 59
      BevelOuter = bvNone
      Color = 10526880
      TabOrder = 4
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
    object seFPS: TMultiObjSpinEdit
      Left = 96
      Top = 28
      Width = 132
      Height = 19
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 1
      ButtonWidth = 16
      MaxValue = 1000
      MinValue = 0.1
      ValueType = vtFloat
      Value = 30
      AutoSize = False
      Color = 10526880
      Ctl3D = True
      ParentCtl3D = False
      TabOrder = 1
      OnLWChange = seFPSLWChange
      OnExit = seFPSExit
      OnKeyDown = seFPSKeyDown
    end
    object edName: TEdit
      Left = 96
      Top = 7
      Width = 132
      Height = 19
      AutoSize = False
      Color = 10526880
      Enabled = False
      TabOrder = 0
    end
    object seFrameCount: TMultiObjSpinEdit
      Left = 96
      Top = 49
      Width = 132
      Height = 19
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      ButtonWidth = 16
      MaxValue = 100000
      MinValue = 1
      Value = 1
      AutoSize = False
      Color = 10526880
      Ctl3D = True
      ParentCtl3D = False
      TabOrder = 2
      OnLWChange = seFrameCountLWChange
      OnExit = seFrameCountExit
      OnKeyDown = seFrameCountKeyDown
    end
    object sePointer: TMultiObjSpinEdit
      Left = 142
      Top = 82
      Width = 79
      Height = 17
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      ButtonWidth = 16
      MaxValue = 100000
      AutoSize = False
      Color = 12582911
      Ctl3D = True
      ParentCtl3D = False
      TabOrder = 3
      OnLWChange = sePointerLWChange
      OnChange = sePointerChange
      OnExit = sePointerExit
      OnKeyDown = sePointerKeyDown
    end
    object stStartFrame: TStaticText
      Left = 25
      Top = 84
      Width = 38
      Height = 15
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = '0'
      Color = 10526880
      ParentColor = False
      TabOrder = 5
    end
    object stEndFrame: TStaticText
      Left = 300
      Top = 84
      Width = 38
      Height = 15
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = '0'
      Color = 10526880
      ParentColor = False
      TabOrder = 6
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 164
    Height = 151
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 1
    object tvItems: TElTree
      Left = 0
      Top = 0
      Width = 164
      Height = 113
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
        F4FFFFFF07000000789F640700000000FFFFFFFF000001010100697478000000
        000000001027000000010014C0CA53050000000000005C4C0000000000000174
        0000000000000000000100000000000070E76407E4E20F1444000000523A5C44
        697265637458382E315C696E636C7564655C5C503A5C4C6576656C456469746F
        725C656469745C5363656E652E64666D90000000010000000001000000000000
        000000000000789F640700000000FFFFFFFF0000010101006974780000000000
        00001027000000010014702B49050100000000005C4C00000000000001740000
        000000000000000100000000000070E76407E4E20F1444000000523A5C446972
        65637458382E315C696E636C7564655C5C503A5C4C6576656C456469746F725C
        656469745C5363656E652E64666D900000000100000000010000000000000000
        00000000789F640700000000FFFFFFFF00000101010069747800000000000000
        1027000000010014504F150E0200000000005C4C000000000000017400000000
        00000000000100000000000070E76407E4E20F1444000000523A5C4469726563
        7458382E315C696E636C7564655C5C503A5C4C6576656C456469746F725C6564
        69745C5363656E652E64666D9000000001000000000100000000000000000000
        0000789F640700000000FFFFFFFF000001010100697478000000000000001027
        00000001001484DF650E0300000000005C4C0000000000000174000000000000
        0000000100000000000070E76407E4E20F1444000000523A5C44697265637458
        382E315C696E636C7564655C5C503A5C4C6576656C456469746F725C65646974
        5C5363656E652E64666D90000000010000000001000000000000000000000000
        789F640700000000FFFFFFFF0000010101006974780000000000000010270000
        00010014709814040400000000005C4C00000000000001740000000000000000
        000100000000000070E76407E4E20F1444000000523A5C44697265637458382E
        315C696E636C7564655C5C503A5C4C6576656C456469746F725C656469745C53
        63656E652E64666D90000000010000000001000000000000000000000000789F
        640700000000FFFFFFFF00000101010069747800000000000000102700000001
        00140427B6190500000000005C4C000000000000017400000000000000000001
        00000000000070E76407E4E20F1444000000523A5C44697265637458382E315C
        696E636C7564655C5C503A5C4C6576656C456469746F725C656469745C536365
        6E652E64666D90000000010000000001000000000000000000000000789F6407
        00000000FFFFFFFF000001010100697478000000000000001027000000010014
        54C545040600000000005C4C0000000000000174000000000000000000010000
        0000000070E76407E4E20F1444000000523A5C44697265637458382E315C696E
        636C7564655C5C503A5C4C6576656C456469746F725C656469745C5363656E65
        2E64666D90000000010000000001000000000000000000000000}
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
      Top = 113
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
    Tree = tvItems
    Types = [sftText]
    OnValidateResult = InplaceTextEditValidateResult
    Left = 30
    Top = 30
  end
end
