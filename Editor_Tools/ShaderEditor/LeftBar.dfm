object fraLeftBar: TfraLeftBar
  Left = 0
  Top = 0
  Width = 443
  Height = 548
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
    Height = 353
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 145
    Constraints.MinWidth = 145
    TabOrder = 0
    object paScene: TPanel
      Left = 1
      Top = 1
      Width = 143
      Height = 67
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
        OnClick = PanelMimimizeClickClick
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
        Caption = 'Commands'
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
    end
    object paShaders: TPanel
      Left = 1
      Top = 68
      Width = 143
      Height = 277
      Align = alTop
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
      object ebShaderList: TExtBtn
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
        OnClick = PanelMimimizeClickClick
      end
      object paShaderList: TPanel
        Left = 1
        Top = 32
        Width = 141
        Height = 203
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object ElTree1: TElTree
          Left = 0
          Top = 0
          Width = 141
          Height = 203
          Cursor = crDefault
          LeftPosition = 0
          DragCursor = crDrag
          Align = alClient
          AutoCollapse = False
          DockOrientation = doNoOrient
          DefaultSectionWidth = 120
          BorderStyle = bsNone
          BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
          ExplorerEditMode = False
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
          HorzScrollBarStyles.ShowTrackHint = False
          HorzScrollBarStyles.Width = 16
          HorzScrollBarStyles.ButtonSize = 16
          IgnoreEnabled = False
          IncrementalSearch = False
          KeepSelectionWithinLevel = False
          LineBorderActiveColor = clBlack
          LineBorderInactiveColor = clBlack
          LineHeight = 17
          MouseFrameSelect = True
          OwnerDrawMask = '~~@~~'
          ScrollbarOpposite = False
          ShowLeafButton = False
          StoragePath = '\Tree'
          TabOrder = 0
          TabStop = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VirtualityLevel = vlNone
          BkColor = clGray
        end
      end
      object Panel1: TPanel
        Left = 1
        Top = 14
        Width = 141
        Height = 18
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 1
        object ExtBtn1: TExtBtn
          Left = 1
          Top = 2
          Width = 70
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          CloseButton = False
          Caption = 'Expand'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Margin = 13
          ParentFont = False
          Transparent = False
        end
        object ExtBtn2: TExtBtn
          Left = 71
          Top = 2
          Width = 70
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          CloseButton = False
          Caption = 'Collapse'
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
      object Panel2: TPanel
        Left = 1
        Top = 235
        Width = 141
        Height = 37
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 2
        object ExtBtn3: TExtBtn
          Left = 1
          Top = 2
          Width = 70
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          CloseButton = False
          Caption = 'Append'
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
        object ExtBtn4: TExtBtn
          Left = 71
          Top = 2
          Width = 70
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          CloseButton = False
          Caption = 'Collapse'
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
      end
    end
  end
  object paFrames: TPanel
    Left = 0
    Top = 379
    Width = 145
    Height = 126
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 145
    Constraints.MinWidth = 145
    TabOrder = 1
  end
  object fsStorage: TFormStorage
    IniSection = 'Left Bar'
    Options = []
    RegistryRoot = prLocalMachine
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
    SelFontColor = clWhite
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 117
    Top = 16
    object Clear1: TMenuItem
      Caption = 'Clear'
      OnClick = ebClearClick
    end
    object Load1: TMenuItem
      Caption = 'Load'
      OnClick = ebLoadClick
    end
    object Save1: TMenuItem
      Caption = 'Save'
      OnClick = ebSaveClick
    end
    object SaveAs1: TMenuItem
      Caption = 'Save As'
      OnClick = ebSaveAsClick
    end
  end
  object pmSceneCommands: TMxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    TrackButton = tbLeftButton
    MarginStartColor = 13158600
    MarginEndColor = 1644825
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = clWhite
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 117
    Top = 26
    object Refresh1: TMenuItem
      Caption = 'RefreshTextures'
      OnClick = ebRefreshEditorClick
    end
    object ResetAniamation1: TMenuItem
      Caption = 'Reset Animation'
      OnClick = ebResetAnimationClick
    end
  end
end
