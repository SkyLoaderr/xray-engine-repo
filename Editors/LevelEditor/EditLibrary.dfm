object frmEditLibrary: TfrmEditLibrary
  Left = 461
  Top = 365
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Object Library'
  ClientHeight = 362
  ClientWidth = 308
  Color = 10528425
  Constraints.MinHeight = 389
  Constraints.MinWidth = 316
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 192
    Top = 0
    Width = 116
    Height = 362
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object ebCancel: TExtBtn
      Left = 4
      Top = 342
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
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
      BtnColor = 10528425
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
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Properties'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebPropertiesClick
    end
    object ebSave: TExtBtn
      Left = 4
      Top = 324
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Save'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSaveClick
    end
    object ebExportDO: TExtBtn
      Left = 4
      Top = 212
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Export DO'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      Visible = False
      OnClick = ebExportDOClick
    end
    object ebImport: TExtBtn
      Left = 4
      Top = 289
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Import Object'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebImportClick
    end
    object ebExportHOM: TExtBtn
      Left = 4
      Top = 230
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Export HOM'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      Visible = False
      OnClick = ebExportHOMClick
    end
    object ExtBtn1: TExtBtn
      Left = 4
      Top = 194
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Export Slipch'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      Visible = False
      OnClick = ExtBtn1Click
    end
    object ebMakeLOD: TExtBtn
      Left = 4
      Top = 158
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Make LOD'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMakeLODClick
    end
    object cbPreview: TCheckBox
      Left = 4
      Top = 176
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
      ParentColor = True
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 114
        Height = 114
        Align = alClient
        OnPaint = pbImagePaint
      end
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 362
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alLeft
    AlwaysKeepSelection = False
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
      F4FFFFFF07000000DC412C1100000000FFFFFFFF000001010100295C78000000
      000000001027000000010050C0CA5305000000000000623B0000000000000167
      000000000000000000010000000000006970653B456E67696E655C416E696D61
      74696F6E733B456E67696E655C426C656E646572733B523A5C426F726C616E64
      5C436F6D706F6E656E7473365C616C6590000000010000000001000000000000
      000000000000DC412C1100000000FFFFFFFF000001010100295C780000000000
      00001027000000010050702B4905010000000000623B00000000000001670000
      00000000000000010000000000006970653B456E67696E655C416E696D617469
      6F6E733B456E67696E655C426C656E646572733B523A5C426F726C616E645C43
      6F6D706F6E656E7473365C616C65900000000100000000010000000000000000
      00000000DC412C1100000000FFFFFFFF000001010100295C7800000000000000
      1027000000010050504F150E020000000000623B000000000000016700000000
      0000000000010000000000006970653B456E67696E655C416E696D6174696F6E
      733B456E67696E655C426C656E646572733B523A5C426F726C616E645C436F6D
      706F6E656E7473365C616C659000000001000000000100000000000000000000
      0000DC412C1100000000FFFFFFFF000001010100295C78000000000000001027
      00000001005084DF650E030000000000623B0000000000000167000000000000
      000000010000000000006970653B456E67696E655C416E696D6174696F6E733B
      456E67696E655C426C656E646572733B523A5C426F726C616E645C436F6D706F
      6E656E7473365C616C6590000000010000000001000000000000000000000000
      DC412C1100000000FFFFFFFF000001010100295C780000000000000010270000
      0001005070981404040000000000623B00000000000001670000000000000000
      00010000000000006970653B456E67696E655C416E696D6174696F6E733B456E
      67696E655C426C656E646572733B523A5C426F726C616E645C436F6D706F6E65
      6E7473365C616C6590000000010000000001000000000000000000000000DC41
      2C1100000000FFFFFFFF000001010100295C7800000000000000102700000001
      00500427B619050000000000623B000000000000016700000000000000000001
      0000000000006970653B456E67696E655C416E696D6174696F6E733B456E6769
      6E655C426C656E646572733B523A5C426F726C616E645C436F6D706F6E656E74
      73365C616C6590000000010000000001000000000000000000000000DC412C11
      00000000FFFFFFFF000001010100295C78000000000000001027000000010050
      54C54504060000000000623B0000000000000167000000000000000000010000
      000000006970653B456E67696E655C416E696D6174696F6E733B456E67696E65
      5C426C656E646572733B523A5C426F726C616E645C436F6D706F6E656E747336
      5C616C6590000000010000000001000000000000000000000000}
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
    OnItemFocused = tvObjectsItemFocused
    OnDblClick = tvObjectsDblClick
    OnKeyPress = tvObjectsKeyPress
  end
  object fsStorage: TFormStorage
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
