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
      F5FFFFFF07000000BC73EC0600000000FFFFFFFF0000010101006E6478000000
      000000001027000000010043C0CA5305000000000000456C0000000000000165
      00000000000000000001000000000000616E645C636F6D706F6E656E74735C61
      6C65786D785C3B656E67696E653B6C69625C44583B633A5C7261645C426F726C
      616E645C434275696C646572355C426990000000010000000001000000000000
      000000000000BC73EC0600000000FFFFFFFF0000010101006E64780000000000
      00001027000000010043702B4905010000000000456C00000000000001650000
      0000000000000001000000000000616E645C636F6D706F6E656E74735C616C65
      786D785C3B656E67696E653B6C69625C44583B633A5C7261645C426F726C616E
      645C434275696C646572355C4269900000000100000000010000000000000000
      00000000BC73EC0600000000FFFFFFFF0000010101006E647800000000000000
      1027000000010043504F150E020000000000456C000000000000016500000000
      000000000001000000000000616E645C636F6D706F6E656E74735C616C65786D
      785C3B656E67696E653B6C69625C44583B633A5C7261645C426F726C616E645C
      434275696C646572355C42699000000001000000000100000000000000000000
      0000BC73EC0600000000FFFFFFFF0000010101006E6478000000000000001027
      00000001004384DF650E030000000000456C0000000000000165000000000000
      00000001000000000000616E645C636F6D706F6E656E74735C616C65786D785C
      3B656E67696E653B6C69625C44583B633A5C7261645C426F726C616E645C4342
      75696C646572355C426990000000010000000001000000000000000000000000
      BC73EC0600000000FFFFFFFF0000010101006E64780000000000000010270000
      0001004370981404040000000000456C00000000000001650000000000000000
      0001000000000000616E645C636F6D706F6E656E74735C616C65786D785C3B65
      6E67696E653B6C69625C44583B633A5C7261645C426F726C616E645C43427569
      6C646572355C426990000000010000000001000000000000000000000000BC73
      EC0600000000FFFFFFFF0000010101006E647800000000000000102700000001
      00430427B619050000000000456C000000000000016500000000000000000001
      000000000000616E645C636F6D706F6E656E74735C616C65786D785C3B656E67
      696E653B6C69625C44583B633A5C7261645C426F726C616E645C434275696C64
      6572355C426990000000010000000001000000000000000000000000BC73EC06
      00000000FFFFFFFF0000010101006E6478000000000000001027000000010043
      54C54504060000000000456C0000000000000165000000000000000000010000
      00000000616E645C636F6D706F6E656E74735C616C65786D785C3B656E67696E
      653B6C69625C44583B633A5C7261645C426F726C616E645C434275696C646572
      355C426990000000010000000001000000000000000000000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
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
    LineHeight = 15
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
