object fraLeftBar: TfraLeftBar
  Left = 0
  Top = 0
  Width = 443
  Height = 684
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
    Width = 212
    Height = 684
    Align = alLeft
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 212
    Constraints.MinWidth = 212
    TabOrder = 0
    object paScene: TPanel
      Left = 1
      Top = 1
      Width = 210
      Height = 68
      Hint = 'Scene commands'
      Align = alTop
      Color = 10528425
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      object APHeadLabel2: TLabel
        Left = 1
        Top = 1
        Width = 208
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Scene'
        Color = clGray
        ParentColor = False
        OnClick = PanelMaximizeClick
      end
      object ebSceneMin: TExtBtn
        Left = 197
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
        Width = 207
        Height = 15
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
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
        OnMouseDown = ebSceneCommandsMouseDown
      end
      object ebPreferences: TExtBtn
        Left = 2
        Top = 50
        Width = 207
        Height = 15
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        CloseButton = False
        Caption = 'Preferences'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Margin = 13
        ParentFont = False
        OnClick = ebEditorPreferencesClick
      end
      object ebImageCommands: TExtBtn
        Left = 2
        Top = 33
        Width = 207
        Height = 15
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        BtnColor = 10528425
        CloseButton = False
        Caption = 'Images'
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
        OnMouseDown = ebImageCommandsMouseDown
      end
    end
    object paParticles: TPanel
      Left = 1
      Top = 69
      Width = 210
      Height = 222
      Align = alClient
      Color = 10528425
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      object Label1: TLabel
        Left = 1
        Top = 1
        Width = 208
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Particles'
        Color = clGray
        ParentColor = False
        OnClick = PanelMaximizeClick
      end
      object paPSList: TPanel
        Left = 1
        Top = 14
        Width = 208
        Height = 191
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 0
        object Bevel2: TBevel
          Left = 0
          Top = 189
          Width = 208
          Height = 2
          Align = alBottom
        end
        object Bevel1: TBevel
          Left = 0
          Top = 34
          Width = 208
          Height = 2
          Align = alTop
        end
        object tvParticles: TElTree
          Left = 0
          Top = 36
          Width = 208
          Height = 153
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
          OnItemFocused = tvParticlesItemFocused
          OnDragDrop = tvParticlesDragDrop
          OnDragOver = tvParticlesDragOver
          OnStartDrag = tvParticlesStartDrag
          OnMouseDown = tvParticlesMouseDown
          OnKeyDown = tvParticlesKeyDown
        end
        object Panel1: TPanel
          Left = 0
          Top = 0
          Width = 208
          Height = 34
          Align = alTop
          BevelOuter = bvNone
          Color = 10528425
          TabOrder = 1
          object ebEngineShaderRemove: TExtBtn
            Left = 69
            Top = 18
            Width = 69
            Height = 15
            Align = alNone
            BevelShow = False
            HotTrack = True
            HotColor = 15790320
            CloseButton = False
            Caption = 'Remove'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            Margin = 13
            ParentFont = False
            OnClick = ebParticleShaderRemoveClick
          end
          object ebEngineShaderClone: TExtBtn
            Left = 138
            Top = 18
            Width = 69
            Height = 15
            Align = alNone
            BevelShow = False
            HotTrack = True
            HotColor = 15790320
            CloseButton = False
            Caption = 'Clone'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            Margin = 13
            ParentFont = False
            OnClick = ebParticleCloneClick
          end
          object ebEngineShaderFile: TExtBtn
            Left = 1
            Top = 2
            Width = 205
            Height = 15
            Align = alNone
            BevelShow = False
            HotTrack = True
            HotColor = 15790320
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
            OnMouseDown = ebEngineShaderFileMouseDown
          end
          object ebCShaderCreate: TExtBtn
            Left = 1
            Top = 18
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            HotTrack = True
            HotColor = 15790320
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
            OnClick = ebPSCreateClick
          end
        end
      end
      object paAction: TPanel
        Left = 1
        Top = 205
        Width = 208
        Height = 16
        Align = alBottom
        BevelOuter = bvNone
        Color = 10528425
        TabOrder = 1
        object ebEngineApplyChanges: TExtBtn
          Left = 1
          Top = 0
          Width = 202
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
          CloseButton = False
          Caption = 'Apply Changes'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Margin = 13
          ParentFont = False
          OnClick = ebEngineApplyChangesClick
        end
      end
    end
    object paProperties: TPanel
      Left = 1
      Top = 333
      Width = 210
      Height = 350
      Hint = 'Scene commands'
      Align = alBottom
      Color = 10528425
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      object Label2: TLabel
        Left = 1
        Top = 1
        Width = 208
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Properties'
        Color = clGray
        ParentColor = False
        OnClick = PanelMaximizeClick
      end
      object ExtBtn1: TExtBtn
        Left = 197
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
      object paPSProps: TPanel
        Left = 1
        Top = 14
        Width = 208
        Height = 335
        Align = alClient
        BevelOuter = bvNone
        Color = 10528425
        TabOrder = 0
      end
    end
    object paCurrentPS: TPanel
      Left = 1
      Top = 291
      Width = 210
      Height = 42
      Hint = 'Scene commands'
      Align = alBottom
      Color = 10528425
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      object Label3: TLabel
        Left = 1
        Top = 1
        Width = 208
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Current PS'
        Color = clGray
        ParentColor = False
        OnClick = PanelMaximizeClick
      end
      object ExtBtn4: TExtBtn
        Left = 197
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
      object ebCurrentPSPlay: TExtBtn
        Left = 2
        Top = 19
        Width = 31
        Height = 19
        Align = alNone
        BevelShow = False
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFBABABA0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFBABABA0000000000000000004F4F4F7B7B7BFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000000000000000000008080828
          2828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA000000000000000000000000
          0000000000001212121F1F1FC6C6C6FFFFFFFFFFFFBABABA0000000000000000
          00000000080808282828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000
          00000000004F4F4F7B7B7BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
          0000005050508E8E8EEDEDEDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFEBEBEBA7A7A7E0E0E0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebCurrentPSPlayClick
      end
      object ebCurrentPSStop: TExtBtn
        Left = 33
        Top = 19
        Width = 31
        Height = 19
        Align = alNone
        BevelShow = False
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          9E010000424D9E0100000000000036000000280000000C0000000A0000000100
          18000000000068010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFDDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFF
          FFFFFFFFFFFFFFFF969696000000000000000000000000000000000000969696
          FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000000000000000000000000000
          00969696FFFFFFFFFFFFFFFFFFFFFFFF96969600000000000000000000000000
          0000000000969696FFFFFFFFFFFFFFFFFFFFFFFF969696000000000000000000
          000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000
          00000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF96969600
          0000000000000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF
          DDDDDDA5A5A5ADADADADADADADADADADADADA5A5A5DDDDDDFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFF}
        ParentFont = False
        FlatAlwaysEdge = True
        OnClick = ebCurrentPSStopClick
      end
      object lbCurState: TMxLabel
        Left = 113
        Top = 15
        Width = 95
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object lbParticleCount: TMxLabel
        Left = 113
        Top = 28
        Width = 95
        Height = 13
        AutoSize = False
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMaroon
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel2: TMxLabel
        Left = 65
        Top = 28
        Width = 45
        Height = 13
        Caption = 'Particles:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
      object RxLabel1: TMxLabel
        Left = 65
        Top = 15
        Width = 30
        Height = 13
        Caption = 'State:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ShadowColor = 10528425
      end
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Left Bar'
    Options = []
    RegistryRoot = prLocalMachine
    Version = 4
    StoredProps.Strings = (
      'paScene.Tag'
      'paScene.Height'
      'paParticles.Tag'
      'paParticles.Height'
      'paCurrentPS.Tag'
      'paCurrentPS.Height')
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
    Left = 181
    Top = 80
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
    Left = 181
    Top = 18
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
    Left = 34
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
  object InplaceParticleEdit: TElTreeInplaceAdvancedEdit
    Tree = tvParticles
    Types = [sftText]
    OnValidateResult = InplaceParticleEditValidateResult
    Left = 5
    Top = 138
  end
  object pmImages: TMxPopupMenu
    Alignment = paCenter
    AutoPopup = False
    TrackButton = tbLeftButton
    MarginStartColor = 10921638
    MarginEndColor = 2763306
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 181
    Top = 34
    object ImageEditor1: TMenuItem
      Caption = 'Image Editor'
      OnClick = ImageEditor1Click
    end
    object N6: TMenuItem
      Caption = '-'
    end
    object Refresh1: TMenuItem
      Caption = 'Synchronize Textures'
      OnClick = Refresh1Click
    end
    object Checknewtextures1: TMenuItem
      Caption = 'Check New Textures'
      OnClick = Checknewtextures1Click
    end
  end
end
