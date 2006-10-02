object frmStatistic: TfrmStatistic
  Left = 280
  Top = 312
  Width = 544
  Height = 417
  Caption = 'Statistic'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object sgData: TElTreeStringGrid
    Left = 0
    Top = 18
    Width = 536
    Height = 352
    Cursor = crDefault
    LeftPosition = 0
    ColCount = 0
    goRowSelect = True
    goEditing = False
    goTabs = True
    Align = alClient
    DockOrientation = doNoOrient
    DragCursor = crDrag
    ExplorerEditMode = False
    Flat = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GradientSteps = 64
    HeaderHeight = 17
    HeaderHotTrack = False
    HeaderSections.Data = {F4FFFFFF00000000}
    HeaderFlat = True
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 16
    Items.Data = {
      F3FFFFFF0000000080000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF83871200
      D0404B050000000000000000000000000000887C0000000000E9120000000000
      0000000000000000FFFFFFFFE8E9120090421901ACE912007067190170671901
      1800000088553B0744131E05E0E912006300000075000000180000006C421901
      8F42000000000000FFFFFFFF00000000000000000005000000F3FFFFFF000000
      0000001200FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8387120008061400000000
      00000000000000000000001200000000000001817C0000000000000000000000
      00FFFFFFFF0103000090000000040000005AF5024008E91200F8E81200647E02
      400400000040ED0240630000007500000044131E0522BB8404E0E812004DBB84
      04FFFFFFFF00000000000000000000000000F3FFFFFF0000000000001200FFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFF838712000806140000000000000000000000
      000000001200000000000001817C000000000000000000000000FFFFFFFF0103
      000090000000040000005AF5024008E91200F8E81200647E02400400000040ED
      0240630000007500000044131E0522BB8404E0E812004DBB8404FFFFFFFF0000
      0000000000000000000000F3FFFFFF0000000000001200FFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFF83871200080614000000000000000000000000000000120000
      0000000001817C000000000000000000000000FFFFFFFF010300009000000004
      0000005AF5024008E91200F8E81200647E02400400000040ED02406300000075
      00000044131E0522BB8404E0E812004DBB8404FFFFFFFF000000000000000000
      00000000F3FFFFFF0000000000001200FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      838712000806140000000000000000000000000000001200000000000001817C
      000000000000000000000000FFFFFFFF0103000090000000040000005AF50240
      08E91200F8E81200647E02400400000040ED0240630000007500000044131E05
      22BB8404E0E812004DBB8404FFFFFFFF00000000000000000000000000F3FFFF
      FF0000000000001200FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF83871200080614
      0000000000000000000000000000001200000000000001817C00000000000000
      0000000000FFFFFFFF0103000090000000040000005AF5024008E91200F8E812
      00647E02400400000040ED0240630000007500000044131E0522BB8404E0E812
      004DBB8404FFFFFFFF00000000000000000000000000}
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    ScrollbarOpposite = False
    ShowRootButtons = False
    StoragePath = '\Tree'
    TabOrder = 0
    TabStop = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 16
    BkColor = 15790320
    OnHeaderColumnClick = sgDataHeaderColumnClick
  end
  object paCaptionMem: TPanel
    Left = 0
    Top = 0
    Width = 536
    Height = 18
    Align = alTop
    Alignment = taLeftJustify
    BevelInner = bvLowered
    Caption = ' Items'
    Color = clGray
    TabOrder = 1
  end
  object Panel1: TPanel
    Left = 0
    Top = 370
    Width = 536
    Height = 20
    Align = alBottom
    Alignment = taLeftJustify
    BevelOuter = bvLowered
    TabOrder = 2
    object paQuickSearchEdit: TPanel
      Left = 257
      Top = 1
      Width = 278
      Height = 18
      Align = alClient
      BevelInner = bvLowered
      BevelOuter = bvNone
      TabOrder = 0
      OnResize = paQuickSearchEditResize
      object edQuickSearch: TEdit
        Left = 1
        Top = 1
        Width = 429
        Height = 15
        BorderStyle = bsNone
        Color = 15790320
        TabOrder = 0
        OnKeyDown = edQuickSearchKeyDown
      end
    end
    object paQuickSearchCaption: TPanel
      Left = 121
      Top = 1
      Width = 136
      Height = 18
      Align = alLeft
      Alignment = taLeftJustify
      BevelInner = bvLowered
      TabOrder = 1
      object lbQuickSearchCaption: TLabel
        Left = 2
        Top = 3
        Width = 30
        Height = 13
        Caption = '----------'
      end
    end
    object paItemsCount: TPanel
      Left = 1
      Top = 1
      Width = 120
      Height = 18
      Align = alLeft
      Alignment = taLeftJustify
      BevelInner = bvLowered
      Caption = ' Items count: 999999'
      TabOrder = 2
    end
  end
  object fsStorage: TFormStorage
    UseRegistry = True
    StoredProps.Strings = (
      'sgData.HeaderSections')
    StoredValues = <>
    Left = 8
    Top = 40
  end
end
