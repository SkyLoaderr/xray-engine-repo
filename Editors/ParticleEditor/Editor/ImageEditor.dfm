object frmImageLib: TfrmImageLib
  Left = 247
  Top = 213
  Width = 350
  Height = 454
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
    Height = 427
    Cursor = crHSplit
    Align = alRight
  end
  object paRight: TPanel
    Left = 170
    Top = 0
    Width = 172
    Height = 427
    Align = alRight
    BevelOuter = bvLowered
    Color = 10528425
    Constraints.MinWidth = 172
    TabOrder = 0
    object Bevel2: TBevel
      Left = 1
      Top = 197
      Width = 170
      Height = 2
      Align = alTop
      Shape = bsBottomLine
    end
    object paCommand: TPanel
      Left = 1
      Top = 355
      Width = 170
      Height = 71
      Align = alBottom
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 0
      object ebClose: TExtBtn
        Left = 0
        Top = 36
        Width = 170
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Update&&Close'
        FlatAlwaysEdge = True
        OnClick = ebCloseClick
      end
      object Bevel1: TBevel
        Left = 0
        Top = 34
        Width = 170
        Height = 2
        Align = alTop
        Shape = bsLeftLine
        Style = bsRaised
      end
      object ebCheckAllCompliance: TExtBtn
        Left = 0
        Top = 17
        Width = 170
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Check All Compliance'
        FlatAlwaysEdge = True
        OnClick = ebCheckAllComplianceClick
      end
      object ebCheckSelCompliance: TExtBtn
        Left = 0
        Top = 0
        Width = 170
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Check Sel Compliance'
        FlatAlwaysEdge = True
        OnClick = ebCheckSelComplianceClick
      end
      object ebCancel: TExtBtn
        Left = 0
        Top = 53
        Width = 170
        Height = 17
        Align = alTop
        BevelShow = False
        CloseButton = False
        Caption = 'Cancel'
        FlatAlwaysEdge = True
        OnClick = ebCancelClick
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
      Height = 156
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
    Height = 427
    Align = alClient
    TabOrder = 1
    object paTextureCount: TPanel
      Left = 1
      Top = 410
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
      Height = 409
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
        F4FFFFFF0700000088CFFC00002F0000FFFFFFFF0000010101002F2078000000
        000000001027000000010054E8211B070000000000006F640000000000000120
        000000000000000000010000000000003D2074764974656D732D3E4974656D73
        2D3E47657446697273744E6F646528293B0D0A20202020656C73652009096E6F
        6465203D20464F4C4445523A3A46696E644F626A010000000001000000000000
        00000000000088CFFC00002F0000FFFFFFFF0000010101002F20780000000000
        00001027000000010054B0261B070100000000006F6400000000000001200000
        00000000000000010000000000003D2074764974656D732D3E4974656D732D3E
        47657446697273744E6F646528293B0D0A20202020656C73652009096E6F6465
        203D20464F4C4445523A3A46696E644F626A0100000000010000000000000000
        0000000088CFFC00002F0000FFFFFFFF0000010101002F207800000000000000
        102700000001005474271B070200000000006F64000000000000012000000000
        0000000000010000000000003D2074764974656D732D3E4974656D732D3E4765
        7446697273744E6F646528293B0D0A20202020656C73652009096E6F6465203D
        20464F4C4445523A3A46696E644F626A01000000000100000000000000000000
        000088CFFC00002F0000FFFFFFFF0000010101002F2078000000000000001027
        00000001005438281B070300000000006F640000000000000120000000000000
        000000010000000000003D2074764974656D732D3E4974656D732D3E47657446
        697273744E6F646528293B0D0A20202020656C73652009096E6F6465203D2046
        4F4C4445523A3A46696E644F626A010000000001000000000000000000000000
        88CFFC00002F0000FFFFFFFF0000010101002F20780000000000000010270000
        00010054FC281B070400000000006F6400000000000001200000000000000000
        00010000000000003D2074764974656D732D3E4974656D732D3E476574466972
        73744E6F646528293B0D0A20202020656C73652009096E6F6465203D20464F4C
        4445523A3A46696E644F626A01000000000100000000000000000000000088CF
        FC00002F0000FFFFFFFF0000010101002F207800000000000000102700000001
        0054C0291B070500000000006F64000000000000012000000000000000000001
        0000000000003D2074764974656D732D3E4974656D732D3E4765744669727374
        4E6F646528293B0D0A20202020656C73652009096E6F6465203D20464F4C4445
        523A3A46696E644F626A01000000000100000000000000000000000088CFFC00
        002F0000FFFFFFFF0000010101002F2078000000000000001027000000010054
        842A1B070600000000006F640000000000000120000000000000000000010000
        000000003D2074764974656D732D3E4974656D732D3E47657446697273744E6F
        646528293B0D0A20202020656C73652009096E6F6465203D20464F4C4445523A
        3A46696E644F626A010000000001000000000000000000000000}
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
