object frmImageLib: TfrmImageLib
  Left = 824
  Top = 438
  Width = 350
  Height = 455
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
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 168
    Top = 0
    Width = 2
    Height = 428
    Cursor = crHSplit
    Align = alRight
  end
  object paRight: TPanel
    Left = 170
    Top = 0
    Width = 172
    Height = 428
    Align = alRight
    BevelOuter = bvLowered
    Color = 10528425
    Constraints.MinWidth = 172
    TabOrder = 0
    object Bevel2: TBevel
      Left = 1
      Top = 310
      Width = 170
      Height = 2
      Align = alBottom
      Shape = bsBottomLine
    end
    object Bevel4: TBevel
      Left = 1
      Top = 197
      Width = 170
      Height = 2
      Align = alTop
      Shape = bsBottomLine
    end
    object paCommand: TPanel
      Left = 1
      Top = 312
      Width = 170
      Height = 115
      Align = alBottom
      BevelInner = bvLowered
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 0
      object ebOk: TExtBtn
        Left = 1
        Top = 81
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Ok'
        FlatAlwaysEdge = True
        OnClick = ebOkClick
      end
      object Bevel1: TBevel
        Left = 1
        Top = 75
        Width = 168
        Height = 6
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
      object ebCheckAllCompliance: TExtBtn
        Left = 1
        Top = 58
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Check All Compliance'
        FlatAlwaysEdge = True
        OnClick = ebCheckAllComplianceClick
      end
      object ebCheckSelCompliance: TExtBtn
        Left = 1
        Top = 41
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Check Sel Compliance'
        FlatAlwaysEdge = True
        OnClick = ebCheckSelComplianceClick
      end
      object ebCancel: TExtBtn
        Left = 1
        Top = 98
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Cancel'
        FlatAlwaysEdge = True
        OnClick = ebCancelClick
      end
      object Bevel3: TBevel
        Left = 1
        Top = 35
        Width = 168
        Height = 6
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
      object ebExportAssociation: TExtBtn
        Left = 1
        Top = 18
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Export Association'
        FlatAlwaysEdge = True
        OnClick = ebExportAssociationClick
      end
      object ebRemoveTexture: TExtBtn
        Left = 1
        Top = 1
        Width = 168
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Remove Texture'
        FlatAlwaysEdge = True
        OnClick = ebRemoveTextureClick
      end
    end
    object paImage: TPanel
      Left = 1
      Top = 1
      Width = 170
      Height = 162
      Align = alTop
      BevelOuter = bvLowered
      Caption = '<no image>'
      Color = 10528425
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 168
        Height = 160
        Align = alClient
        Color = clGray
        ParentColor = False
        OnPaint = pbImagePaint
      end
    end
    object Panel3: TPanel
      Left = 1
      Top = 163
      Width = 170
      Height = 34
      Align = alTop
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 2
      object lbFileName: TLabel
        Left = 33
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
      end
      object RxLabel3: TLabel
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
      end
      object lbInfo: TLabel
        Left = 33
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
    object paProperties: TPanel
      Left = 1
      Top = 199
      Width = 170
      Height = 111
      Align = alClient
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 3
    end
  end
  object paLeft: TPanel
    Left = 0
    Top = 0
    Width = 168
    Height = 428
    Align = alClient
    TabOrder = 1
    object paTextureCount: TPanel
      Left = 1
      Top = 411
      Width = 166
      Height = 16
      Align = alBottom
      Alignment = taLeftJustify
      BevelOuter = bvNone
      Caption = 'Textures: 123'
      Color = 10528425
      TabOrder = 0
    end
    object tvItems: TElTree
      Left = 1
      Top = 1
      Width = 166
      Height = 410
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
        F4FFFFFF070000005C5D700600000000FFFFFFFF000001010100730078000000
        000000001027000000010073E8211B0700000000000031320000000000000106
        00000000000000000001000000000000231C0C40491C0C406E1C0C40911C0C40
        B41C0C40D61C0C40FD1C0C401C1D0C40451D0C406B1D0C40901D0C40B91D0C40
        DD1D0C40051E0C40281E0C404E1E0C406F1E0C40010000000001000000000000
        0000000000005C5D700600000000FFFFFFFF0000010101007300780000000000
        00001027000000010073B0261B07010000000000313200000000000001060000
        0000000000000001000000000000231C0C40491C0C406E1C0C40911C0C40B41C
        0C40D61C0C40FD1C0C401C1D0C40451D0C406B1D0C40901D0C40B91D0C40DD1D
        0C40051E0C40281E0C404E1E0C406F1E0C400100000000010000000000000000
        000000005C5D700600000000FFFFFFFF00000101010073007800000000000000
        102700000001007374271B070200000000003132000000000000010600000000
        000000000001000000000000231C0C40491C0C406E1C0C40911C0C40B41C0C40
        D61C0C40FD1C0C401C1D0C40451D0C406B1D0C40901D0C40B91D0C40DD1D0C40
        051E0C40281E0C404E1E0C406F1E0C4001000000000100000000000000000000
        00005C5D700600000000FFFFFFFF000001010100730078000000000000001027
        00000001007338281B0703000000000031320000000000000106000000000000
        00000001000000000000231C0C40491C0C406E1C0C40911C0C40B41C0C40D61C
        0C40FD1C0C401C1D0C40451D0C406B1D0C40901D0C40B91D0C40DD1D0C40051E
        0C40281E0C404E1E0C406F1E0C40010000000001000000000000000000000000
        5C5D700600000000FFFFFFFF0000010101007300780000000000000010270000
        00010073FC281B07040000000000313200000000000001060000000000000000
        0001000000000000231C0C40491C0C406E1C0C40911C0C40B41C0C40D61C0C40
        FD1C0C401C1D0C40451D0C406B1D0C40901D0C40B91D0C40DD1D0C40051E0C40
        281E0C404E1E0C406F1E0C400100000000010000000000000000000000005C5D
        700600000000FFFFFFFF00000101010073007800000000000000102700000001
        0073C0291B070500000000003132000000000000010600000000000000000001
        000000000000231C0C40491C0C406E1C0C40911C0C40B41C0C40D61C0C40FD1C
        0C401C1D0C40451D0C406B1D0C40901D0C40B91D0C40DD1D0C40051E0C40281E
        0C404E1E0C406F1E0C400100000000010000000000000000000000005C5D7006
        00000000FFFFFFFF000001010100730078000000000000001027000000010073
        842A1B0706000000000031320000000000000106000000000000000000010000
        00000000231C0C40491C0C406E1C0C40911C0C40B41C0C40D61C0C40FD1C0C40
        1C1D0C40451D0C406B1D0C40901D0C40B91D0C40DD1D0C40051E0C40281E0C40
        4E1E0C406F1E0C40010000000001000000000000000000000000}
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
      Images = ImageList1
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
      OnKeyDown = tvItemsKeyDown
      OnKeyPress = tvItemsKeyPress
    end
  end
  object fsStorage: TFormStorage
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paRight.Width')
    StoredValues = <>
  end
  object ImageList1: TImageList
    Height = 11
    Width = 4
    Left = 32
    Bitmap = {
      494C010101000400040004000B00FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000100000000B0000000100200000000000C002
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002E2E6E002E2E6E000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002F2F6F002F2F6F000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002E2E6E002E2E6E000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002E2E6E002E2E6E000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002E2E6E002E2E6E000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002E2E6E002E2E6E000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002E2E6E002E2E6E000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000002E2E6E002E2E6E000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      28000000100000000B00000001000100000000002C0000000000000000000000
      000000000000000000000000FFFFFF00F00000009000000090000000F0000000
      F000000090000000900000009000000090000000900000009000000000000000
      000000000000000000000000000000000000}
  end
end
