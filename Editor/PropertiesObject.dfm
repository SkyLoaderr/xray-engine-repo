object frmPropertiesObject: TfrmPropertiesObject
  Left = 205
  Top = 169
  BorderStyle = bsDialog
  Caption = 'Object properties'
  ClientHeight = 245
  ClientWidth = 336
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 336
    Height = 222
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object pcObjects: TPageControl
      Left = 0
      Top = 0
      Width = 336
      Height = 222
      ActivePage = tsMainOptions
      Align = alClient
      TabOrder = 0
      object tsMainOptions: TTabSheet
        Caption = 'Main options'
        ImageIndex = 4
        OnShow = tsMainOptionsShow
        object RxLabel2: TRxLabel
          Left = 3
          Top = 16
          Width = 67
          Height = 13
          Caption = 'Object Name:'
          ShadowColor = 15263976
        end
        object RxLabel6: TRxLabel
          Left = 3
          Top = 40
          Width = 61
          Height = 13
          Caption = 'Class Name:'
          ShadowColor = 15263976
        end
        object Bevel2: TBevel
          Left = 72
          Top = 14
          Width = 244
          Height = 17
        end
        object Bevel3: TBevel
          Left = 72
          Top = 37
          Width = 244
          Height = 17
        end
        object lbLibRef: TRxLabel
          Left = 3
          Top = 176
          Width = 29
          Height = 13
          Caption = '<...>'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          ShadowColor = 15263976
        end
        object edName: TEdit
          Left = 73
          Top = 15
          Width = 242
          Height = 15
          AutoSize = False
          BorderStyle = bsNone
          Color = 15263976
          MaxLength = 32
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          OnChange = OnModified
        end
        object edClass: TEdit
          Left = 73
          Top = 38
          Width = 242
          Height = 15
          AutoSize = False
          BorderStyle = bsNone
          Color = 15263976
          MaxLength = 32
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          OnChange = OnModified
        end
        object cbMakeUnique: TMultiObjCheck
          Left = 6
          Top = 62
          Width = 209
          Height = 17
          Caption = 'Make unique when building'
          TabOrder = 2
          OnClick = OnModified
        end
        object cbMakeDynamic: TMultiObjCheck
          Left = 6
          Top = 85
          Width = 209
          Height = 17
          Caption = 'Dynamic behaviour'
          Checked = True
          State = cbChecked
          TabOrder = 3
          OnClick = cbMakeDynamicClick
        end
      end
      object tsMeshes: TTabSheet
        Caption = 'Meshes'
        ImageIndex = 3
        OnShow = tsMeshesShow
        object ebDeleteMesh: TExtBtn
          Left = 215
          Top = 21
          Width = 113
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Delete Selected'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebDeleteMeshClick
        end
        object ebEditMesh: TExtBtn
          Left = 215
          Top = 74
          Width = 113
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Properties...'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebEditMeshClick
        end
        object ebAddMesh: TExtBtn
          Left = 215
          Top = 0
          Width = 113
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Add New'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebAddMeshClick
        end
        object ebMergeMesh: TExtBtn
          Left = 215
          Top = 42
          Width = 113
          Height = 18
          Align = alNone
          BevelShow = False
          CloseButton = False
          Caption = 'Merge Selected'
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebMergeMeshClick
        end
        object tvMeshes: TElTree
          Left = 0
          Top = 0
          Width = 214
          Height = 194
          LeftPosition = 0
          ActiveBorderType = fbtSunken
          Align = alLeft
          AlwaysKeepFocus = False
          AlwaysKeepSelection = True
          AutoExpand = False
          AutoLookup = False
          DockOrientation = doNoOrient
          DoubleBuffered = False
          AutoLineHeight = True
          BarStyle = False
          BarStyleVerticalLines = False
          BorderStyle = bsSingle
          ChangeStateImage = False
          CustomCheckboxes = False
          CustomPlusMinus = False
          DeselectChildrenOnCollapse = False
          DivLinesColor = clBtnFace
          DoInplaceEdit = False
          DraggableSections = False
          DragAllowed = False
          DragTrgDrawMode = SelColorRect
          DragType = dtDelphi
          DragImageMode = dimNever
          DrawFocusRect = True
          ExpandOnDblClick = True
          ExpandOnDragOver = False
          FilteredVisibility = False
          Flat = False
          FlatFocusedScrollbars = True
          FocusedSelectColor = clHighlight
          ForcedScrollBars = ssNone
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          FullRowSelect = True
          HeaderActiveFilterColor = clBlack
          HeaderHeight = 19
          HeaderHotTrack = False
          HeaderInvertSortArrows = False
          HeaderSections.Data = {F8FFFFFF00000000}
          HeaderFilterColor = clBtnText
          HeaderFlat = False
          HideFocusRect = False
          HideHintOnTimer = False
          HideHintOnMove = True
          HideSelectColor = clBtnFace
          HideSelection = False
          HorizontalLines = False
          HideHorzScrollBar = False
          HideVertScrollBar = False
          HorzScrollBarStyles.ActiveFlat = False
          HorzScrollBarStyles.BlendBackground = True
          HorzScrollBarStyles.Color = clBtnFace
          HorzScrollBarStyles.Flat = True
          HorzScrollBarStyles.MinThumbSize = 4
          HorzScrollBarStyles.NoDisableButtons = False
          HorzScrollBarStyles.NoSunkenThumb = False
          HorzScrollBarStyles.OwnerDraw = False
          HorzScrollBarStyles.SecondaryButtons = False
          HorzScrollBarStyles.SecondBtnKind = sbkOpposite
          HorzScrollBarStyles.ShowLeftArrows = True
          HorzScrollBarStyles.ShowRightArrows = True
          HorzScrollBarStyles.ShowTrackHint = False
          HorzScrollBarStyles.ThumbMode = etmAuto
          HorzScrollBarStyles.ThumbSize = 0
          HorzScrollBarStyles.Width = 16
          HorzScrollBarStyles.ButtonSize = 16
          HorzScrollBarStyles.UseSystemMetrics = False
          InactiveBorderType = fbtSunkenOuter
          ItemIndent = 17
          LineHeight = 19
          LinesColor = clBtnFace
          LinesStyle = psDot
          LineHintColor = clWindow
          LockHeaderHeight = False
          MainTreeColumn = 0
          MoveColumnOnDrag = False
          MoveFocusOnCollapse = False
          MultiSelect = True
          OwnerDrawMask = '~~@~~'
          PathSeparator = '\'
          PlusMinusTransparent = False
          RightAlignedText = False
          RightAlignedTree = False
          RowHotTrack = False
          RowSelect = True
          ScrollTracking = False
          SelectColumn = -1
          ShowButtons = False
          ShowColumns = False
          ShowCheckboxes = False
          ShowImages = False
          ShowLines = True
          ShowRoot = False
          ShowRootButtons = False
          SelectionMode = smUsual
          SortDir = sdAscend
          SortMode = smNone
          SortSection = 0
          SortType = stText
          StickyHeaderSections = False
          TabOrder = 0
          TabStop = True
          Tracking = True
          TrackColor = clHighlight
          UnderlineTracked = True
          VerticalLines = False
          VertScrollBarStyles.ActiveFlat = False
          VertScrollBarStyles.BlendBackground = True
          VertScrollBarStyles.Color = clBtnFace
          VertScrollBarStyles.Flat = True
          VertScrollBarStyles.MinThumbSize = 4
          VertScrollBarStyles.NoDisableButtons = False
          VertScrollBarStyles.NoSunkenThumb = False
          VertScrollBarStyles.OwnerDraw = False
          VertScrollBarStyles.SecondaryButtons = False
          VertScrollBarStyles.SecondBtnKind = sbkOpposite
          VertScrollBarStyles.ShowLeftArrows = True
          VertScrollBarStyles.ShowRightArrows = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.ThumbMode = etmAuto
          VertScrollBarStyles.ThumbSize = 0
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          TextColor = clWindowText
          BkColor = clBtnShadow
          OnItemSelectedChange = tvMeshesItemSelectedChange
          OnDblClick = tvMeshesDblClick
        end
      end
      object tsInfo: TTabSheet
        Caption = 'Information'
        ImageIndex = 1
        OnShow = tsInfoShow
        object RxLabel1: TRxLabel
          Left = 3
          Top = 0
          Width = 43
          Height = 13
          Caption = 'Vertices:'
          ShadowColor = 15263976
        end
        object RxLabel3: TRxLabel
          Left = 115
          Top = 0
          Width = 34
          Height = 13
          Caption = 'Faces:'
          ShadowColor = 15263976
        end
        object RxLabel4: TRxLabel
          Left = 3
          Top = 16
          Width = 82
          Height = 13
          Caption = 'Transform matrix:'
          ShadowColor = 15263976
        end
        object RxLabel5: TRxLabel
          Left = 4
          Top = 112
          Width = 71
          Height = 13
          Caption = 'Bounding Box:'
          ShadowColor = 15263976
        end
        object lbVertices: TRxLabel
          Left = 51
          Top = 0
          Width = 15
          Height = 13
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          ShadowColor = 15263976
        end
        object lbFaces: TRxLabel
          Left = 163
          Top = 0
          Width = 15
          Height = 13
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          ShadowColor = 15263976
        end
        object RxLabel10: TRxLabel
          Left = 35
          Top = 64
          Width = 144
          Height = 13
          Caption = 'Noa available in multiselection'
          ShadowColor = 15263976
        end
        object RxLabel11: TRxLabel
          Left = 51
          Top = 156
          Width = 144
          Height = 13
          Caption = 'Noa available in multiselection'
          ShadowColor = 15263976
        end
        object paMatrix: TPanel
          Left = 6
          Top = 31
          Width = 222
          Height = 79
          BevelOuter = bvLowered
          TabOrder = 0
          object sgMatrix: TStringGrid
            Left = 1
            Top = 18
            Width = 220
            Height = 59
            BorderStyle = bsNone
            Color = 15263976
            DefaultColWidth = 49
            DefaultRowHeight = 14
            FixedCols = 0
            RowCount = 4
            FixedRows = 0
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clGray
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
            ParentFont = False
            TabOrder = 0
            ColWidths = (
              19
              49
              49
              49
              49)
          end
          object ElHeader2: TElHeader
            Left = 1
            Top = 1
            Width = 220
            Height = 17
            Color = clBtnFace
            ActiveFilterColor = clBlack
            Flat = True
            Sections.Data = {
              F8FFFFFF05000000C4EEDC0400290000FFFFFFFF000001000000720314000000
              0000000010270000000001030000000000007203000000000000010300000000
              000000000001000302000000230001000000000000000000000000C4EEDC0400
              290000FFFFFFFF00000100000072033200000000000000102700000000010301
              0000000000720300000000000001030000000000000000000100030200000030
              0001000000000000000000000000C4EEDC0400290000FFFFFFFF000001000000
              7203320000000000000010270000000001030200000000007203000000000000
              0103000000000000000000010003020000003100010000000000000000000000
              00C4EEDC0400290000FFFFFFFF00000100000072033200000000000000102700
              0000000103030000000000720300000000000001030000000000000000000100
              0302000000320001000000000000000000000000C4EEDC0400290000FFFFFFFF
              0000010000007203320000000000000010270000000001030400000000007203
              0000000000000103000000000000000000010003020000003300010000000000
              00000000000000}
            Tracking = True
            AllowDrag = True
            Align = alTop
            DockOrientation = doNoOrient
            DoubleBuffered = False
            MoveOnDrag = False
            FilterColor = clBtnText
            LockHeight = False
            ResizeOnDrag = False
            RightAlignedText = False
            RightAlignedOrder = False
            StickySections = False
          end
        end
        object paBB: TPanel
          Left = 6
          Top = 127
          Width = 262
          Height = 64
          BevelOuter = bvLowered
          TabOrder = 1
          object sgBB: TStringGrid
            Left = 1
            Top = 18
            Width = 260
            Height = 45
            BorderStyle = bsNone
            Color = 15263976
            DefaultColWidth = 59
            DefaultRowHeight = 14
            FixedCols = 0
            RowCount = 3
            FixedRows = 0
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clGray
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
            ParentFont = False
            TabOrder = 0
            ColWidths = (
              19
              59
              59
              59
              59)
          end
          object ElHeader1: TElHeader
            Left = 1
            Top = 1
            Width = 260
            Height = 17
            Color = clBtnFace
            ActiveFilterColor = clBlack
            Flat = True
            Sections.Data = {
              F8FFFFFF05000000C4EEDC0400290000FFFFFFFF000001000000720314000000
              0000000010270000000000030400000000007203000000000000010300000000
              0000000000010003010000000001000000000000000000000000C4EEDC040029
              0000FFFFFFFF00000100000072033C0000000000000010270000000001030000
              0000000072030000000000000103000000000000000000010003080000004D69
              6E696D756D0001000000000000000000000000C4EEDC0400290000FFFFFFFF00
              000100000072033C000000000000001027000000000103010000000000720300
              00000000000103000000000000000000010003080000004D6178696D756D0001
              000000000000000000000000C4EEDC0400290000FFFFFFFF0000010000007203
              3C00000000000000102700000000010302000000000072030000000000000103
              0000000000000000000100030500000053697A65000100000000000000000000
              0000C4EEDC0400290000FFFFFFFF00000100000072033C000000000000001027
              0000000001030300000000007203000000000000010300000000000000000001
              00030700000043656E7465720001000000000000000000000000}
            Tracking = True
            AllowDrag = True
            Align = alTop
            DockOrientation = doNoOrient
            DoubleBuffered = False
            MoveOnDrag = False
            FilterColor = clBtnText
            LockHeight = False
            ResizeOnDrag = False
            RightAlignedText = False
            RightAlignedOrder = False
            StickySections = False
          end
        end
      end
      object tsTextures: TTabSheet
        Caption = 'Textures'
        ImageIndex = 2
        OnShow = tsTexturesShow
        object RxLabel7: TRxLabel
          Left = 191
          Top = 8
          Width = 33
          Height = 13
          Caption = 'Width:'
          ShadowColor = 15263976
        end
        object RxLabel8: TRxLabel
          Left = 191
          Top = 24
          Width = 36
          Height = 13
          Caption = 'Height:'
          ShadowColor = 15263976
        end
        object RxLabel9: TRxLabel
          Left = 191
          Top = 40
          Width = 32
          Height = 13
          Caption = 'Alpha:'
          ShadowColor = 15263976
        end
        object lbWidth: TRxLabel
          Left = 239
          Top = 8
          Width = 15
          Height = 13
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          ShadowColor = 15263976
        end
        object lbHeight: TRxLabel
          Left = 239
          Top = 24
          Width = 15
          Height = 13
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          ShadowColor = 15263976
        end
        object lbAlpha: TRxLabel
          Left = 239
          Top = 40
          Width = 15
          Height = 13
          Caption = '...'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          ShadowColor = 15263976
        end
        object tvTextures: TElTree
          Left = 0
          Top = 0
          Width = 185
          Height = 193
          LeftPosition = 0
          ActiveBorderType = fbtSunken
          AlwaysKeepFocus = False
          AlwaysKeepSelection = True
          AutoExpand = False
          AutoLookup = False
          DockOrientation = doNoOrient
          DoubleBuffered = False
          AutoLineHeight = True
          BarStyle = False
          BarStyleVerticalLines = False
          BorderStyle = bsSingle
          ChangeStateImage = False
          CustomCheckboxes = False
          CustomPlusMinus = False
          DeselectChildrenOnCollapse = False
          DivLinesColor = clBtnFace
          DoInplaceEdit = False
          DraggableSections = False
          DragAllowed = False
          DragTrgDrawMode = SelColorRect
          DragType = dtDelphi
          DragImageMode = dimNever
          DrawFocusRect = True
          ExpandOnDblClick = True
          ExpandOnDragOver = False
          FilteredVisibility = False
          Flat = False
          FlatFocusedScrollbars = True
          FocusedSelectColor = clHighlight
          ForcedScrollBars = ssNone
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          FullRowSelect = True
          HeaderActiveFilterColor = clBlack
          HeaderHeight = 19
          HeaderHotTrack = False
          HeaderInvertSortArrows = False
          HeaderSections.Data = {F8FFFFFF00000000}
          HeaderFilterColor = clBtnText
          HeaderFlat = False
          HideFocusRect = False
          HideHintOnTimer = False
          HideHintOnMove = True
          HideSelectColor = clBtnFace
          HideSelection = False
          HorizontalLines = False
          HideHorzScrollBar = False
          HideVertScrollBar = False
          HorzScrollBarStyles.ActiveFlat = False
          HorzScrollBarStyles.BlendBackground = True
          HorzScrollBarStyles.Color = clBtnFace
          HorzScrollBarStyles.Flat = True
          HorzScrollBarStyles.MinThumbSize = 4
          HorzScrollBarStyles.NoDisableButtons = False
          HorzScrollBarStyles.NoSunkenThumb = False
          HorzScrollBarStyles.OwnerDraw = False
          HorzScrollBarStyles.SecondaryButtons = False
          HorzScrollBarStyles.SecondBtnKind = sbkOpposite
          HorzScrollBarStyles.ShowLeftArrows = True
          HorzScrollBarStyles.ShowRightArrows = True
          HorzScrollBarStyles.ShowTrackHint = False
          HorzScrollBarStyles.ThumbMode = etmAuto
          HorzScrollBarStyles.ThumbSize = 0
          HorzScrollBarStyles.Width = 16
          HorzScrollBarStyles.ButtonSize = 16
          HorzScrollBarStyles.UseSystemMetrics = False
          InactiveBorderType = fbtSunkenOuter
          ItemIndent = 17
          LineHeight = 19
          LinesColor = clBtnFace
          LinesStyle = psDot
          LineHintColor = clWindow
          LockHeaderHeight = False
          MainTreeColumn = 0
          MoveColumnOnDrag = False
          MoveFocusOnCollapse = False
          MultiSelect = False
          OwnerDrawMask = '~~@~~'
          PathSeparator = '\'
          PlusMinusTransparent = False
          RightAlignedText = False
          RightAlignedTree = False
          RowHotTrack = False
          RowSelect = True
          ScrollTracking = False
          SelectColumn = -1
          ShowButtons = False
          ShowColumns = False
          ShowCheckboxes = False
          ShowImages = False
          ShowLines = True
          ShowRoot = False
          ShowRootButtons = False
          SelectionMode = smUsual
          SortDir = sdAscend
          SortMode = smNone
          SortSection = 0
          SortType = stText
          StickyHeaderSections = False
          TabOrder = 0
          TabStop = True
          Tracking = True
          TrackColor = clHighlight
          UnderlineTracked = True
          VerticalLines = False
          VertScrollBarStyles.ActiveFlat = False
          VertScrollBarStyles.BlendBackground = True
          VertScrollBarStyles.Color = clBtnFace
          VertScrollBarStyles.Flat = True
          VertScrollBarStyles.MinThumbSize = 4
          VertScrollBarStyles.NoDisableButtons = False
          VertScrollBarStyles.NoSunkenThumb = False
          VertScrollBarStyles.OwnerDraw = False
          VertScrollBarStyles.SecondaryButtons = False
          VertScrollBarStyles.SecondBtnKind = sbkOpposite
          VertScrollBarStyles.ShowLeftArrows = True
          VertScrollBarStyles.ShowRightArrows = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.ThumbMode = etmAuto
          VertScrollBarStyles.ThumbSize = 0
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          TextColor = clWindowText
          BkColor = clBtnShadow
          OnItemSelectedChange = tvTexturesItemSelectedChange
        end
        object paImage: TPanel
          Left = 192
          Top = 62
          Width = 130
          Height = 130
          BevelOuter = bvLowered
          Caption = '<no image>'
          TabOrder = 1
          object im: TPaintBox
            Left = 1
            Top = 1
            Width = 128
            Height = 128
            Align = alClient
            OnPaint = imPaint
          end
        end
      end
      object tsScript: TTabSheet
        Caption = 'LTX Script'
        object mmScript: TMemo
          Left = 0
          Top = 0
          Width = 328
          Height = 194
          Hint = 'Sample LTX-script:'#13#10'[object_name]'#13#10'prop1=...'#13#10'prop2=...'
          Align = alClient
          Color = 16646143
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          OnChange = OnModified
        end
      end
    end
  end
  object paBottom: TPanel
    Left = 0
    Top = 222
    Width = 336
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 168
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 250
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object odMesh: TOpenDialog
    Filter = 'Mesh file (*.mesh)|*.mesh'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Open Mesh file'
    Left = 298
    Top = 187
  end
end
