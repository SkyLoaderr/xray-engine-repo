object frmEditParticles: TfrmEditParticles
  Left = 414
  Top = 329
  Width = 314
  Height = 425
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSizeToolWin
  Caption = 'Particles'
  Color = clBtnFace
  Constraints.MinHeight = 425
  Constraints.MinWidth = 314
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
    Left = 173
    Top = 0
    Width = 133
    Height = 398
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object GroupBox1: TGroupBox
      Left = 0
      Top = 0
      Width = 133
      Height = 104
      Align = alTop
      Caption = ' Command '
      TabOrder = 0
      object ebNewPS: TExtBtn
        Left = 3
        Top = 15
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'New'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebAppendPSClick
      end
      object ebRemovePS: TExtBtn
        Left = 3
        Top = 51
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Remove Selected'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebRemovePSClick
      end
      object ebPropertiesPS: TExtBtn
        Left = 3
        Top = 82
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Properties'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebPropertiesPSClick
      end
      object ebClonePS: TExtBtn
        Left = 3
        Top = 33
        Width = 127
        Height = 18
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Clone Selected'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebClonePSClick
      end
    end
    object Panel2: TPanel
      Left = 0
      Top = 320
      Width = 133
      Height = 78
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      object ebCancel: TExtBtn
        Left = 2
        Top = 60
        Width = 131
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
        Width = 131
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
      object ebReloadPS: TExtBtn
        Left = 2
        Top = 19
        Width = 131
        Height = 17
        Align = alNone
        BevelShow = False
        CloseButton = False
        Caption = 'Reload'
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebReloadPSClick
      end
      object ebMerge: TExtBtn
        Left = 2
        Top = 2
        Width = 131
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
    object gbCurrentPS: TGroupBox
      Left = 0
      Top = 104
      Width = 133
      Height = 60
      Align = alTop
      Caption = ' Current PS '
      TabOrder = 2
      Visible = False
      object ExtBtn2: TExtBtn
        Left = 3
        Top = 15
        Width = 63
        Height = 16
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFBABABA0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFBABABA0000000000000000004F4F4F7B7B7BFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000000000000000000008080828
          2828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA000000000000000000000000
          0000000000001212121F1F1FC6C6C6FFFFFFFFFFFFBABABA0000000000000000
          00000000080808282828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000
          00000000004F4F4F7B7B7BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
          0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn2Click
      end
      object ExtBtn5: TExtBtn
        Left = 67
        Top = 15
        Width = 63
        Height = 16
        Align = alNone
        BevelShow = False
        CloseButton = False
        Glyph.Data = {
          9E010000424D9E0100000000000036000000280000000C0000000A0000000100
          18000000000068010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFDDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFF
          FFFFFFFFFFFFFFFF969696000000000000000000000000000000000000969696
          FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000000000000000000000000000
          00969696FFFFFFFFFFFFFFFFFFFFFFFF96969600000000000000000000000000
          0000000000969696FFFFFFFFFFFFFFFFFFFFFFFF969696000000000000000000
          000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000
          00000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF96969600
          0000000000000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF
          DDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFF}
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ExtBtn5Click
      end
      object lbCurState: TMxLabel
        Left = 47
        Top = 32
        Width = 84
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
      object lbParticleCount: TMxLabel
        Left = 47
        Top = 45
        Width = 84
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
      object RxLabel1: TMxLabel
        Left = 2
        Top = 32
        Width = 30
        Height = 13
        Caption = 'State:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object RxLabel2: TMxLabel
        Left = 2
        Top = 45
        Width = 45
        Height = 13
        Caption = 'Particles:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 173
    Height = 398
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
      F5FFFFFF07000000348BAE0D00000000FFFFFFFF000001010100000078000000
      000000001027000000010000E8211B0700000000000000000000000000000100
      0000000000000000000100000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000090000000010000000001000000000000
      000000000000348BAE0D00000000FFFFFFFF0000010101000000780000000000
      00001027000000010000B0261B07010000000000000000000000000001000000
      0000000000000001000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000900000000100000000010000000000000000
      00000000348BAE0D00000000FFFFFFFF00000101010000007800000000000000
      102700000001000074271B070200000000000000000000000000010000000000
      0000000000010000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000009000000001000000000100000000000000000000
      0000348BAE0D00000000FFFFFFFF000001010100000078000000000000001027
      00000001000038281B0703000000000000000000000000000100000000000000
      0000000100000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000090000000010000000001000000000000000000000000
      348BAE0D00000000FFFFFFFF0000010101000000780000000000000010270000
      00010000FC281B07040000000000000000000000000001000000000000000000
      0001000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000090000000010000000001000000000000000000000000348B
      AE0D00000000FFFFFFFF00000101010000007800000000000000102700000001
      0000C0291B070500000000000000000000000000010000000000000000000001
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000090000000010000000001000000000000000000000000348BAE0D
      00000000FFFFFFFF000001010100000078000000000000001027000000010000
      842A1B0706000000000000000000000000000100000000000000000000010000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000090000000010000000001000000000000000000000000}
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
    OnItemChange = tvItemsItemChange
    OnItemFocused = tvItemsItemFocused
    OnDragDrop = tvItemsDragDrop
    OnDragOver = tvItemsDragOver
    OnStartDrag = tvItemsStartDrag
    OnDblClick = tvItemsDblClick
    OnKeyPress = tvItemsKeyPress
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
      OnClick = ebRemovePSClick
    end
    object N1: TMenuItem
      Caption = '-'
      GroupIndex = 1
    end
    object Object1: TMenuItem
      Caption = 'PS'
      GroupIndex = 1
      object LoadObject1: TMenuItem
        Caption = 'Append'
        GroupIndex = 1
        OnClick = ebAppendPSClick
      end
      object DeleteObject1: TMenuItem
        Caption = 'Remove'
        GroupIndex = 1
        OnClick = ebRemovePSClick
      end
      object N2: TMenuItem
        Caption = '-'
        GroupIndex = 1
      end
      object miObjectProperties: TMenuItem
        Caption = 'Properties'
        GroupIndex = 1
        OnClick = ebPropertiesPSClick
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
    Left = 24
    Top = 8
  end
  object pmEnum: TMxPopupMenu
    Alignment = paCenter
    TrackButton = tbLeftButton
    MarginStartColor = 10921638
    MarginEndColor = 2763306
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Top = 40
    object CreateFolder1: TMenuItem
      Caption = 'Create Folder'
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object Rename1: TMenuItem
      Caption = 'Rename'
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object PS1: TMenuItem
      Caption = 'PS'
      GroupIndex = 1
      object Append1: TMenuItem
        Caption = 'Append'
        GroupIndex = 1
        OnClick = ebAppendPSClick
      end
      object Remove1: TMenuItem
        Caption = 'Remove'
        GroupIndex = 1
        OnClick = ebRemovePSClick
      end
      object N3: TMenuItem
        Caption = '-'
        GroupIndex = 1
      end
      object Properties1: TMenuItem
        Caption = 'Properties'
        GroupIndex = 1
        OnClick = ebPropertiesPSClick
      end
    end
  end
end
