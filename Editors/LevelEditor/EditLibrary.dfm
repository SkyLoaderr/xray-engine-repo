object frmEditLibrary: TfrmEditLibrary
  Left = 365
  Top = 245
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Object Library'
  ClientHeight = 378
  ClientWidth = 324
  Color = 10528425
  Constraints.MinHeight = 405
  Constraints.MinWidth = 332
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
    Width = 132
    Height = 378
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object ebCancel: TExtBtn
      Left = 2
      Top = 360
      Width = 129
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
      Left = 2
      Top = 188
      Width = 129
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
      Left = 2
      Top = 170
      Width = 129
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
      Left = 2
      Top = 342
      Width = 129
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
      Left = 2
      Top = 262
      Width = 129
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
      Left = 2
      Top = 314
      Width = 129
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
      Left = 2
      Top = 280
      Width = 129
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
      Left = 2
      Top = 244
      Width = 129
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
      Left = 2
      Top = 205
      Width = 129
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
    object Bevel4: TBevel
      Left = 0
      Top = 166
      Width = 132
      Height = 2
      Align = alTop
      Shape = bsBottomLine
    end
    object ebMakeLWO: TExtBtn
      Left = 2
      Top = 297
      Width = 129
      Height = 17
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Export LWO'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebMakeLWOClick
    end
    object cbPreview: TCheckBox
      Left = 2
      Top = 223
      Width = 128
      Height = 17
      Caption = 'Preview'
      TabOrder = 0
      OnClick = cbPreviewClick
    end
    object paImage: TPanel
      Left = 0
      Top = 0
      Width = 132
      Height = 132
      Align = alTop
      BevelOuter = bvLowered
      Caption = '<no image>'
      ParentColor = True
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 130
        Height = 130
        Align = alClient
        OnPaint = pbImagePaint
      end
    end
    object Panel3: TPanel
      Left = 0
      Top = 132
      Width = 132
      Height = 34
      Align = alTop
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 2
      object lbFaces: TLabel
        Left = 81
        Top = 2
        Width = 9
        Height = 13
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel2: TLabel
        Left = 4
        Top = 2
        Width = 69
        Height = 13
        Caption = 'Face count:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object RxLabel3: TLabel
        Left = 4
        Top = 18
        Width = 77
        Height = 13
        Caption = 'Vertex count:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbVertices: TLabel
        Left = 81
        Top = 18
        Width = 9
        Height = 13
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object tvObjects: TElTree
    Left = 0
    Top = 0
    Width = 192
    Height = 378
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
      F4FFFFFF0700000028C4990500000000FFFFFFFF000001010100000078000000
      000000001027000000010000C0CA53050000000000005B000000000000000100
      0000000000000000000100000000000089E35B0001000000000000000D000000
      00000000D8EB5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B0000000000
      5F02B2006F02B2000C7AFF00F097630590000000010000000001000000000000
      00000000000028C4990500000000FFFFFFFF0000010101000000780000000000
      00001027000000010000702B49050100000000005B0000000000000001000000
      000000000000000100000000000089E35B0001000000000000000D0000000000
      0000D8EB5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B00000000005F02
      B2006F02B2000C7AFF00F0976305900000000100000000010000000000000000
      0000000028C4990500000000FFFFFFFF00000101010000007800000000000000
      1027000000010000504F150E0200000000005B00000000000000010000000000
      00000000000100000000000089E35B0001000000000000000D00000000000000
      D8EB5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B00000000005F02B200
      6F02B2000C7AFF00F09763059000000001000000000100000000000000000000
      000028C4990500000000FFFFFFFF000001010100000078000000000000001027
      00000001000084DF650E0300000000005B000000000000000100000000000000
      0000000100000000000089E35B0001000000000000000D00000000000000D8EB
      5B00E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B00000000005F02B2006F02
      B2000C7AFF00F097630590000000010000000001000000000000000000000000
      28C4990500000000FFFFFFFF0000010101000000780000000000000010270000
      00010000709814040400000000005B0000000000000001000000000000000000
      000100000000000089E35B0001000000000000000D00000000000000D8EB5B00
      E8EB5B0000EC5B0010EC5B0020EC5B004CEC5B00000000005F02B2006F02B200
      0C7AFF00F09763059000000001000000000100000000000000000000000028C4
      990500000000FFFFFFFF00000101010000007800000000000000102700000001
      00000427B6190500000000005B00000000000000010000000000000000000001
      00000000000089E35B0001000000000000000D00000000000000D8EB5B00E8EB
      5B0000EC5B0010EC5B0020EC5B004CEC5B00000000005F02B2006F02B2000C7A
      FF00F09763059000000001000000000100000000000000000000000028C49905
      00000000FFFFFFFF000001010100000078000000000000001027000000010000
      54C545040600000000005B000000000000000100000000000000000000010000
      0000000089E35B0001000000000000000D00000000000000D8EB5B00E8EB5B00
      00EC5B0010EC5B0020EC5B004CEC5B00000000005F02B2006F02B2000C7AFF00
      F097630590000000010000000001000000000000000000000000}
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
