object frmPropertiesObject: TfrmPropertiesObject
  Left = 369
  Top = 357
  BorderStyle = bsDialog
  Caption = 'Object properties'
  ClientHeight = 365
  ClientWidth = 372
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
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
    Width = 372
    Height = 342
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object pcObjects: TPageControl
      Left = 0
      Top = 0
      Width = 372
      Height = 342
      ActivePage = tsSAnimation
      Align = alClient
      MultiLine = True
      TabOrder = 0
      OnChange = pcObjectsChange
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
        object Bevel2: TBevel
          Left = 72
          Top = 14
          Width = 328
          Height = 17
        end
        object lbLibRef: TRxLabel
          Left = 3
          Top = 59
          Width = 29
          Height = 13
          Caption = '<...>'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          ShadowColor = 15263976
        end
        object edName: TEdit
          Left = 73
          Top = 15
          Width = 327
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
        object cbMakeDynamic: TMultiObjCheck
          Left = 4
          Top = 36
          Width = 209
          Height = 17
          Caption = 'Dynamic behaviour ("dynamic object")'
          Checked = True
          State = cbChecked
          TabOrder = 1
          OnClick = cbMakeDynamicClick
        end
      end
      object tsInfo: TTabSheet
        Caption = 'Transformation'
        ImageIndex = 1
        OnShow = tsInfoShow
        object RxLabel4: TRxLabel
          Left = 0
          Top = 10
          Width = 52
          Height = 13
          Caption = 'Transform:'
          ShadowColor = 15263976
        end
        object RxLabel5: TRxLabel
          Left = 1
          Top = 110
          Width = 71
          Height = 13
          Caption = 'Bounding Box:'
          ShadowColor = 15263976
        end
        object RxLabel10: TRxLabel
          Left = 32
          Top = 58
          Width = 144
          Height = 13
          Caption = 'Noa available in multiselection'
          ShadowColor = 15263976
        end
        object RxLabel11: TRxLabel
          Left = 48
          Top = 154
          Width = 144
          Height = 13
          Caption = 'Noa available in multiselection'
          ShadowColor = 15263976
        end
        object paMatrix: TPanel
          Left = 3
          Top = 25
          Width = 222
          Height = 64
          BevelOuter = bvLowered
          TabOrder = 0
          object sgTransform: TStringGrid
            Left = 1
            Top = 18
            Width = 222
            Height = 46
            BorderStyle = bsNone
            Color = 15263976
            ColCount = 4
            DefaultColWidth = 49
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
            ScrollBars = ssNone
            TabOrder = 0
            ColWidths = (
              69
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
              F8FFFFFF04000000BC11640400000000FFFFFFFF000001000000064046000000
              0000000010270000000001400000000000000740000000000000014000000000
              000000000001000002000000230001000000000000000000000000BC11640400
              000000FFFFFFFF00000100000006403200000000000000102700000000014001
              0000000000074000000000000001400000000000000000000100000200000058
              0001000000000000000000000000BC11640400000000FFFFFFFF000001000000
              0640320000000000000010270000000001400200000000000740000000000000
              0140000000000000000000010000020000005900010000000000000000000000
              00BC11640400000000FFFFFFFF00000100000006403200000000000000102700
              0000000140030000000000074000000000000001400000000000000000000100
              00020000005A0001000000000000000000000000}
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
          Left = 3
          Top = 125
          Width = 262
          Height = 64
          BevelOuter = bvLowered
          TabOrder = 1
          object sgBB: TStringGrid
            Left = 1
            Top = 18
            Width = 262
            Height = 47
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
            ScrollBars = ssNone
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
              F8FFFFFF05000000BC11640400000000FFFFFFFF000001000000064014000000
              0000000010270000000000400400000000000740000000000000014000000000
              0000000000010000010000000001000000000000000000000000BC1164040000
              0000FFFFFFFF00000100000006403C0000000000000010270000000001400000
              0000000007400000000000000140000000000000000000010000080000004D69
              6E696D756D0001000000000000000000000000BC11640400000000FFFFFFFF00
              000100000006403C000000000000001027000000000140010000000000074000
              00000000000140000000000000000000010000080000004D6178696D756D0001
              000000000000000000000000BC11640400000000FFFFFFFF0000010000000640
              3C00000000000000102700000000014002000000000007400000000000000140
              0000000000000000000100000500000053697A65000100000000000000000000
              0000BC11640400000000FFFFFFFF00000100000006403C000000000000001027
              0000000001400300000000000740000000000000014000000000000000000001
              00000700000043656E7465720001000000000000000000000000}
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
        object stNumericSet: TStaticText
          Left = 231
          Top = 25
          Width = 93
          Height = 16
          Alignment = taCenter
          AutoSize = False
          BorderStyle = sbsSunken
          Caption = 'Numeric Set...'
          Enabled = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 2
          OnMouseDown = ebNumericSetMouseDown
        end
      end
      object tsMeshes: TTabSheet
        Caption = 'Meshes'
        ImageIndex = 3
        OnShow = tsMeshesShow
        object tvMeshes: TElTree
          Left = 0
          Top = 0
          Width = 227
          Height = 296
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
          Font.Color = clBlack
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
          OnItemSelectedChange = tvMeshesItemSelectedChange
        end
        object Panel2: TPanel
          Left = 227
          Top = 0
          Width = 137
          Height = 296
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 1
          object GroupBox4: TGroupBox
            Left = 0
            Top = 0
            Width = 136
            Height = 53
            Align = alTop
            Caption = ' Summary Info '
            TabOrder = 0
            object RxLabel1: TRxLabel
              Left = 5
              Top = 18
              Width = 43
              Height = 13
              Caption = 'Vertices:'
              ShadowColor = 15263976
            end
            object RxLabel3: TRxLabel
              Left = 5
              Top = 32
              Width = 34
              Height = 13
              Caption = 'Faces:'
              ShadowColor = 15263976
            end
            object lbVertices: TRxLabel
              Left = 67
              Top = 18
              Width = 11
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 15263976
            end
            object lbFaces: TRxLabel
              Left = 67
              Top = 32
              Width = 11
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 15263976
            end
          end
        end
      end
      object tsSurfaces: TTabSheet
        Caption = 'Surfaces'
        ImageIndex = 2
        OnShow = tsSurfacesShow
        object tvSurfaces: TElTree
          Left = 0
          Top = 0
          Width = 210
          Height = 296
          LeftPosition = 0
          ActiveBorderType = fbtSunken
          Align = alClient
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
          Font.Color = clBlack
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
          ParentFont = False
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
          OnItemSelectedChange = tvSurfacesItemSelectedChange
        end
        object paTex_Surf: TPanel
          Left = 210
          Top = 0
          Width = 154
          Height = 296
          Align = alRight
          BevelOuter = bvNone
          TabOrder = 1
          object paTexture: TPanel
            Left = 1
            Top = 1
            Width = 152
            Height = 277
            BevelOuter = bvNone
            TabOrder = 0
            Visible = False
            object GroupBox2: TGroupBox
              Left = 0
              Top = 0
              Width = 152
              Height = 277
              Align = alClient
              Caption = ' Texture Properties '
              TabOrder = 0
              object RxLabel7: TRxLabel
                Left = 4
                Top = 18
                Width = 33
                Height = 13
                Caption = 'Width:'
                ShadowColor = 15263976
              end
              object RxLabel8: TRxLabel
                Left = 4
                Top = 33
                Width = 36
                Height = 13
                Caption = 'Height:'
                ShadowColor = 15263976
              end
              object RxLabel9: TRxLabel
                Left = 4
                Top = 48
                Width = 32
                Height = 13
                Caption = 'Alpha:'
                ShadowColor = 15263976
              end
              object lbWidth: TRxLabel
                Left = 45
                Top = 18
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
                Left = 45
                Top = 33
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
                Left = 45
                Top = 48
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
              object paImage: TPanel
                Left = 3
                Top = 67
                Width = 147
                Height = 147
                BevelOuter = bvLowered
                Caption = '<image>'
                TabOrder = 0
                object im: TPaintBox
                  Left = 1
                  Top = 1
                  Width = 145
                  Height = 145
                  Align = alClient
                  OnPaint = imPaint
                end
              end
            end
          end
          object paSurface: TPanel
            Left = 0
            Top = 0
            Width = 154
            Height = 277
            BevelOuter = bvNone
            TabOrder = 1
            Visible = False
            object GroupBox1: TGroupBox
              Left = 0
              Top = 0
              Width = 154
              Height = 277
              Align = alClient
              Caption = ' Surface Properties '
              TabOrder = 0
              object RxLabel18: TRxLabel
                Left = 4
                Top = 29
                Width = 66
                Height = 13
                Caption = 'Double Sided'
                ShadowColor = 15263976
              end
              object lbSurfSideFlag: TRxLabel
                Left = 95
                Top = 29
                Width = 11
                Height = 13
                Caption = '...'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clMaroon
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentFont = False
                ShadowColor = 15263976
              end
              object RxLabel35: TRxLabel
                Left = 5
                Top = 15
                Width = 34
                Height = 13
                Caption = 'Faces:'
                ShadowColor = 15263976
              end
              object lbSurfFaces: TRxLabel
                Left = 95
                Top = 15
                Width = 11
                Height = 13
                Caption = '...'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clMaroon
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentFont = False
                ShadowColor = 15263976
              end
              object ebSelectShader: TExtBtn
                Left = 4
                Top = 45
                Width = 40
                Height = 16
                Align = alNone
                BevelShow = False
                CloseButton = False
                Caption = 'Shader'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentFont = False
                Transparent = False
                FlatAlwaysEdge = True
                OnClick = ebSelectShaderClick
              end
              object lbShader: TRxLabel
                Left = 44
                Top = 46
                Width = 107
                Height = 13
                Alignment = taCenter
                AutoSize = False
                Caption = '...'
                Color = clBtnFace
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clNavy
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentColor = False
                ParentFont = False
                ShadowColor = 15263976
              end
              object ebSelectTexture: TExtBtn
                Left = 4
                Top = 61
                Width = 40
                Height = 16
                Align = alNone
                BevelShow = False
                CloseButton = False
                Caption = 'Texture'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentFont = False
                Transparent = False
                FlatAlwaysEdge = True
                OnClick = ebSelectTextureClick
              end
              object lbTexture: TRxLabel
                Left = 44
                Top = 64
                Width = 107
                Height = 109
                Alignment = taCenter
                AutoSize = False
                Caption = '...'
                Color = clBtnFace
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clNavy
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentColor = False
                ParentFont = False
                ShadowColor = 15263976
                WordWrap = True
              end
              object gbFVF: TGroupBox
                Left = 2
                Top = 174
                Width = 150
                Height = 101
                Align = alBottom
                Caption = ' Surface FVF '
                TabOrder = 0
                object RxLabel14: TRxLabel
                  Left = 4
                  Top = 82
                  Width = 68
                  Height = 13
                  Caption = 'UV'#39's channel:'
                  ShadowColor = 15263976
                end
                object lbSurfFVF_TCCount: TRxLabel
                  Left = 90
                  Top = 82
                  Width = 11
                  Height = 13
                  Caption = '...'
                  Font.Charset = DEFAULT_CHARSET
                  Font.Color = clMaroon
                  Font.Height = -11
                  Font.Name = 'MS Sans Serif'
                  Font.Style = []
                  ParentFont = False
                  ShadowColor = 15263976
                end
                object RxLabel6: TRxLabel
                  Left = 4
                  Top = 15
                  Width = 26
                  Height = 13
                  Caption = 'XYZ:'
                  ShadowColor = 15263976
                end
                object RxLabel12: TRxLabel
                  Left = 4
                  Top = 28
                  Width = 38
                  Height = 13
                  Caption = 'Normal:'
                  ShadowColor = 15263976
                end
                object RxLabel15: TRxLabel
                  Left = 4
                  Top = 41
                  Width = 38
                  Height = 13
                  Caption = 'Diffuse:'
                  ShadowColor = 15263976
                end
                object RxLabel16: TRxLabel
                  Left = 4
                  Top = 54
                  Width = 47
                  Height = 13
                  Caption = 'Specular:'
                  ShadowColor = 15263976
                end
                object RxLabel17: TRxLabel
                  Left = 4
                  Top = 69
                  Width = 53
                  Height = 13
                  Caption = 'XYZRHW:'
                  ShadowColor = 15263976
                end
                object lbSurfFVF_XYZ: TRxLabel
                  Left = 90
                  Top = 15
                  Width = 11
                  Height = 13
                  Caption = '...'
                  Font.Charset = DEFAULT_CHARSET
                  Font.Color = clMaroon
                  Font.Height = -11
                  Font.Name = 'MS Sans Serif'
                  Font.Style = []
                  ParentFont = False
                  ShadowColor = 15263976
                end
                object lbSurfFVF_Normal: TRxLabel
                  Left = 90
                  Top = 28
                  Width = 11
                  Height = 13
                  Caption = '...'
                  Font.Charset = DEFAULT_CHARSET
                  Font.Color = clMaroon
                  Font.Height = -11
                  Font.Name = 'MS Sans Serif'
                  Font.Style = []
                  ParentFont = False
                  ShadowColor = 15263976
                end
                object lbSurfFVF_Diffuse: TRxLabel
                  Left = 90
                  Top = 41
                  Width = 11
                  Height = 13
                  Caption = '...'
                  Font.Charset = DEFAULT_CHARSET
                  Font.Color = clMaroon
                  Font.Height = -11
                  Font.Name = 'MS Sans Serif'
                  Font.Style = []
                  ParentFont = False
                  ShadowColor = 15263976
                end
                object lbSurfFVF_Specular: TRxLabel
                  Left = 90
                  Top = 54
                  Width = 11
                  Height = 13
                  Caption = '...'
                  Font.Charset = DEFAULT_CHARSET
                  Font.Color = clMaroon
                  Font.Height = -11
                  Font.Name = 'MS Sans Serif'
                  Font.Style = []
                  ParentFont = False
                  ShadowColor = 15263976
                end
                object lbSurfFVF_XYZRHW: TRxLabel
                  Left = 90
                  Top = 69
                  Width = 11
                  Height = 13
                  Caption = '...'
                  Font.Charset = DEFAULT_CHARSET
                  Font.Color = clMaroon
                  Font.Height = -11
                  Font.Name = 'MS Sans Serif'
                  Font.Style = []
                  ParentFont = False
                  ShadowColor = 15263976
                end
              end
            end
          end
          object Panel5: TPanel
            Left = 0
            Top = 279
            Width = 154
            Height = 17
            Align = alBottom
            BevelOuter = bvNone
            TabOrder = 2
            object ExtBtn1: TExtBtn
              Left = 4
              Top = 0
              Width = 70
              Height = 17
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Expand Tree'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ExtBtn1Click
            end
            object ExtBtn2: TExtBtn
              Left = 81
              Top = 0
              Width = 70
              Height = 17
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Collapse Tree'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ExtBtn2Click
            end
          end
        end
      end
      object tsOAnimation: TTabSheet
        Caption = 'Object Animation'
        ImageIndex = 5
        OnShow = tsOAnimationShow
        object gbOMotion: TGroupBox
          Left = 224
          Top = 0
          Width = 140
          Height = 235
          Align = alRight
          Caption = ' Current Motion '
          TabOrder = 0
          Visible = False
          object RxLabel20: TRxLabel
            Left = 5
            Top = 13
            Width = 33
            Height = 13
            Caption = 'Name:'
            ShadowColor = 15263976
          end
          object lbOMotionName: TRxLabel
            Left = 51
            Top = 13
            Width = 11
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 15263976
          end
          object RxLabel23: TRxLabel
            Left = 5
            Top = 27
            Width = 39
            Height = 13
            Caption = 'Frames:'
            ShadowColor = 15263976
          end
          object lbOMotionFrames: TRxLabel
            Left = 51
            Top = 27
            Width = 11
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 15263976
          end
          object lbOMotionFPS: TRxLabel
            Left = 51
            Top = 41
            Width = 11
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 15263976
          end
          object RxLabel28: TRxLabel
            Left = 5
            Top = 41
            Width = 25
            Height = 13
            Caption = 'FPS:'
            ShadowColor = 15263976
          end
        end
        object tvOMotions: TElTree
          Left = 0
          Top = 0
          Width = 223
          Height = 235
          LeftPosition = 0
          ActiveBorderType = fbtSunken
          Align = alLeft
          AlwaysKeepFocus = True
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
          Font.Color = clBlack
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
          ShowCheckboxes = True
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
          TabOrder = 1
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
          OnItemChange = tvOMotionsItemChange
          OnTryEdit = tvOMotionsTryEdit
          OnItemSelectedChange = tvOMotionsItemSelectedChange
        end
        object Panel4: TPanel
          Left = 0
          Top = 235
          Width = 364
          Height = 61
          Align = alBottom
          BevelOuter = bvNone
          TabOrder = 2
          object GroupBox3: TGroupBox
            Left = 0
            Top = 0
            Width = 223
            Height = 61
            Align = alClient
            Caption = ' Summary Info '
            TabOrder = 0
            object RxLabel19: TRxLabel
              Left = 5
              Top = 13
              Width = 67
              Height = 13
              Caption = 'Motion count:'
              ShadowColor = 15263976
            end
            object lbOMotionCount: TRxLabel
              Left = 75
              Top = 13
              Width = 11
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 15263976
            end
            object RxLabel21: TRxLabel
              Left = 5
              Top = 27
              Width = 70
              Height = 13
              Caption = 'Active Motion:'
              ShadowColor = 15263976
            end
            object lbActiveOMotion: TRxLabel
              Left = 75
              Top = 27
              Width = 11
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 15263976
            end
          end
          object gbOMotionCommand: TGroupBox
            Left = 223
            Top = 0
            Width = 141
            Height = 61
            Align = alRight
            Caption = ' Motions '
            TabOrder = 1
            object ebOMotionAppend: TExtBtn
              Left = 4
              Top = 13
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Append'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebOAppendMotionClick
            end
            object ebOMotionRename: TExtBtn
              Left = 71
              Top = 13
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Rename'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebORenameMotionClick
            end
            object ebOMotionDelete: TExtBtn
              Left = 4
              Top = 28
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Delete'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebODeleteMotionClick
            end
            object ebOMotionClear: TExtBtn
              Left = 71
              Top = 28
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Clear'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebOMotionClearClick
            end
            object ebOMotionSave: TExtBtn
              Left = 4
              Top = 43
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Save'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebOMotionSaveClick
            end
            object ebOMotionLoad: TExtBtn
              Left = 71
              Top = 43
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Load'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebOMotionLoadClick
            end
          end
        end
      end
      object tsSAnimation: TTabSheet
        Caption = 'Skeletal Animation'
        ImageIndex = 6
        OnShow = tsSAnimationShow
        object tvSMotions: TElTree
          Left = 0
          Top = 0
          Width = 223
          Height = 220
          LeftPosition = 0
          ActiveBorderType = fbtSunken
          Align = alLeft
          AlwaysKeepFocus = True
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
          Font.Color = clBlack
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
          ShowCheckboxes = True
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
          OnItemChange = tvSMotionsItemChange
          OnTryEdit = tvSMotionsTryEdit
          OnItemSelectedChange = tvSMotionsItemSelectedChange
        end
        object gbSMotion: TGroupBox
          Left = 225
          Top = 0
          Width = 139
          Height = 220
          Align = alRight
          Caption = ' Current Motion '
          TabOrder = 1
          Visible = False
          object RxLabel29: TRxLabel
            Left = 5
            Top = 13
            Width = 33
            Height = 13
            Caption = 'Name:'
            ShadowColor = 15263976
          end
          object lbSMotionName: TRxLabel
            Left = 51
            Top = 13
            Width = 11
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 15263976
          end
          object RxLabel24: TRxLabel
            Left = 5
            Top = 25
            Width = 39
            Height = 13
            Caption = 'Frames:'
            ShadowColor = 15263976
          end
          object lbSMotionFrames: TRxLabel
            Left = 51
            Top = 25
            Width = 11
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 15263976
          end
          object RxLabel27: TRxLabel
            Left = 5
            Top = 37
            Width = 25
            Height = 13
            Caption = 'FPS:'
            ShadowColor = 15263976
          end
          object lbSMotionFPS: TRxLabel
            Left = 51
            Top = 37
            Width = 11
            Height = 13
            Caption = '...'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 15263976
          end
          object Bevel1: TBevel
            Left = 2
            Top = 52
            Width = 135
            Height = 2
          end
          object RxLabel30: TRxLabel
            Left = 5
            Top = 139
            Width = 36
            Height = 13
            Caption = 'Speed:'
            ShadowColor = 15263976
          end
          object RxLabel31: TRxLabel
            Left = 5
            Top = 156
            Width = 39
            Height = 13
            Caption = 'Accrue:'
            ShadowColor = 15263976
          end
          object RxLabel32: TRxLabel
            Left = 5
            Top = 174
            Width = 33
            Height = 13
            Caption = 'Falloff:'
            ShadowColor = 15263976
          end
          object seSMotionSpeed: TMultiObjSpinEdit
            Left = 44
            Top = 137
            Width = 92
            Height = 18
            Hint = 
              'speed scale (in units), 1.0 = full speed, 0.0 = not playing at a' +
              'll'
            LWSensitivity = 1
            ButtonKind = bkLightWave
            MaxValue = 1000
            ValueType = vtFloat
            Value = 1
            AutoSize = False
            TabOrder = 0
            OnLWChange = seSMotionSpeedLWChange
            OnChange = seSMotionChange
            OnExit = seSMotionChange
            OnKeyDown = seSMotionSpeedKeyDown
          end
          object seSMotionAccrue: TMultiObjSpinEdit
            Left = 44
            Top = 155
            Width = 92
            Height = 18
            Hint = 'accruation speed'
            LWSensitivity = 1
            ButtonKind = bkLightWave
            MaxValue = 10
            ValueType = vtFloat
            Value = 2
            AutoSize = False
            TabOrder = 1
            OnChange = seSMotionChange
          end
          object seSMotionFalloff: TMultiObjSpinEdit
            Left = 44
            Top = 173
            Width = 92
            Height = 18
            Hint = 
              'CYCLE: falloff speed (of PREVIOUS animation)'#13#10'FX: falloff speed ' +
              '(of THIS animation)'
            LWSensitivity = 1
            ButtonKind = bkLightWave
            MaxValue = 10
            ValueType = vtFloat
            Value = 2
            AutoSize = False
            TabOrder = 2
            OnChange = seSMotionChange
          end
          object pcAnimType: TPageControl
            Left = 2
            Top = 55
            Width = 135
            Height = 81
            ActivePage = tsCycle
            Style = tsFlatButtons
            TabOrder = 3
            OnChange = seSMotionChange
            object tsCycle: TTabSheet
              Caption = 'Cycle'
              object RxLabel39: TRxLabel
                Left = 0
                Top = -2
                Width = 51
                Height = 13
                Caption = 'Bone part:'
                ShadowColor = 15263976
              end
              object cbSBonePart: TComboBox
                Left = 12
                Top = 12
                Width = 118
                Height = 19
                Style = csOwnerDrawVariable
                ItemHeight = 13
                TabOrder = 0
                OnChange = seSMotionChange
              end
              object cbSMotionStopAtEnd: TMultiObjCheck
                Left = 0
                Top = 35
                Width = 75
                Height = 15
                Alignment = taLeftJustify
                Caption = 'Stop at end'
                TabOrder = 1
                OnClick = seSMotionChange
              end
            end
            object tsFX: TTabSheet
              Caption = 'FX'
              ImageIndex = 1
              object RxLabel40: TRxLabel
                Left = 0
                Top = -2
                Width = 54
                Height = 13
                Caption = 'Start bone:'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentFont = False
                ShadowColor = 15263976
              end
              object RxLabel34: TRxLabel
                Left = 0
                Top = 35
                Width = 35
                Height = 13
                Caption = 'Power:'
                ShadowColor = 15263976
              end
              object cbSStartMotionBone: TComboBox
                Left = 12
                Top = 12
                Width = 118
                Height = 19
                Style = csOwnerDrawVariable
                ItemHeight = 13
                Sorted = True
                TabOrder = 0
                OnChange = seSMotionChange
              end
              object seSMotionPower: TMultiObjSpinEdit
                Left = 38
                Top = 32
                Width = 91
                Height = 18
                Hint = 
                  'CYCLE: power must ALWAYS be 1.0'#13#10'FX: power of modification of cu' +
                  'rrent cycle+fx'
                LWSensitivity = 1
                ButtonKind = bkLightWave
                ValueType = vtFloat
                Value = 1
                AutoSize = False
                TabOrder = 1
                OnChange = seSMotionChange
              end
            end
          end
        end
        object paSAnimBottom: TPanel
          Left = 0
          Top = 220
          Width = 364
          Height = 76
          Align = alBottom
          BevelOuter = bvNone
          TabOrder = 2
          object GroupBox5: TGroupBox
            Left = 0
            Top = 0
            Width = 223
            Height = 76
            Align = alClient
            Caption = ' Summary Info '
            TabOrder = 0
            object RxLabel22: TRxLabel
              Left = 5
              Top = 27
              Width = 67
              Height = 13
              Caption = 'Motion count:'
              ShadowColor = 15263976
            end
            object lbSMotionCount: TRxLabel
              Left = 75
              Top = 27
              Width = 11
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 15263976
            end
            object RxLabel26: TRxLabel
              Left = 5
              Top = 41
              Width = 70
              Height = 13
              Caption = 'Active Motion:'
              ShadowColor = 15263976
            end
            object lbActiveSMotion: TRxLabel
              Left = 75
              Top = 41
              Width = 11
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 15263976
            end
            object RxLabel37: TRxLabel
              Left = 5
              Top = 13
              Width = 60
              Height = 13
              Caption = 'Bone count:'
              ShadowColor = 15263976
            end
            object lbSBoneCount: TRxLabel
              Left = 75
              Top = 13
              Width = 11
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 15263976
            end
          end
          object gbSMotionCommand: TGroupBox
            Left = 223
            Top = 0
            Width = 141
            Height = 76
            Align = alRight
            Caption = ' Motions '
            TabOrder = 1
            object ebSAppendMotion: TExtBtn
              Left = 4
              Top = 13
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Append'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebSAppendMotionClick
            end
            object ebSRenameMotion: TExtBtn
              Left = 71
              Top = 13
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Rename'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebSRenameMotionClick
            end
            object ebSDeleteMotion: TExtBtn
              Left = 71
              Top = 28
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Delete'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebSDeleteMotionClick
            end
            object ebSMotionClear: TExtBtn
              Left = 4
              Top = 43
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Clear'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebSMotionClearClick
            end
            object ebSMotionSave: TExtBtn
              Left = 4
              Top = 58
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Save'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebSMotionSaveClick
            end
            object ebSMotionLoad: TExtBtn
              Left = 71
              Top = 58
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Load'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebSMotionLoadClick
            end
            object ebSReloadMotion: TExtBtn
              Left = 4
              Top = 28
              Width = 67
              Height = 15
              Align = alNone
              BevelShow = False
              CloseButton = False
              Caption = 'Reload'
              Enabled = False
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              FlatAlwaysEdge = True
              OnClick = ebSReloadMotionClick
            end
          end
        end
      end
      object tsScript: TTabSheet
        Caption = 'LTX Script'
        object mmScript: TMemo
          Left = 0
          Top = 0
          Width = 262
          Height = 296
          Hint = 'Sample LTX-script:'#13#10'[object_name]'#13#10'prop1=...'#13#10'prop2=...'
          Align = alClient
          Color = 16646143
          ParentShowHint = False
          ScrollBars = ssVertical
          ShowHint = True
          TabOrder = 0
          WantTabs = True
          WordWrap = False
          OnChange = OnModified
        end
        object gbTemplates: TGroupBox
          Left = 262
          Top = 0
          Width = 101
          Height = 296
          Align = alRight
          Caption = ' Templates '
          TabOrder = 1
          object ExtBtn6: TExtBtn
            Left = 3
            Top = 19
            Width = 94
            Height = 17
            Align = alNone
            BevelShow = False
            CloseButton = False
            Caption = 'Monster'
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ExtBtn7: TExtBtn
            Left = 3
            Top = 37
            Width = 94
            Height = 17
            Align = alNone
            BevelShow = False
            CloseButton = False
            Caption = 'Helicopter'
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ExtBtn8: TExtBtn
            Left = 3
            Top = 73
            Width = 94
            Height = 17
            Align = alNone
            BevelShow = False
            CloseButton = False
            Caption = 'Doors'
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ExtBtn9: TExtBtn
            Left = 3
            Top = 55
            Width = 94
            Height = 17
            Align = alNone
            BevelShow = False
            CloseButton = False
            Caption = 'Boat'
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ExtBtn10: TExtBtn
            Left = 3
            Top = 91
            Width = 94
            Height = 17
            Align = alNone
            BevelShow = False
            CloseButton = False
            Caption = 'Lift'
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ExtBtn3: TExtBtn
            Left = 3
            Top = 109
            Width = 94
            Height = 17
            Align = alNone
            BevelShow = False
            CloseButton = False
            Caption = 'Weapon'
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ExtBtn4: TExtBtn
            Left = 3
            Top = 127
            Width = 94
            Height = 17
            Align = alNone
            BevelShow = False
            CloseButton = False
            Caption = 'Item'
            Transparent = False
            FlatAlwaysEdge = True
          end
        end
      end
    end
  end
  object paBottom: TPanel
    Left = 0
    Top = 342
    Width = 372
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 201
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 283
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
    object ebDropper: TExtBtn
      Left = 2
      Top = 2
      Width = 20
      Height = 20
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      CloseButton = False
      GroupIndex = 1
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        1E010000424D1E01000000000000460000002800000012000000120000000100
        040000000000D8000000120B0000120B00000400000004000000CED3D600F7F7
        F70000000000FFFFFF0000000000000000000000202000200000000000000000
        0000021220000000000000000020002112000000000000000212002111200000
        0000000000210002111200000000000000210000211120000000000000020000
        0211120000000000000000000021112000000000000000000002111202000000
        0000000000002111220000000000000000000212220000000000000000000022
        2222000000000000000002222222200000000000000000002222200000000000
        0000000022222000000000000000000002220000000000000000000000000000
        0000}
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
    end
  end
  object odMesh: TOpenDialog
    Filter = 'Mesh file (*.mesh)|*.mesh'
    Options = [ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Open Mesh file'
    Left = 10
    Top = 3
  end
  object pmNumericSet: TRxPopupMenu
    AutoPopup = False
    MenuAnimation = [maTopToBottom]
    LeftMargin = 15
    Style = msBtnLowered
    Left = 38
    Top = 3
    object Position1: TMenuItem
      Caption = 'Position'
      OnClick = Position1Click
    end
    object Rotation1: TMenuItem
      Caption = 'Rotation'
      OnClick = Rotation1Click
    end
    object Scale1: TMenuItem
      Caption = 'Scale'
      OnClick = Scale1Click
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
    Left = 66
    Top = 3
  end
end
