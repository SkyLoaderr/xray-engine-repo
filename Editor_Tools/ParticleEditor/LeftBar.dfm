object fraLeftBar: TfraLeftBar
  Left = 0
  Top = 0
  Width = 443
  Height = 277
  HorzScrollBar.Visible = False
  VertScrollBar.Increment = 34
  VertScrollBar.Size = 13
  VertScrollBar.Style = ssHotTrack
  VertScrollBar.Tracking = True
  Align = alClient
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentColor = False
  ParentFont = False
  TabOrder = 0
  object paLeftBar: TPanel
    Left = 0
    Top = 0
    Width = 145
    Height = 277
    Align = alLeft
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 145
    Constraints.MinWidth = 145
    TabOrder = 0
    object paScene: TPanel
      Left = 1
      Top = 1
      Width = 143
      Height = 68
      Hint = 'Scene commands'
      Align = alTop
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      object APHeadLabel2: TLabel
        Left = 1
        Top = 1
        Width = 141
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Scene'
        Color = clGray
        ParentColor = False
        OnClick = PanelMaximizeClick
      end
      object ebSceneMin: TExtBtn
        Left = 130
        Top = 2
        Width = 11
        Height = 11
        Align = alNone
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          DE000000424DDE00000000000000360000002800000007000000070000000100
          180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
          0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
          000000000000000000000000FFFFFF0000000000000000000000000000000000
          00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000}
        ParentFont = False
        OnClick = PanelMimimizeClick
      end
      object ebSceneCommands: TExtBtn
        Left = 2
        Top = 16
        Width = 140
        Height = 15
        Align = alNone
        BevelShow = False
        HotTrack = True
        CloseButton = False
        Caption = 'Preview Object'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          DE000000424DDE00000000000000360000002800000007000000070000000100
          180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
          0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
          000000000000000000000000FFFFFF0000000000000000000000000000000000
          00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000}
        Margin = 3
        ParentFont = False
        Spacing = 3
        Transparent = False
        OnMouseDown = ebSceneCommandsMouseDown
      end
      object ebPreferences: TExtBtn
        Left = 2
        Top = 50
        Width = 140
        Height = 15
        Align = alNone
        BevelShow = False
        HotTrack = True
        CloseButton = False
        Caption = 'Preferences'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Margin = 13
        ParentFont = False
        Transparent = False
        OnClick = ebEditorPreferencesClick
      end
      object ExtBtn3: TExtBtn
        Left = 2
        Top = 33
        Width = 140
        Height = 15
        Align = alNone
        BevelShow = False
        HotTrack = True
        CloseButton = False
        Caption = 'Refresh Textures'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Margin = 13
        ParentFont = False
        Transparent = False
        OnClick = ebRefreshTexturesClick
      end
    end
    object paEngineShaders: TPanel
      Left = 1
      Top = 69
      Width = 143
      Height = 207
      Align = alClient
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      object Label1: TLabel
        Left = 1
        Top = 1
        Width = 141
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Shaders'
        Color = clGray
        ParentColor = False
        OnClick = PanelMaximizeClick
      end
      object pcShaders: TElPageControl
        Left = 1
        Top = 14
        Width = 141
        Height = 192
        BorderWidth = 0
        DrawFocus = False
        Flat = True
        HotTrack = True
        Multiline = False
        OnChange = pcShadersChange
        RaggedRight = False
        ScrollOpposite = False
        Style = etsNetTabs
        TabIndex = 1
        TabPosition = etpTop
        HotTrackFont.Charset = DEFAULT_CHARSET
        HotTrackFont.Color = clBlue
        HotTrackFont.Height = -11
        HotTrackFont.Name = 'MS Sans Serif'
        HotTrackFont.Style = []
        ActivePage = tsCompiler
        FlatTabBorderColor = clBtnShadow
        Align = alClient
        ParentColor = False
        TabOrder = 0
        object tsEngine: TElTabSheet
          PageControl = pcShaders
          ImageIndex = -1
          TabVisible = True
          Caption = 'Engine'
          Visible = False
          object Bevel1: TBevel
            Left = 0
            Top = 50
            Width = 137
            Height = 2
            Align = alTop
          end
          object Bevel2: TBevel
            Left = 0
            Top = 149
            Width = 137
            Height = 2
            Align = alBottom
          end
          object Panel1: TPanel
            Left = 0
            Top = 0
            Width = 137
            Height = 50
            Align = alTop
            BevelOuter = bvNone
            TabOrder = 0
            object ebEngineShaderRemove: TExtBtn
              Left = 1
              Top = 34
              Width = 70
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Remove'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Margin = 13
              ParentFont = False
              Transparent = False
              OnClick = ebEngineShaderRemoveClick
            end
            object ebEngineShaderClone: TExtBtn
              Left = 71
              Top = 34
              Width = 70
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Clone'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Margin = 13
              ParentFont = False
              Transparent = False
              OnClick = ebEngineShaderCloneClick
            end
            object ebEngineShaderFile: TExtBtn
              Left = 1
              Top = 2
              Width = 140
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              CloseWidth = 24
              Caption = 'File'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Glyph.Data = {
                DE000000424DDE00000000000000360000002800000007000000070000000100
                180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
                0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
                000000000000000000000000FFFFFF0000000000000000000000000000000000
                00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
                0000}
              Kind = knMinimize
              Margin = 3
              ParentFont = False
              Spacing = 3
              Transparent = False
              OnMouseDown = ebEngineShaderFileMouseDown
            end
            object ebEngineShaderCreate: TExtBtn
              Left = 1
              Top = 18
              Width = 140
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              CloseWidth = 24
              Caption = 'Create'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Glyph.Data = {
                DE000000424DDE00000000000000360000002800000007000000070000000100
                180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
                0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
                000000000000000000000000FFFFFF0000000000000000000000000000000000
                00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
                0000}
              Kind = knMinimize
              Margin = 3
              ParentFont = False
              Spacing = 3
              Transparent = False
              OnMouseDown = ebEngineShaderCreateMouseDown
            end
          end
          object tvEngine: TElTree
            Left = 0
            Top = 52
            Width = 137
            Height = 97
            Cursor = crDefault
            LeftPosition = 0
            DragCursor = crDrag
            Align = alClient
            AutoCollapse = False
            DockOrientation = doNoOrient
            DefaultSectionWidth = 120
            BorderStyle = bsNone
            BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
            CustomPlusMinus = True
            DragAllowed = True
            DrawFocusRect = False
            DragTrgDrawMode = dtdDownColorLine
            ExplorerEditMode = False
            FocusedSelectColor = 10526880
            FocusedSelectTextColor = clBlack
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clBlack
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            GradientSteps = 64
            HeaderHeight = 19
            HeaderHotTrack = False
            HeaderSections.Data = {F5FFFFFF00000000}
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
            IgnoreEnabled = False
            IncrementalSearch = False
            ItemIndent = 14
            KeepSelectionWithinLevel = False
            LineBorderActiveColor = clBlack
            LineBorderInactiveColor = clBlack
            LineHeight = 16
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
            StoragePath = '\Tree'
            TabOrder = 1
            TabStop = True
            Tracking = False
            TrackColor = 10526880
            VertDivLinesColor = 7368816
            VertScrollBarStyles.ShowTrackHint = True
            VertScrollBarStyles.Width = 16
            VertScrollBarStyles.ButtonSize = 16
            VirtualityLevel = vlNone
            BkColor = clGray
            OnItemFocused = tvEngineItemFocused
            OnDragDrop = tvEngineDragDrop
            OnDragOver = tvEngineDragOver
            OnStartDrag = tvEngineStartDrag
            OnMouseDown = tvEngineMouseDown
            OnDblClick = tvViewDblClick
            OnKeyDown = tvEngineKeyDown
          end
          object paAction: TPanel
            Left = 0
            Top = 151
            Width = 137
            Height = 16
            Align = alBottom
            BevelOuter = bvNone
            TabOrder = 2
            object ebEngineShaderProperties: TExtBtn
              Left = 1
              Top = 1
              Width = 60
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Properties'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              OnClick = ebEngineShaderPropertiesClick
            end
            object ebEngineApplyChanges: TExtBtn
              Left = 61
              Top = 1
              Width = 80
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Apply Changes'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              OnClick = ebEngineApplyChangesClick
            end
          end
        end
        object tsCompiler: TElTabSheet
          PageControl = pcShaders
          ImageIndex = -1
          TabVisible = True
          Caption = 'Compiler'
          object Bevel4: TBevel
            Left = 0
            Top = 149
            Width = 137
            Height = 2
            Align = alBottom
          end
          object Bevel3: TBevel
            Left = 0
            Top = 50
            Width = 137
            Height = 2
            Align = alTop
          end
          object Panel4: TPanel
            Left = 0
            Top = 0
            Width = 137
            Height = 50
            Align = alTop
            BevelOuter = bvNone
            TabOrder = 0
            object ebCompilerShaderRemove: TExtBtn
              Left = 1
              Top = 34
              Width = 70
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Remove'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Margin = 13
              ParentFont = False
              Transparent = False
              OnClick = ebCompilerShaderRemoveClick
            end
            object ebCompilerShaderClone: TExtBtn
              Left = 71
              Top = 34
              Width = 70
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Clone'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Margin = 13
              ParentFont = False
              Transparent = False
              OnClick = ebCompilerShaderCloneClick
            end
            object ExtBtn4: TExtBtn
              Left = 1
              Top = 2
              Width = 140
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              CloseWidth = 24
              Caption = 'File'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Glyph.Data = {
                DE000000424DDE00000000000000360000002800000007000000070000000100
                180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
                0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
                000000000000000000000000FFFFFF0000000000000000000000000000000000
                00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
                0000}
              Kind = knMinimize
              Margin = 3
              ParentFont = False
              Spacing = 3
              Transparent = False
              OnMouseDown = ebEngineShaderFileMouseDown
            end
            object ebCShaderCreate: TExtBtn
              Left = 1
              Top = 18
              Width = 140
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              CloseWidth = 24
              Caption = 'Create'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Kind = knMinimize
              Margin = 13
              ParentFont = False
              Spacing = 3
              Transparent = False
              OnClick = ebCShaderCreateClick
            end
          end
          object Panel5: TPanel
            Left = 0
            Top = 151
            Width = 137
            Height = 16
            Align = alBottom
            BevelOuter = bvNone
            TabOrder = 1
            object ExtBtn6: TExtBtn
              Left = 1
              Top = 1
              Width = 60
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Properties'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              OnClick = ebEngineShaderPropertiesClick
            end
            object ExtBtn7: TExtBtn
              Left = 61
              Top = 1
              Width = 80
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              CloseButton = False
              Caption = 'Apply Changes'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = False
              OnClick = ebEngineApplyChangesClick
            end
          end
          object tvCompiler: TElTree
            Left = 0
            Top = 52
            Width = 137
            Height = 97
            Cursor = crDefault
            LeftPosition = 0
            DragCursor = crDrag
            Align = alClient
            AutoCollapse = False
            DockOrientation = doNoOrient
            DefaultSectionWidth = 120
            BorderStyle = bsNone
            BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
            CustomPlusMinus = True
            DrawFocusRect = False
            ExplorerEditMode = False
            FocusedSelectColor = 10526880
            FocusedSelectTextColor = clBlack
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clBlack
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            GradientSteps = 64
            HeaderHeight = 19
            HeaderHotTrack = False
            HeaderSections.Data = {F5FFFFFF00000000}
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
            IgnoreEnabled = False
            IncrementalSearch = False
            ItemIndent = 14
            KeepSelectionWithinLevel = False
            LineBorderActiveColor = clBlack
            LineBorderInactiveColor = clBlack
            LineHeight = 16
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
            StoragePath = '\Tree'
            TabOrder = 2
            TabStop = True
            Tracking = False
            TrackColor = 10526880
            VertDivLinesColor = 7368816
            VertScrollBarStyles.ShowTrackHint = True
            VertScrollBarStyles.Width = 16
            VertScrollBarStyles.ButtonSize = 16
            VirtualityLevel = vlNone
            BkColor = clGray
            OnItemFocused = tvEngineItemFocused
            OnDragDrop = tvEngineDragDrop
            OnDragOver = tvEngineDragOver
            OnStartDrag = tvEngineStartDrag
            OnMouseDown = tvEngineMouseDown
            OnDblClick = tvViewDblClick
            OnKeyDown = tvEngineKeyDown
          end
        end
      end
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Left Bar'
    Options = []
    RegistryRoot = prLocalMachine
    Version = 2
    StoredProps.Strings = (
      'paScene.Tag'
      'paScene.Height'
      'paEngineShaders.Height'
      'pcShaders.ActivePage')
    StoredValues = <>
    Left = 65529
    Top = 65526
  end
  object pmEngineShadersFile: TMxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    TrackButton = tbLeftButton
    MarginStartColor = 13158600
    MarginEndColor = 1644825
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 117
    Top = 104
    object Save1: TMenuItem
      Caption = 'Save'
      OnClick = ebSaveClick
    end
    object Reload1: TMenuItem
      Caption = 'Reload'
      OnClick = ebReloadClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Import1: TMenuItem
      Caption = 'Import'
      Enabled = False
    end
    object Export1: TMenuItem
      Caption = 'Export'
      Enabled = False
    end
  end
  object pmPreviewObject: TMxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    TrackButton = tbLeftButton
    MarginStartColor = 13158600
    MarginEndColor = 1644825
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 117
    Top = 18
    object Plane1: TMenuItem
      Caption = 'Plane'
      OnClick = PreviewClick
    end
    object Box1: TMenuItem
      Tag = 1
      Caption = 'Box'
      OnClick = PreviewClick
    end
    object Ball1: TMenuItem
      Tag = 2
      Caption = 'Ball'
      OnClick = PreviewClick
    end
    object Teapot1: TMenuItem
      Tag = 3
      Caption = 'Teapot'
      OnClick = PreviewClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object Custom1: TMenuItem
      Tag = -1
      Caption = 'Custom...'
      OnClick = PreviewClick
    end
  end
  object pmShaderList: TMxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    MarginStartColor = 13158600
    MarginEndColor = 1644825
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 109
    Top = 165
    object CreateFolder1: TMenuItem
      Caption = 'Create Folder'
      OnClick = CreateFolder1Click
    end
    object Rename1: TMenuItem
      Caption = 'Rename'
      OnClick = Rename1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object ExpandAll1: TMenuItem
      Caption = 'Expand All'
      OnClick = ExpandAll1Click
    end
    object CollapseAll1: TMenuItem
      Caption = 'Collapse All'
      OnClick = CollapseAll1Click
    end
  end
  object pmTemplateList: TMxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    TrackButton = tbLeftButton
    MarginStartColor = 13158600
    MarginEndColor = 1644825
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 117
    Top = 121
  end
  object InplaceEngineEdit: TElTreeInplaceAdvancedEdit
    Tree = tvEngine
    Types = [sftText]
    OnValidateResult = InplaceEngineEditValidateResult
    Left = 10
    Top = 197
  end
  object InplaceCompilerEdit: TElTreeInplaceAdvancedEdit
    Tree = tvCompiler
    Types = [sftText]
    OnValidateResult = InplaceEngineEditValidateResult
    Left = 10
    Top = 165
  end
end
