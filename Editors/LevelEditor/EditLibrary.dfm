object frmEditLibrary: TfrmEditLibrary
  Left = 461
  Top = 365
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
    Align = alLeft
    AlwaysKeepSelection = False
    AutoExpand = True
    CustomPlusMinus = True
    DoInplaceEdit = False
    DrawFocusRect = False
    DragImageMode = dimOne
    DragTrgDrawMode = ColorRect
    Flat = True
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF070000007474A41500000000FFFFFFFF0000010101006E7578000000
      00000000102700000001006EC0CA530500000000000053740000000000000174
      000000000000000000010000000000006E742A2041466F6E742C204772617068
      6963733A3A54436F6C6F722026436F6C6F722C2047726170686963733A3A5447
      7261706869632A20264772617068696390000000010000000001000000000000
      0000000000007474A41500000000FFFFFFFF0000010101006E75780000000000
      0000102700000001006E702B4905010000000000537400000000000001740000
      00000000000000010000000000006E742A2041466F6E742C2047726170686963
      733A3A54436F6C6F722026436F6C6F722C2047726170686963733A3A54477261
      706869632A202647726170686963900000000100000000010000000000000000
      000000007474A41500000000FFFFFFFF0000010101006E757800000000000000
      102700000001006E504F150E0200000000005374000000000000017400000000
      0000000000010000000000006E742A2041466F6E742C2047726170686963733A
      3A54436F6C6F722026436F6C6F722C2047726170686963733A3A544772617068
      69632A2026477261706869639000000001000000000100000000000000000000
      00007474A41500000000FFFFFFFF0000010101006E7578000000000000001027
      00000001006E84DF650E03000000000053740000000000000174000000000000
      000000010000000000006E742A2041466F6E742C2047726170686963733A3A54
      436F6C6F722026436F6C6F722C2047726170686963733A3A5447726170686963
      2A20264772617068696390000000010000000001000000000000000000000000
      7474A41500000000FFFFFFFF0000010101006E75780000000000000010270000
      0001006E70981404040000000000537400000000000001740000000000000000
      00010000000000006E742A2041466F6E742C2047726170686963733A3A54436F
      6C6F722026436F6C6F722C2047726170686963733A3A54477261706869632A20
      2647726170686963900000000100000000010000000000000000000000007474
      A41500000000FFFFFFFF0000010101006E757800000000000000102700000001
      006E0427B6190500000000005374000000000000017400000000000000000001
      0000000000006E742A2041466F6E742C2047726170686963733A3A54436F6C6F
      722026436F6C6F722C2047726170686963733A3A54477261706869632A202647
      726170686963900000000100000000010000000000000000000000007474A415
      00000000FFFFFFFF0000010101006E757800000000000000102700000001006E
      54C5450406000000000053740000000000000174000000000000000000010000
      000000006E742A2041466F6E742C2047726170686963733A3A54436F6C6F7220
      26436F6C6F722C2047726170686963733A3A54477261706869632A2026477261
      7068696390000000010000000001000000000000000000000000}
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    ItemIndent = 14
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
    MultiSelect = False
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
