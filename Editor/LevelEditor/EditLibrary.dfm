object frmEditLibrary: TfrmEditLibrary
  Left = 532
  Top = 247
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Object Library'
  ClientHeight = 322
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
    Height = 322
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object ebCancel: TExtBtn
      Left = 4
      Top = 304
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
    object ebMakeThm: TExtBtn
      Left = 4
      Top = 140
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
    object ebProperties: TExtBtn
      Left = 4
      Top = 121
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
    object ebSave: TExtBtn
      Left = 4
      Top = 286
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
    object ebExportDO: TExtBtn
      Left = 4
      Top = 232
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Export DO'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebExportDOClick
    end
    object ebImport: TExtBtn
      Left = 4
      Top = 209
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Import Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebImportClick
    end
    object ebExportSkel: TExtBtn
      Left = 4
      Top = 249
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Export Skeleton'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebExportSkelClick
    end
    object ebExportHOM: TExtBtn
      Left = 4
      Top = 266
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Export HOM'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebExportHOMClick
    end
    object cbPreview: TCheckBox
      Left = 4
      Top = 160
      Width = 108
      Height = 17
      Caption = 'Preview'
      TabOrder = 0
      OnClick = cbPreviewClick
    end
    object paImage: TPanel
      Left = 0
      Top = 1
      Width = 116
      Height = 116
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 114
        Height = 114
        Align = alClient
        Color = clGray
        ParentColor = False
        OnPaint = pbImagePaint
      end
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 322
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
      F4FFFFFF07000000E8A7C00D00000000FFFFFFFF000001010100480478000000
      000000001027000000010004C0CA53050000000000004804000000000000010D
      00000000000000000001000000000000B8B64804CCB74804C4B34804D8B34804
      A0B64804E0B648042409E505285248042C00000010000000340000001B000000
      000000000A0000007061436F6D6D616E90000000010000000001000000000000
      000000000000E8A7C00D00000000FFFFFFFF0000010101004804780000000000
      00001027000000010004702B49050100000000004804000000000000010D0000
      0000000000000001000000000000B8B64804CCB74804C4B34804D8B34804A0B6
      4804E0B648042409E505285248042C00000010000000340000001B0000000000
      00000A0000007061436F6D6D616E900000000100000000010000000000000000
      00000000E8A7C00D00000000FFFFFFFF00000101010048047800000000000000
      1027000000010004504F150E0200000000004804000000000000010D00000000
      000000000001000000000000B8B64804CCB74804C4B34804D8B34804A0B64804
      E0B648042409E505285248042C00000010000000340000001B00000000000000
      0A0000007061436F6D6D616E9000000001000000000100000000000000000000
      0000E8A7C00D00000000FFFFFFFF000001010100480478000000000000001027
      00000001000484DF650E0300000000004804000000000000010D000000000000
      00000001000000000000B8B64804CCB74804C4B34804D8B34804A0B64804E0B6
      48042409E505285248042C00000010000000340000001B000000000000000A00
      00007061436F6D6D616E90000000010000000001000000000000000000000000
      E8A7C00D00000000FFFFFFFF0000010101004804780000000000000010270000
      00010004709814040400000000004804000000000000010D0000000000000000
      0001000000000000B8B64804CCB74804C4B34804D8B34804A0B64804E0B64804
      2409E505285248042C00000010000000340000001B000000000000000A000000
      7061436F6D6D616E90000000010000000001000000000000000000000000E8A7
      C00D00000000FFFFFFFF00000101010048047800000000000000102700000001
      00040427B6190500000000004804000000000000010D00000000000000000001
      000000000000B8B64804CCB74804C4B34804D8B34804A0B64804E0B648042409
      E505285248042C00000010000000340000001B000000000000000A0000007061
      436F6D6D616E90000000010000000001000000000000000000000000E8A7C00D
      00000000FFFFFFFF000001010100480478000000000000001027000000010004
      54C545040600000000004804000000000000010D000000000000000000010000
      00000000B8B64804CCB74804C4B34804D8B34804A0B64804E0B648042409E505
      285248042C00000010000000340000001B000000000000000A0000007061436F
      6D6D616E90000000010000000001000000000000000000000000}
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
    OnItemFocused = tvObjectsItemFocused
    OnDblClick = tvObjectsDblClick
    OnKeyPress = tvObjectsKeyPress
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
