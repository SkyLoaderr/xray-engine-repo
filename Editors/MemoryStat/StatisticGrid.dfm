object frmStatistic: TfrmStatistic
  Left = 1458
  Top = 172
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
    Height = 353
    Cursor = crDefault
    LeftPosition = 0
    ColCount = 0
    RowCount = 0
    goEditing = False
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
  object sbStatus: TStatusBar
    Left = 0
    Top = 371
    Width = 536
    Height = 19
    Panels = <
      item
        Width = 150
      end
      item
        Width = 50
      end>
    SimplePanel = False
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
