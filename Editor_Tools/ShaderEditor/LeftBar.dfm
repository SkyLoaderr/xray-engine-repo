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
      Height = 86
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
      object ebSceneFile: TExtBtn
        Left = 2
        Top = 16
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
        OnMouseDown = ebSceneFileMouseDown
      end
      object ebSceneCommands: TExtBtn
        Left = 2
        Top = 33
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
        Top = 67
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
        Top = 50
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
    object paShaders: TPanel
      Left = 1
      Top = 87
      Width = 143
      Height = 189
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
      object paShaderList: TPanel
        Left = 1
        Top = 48
        Width = 141
        Height = 124
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 0
        object Bevel1: TBevel
          Left = 0
          Top = 0
          Width = 141
          Height = 2
          Align = alTop
        end
        object Bevel2: TBevel
          Left = 0
          Top = 122
          Width = 141
          Height = 2
          Align = alBottom
        end
        object tvShaders: TElTree
          Left = 0
          Top = 2
          Width = 141
          Height = 120
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
          ShowLeafButton = False
          ShowLines = False
          StoragePath = '\Tree'
          TabOrder = 0
          TabStop = True
          Tracking = False
          TrackColor = 10526880
          VertDivLinesColor = 7368816
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VirtualityLevel = vlNone
          BkColor = clGray
          OnItemFocused = tvShadersItemFocused
          OnMouseDown = tvShadersMouseDown
          OnDblClick = tvShadersDblClick
          OnKeyDown = tvShadersKeyDown
        end
      end
      object Panel1: TPanel
        Left = 1
        Top = 14
        Width = 141
        Height = 34
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 1
        object ebShaderCreate: TExtBtn
          Left = 1
          Top = 2
          Width = 46
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          CloseButton = False
          Caption = 'Create'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          Transparent = False
          OnMouseDown = ebShaderCreateMouseDown
        end
        object ebShaderRemove: TExtBtn
          Left = 47
          Top = 2
          Width = 48
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
          ParentFont = False
          Transparent = False
          OnClick = ebShaderRemoveClick
        end
        object ebShaderClone: TExtBtn
          Left = 95
          Top = 2
          Width = 46
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
          ParentFont = False
          Transparent = False
          OnClick = ebShaderCloneClick
        end
        object ebShaderImport: TExtBtn
          Left = 1
          Top = 18
          Width = 70
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          CloseButton = False
          Caption = 'Import'
          Enabled = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Margin = 13
          ParentFont = False
          Transparent = False
        end
        object ebShaderExport: TExtBtn
          Left = 71
          Top = 18
          Width = 70
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          CloseButton = False
          Caption = 'Export'
          Enabled = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Margin = 13
          ParentFont = False
          Transparent = False
        end
      end
      object paAction: TPanel
        Left = 1
        Top = 172
        Width = 141
        Height = 16
        Align = alBottom
        BevelOuter = bvNone
        TabOrder = 2
        object ebShaderProperties: TExtBtn
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
          OnClick = ebShaderPropertiesClick
        end
        object ebApplyChanges: TExtBtn
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
          OnClick = ebApplyChangesClick
        end
      end
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Left Bar'
    Options = []
    RegistryRoot = prLocalMachine
    Version = 1
    StoredProps.Strings = (
      'paScene.Tag'
      'paScene.Height')
    StoredValues = <>
    Left = 65529
    Top = 65526
  end
  object pmSceneFile: TMxPopupMenu
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
    Top = 16
    object Save1: TMenuItem
      Caption = 'Save'
      OnClick = ebSaveClick
    end
    object Reload1: TMenuItem
      Caption = 'Reload'
      OnClick = ebReloadClick
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
    Top = 34
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
    Left = 117
    Top = 138
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
  object pmBlenderList: TMxPopupMenu
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
    Left = 13
    Top = 81
  end
  object InplaceEdit: TElTreeInplaceAdvancedEdit
    Tree = tvShaders
    Types = [sftText]
    OnValidateResult = InplaceEditValidateResult
    Left = 10
    Top = 151
  end
end
