object frmEditShaders: TfrmEditShaders
  Left = 457
  Top = 213
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Shaders'
  ClientHeight = 435
  ClientWidth = 306
  Color = clBtnFace
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
  object Panel1: TPanel
    Left = 192
    Top = 0
    Width = 114
    Height = 435
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object ExtBtn1: TExtBtn
      Left = 2
      Top = 339
      Width = 108
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'All Priority --->> 1'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ExtBtn1Click
    end
    object GroupBox1: TGroupBox
      Left = 0
      Top = 0
      Width = 114
      Height = 129
      Align = alTop
      Caption = ' Command '
      TabOrder = 0
      object ebNewShader: TExtBtn
        Left = 3
        Top = 15
        Width = 108
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'New'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebNewShaderClick
      end
      object ebRemoveShader: TExtBtn
        Left = 3
        Top = 51
        Width = 108
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Remove Selected'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebRemoveShaderClick
      end
      object ebPropertiesShader: TExtBtn
        Left = 3
        Top = 82
        Width = 108
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Properties'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebPropertiesShaderClick
      end
      object ebCloneShader: TExtBtn
        Left = 3
        Top = 33
        Width = 108
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Clone Selected'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebCloneShaderClick
      end
      object cbPreview: TCheckBox
        Left = 3
        Top = 107
        Width = 107
        Height = 17
        Caption = 'Preview'
        Checked = True
        State = cbChecked
        TabOrder = 0
        OnClick = cbPreviewClick
      end
    end
    object rgTestObject: TRadioGroup
      Left = 0
      Top = 129
      Width = 114
      Height = 88
      Align = alTop
      Caption = ' Test object '
      ItemIndex = 0
      Items.Strings = (
        'Plane'
        'Box'
        'Sphere'
        'Teapot'
        'External...')
      TabOrder = 1
      OnClick = rgTestObjectClick
    end
    object gbExtern: TGroupBox
      Left = 0
      Top = 217
      Width = 114
      Height = 118
      Align = alTop
      Caption = ' External '
      TabOrder = 2
      Visible = False
      object ebExternSelect: TExtBtn
        Left = 3
        Top = 28
        Width = 54
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Select'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebExternSelectClick
      end
      object lbSelectObject: TLabel
        Left = 2
        Top = 12
        Width = 110
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object ebSaveExternObject: TExtBtn
        Left = 57
        Top = 28
        Width = 54
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Save'
        Enabled = False
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebSaveExternObjectClick
      end
      object GroupBox2: TGroupBox
        Left = 2
        Top = 48
        Width = 110
        Height = 68
        Align = alBottom
        Caption = ' Current material '
        TabOrder = 0
        object ebAssignShader: TExtBtn
          Left = 24
          Top = 44
          Width = 83
          Height = 20
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Assign shader'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebAssignShaderClick
        end
        object lbExternSelMat: TLabel
          Left = 43
          Top = 15
          Width = 65
          Height = 13
          AutoSize = False
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clMaroon
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object RxLabel18: TRxLabel
          Left = 4
          Top = 15
          Width = 33
          Height = 13
          Caption = 'Name:'
          ShadowColor = 15263976
        end
        object RxLabel1: TRxLabel
          Left = 4
          Top = 29
          Width = 39
          Height = 13
          Caption = 'Shader:'
          ShadowColor = 15263976
        end
        object lbExternSelShader: TLabel
          Left = 43
          Top = 29
          Width = 65
          Height = 13
          AutoSize = False
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clMaroon
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object ebDropper: TExtBtn
          Left = 4
          Top = 44
          Width = 20
          Height = 20
          Align = alNone
          AllowAllUp = True
          BevelShow = False
          CloseButton = False
          GroupIndex = 1
          Glyph.Data = {
            1E010000424D1E01000000000000460000002800000012000000120000000100
            040000000000D8000000120B0000120B00000400000004000000CED3D600F7F7
            F70000000000FFFFFF0000000000000000000000202000200000000000000000
            0000021220000000000000000020002112000000000000000212002111200000
            0000000000210002111200000000000000210000211120000000000000020000
            0211120000000000000000000021112000000000000000000002111202000000
            0000000000002111220000000000000000000212220000000000000000000022
            2222000000000000000002222222200000000000000000002222200000000000
            0000000022222000000000000000000002220000000000000000000000000000
            0000}
          Transparent = False
          FlatAlwaysEdge = True
        end
      end
    end
    object Panel2: TPanel
      Left = 0
      Top = 357
      Width = 114
      Height = 78
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 3
      object ebCancel: TExtBtn
        Left = 2
        Top = 60
        Width = 108
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Close'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebCancelClick
      end
      object ebSave: TExtBtn
        Left = 2
        Top = 43
        Width = 108
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Save'
        Enabled = False
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebSaveClick
      end
      object ebReloadShaders: TExtBtn
        Left = 2
        Top = 19
        Width = 108
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Reload'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebReloadShadersClick
      end
      object ebMerge: TExtBtn
        Left = 2
        Top = 2
        Width = 108
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Merge...'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebMergeClick
      end
    end
  end
  object tvShaders: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 435
    LeftPosition = 0
    ActiveBorderType = fbtSunken
    Align = alLeft
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
    CustomCheckboxes = False
    CustomPlusMinus = False
    DeselectChildrenOnCollapse = False
    DivLinesColor = clBtnFace
    DoInplaceEdit = False
    DraggableSections = False
    DragAllowed = False
    DragTrgDrawMode = ColorRect
    DragType = dtDelphi
    DragImageMode = dimOne
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
      F8FFFFFF07000000686F400E00000000FFFFFFFF0000010101006C6178000000
      000000001027000000010063000000000000636C000000000000017700000000
      0000000000010000010000000001000000000000000000000000686F400E0000
      0000FFFFFFFF0000010101006C61780000000000000010270000000100630100
      00000000636C0000000000000177000000000000000000010000010000000001
      000000000000000000000000686F400E00000000FFFFFFFF0000010101006C61
      78000000000000001027000000010063020000000000636C0000000000000177
      000000000000000000010000010000000001000000000000000000000000686F
      400E00000000FFFFFFFF0000010101006C617800000000000000102700000001
      0063030000000000636C00000000000001770000000000000000000100000100
      00000001000000000000000000000000686F400E00000000FFFFFFFF00000101
      01006C6178000000000000001027000000010063040000000000636C00000000
      0000017700000000000000000001000001000000000100000000000000000000
      0000686F400E00000000FFFFFFFF0000010101006C6178000000000000001027
      000000010063050000000000636C000000000000017700000000000000000001
      0000010000000001000000000000000000000000686F400E00000000FFFFFFFF
      0000010101006C6178000000000000001027000000010063060000000000636C
      0000000000000177000000000000000000010000010000000001000000000000
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
    LinesColor = clBtnShadow
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
    PopupMenu = pmShaders
    RightAlignedText = False
    RightAlignedTree = False
    RowHotTrack = False
    RowSelect = True
    ScrollTracking = True
    SelectColumn = -1
    ShowButtons = True
    ShowColumns = False
    ShowCheckboxes = False
    ShowImages = True
    ShowLines = False
    ShowRoot = False
    ShowRootButtons = True
    SelectionMode = smUsual
    SortDir = sdAscend
    SortMode = smNone
    SortSection = 0
    SortType = stText
    StickyHeaderSections = False
    TabOrder = 1
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
    OnItemChange = tvShadersItemChange
    OnTryEdit = tvShadersTryEdit
    OnItemSelectedChange = tvShadersItemSelectedChange
    OnDragDrop = tvShadersDragDrop
    OnDragOver = tvShadersDragOver
    OnStartDrag = tvShadersStartDrag
    OnDblClick = tvShadersDblClick
    OnKeyPress = tvShadersKeyPress
  end
  object pmShaders: TPopupMenu
    object miNewFolder: TMenuItem
      Caption = 'New Folder...'
      GroupIndex = 1
      OnClick = miNewFolderClick
    end
    object miEditFolder: TMenuItem
      Caption = 'Edit Folder'
      GroupIndex = 1
      OnClick = miEditFolderClick
    end
    object miDeleteFolder: TMenuItem
      Caption = 'Delete Folder'
      GroupIndex = 1
      OnClick = miDeleteFolderClick
    end
    object N1: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object Object1: TMenuItem
      Caption = 'Shader'
      GroupIndex = 1
      object LoadObject1: TMenuItem
        Caption = 'Append'
        GroupIndex = 1
        OnClick = ebNewShaderClick
      end
      object DeleteObject1: TMenuItem
        Caption = 'Delete'
        GroupIndex = 1
        OnClick = ebRemoveShaderClick
      end
      object N2: TMenuItem
        Caption = '-'
        GroupIndex = 1
      end
      object miObjectProperties: TMenuItem
        Caption = 'Properties'
        GroupIndex = 1
        OnClick = ebPropertiesShaderClick
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
    Left = 8
    Top = 8
  end
end
