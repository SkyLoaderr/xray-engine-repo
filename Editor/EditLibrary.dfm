object frmEditLibrary: TfrmEditLibrary
  Left = 888
  Top = 579
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Edit Library'
  ClientHeight = 309
  ClientWidth = 308
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
    Width = 116
    Height = 309
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object ebCancel: TExtBtn
      Left = 4
      Top = 290
      Width = 109
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
      Left = 4
      Top = 273
      Width = 109
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
    object ebValidate: TExtBtn
      Left = 4
      Top = 249
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Validate Library'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebValidateClick
    end
    object ebMakeThm: TExtBtn
      Left = 4
      Top = 232
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Make Thumbnail'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMakeThmClick
    end
    object ebUnload: TExtBtn
      Left = 4
      Top = 208
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Unload Objects'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebUnloadClick
    end
    object ebLoadObject: TExtBtn
      Left = 4
      Top = 5
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Append'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebLoadObjectClick
    end
    object ebDeleteObject: TExtBtn
      Left = 4
      Top = 43
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Delete'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebDeleteObjectClick
    end
    object ebProperties: TExtBtn
      Left = 4
      Top = 66
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Properties'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebPropertiesClick
    end
    object ebSaveObjectOGF: TExtBtn
      Left = 4
      Top = 109
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save OGF'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveObjectOGFClick
    end
    object ebSaveObjectVCF: TExtBtn
      Left = 4
      Top = 91
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save Collision Form'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveObjectVCFClick
    end
    object ebSaveObjectSkeletonOGF: TExtBtn
      Left = 4
      Top = 127
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save Skeleton'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveObjectSkeletonOGFClick
    end
    object ebReloadObject: TExtBtn
      Left = 4
      Top = 23
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Reload'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebReloadObjectClick
    end
    object ebSaveObjectDO: TExtBtn
      Left = 4
      Top = 145
      Width = 109
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Save DO'
      Transparent = False
      FlatAlwaysEdge = True
      Visible = False
    end
    object cbPreview: TCheckBox
      Left = 4
      Top = 168
      Width = 108
      Height = 17
      Caption = 'Preview'
      TabOrder = 0
      OnClick = cbPreviewClick
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 309
    Cursor = crDefault
    LeftPosition = 0
    DoubleBuffered = False
    DragCursor = crDrag
    Align = alLeft
    AlwaysKeepFocus = True
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    DoInplaceEdit = False
    DragAllowed = True
    DragImageMode = dimOne
    DragTrgDrawMode = ColorRect
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
      F5FFFFFF070000002475004100300000FFFFFFFF000001010100044078000000
      000000001027000000010041C0CA530500000000000020200000000000000145
      000000000000000000010000000000006F726D6174696F6E2C20224F626A6563
      742027257327206578706F7274207375636365737366756C6C792E222C205F70
      542D3E4765744E616D652829293B0D0A20202020010000000001000000000000
      0000000000002475004100300000FFFFFFFF0000010101000440780000000000
      00001027000000010041702B4905010000000000202000000000000001450000
      00000000000000010000000000006F726D6174696F6E2C20224F626A65637420
      27257327206578706F7274207375636365737366756C6C792E222C205F70542D
      3E4765744E616D652829293B0D0A202020200100000000010000000000000000
      000000002475004100300000FFFFFFFF00000101010004407800000000000000
      1027000000010041504F150E0200000000002020000000000000014500000000
      0000000000010000000000006F726D6174696F6E2C20224F626A656374202725
      7327206578706F7274207375636365737366756C6C792E222C205F70542D3E47
      65744E616D652829293B0D0A2020202001000000000100000000000000000000
      00002475004100300000FFFFFFFF000001010100044078000000000000001027
      00000001004184DF650E03000000000020200000000000000145000000000000
      000000010000000000006F726D6174696F6E2C20224F626A6563742027257327
      206578706F7274207375636365737366756C6C792E222C205F70542D3E476574
      4E616D652829293B0D0A20202020010000000001000000000000000000000000
      2475004100300000FFFFFFFF0000010101000440780000000000000010270000
      0001004170981404040000000000202000000000000001450000000000000000
      00010000000000006F726D6174696F6E2C20224F626A65637420272573272065
      78706F7274207375636365737366756C6C792E222C205F70542D3E4765744E61
      6D652829293B0D0A202020200100000000010000000000000000000000002475
      004100300000FFFFFFFF00000101010004407800000000000000102700000001
      00410427B6190500000000002020000000000000014500000000000000000001
      0000000000006F726D6174696F6E2C20224F626A656374202725732720657870
      6F7274207375636365737366756C6C792E222C205F70542D3E4765744E616D65
      2829293B0D0A2020202001000000000100000000000000000000000024750041
      00300000FFFFFFFF000001010100044078000000000000001027000000010041
      54C5450406000000000020200000000000000145000000000000000000010000
      000000006F726D6174696F6E2C20224F626A6563742027257327206578706F72
      74207375636365737366756C6C792E222C205F70542D3E4765744E616D652829
      293B0D0A20202020010000000001000000000000000000000000}
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
    LinesColor = clBtnShadow
    MouseFrameSelect = True
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    PlusMinusTransparent = True
    PopupMenu = pmObjects
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowLeafButton = False
    ShowLines = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 1
    TabStop = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    VirtualityLevel = vlNone
    TextColor = clBtnText
    BkColor = clBtnShadow
    OnItemChange = tvObjectsItemChange
    OnItemSelectedChange = tvObjectsItemSelectedChange
    OnDragDrop = tvObjectsDragDrop
    OnDragOver = tvObjectsDragOver
    OnStartDrag = tvObjectsStartDrag
    OnDblClick = tvObjectsDblClick
    OnKeyPress = tvObjectsKeyPress
  end
  object pmObjects: TPopupMenu
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
    object Folder1: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object LoadObject1: TMenuItem
      Caption = 'Append Object'
      GroupIndex = 1
      OnClick = ebLoadObjectClick
    end
    object DeleteObject1: TMenuItem
      Caption = 'Delete Object'
      GroupIndex = 1
      OnClick = ebDeleteObjectClick
    end
    object miObjectProperties: TMenuItem
      Caption = 'Object Properties'
      GroupIndex = 1
      OnClick = ebPropertiesClick
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
