object frmBonePart: TfrmBonePart
  Left = 1280
  Top = 249
  BorderStyle = bsToolWindow
  Caption = 'Bone Part'
  ClientHeight = 439
  ClientWidth = 441
  Color = 10528425
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object RxLabel29: TLabel
    Left = 0
    Top = 5
    Width = 38
    Height = 13
    Caption = 'Part #1:'
  end
  object RxLabel1: TLabel
    Left = 224
    Top = 5
    Width = 38
    Height = 13
    Caption = 'Part #2:'
  end
  object RxLabel2: TLabel
    Left = 0
    Top = 213
    Width = 38
    Height = 13
    Caption = 'Part #3:'
  end
  object RxLabel3: TLabel
    Left = 224
    Top = 213
    Width = 38
    Height = 13
    Caption = 'Part #4:'
  end
  object ebSave: TExtBtn
    Left = 247
    Top = 420
    Width = 96
    Height = 17
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Ok'
    FlatAlwaysEdge = True
    OnClick = ebSaveClick
  end
  object ebCancel: TExtBtn
    Left = 344
    Top = 420
    Width = 96
    Height = 17
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Cancel'
    FlatAlwaysEdge = True
    OnClick = ebCancelClick
  end
  object ebCopyMode: TExtBtn
    Left = 2
    Top = 420
    Width = 50
    Height = 17
    Align = alNone
    BevelShow = False
    HotTrack = True
    HotColor = 15790320
    CloseButton = False
    GroupIndex = 1
    Caption = 'Copy'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Glyph.Data = {
      6E040000424D6E04000000000000360000002800000028000000090000000100
      18000000000038040000120B0000120B00000000000000000000FF00FF000000
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C40000000000000000000000000000000000
      00000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000999999999999999999999999999999999999C4C4C4FF00FF
      FF00FF000000999999999999999999999999999999999999C4C4C4FF00FFFF00
      FF000000999999999999999999999999999999999999C4C4C4FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FF000000
      999999999999999999999999999999999999C4C4C4FF00FFFF00FF0000009999
      99999999999999999999999999999999C4C4C4FF00FFFF00FF00000099999999
      9999999999999999999999999999C4C4C4FF00FFC4C4C4999999999999999999
      999999999999999999000000FF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C49999999999999999999999999999999999
      99000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000000000000000000000000000000000000000000000FF00FF
      FF00FF000000000000000000000000000000000000000000000000FF00FFFF00
      FF000000000000000000000000000000000000000000000000FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC4C4C4C4C4C4C4C4C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF}
    Margin = 3
    NumGlyphs = 4
    ParentFont = False
    Spacing = 3
  end
  object ebMoveMode: TExtBtn
    Left = 50
    Top = 420
    Width = 50
    Height = 17
    Align = alNone
    BevelShow = False
    HotTrack = True
    HotColor = 15790320
    CloseButton = False
    GroupIndex = 1
    Down = True
    Caption = 'Move'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Glyph.Data = {
      6E040000424D6E04000000000000360000002800000028000000090000000100
      18000000000038040000120B0000120B00000000000000000000FF00FF000000
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C40000000000000000000000000000000000
      00000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000999999999999999999999999999999999999C4C4C4FF00FF
      FF00FF000000999999999999999999999999999999999999C4C4C4FF00FFFF00
      FF000000999999999999999999999999999999999999C4C4C4FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FF000000
      999999999999999999999999999999999999C4C4C4FF00FFFF00FF0000009999
      99999999999999999999999999999999C4C4C4FF00FFFF00FF00000099999999
      9999999999999999999999999999C4C4C4FF00FFC4C4C4999999999999999999
      999999999999999999000000FF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C49999999999999999999999999999999999
      99000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000000000000000000000000000000000000000000000FF00FF
      FF00FF000000000000000000000000000000000000000000000000FF00FFFF00
      FF000000000000000000000000000000000000000000000000FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC4C4C4C4C4C4C4C4C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF}
    Margin = 3
    NumGlyphs = 4
    ParentFont = False
    Spacing = 3
  end
  object ExtBtn1: TExtBtn
    Left = 104
    Top = 420
    Width = 100
    Height = 17
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Reset To Default'
    FlatAlwaysEdge = True
    OnClick = ExtBtn1Click
  end
  object tvPart1: TElTree
    Left = 0
    Top = 24
    Width = 217
    Height = 185
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    AlwaysKeepFocus = True
    AutoCollapse = False
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    DoInplaceEdit = False
    DragAllowed = True
    DrawFocusRect = False
    ExplorerEditMode = False
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {F4FFFFFF00000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 16
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    MouseFrameSelect = True
    OwnerDrawMask = '~~@~~'
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowButtons = False
    ShowCheckboxes = True
    ShowImages = False
    ShowLeafButton = False
    ShowLines = False
    ShowRootButtons = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 0
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 16
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    BkColor = clGray
    OnDragDrop = tvPartDragDrop
    OnDragOver = tvPartDragOver
    OnStartDrag = tvPartStartDrag
  end
  object edPart1Name: TEdit
    Left = 40
    Top = 4
    Width = 176
    Height = 16
    AutoSize = False
    Color = 10526880
    TabOrder = 1
  end
  object tvPart2: TElTree
    Left = 224
    Top = 24
    Width = 217
    Height = 185
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    AlwaysKeepFocus = True
    AutoCollapse = False
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    DoInplaceEdit = False
    DragAllowed = True
    DrawFocusRect = False
    ExplorerEditMode = False
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {F4FFFFFF00000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 16
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    MouseFrameSelect = True
    OwnerDrawMask = '~~@~~'
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowButtons = False
    ShowCheckboxes = True
    ShowImages = False
    ShowLeafButton = False
    ShowLines = False
    ShowRootButtons = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 2
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 16
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    BkColor = clGray
    OnDragDrop = tvPartDragDrop
    OnDragOver = tvPartDragOver
    OnStartDrag = tvPartStartDrag
  end
  object edPart2Name: TEdit
    Left = 264
    Top = 4
    Width = 176
    Height = 16
    AutoSize = False
    Color = 10526880
    TabOrder = 3
  end
  object tvPart3: TElTree
    Left = 0
    Top = 232
    Width = 217
    Height = 185
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    AlwaysKeepFocus = True
    AutoCollapse = False
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    DoInplaceEdit = False
    DragAllowed = True
    DrawFocusRect = False
    ExplorerEditMode = False
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {F4FFFFFF00000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 16
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    MouseFrameSelect = True
    OwnerDrawMask = '~~@~~'
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowButtons = False
    ShowCheckboxes = True
    ShowImages = False
    ShowLeafButton = False
    ShowLines = False
    ShowRootButtons = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 4
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 16
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    BkColor = clGray
    OnDragDrop = tvPartDragDrop
    OnDragOver = tvPartDragOver
    OnStartDrag = tvPartStartDrag
  end
  object edPart3Name: TEdit
    Left = 40
    Top = 212
    Width = 176
    Height = 16
    AutoSize = False
    Color = 10526880
    TabOrder = 5
  end
  object tvPart4: TElTree
    Left = 224
    Top = 232
    Width = 217
    Height = 185
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    AlwaysKeepFocus = True
    AutoCollapse = False
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    DoInplaceEdit = False
    DragAllowed = True
    DrawFocusRect = False
    ExplorerEditMode = False
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    GradientSteps = 64
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {F4FFFFFF00000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 16
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    ItemIndent = 14
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 16
    MouseFrameSelect = True
    OwnerDrawMask = '~~@~~'
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowButtons = False
    ShowCheckboxes = True
    ShowImages = False
    ShowLeafButton = False
    ShowLines = False
    ShowRootButtons = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 6
    TabStop = True
    Tracking = False
    TrackColor = clBlack
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 16
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    BkColor = clGray
    OnDragDrop = tvPartDragDrop
    OnDragOver = tvPartDragOver
    OnStartDrag = tvPartStartDrag
  end
  object edPart4Name: TEdit
    Left = 264
    Top = 212
    Width = 176
    Height = 16
    AutoSize = False
    Color = 10526880
    TabOrder = 7
  end
  object fsStorage: TFormStorage
    IniFileName = 'ss.ini'
    StoredValues = <>
    Left = 65529
    Top = 65526
  end
end
