object frmImageLib: TfrmImageLib
  Left = 533
  Top = 497
  Width = 350
  Height = 499
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSizeToolWin
  Caption = 'Image Editor'
  Color = clBtnFace
  Constraints.MinHeight = 400
  Constraints.MinWidth = 350
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
    Left = 174
    Top = 0
    Width = 168
    Height = 472
    Align = alRight
    BevelOuter = bvLowered
    TabOrder = 0
    object Panel2: TPanel
      Left = 1
      Top = 427
      Width = 166
      Height = 44
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 0
      object ebClose: TExtBtn
        Left = 2
        Top = 26
        Width = 162
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Close'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebCloseClick
      end
      object ebConvert: TExtBtn
        Left = 2
        Top = 2
        Width = 162
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Convert...'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebConvertClick
      end
      object Bevel1: TBevel
        Left = 3
        Top = 21
        Width = 161
        Height = 2
      end
    end
    object paImage: TPanel
      Left = 1
      Top = 1
      Width = 166
      Height = 162
      Align = alTop
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 164
        Height = 160
        Align = alClient
        Color = clGray
        ParentColor = False
        OnDblClick = pbImageDblClick
        OnPaint = pbImagePaint
      end
    end
    object Panel3: TPanel
      Left = 1
      Top = 163
      Width = 166
      Height = 34
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 2
      object lbFileName: TMxLabel
        Left = 33
        Top = 2
        Width = 131
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 15263976
      end
      object RxLabel2: TMxLabel
        Left = 4
        Top = 2
        Width = 29
        Height = 13
        AutoSize = False
        Caption = 'File:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        ShadowColor = 15263976
      end
      object RxLabel3: TMxLabel
        Left = 4
        Top = 18
        Width = 29
        Height = 13
        AutoSize = False
        Caption = 'Info:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        ShadowColor = 15263976
      end
      object lbInfo: TMxLabel
        Left = 33
        Top = 18
        Width = 131
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 15263976
      end
    end
    object paProperties: TPanel
      Left = 1
      Top = 197
      Width = 166
      Height = 230
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 3
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 174
    Height = 472
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
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
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
      F5FFFFFF07000000B4A2FD0000000000FFFFFFFF0000010101006E6478000000
      000000001027000000010043E8211B07000000000000456C0000000000000165
      00000000000000000001000000000000616E645C636F6D706F6E656E74735C61
      6C65786D785C3B656E67696E653B6C69625C44583B633A5C7261645C426F726C
      616E645C434275696C646572355C426990000000010000000001000000000000
      000000000000B4A2FD0000000000FFFFFFFF0000010101006E64780000000000
      00001027000000010043B0261B07010000000000456C00000000000001650000
      0000000000000001000000000000616E645C636F6D706F6E656E74735C616C65
      786D785C3B656E67696E653B6C69625C44583B633A5C7261645C426F726C616E
      645C434275696C646572355C4269900000000100000000010000000000000000
      00000000B4A2FD0000000000FFFFFFFF0000010101006E647800000000000000
      102700000001004374271B07020000000000456C000000000000016500000000
      000000000001000000000000616E645C636F6D706F6E656E74735C616C65786D
      785C3B656E67696E653B6C69625C44583B633A5C7261645C426F726C616E645C
      434275696C646572355C42699000000001000000000100000000000000000000
      0000B4A2FD0000000000FFFFFFFF0000010101006E6478000000000000001027
      00000001004338281B07030000000000456C0000000000000165000000000000
      00000001000000000000616E645C636F6D706F6E656E74735C616C65786D785C
      3B656E67696E653B6C69625C44583B633A5C7261645C426F726C616E645C4342
      75696C646572355C426990000000010000000001000000000000000000000000
      B4A2FD0000000000FFFFFFFF0000010101006E64780000000000000010270000
      00010043FC281B07040000000000456C00000000000001650000000000000000
      0001000000000000616E645C636F6D706F6E656E74735C616C65786D785C3B65
      6E67696E653B6C69625C44583B633A5C7261645C426F726C616E645C43427569
      6C646572355C426990000000010000000001000000000000000000000000B4A2
      FD0000000000FFFFFFFF0000010101006E647800000000000000102700000001
      0043C0291B07050000000000456C000000000000016500000000000000000001
      000000000000616E645C636F6D706F6E656E74735C616C65786D785C3B656E67
      696E653B6C69625C44583B633A5C7261645C426F726C616E645C434275696C64
      6572355C426990000000010000000001000000000000000000000000B4A2FD00
      00000000FFFFFFFF0000010101006E6478000000000000001027000000010043
      842A1B07060000000000456C0000000000000165000000000000000000010000
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
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 17
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
    OnItemFocused = tvItemsItemFocused
    OnKeyPress = tvItemsKeyPress
  end
  object fsStorage: TFormStorage
    StoredValues = <>
  end
end
