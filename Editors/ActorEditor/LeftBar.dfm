object fraLeftBar: TfraLeftBar
  Left = 0
  Top = 0
  Width = 443
  Height = 565
  HorzScrollBar.Visible = False
  VertScrollBar.Increment = 34
  VertScrollBar.Size = 13
  VertScrollBar.Style = ssHotTrack
  VertScrollBar.Tracking = True
  Align = alClient
  Color = 10528425
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
    Height = 565
    Align = alLeft
    BevelInner = bvLowered
    BevelOuter = bvNone
    Color = 10528425
    Constraints.MaxWidth = 212
    Constraints.MinWidth = 212
    TabOrder = 0
    object spProps: TSplitter
      Left = 1
      Top = 80
      Width = 210
      Height = 2
      Cursor = crVSplit
      Align = alBottom
      Color = clBlack
      ParentColor = False
      Visible = False
    end
    object paSkeletonPart: TPanel
      Left = 1
      Top = 82
      Width = 210
      Height = 482
      Align = alBottom
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
      Visible = False
      object Splitter1: TSplitter
        Left = 0
        Top = 241
        Width = 210
        Height = 2
        Cursor = crVSplit
        Align = alBottom
        Color = clBlack
        ParentColor = False
      end
      object paCurrentMotion: TPanel
        Left = 0
        Top = 243
        Width = 210
        Height = 239
        Align = alBottom
        Color = 10528425
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        object Label1: TLabel
          Left = 1
          Top = 1
          Width = 208
          Height = 13
          Align = alTop
          Alignment = taCenter
          Caption = 'Current Motion'
          Color = clGray
          ParentColor = False
          OnClick = PanelMaximizeClick
        end
        object ExtBtn10: TExtBtn
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
        object paPSList: TPanel
          Left = 1
          Top = 14
          Width = 208
          Height = 224
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          object Bevel1: TBevel
            Left = 0
            Top = 29
            Width = 208
            Height = 1
            Align = alTop
            Shape = bsLeftLine
          end
          object Panel1: TPanel
            Left = 0
            Top = 0
            Width = 208
            Height = 29
            Align = alTop
            BevelOuter = bvNone
            Color = 10528425
            TabOrder = 0
            object ebCurrentPlay: TExtBtn
              Left = 2
              Top = 5
              Width = 25
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
                FFFFFFFFA7A7A7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFBABABA0000005050508E8E8EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFBABABA0000000000000000004F4F4F7B7B7BFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000000000000000000008080828
                2828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA000000000000000000000000
                0000000000001212121F1F1FC6C6C6FFFFFFFFFFFFBABABA0000000000000000
                00000000080808282828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000
                00000000004F4F4F7B7B7BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
                0000005050508E8E8EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFA7A7A7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFFF}
              ParentFont = False
              FlatAlwaysEdge = True
              OnClick = ebCurrentPlayClick
            end
            object ebCurrentStop: TExtBtn
              Left = 52
              Top = 5
              Width = 25
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
                FFFFFFFFFFFFFFA5A5A5ADADADADADADADADADADADADA5A5A5FFFFFFFFFFFFFF
                FFFFFFFFFFFFFFFF969696000000000000000000000000000000000000969696
                FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000000000000000000000000000
                00969696FFFFFFFFFFFFFFFFFFFFFFFF96969600000000000000000000000000
                0000000000969696FFFFFFFFFFFFFFFFFFFFFFFF969696000000000000000000
                000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000
                00000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF96969600
                0000000000000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF
                FFFFFFA5A5A5ADADADADADADADADADADADADA5A5A5FFFFFFFFFFFFFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
                FFFF}
              ParentFont = False
              FlatAlwaysEdge = True
              OnClick = ebCurrentStopClick
            end
            object lbCurFrames: TMxLabel
              Left = 148
              Top = 1
              Width = 59
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
            object lbCurFPS: TMxLabel
              Left = 148
              Top = 13
              Width = 59
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
              Left = 108
              Top = 13
              Width = 25
              Height = 13
              Caption = 'FPS:'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clBlack
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 10528425
            end
            object RxLabel1: TMxLabel
              Left = 108
              Top = 1
              Width = 39
              Height = 13
              Caption = 'Frames:'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clBlack
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              ShadowColor = 10528425
            end
            object ebCurrentPause: TExtBtn
              Left = 27
              Top = 5
              Width = 25
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
                9E010000424D9E0100000000000036000000280000000B0000000A0000000100
                18000000000068010000120B0000120B00000000000000000000FFFFFFFFFFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFF
                FFFFFFFFA5A5A5ADADADFFFFFFFFFFFFFFFFFFADADADA5A5A5FFFFFFFFFFFF00
                0000FFFFFF969696000000000000969696FFFFFF969696000000000000969696
                FFFFFF000000FFFFFF969696000000000000969696FFFFFF9696960000000000
                00969696FFFFFF000000FFFFFF969696000000000000969696FFFFFF96969600
                0000000000969696FFFFFF000000FFFFFF969696000000000000969696FFFFFF
                969696000000000000969696FFFFFF000000FFFFFF9696960000000000009696
                96FFFFFF969696000000000000969696FFFFFF000000FFFFFF96969600000000
                0000969696FFFFFF969696000000000000969696FFFFFF000000FFFFFFFFFFFF
                A5A5A5ADADADFFFFFFFFFFFFFFFFFFADADADA5A5A5FFFFFFFFFFFF000000FFFF
                FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
                0000}
              ParentFont = False
              FlatAlwaysEdge = True
              OnClick = ebCurrentPauseClick
            end
            object ebMixMotion: TExtBtn
              Left = 77
              Top = 5
              Width = 25
              Height = 19
              Align = alNone
              AllowAllUp = True
              BevelShow = False
              CloseButton = False
              GroupIndex = 1
              Down = True
              Caption = 'MIX'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Spacing = 0
              FlatAlwaysEdge = True
              OnClick = ebCurrentStopClick
            end
          end
          object paMotionProps: TPanel
            Left = 0
            Top = 30
            Width = 208
            Height = 194
            Align = alClient
            BevelOuter = bvNone
            Color = clGray
            TabOrder = 1
          end
        end
      end
      object paMotions: TPanel
        Left = 0
        Top = 0
        Width = 210
        Height = 241
        Align = alClient
        Color = 10528425
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        object Label2: TLabel
          Left = 1
          Top = 1
          Width = 208
          Height = 13
          Align = alTop
          Alignment = taCenter
          Caption = 'Motions'
          Color = clGray
          ParentColor = False
          OnClick = PanelMaximizeClick
        end
        object Panel4: TPanel
          Left = 1
          Top = 14
          Width = 208
          Height = 210
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          object Bevel3: TBevel
            Left = 0
            Top = 209
            Width = 208
            Height = 1
            Align = alBottom
            Shape = bsLeftLine
          end
          object Bevel4: TBevel
            Left = 0
            Top = 34
            Width = 208
            Height = 1
            Align = alTop
            Shape = bsLeftLine
          end
          object tvMotions: TElTree
            Left = 0
            Top = 35
            Width = 208
            Height = 174
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
            SortMode = smAdd
            StoragePath = '\Tree'
            SortUseCase = False
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
            OnItemFocused = tvMotionsItemFocused
            OnDragDrop = tvMotionsDragDrop
            OnDragOver = tvMotionsDragOver
            OnStartDrag = tvMotionsStartDrag
            OnMouseDown = tvMotionsMouseDown
          end
          object Panel5: TPanel
            Left = 0
            Top = 0
            Width = 208
            Height = 34
            Align = alTop
            BevelOuter = bvNone
            Color = 10528425
            TabOrder = 1
            object ebMotionsRemove: TExtBtn
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
              OnClick = ebMotionsRemoveClick
            end
            object ebMotionsClear: TExtBtn
              Left = 138
              Top = 18
              Width = 69
              Height = 15
              Align = alNone
              BevelShow = False
              HotTrack = True
              HotColor = 15790320
              CloseButton = False
              Caption = 'Clear'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Margin = 13
              ParentFont = False
              OnClick = ebMotionsClearClick
            end
            object ebMotionsFile: TExtBtn
              Left = 2
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
              OnMouseDown = ebActorMotionsFileMouseDown
            end
            object ebMotionsAppend: TExtBtn
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
              Caption = 'Append'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              Kind = knMinimize
              Margin = 13
              ParentFont = False
              Spacing = 3
              OnClick = eMotionsAppendClick
            end
          end
        end
        object Panel6: TPanel
          Left = 1
          Top = 224
          Width = 208
          Height = 16
          Align = alBottom
          BevelOuter = bvNone
          Color = 10528425
          TabOrder = 1
          object lbMotionCount: TMxLabel
            Left = 63
            Top = 1
            Width = 59
            Height = 13
            AutoSize = False
            Caption = '999'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            ShadowColor = 10528425
            ShadowPos = spRightBottom
          end
          object MxLabel2: TMxLabel
            Left = 4
            Top = 1
            Width = 56
            Height = 13
            Caption = 'Items in list:'
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
    end
    object paBasic: TPanel
      Left = 1
      Top = 1
      Width = 210
      Height = 79
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 1
      object paModel: TPanel
        Left = 0
        Top = 82
        Width = 210
        Height = 67
        Hint = 'Scene commands'
        Align = alTop
        Color = 10528425
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        object Label4: TLabel
          Left = 1
          Top = 1
          Width = 208
          Height = 13
          Align = alTop
          Alignment = taCenter
          Caption = 'Model'
          Color = clGray
          ParentColor = False
          OnClick = PanelMaximizeClick
        end
        object ExtBtn2: TExtBtn
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
        object ebMakePreview: TExtBtn
          Left = 2
          Top = 32
          Width = 207
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
          CloseButton = False
          Caption = 'Update Engine Render'
          Enabled = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Margin = 13
          ParentFont = False
          OnClick = ebMakePreviewClick
        end
        object ebRenderEditorStyle: TExtBtn
          Left = 85
          Top = 49
          Width = 60
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
          CloseButton = False
          GroupIndex = 1
          Down = True
          Caption = 'Editor'
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
          OnClick = ebRenderStyleClick
        end
        object ebRenderEngineStyle: TExtBtn
          Left = 145
          Top = 49
          Width = 60
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
          CloseButton = False
          GroupIndex = 1
          Caption = 'Engine'
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
          OnClick = ebRenderStyleClick
        end
        object Label5: TLabel
          Left = 17
          Top = 51
          Width = 64
          Height = 13
          Caption = 'Render Style:'
        end
        object ebBonePart: TExtBtn
          Left = 2
          Top = 16
          Width = 205
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
          CloseButton = False
          CloseWidth = 24
          Caption = 'Bone Parts'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          Kind = knMinimize
          Margin = 13
          ParentFont = False
          OnClick = ebBonePartClick
        end
      end
      object paObjectProperties: TPanel
        Left = 0
        Top = 149
        Width = 210
        Height = 52
        Align = alClient
        ParentShowHint = False
        ShowHint = False
        TabOrder = 1
        object Label6: TLabel
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
        object ExtBtn5: TExtBtn
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
        object Bevel6: TBevel
          Left = 1
          Top = 14
          Width = 208
          Height = 1
          Align = alTop
          Shape = bsLeftLine
        end
        object paObjectProps: TPanel
          Left = 1
          Top = 15
          Width = 208
          Height = 36
          Align = alClient
          BevelOuter = bvNone
          Color = 10528425
          TabOrder = 0
        end
      end
      object paScene: TPanel
        Left = 0
        Top = 0
        Width = 210
        Height = 82
        Hint = 'Scene commands'
        Align = alTop
        Color = 10528425
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
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
        object ebSceneFile: TExtBtn
          Left = 2
          Top = 16
          Width = 207
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
          CloseButton = False
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
          Margin = 3
          ParentFont = False
          Spacing = 3
          OnMouseDown = ebSceneFileMouseDown
        end
        object ebPreferences: TExtBtn
          Left = 2
          Top = 64
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
        object ebPreviewObjectClick: TExtBtn
          Left = 2
          Top = 32
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
          OnMouseDown = ebPreviewObjectClickMouseDown
        end
        object ebSceneCommands1: TExtBtn
          Left = 2
          Top = 48
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
          OnMouseDown = ebSceneCommands1MouseDown
        end
      end
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Left Bar'
    Options = []
    RegistryRoot = prLocalMachine
    Version = 10
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 65529
    Top = 65526
  end
  object pmMotionsFile: TMxPopupMenu
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
    Left = 205
    Top = 313
    object miLoadMotions: TMenuItem
      Caption = 'Load'
      OnClick = LoadClick
    end
    object miSaveMotions: TMenuItem
      Caption = 'Save'
      OnClick = miSaveMotionsClick
    end
  end
  object pmSceneFile: TMxPopupMenu
    Alignment = paCenter
    AutoHotkeys = maManual
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
    object N7: TMenuItem
      Caption = '-'
    end
    object Clear1: TMenuItem
      Caption = 'Clear'
      OnClick = Clear1Click
    end
    object Load1: TMenuItem
      Caption = 'Load'
      OnClick = Load1Click
    end
    object Save2: TMenuItem
      Caption = 'Save'
      OnClick = Save2Click
    end
    object SaevAs1: TMenuItem
      Caption = 'Save As...'
      OnClick = SaevAs1Click
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object miRecentFiles: TMenuItem
      Caption = 'Open Recent'
      Enabled = False
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Import1: TMenuItem
      Caption = 'Import'
      OnClick = Import1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object miExportSkeleton: TMenuItem
      Caption = 'Export Skeleton'
      OnClick = miExportSkeletonClick
    end
    object miExportObject: TMenuItem
      Caption = 'Export Object'
      OnClick = miExportObjectClick
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
    Top = 346
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
    Types = [sftText]
    OnValidateResult = InplaceParticleEditValidateResult
    Left = 5
    Top = 346
  end
  object pmPreviewObject: TMxPopupMenu
    Alignment = paCenter
    AutoHotkeys = maManual
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
    Top = 31
    object N8: TMenuItem
      Caption = '-'
    end
    object Custom1: TMenuItem
      Caption = 'Custom...'
      OnClick = Custom1Click
    end
    object none1: TMenuItem
      Caption = 'Clear'
      OnClick = none1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object Preferences1: TMenuItem
      Caption = 'Preferences'
      OnClick = Preferences1Click
    end
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
    Top = 42
    object N9: TMenuItem
      Caption = '-'
    end
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
