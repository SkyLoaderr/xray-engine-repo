object frmEditLibrary: TfrmEditLibrary
  Left = 532
  Top = 247
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
      Top = 272
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
      Top = 251
      Width = 109
      Height = 17
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Export As DO'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebExportDOClick
    end
    object ebImport: TExtBtn
      Left = 4
      Top = 233
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
      F4FFFFFF070000001C973E04002F0000FFFFFFFF0000010101003E0478000000
      00000000102700000001006DC0CA53050000000000003E04000000000000012D
      000000000000000000010000000000002D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D
      2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D0D0A0D0A766F6964
      205F5F6661737463616C6C205466726D45646974010000000001000000000000
      0000000000001C973E04002F0000FFFFFFFF0000010101003E04780000000000
      0000102700000001006D702B49050100000000003E04000000000000012D0000
      00000000000000010000000000002D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D
      2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D0D0A0D0A766F6964205F
      5F6661737463616C6C205466726D456469740100000000010000000000000000
      000000001C973E04002F0000FFFFFFFF0000010101003E047800000000000000
      102700000001006D504F150E0200000000003E04000000000000012D00000000
      0000000000010000000000002D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D
      2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D0D0A0D0A766F6964205F5F66
      61737463616C6C205466726D4564697401000000000100000000000000000000
      00001C973E04002F0000FFFFFFFF0000010101003E0478000000000000001027
      00000001006D84DF650E0300000000003E04000000000000012D000000000000
      000000010000000000002D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D
      2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D0D0A0D0A766F6964205F5F666173
      7463616C6C205466726D45646974010000000001000000000000000000000000
      1C973E04002F0000FFFFFFFF0000010101003E04780000000000000010270000
      0001006D709814040400000000003E04000000000000012D0000000000000000
      00010000000000002D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D
      2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D0D0A0D0A766F6964205F5F6661737463
      616C6C205466726D456469740100000000010000000000000000000000001C97
      3E04002F0000FFFFFFFF0000010101003E047800000000000000102700000001
      006D0427B6190500000000003E04000000000000012D00000000000000000001
      0000000000002D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D
      2D2D2D2D2D2D2D2D2D2D2D2D2D2D0D0A0D0A766F6964205F5F6661737463616C
      6C205466726D456469740100000000010000000000000000000000001C973E04
      002F0000FFFFFFFF0000010101003E047800000000000000102700000001006D
      54C545040600000000003E04000000000000012D000000000000000000010000
      000000002D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D2D
      2D2D2D2D2D2D2D2D2D2D2D2D0D0A0D0A766F6964205F5F6661737463616C6C20
      5466726D45646974010000000001000000000000000000000000}
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
