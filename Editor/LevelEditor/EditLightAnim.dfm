object frmEditLightAnim: TfrmEditLightAnim
  Left = 312
  Top = 592
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
      Left = 4
      Top = 102
      Width = 353
      Height = 25
      Color = clBlack
      ParentColor = False
      OnClick = pbGClick
      OnPaint = pbGPaint
    end
    object Label4: TLabel
      Left = 95
      Top = 84
      Width = 32
      Height = 13
      Caption = 'Frame:'
    end
    object ExtBtn1: TExtBtn
      Left = 3
      Top = 132
      Width = 67
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Delete Key'
      Transparent = False
      FlatAlwaysEdge = True
    end
    object paColor: TPanel
      Left = 233
      Top = 8
      Width = 125
      Height = 59
      BevelOuter = bvNone
      Color = clBlack
      TabOrder = 4
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
    end
    object edName: TEdit
      Left = 96
      Top = 7
      Width = 132
      Height = 19
      AutoSize = False
      Color = 10526880
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
      Left = 130
      Top = 81
      Width = 98
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      ButtonWidth = 16
      MaxValue = 100000
      AutoSize = False
      Color = 10526880
      Ctl3D = True
      ParentCtl3D = False
      TabOrder = 3
      OnChange = sePointerChange
    end
    object stStartFrame: TStaticText
      Left = 3
      Top = 84
      Width = 47
      Height = 15
      AutoSize = False
      BorderStyle = sbsSunken
      Caption = '0'
      Color = 10526880
      ParentColor = False
      TabOrder = 5
    end
    object stEndFrame: TStaticText
      Left = 312
      Top = 84
      Width = 47
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
        F4FFFFFF07000000BCA8680600000000FFFFFFFF000001010100000078000000
        000000001027000000010000C0CA530500000000000000000000000000000100
        0000000000000000000100000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000090000000010000000001000000000000
        000000000000BCA8680600000000FFFFFFFF0000010101000000780000000000
        00001027000000010000702B4905010000000000000000000000000001000000
        0000000000000001000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000900000000100000000010000000000000000
        00000000BCA8680600000000FFFFFFFF00000101010000007800000000000000
        1027000000010000504F150E0200000000000000000000000000010000000000
        0000000000010000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000009000000001000000000100000000000000000000
        0000BCA8680600000000FFFFFFFF000001010100000078000000000000001027
        00000001000084DF650E03000000000000000000000000000100000000000000
        0000000100000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000090000000010000000001000000000000000000000000
        BCA8680600000000FFFFFFFF0000010101000000780000000000000010270000
        0001000070981404040000000000000000000000000001000000000000000000
        0001000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        000000000000000090000000010000000001000000000000000000000000BCA8
        680600000000FFFFFFFF00000101010000007800000000000000102700000001
        00000427B6190500000000000000000000000000010000000000000000000001
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        00000000000090000000010000000001000000000000000000000000BCA86806
        00000000FFFFFFFF000001010100000078000000000000001027000000010000
        54C5450406000000000000000000000000000100000000000000000000010000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000090000000010000000001000000000000000000000000}
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
      OnDblClick = tvItemsDblClick
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
end
