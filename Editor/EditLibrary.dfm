object frmEditLibrary: TfrmEditLibrary
  Left = 614
  Top = 307
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Object Library'
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
      OnClick = ebDeleteItemClick
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
    object cbPreview: TCheckBox
      Left = 4
      Top = 96
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
    DragCursor = crDrag
    Align = alLeft
    AlwaysKeepFocus = True
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    CustomPlusMinus = True
    DoInplaceEdit = False
    DragAllowed = True
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
      F5FFFFFF07000000E42FED0E00000000FFFFFFFF000001010100321278000000
      000000001027000000010000C0CA530500000000000001040000000000000104
      0000000000000000000100000000000000000000000000000000000000000000
      6CEA01040F00850100000000A8930C04AC83C80728000000703A5C656469746F
      725C696D616765656469746F722E637090000000010000000001000000000000
      000000000000E42FED0E00000000FFFFFFFF0000010101003212780000000000
      00001027000000010000702B4905010000000000010400000000000001040000
      0000000000000001000000000000000000000000000000000000000000006CEA
      01040F00850100000000A8930C04AC83C80728000000703A5C656469746F725C
      696D616765656469746F722E6370900000000100000000010000000000000000
      00000000E42FED0E00000000FFFFFFFF00000101010032127800000000000000
      1027000000010000504F150E0200000000000104000000000000010400000000
      000000000001000000000000000000000000000000000000000000006CEA0104
      0F00850100000000A8930C04AC83C80728000000703A5C656469746F725C696D
      616765656469746F722E63709000000001000000000100000000000000000000
      0000E42FED0E00000000FFFFFFFF000001010100321278000000000000001027
      00000001000084DF650E03000000000001040000000000000104000000000000
      00000001000000000000000000000000000000000000000000006CEA01040F00
      850100000000A8930C04AC83C80728000000703A5C656469746F725C696D6167
      65656469746F722E637090000000010000000001000000000000000000000000
      E42FED0E00000000FFFFFFFF0000010101003212780000000000000010270000
      0001000070981404040000000000010400000000000001040000000000000000
      0001000000000000000000000000000000000000000000006CEA01040F008501
      00000000A8930C04AC83C80728000000703A5C656469746F725C696D61676565
      6469746F722E637090000000010000000001000000000000000000000000E42F
      ED0E00000000FFFFFFFF00000101010032127800000000000000102700000001
      00000427B6190500000000000104000000000000010400000000000000000001
      000000000000000000000000000000000000000000006CEA01040F0085010000
      0000A8930C04AC83C80728000000703A5C656469746F725C696D616765656469
      746F722E637090000000010000000001000000000000000000000000E42FED0E
      00000000FFFFFFFF000001010100321278000000000000001027000000010000
      54C5450406000000000001040000000000000104000000000000000000010000
      00000000000000000000000000000000000000006CEA01040F00850100000000
      A8930C04AC83C80728000000703A5C656469746F725C696D616765656469746F
      722E637090000000010000000001000000000000000000000000}
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
    PopupMenu = pmObjects
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowLeafButton = False
    ShowLines = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 1
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
    OnItemChange = tvObjectsItemChange
    OnItemFocused = tvObjectsItemFocused
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
      OnClick = ebDeleteItemClick
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
      OnClick = ebDeleteItemClick
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
    Left = 32
    Top = 8
  end
end
