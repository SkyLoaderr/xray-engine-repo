object frmPropertiesSceneObject: TfrmPropertiesSceneObject
  Left = 546
  Top = 425
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Scene Object properties'
  ClientHeight = 224
  ClientWidth = 307
  Color = 10528425
  Constraints.MaxHeight = 251
  Constraints.MaxWidth = 315
  Constraints.MinHeight = 251
  Constraints.MinWidth = 315
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
  object pcSceneProps: TElPageControl
    Left = 0
    Top = 0
    Width = 307
    Height = 207
    ActiveTabColor = 10528425
    BorderWidth = 0
    Color = 10528425
    DrawFocus = False
    Flat = True
    HotTrack = False
    InactiveTabColor = 10528425
    Multiline = False
    RaggedRight = False
    ScrollOpposite = False
    Style = etsAngledTabs
    TabHeight = 16
    TabIndex = 2
    TabPosition = etpBottom
    HotTrackFont.Charset = DEFAULT_CHARSET
    HotTrackFont.Color = clBlue
    HotTrackFont.Height = -11
    HotTrackFont.Name = 'MS Sans Serif'
    HotTrackFont.Style = []
    TabBkColor = 10528425
    ActivePage = tsSounds
    FlatTabBorderColor = clBtnShadow
    Align = alClient
    ParentColor = False
    TabOrder = 0
    UseXPThemes = False
    object tsBasic: TElTabSheet
      PageControl = pcSceneProps
      ImageIndex = -1
      TabVisible = True
      Caption = 'Main'
      Color = 10528425
      Visible = False
      object paBasic: TPanel
        Left = 0
        Top = 0
        Width = 303
        Height = 187
        Align = alClient
        BevelOuter = bvLowered
        Color = 10528425
        TabOrder = 0
        object RxLabel1: TLabel
          Left = 3
          Top = 7
          Width = 31
          Height = 13
          Caption = 'Name:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object ebReference: TExtBtn
          Left = 61
          Top = 27
          Width = 239
          Height = 16
          Align = alNone
          BevelShow = False
          HotTrack = True
          BtnColor = 10528425
          CloseButton = False
          Caption = '...'
          Margin = 0
          Transparent = False
          OnClick = ebReferenceClick
        end
        object RxLabel2: TLabel
          Left = 3
          Top = 28
          Width = 53
          Height = 13
          Caption = 'Reference:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object edName: TEdit
          Left = 61
          Top = 4
          Width = 239
          Height = 18
          AutoSize = False
          Color = 14671839
          TabOrder = 0
          OnChange = OnModified
        end
        object gbDynamicFlags: TGroupBox
          Left = 3
          Top = 43
          Width = 148
          Height = 62
          Caption = ' Dynamic Flags '
          TabOrder = 1
          object cbDummy: TMultiObjCheck
            Left = 8
            Top = 16
            Width = 62
            Height = 17
            Alignment = taLeftJustify
            Caption = 'Dummy'
            TabOrder = 0
            OnClick = cbDummyClick
          end
        end
        object GroupBox1: TGroupBox
          Left = 152
          Top = 43
          Width = 148
          Height = 62
          Caption = ' Static Flags '
          TabOrder = 2
        end
      end
    end
    object tsMotions: TElTabSheet
      OnShow = tsMotionsShow
      PageControl = pcSceneProps
      ImageIndex = -1
      TabVisible = True
      Caption = 'Motions'
      Color = 10528425
      Visible = False
      object Label1: TLabel
        Left = 104
        Top = 82
        Width = 93
        Height = 13
        Alignment = taCenter
        Caption = '<Multiple selection>'
      end
      object paMotions: TPanel
        Left = 0
        Top = 0
        Width = 303
        Height = 187
        Align = alClient
        BevelOuter = bvLowered
        Color = 10528425
        TabOrder = 0
        object Bevel1: TBevel
          Left = 159
          Top = 1
          Width = 2
          Height = 185
          Align = alRight
          Shape = bsLeftLine
        end
        object tvOMotions: TElTree
          Left = 1
          Top = 1
          Width = 158
          Height = 185
          Cursor = crDefault
          LeftPosition = 0
          DragCursor = crDrag
          Align = alClient
          AlwaysKeepSelection = False
          AutoCollapse = False
          DockOrientation = doNoOrient
          DefaultSectionWidth = 120
          BorderStyle = bsNone
          BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
          Ctl3D = True
          CustomPlusMinus = True
          DragAllowed = True
          DrawFocusRect = False
          DragTrgDrawMode = dtdDownColorLine
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
          LinesColor = clBtnShadow
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
          ParentCtl3D = False
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
          PopupMenu = pmMotions
          ScrollbarOpposite = False
          ScrollTracking = True
          ShowCheckboxes = True
          ShowImages = False
          ShowLeafButton = False
          ShowLines = False
          StoragePath = '\Tree'
          TabOrder = 0
          TabStop = True
          Tracking = False
          VertDivLinesColor = 7368816
          VerticalLines = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          VertScrollBarStyles.UseXPThemes = False
          VirtualityLevel = vlNone
          UseXPThemes = False
          TextColor = clBlack
          BkColor = clGray
          OnItemChange = tvOMotionsItemChange
          OnItemFocused = tvOMotionsItemFocused
          OnDragDrop = tvOMotionsDragDrop
          OnDragOver = tvOMotionsDragOver
          OnStartDrag = tvOMotionsStartDrag
          OnMouseDown = tvOMotionsMouseDown
        end
        object Panel1: TPanel
          Left = 161
          Top = 1
          Width = 141
          Height = 185
          Align = alRight
          BevelOuter = bvNone
          ParentColor = True
          TabOrder = 1
          object ebOMotionAppend: TExtBtn
            Left = 2
            Top = 137
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            OnClick = ebOMotionAppendClick
          end
          object ebOMotionRename: TExtBtn
            Left = 71
            Top = 137
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            OnClick = ebOMotionRenameClick
          end
          object ebOMotionDelete: TExtBtn
            Left = 2
            Top = 153
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            OnClick = ebOMotionDeleteClick
          end
          object ebOMotionClear: TExtBtn
            Left = 71
            Top = 153
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            Left = 2
            Top = 169
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            Top = 169
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
          object GroupBox3: TGroupBox
            Left = 1
            Top = 1
            Width = 138
            Height = 53
            Caption = ' Summary Info '
            Ctl3D = True
            ParentCtl3D = False
            TabOrder = 0
            object RxLabel19: TLabel
              Left = 5
              Top = 17
              Width = 31
              Height = 13
              Caption = 'Count:'
            end
            object lbOMotionCount: TLabel
              Left = 43
              Top = 17
              Width = 9
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
            object RxLabel21: TLabel
              Left = 5
              Top = 34
              Width = 33
              Height = 13
              Caption = 'Active:'
            end
            object lbActiveOMotion: TLabel
              Left = 43
              Top = 34
              Width = 9
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
          end
          object gbOMotion: TGroupBox
            Left = 1
            Top = 56
            Width = 138
            Height = 70
            Caption = ' Current Motion '
            Ctl3D = True
            ParentCtl3D = False
            TabOrder = 1
            Visible = False
            object RxLabel20: TLabel
              Left = 5
              Top = 17
              Width = 31
              Height = 13
              Caption = 'Name:'
            end
            object lbOMotionName: TLabel
              Left = 43
              Top = 17
              Width = 93
              Height = 13
              AutoSize = False
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
            object RxLabel23: TLabel
              Left = 5
              Top = 34
              Width = 37
              Height = 13
              Caption = 'Frames:'
            end
            object lbOMotionFrames: TLabel
              Left = 43
              Top = 34
              Width = 93
              Height = 13
              AutoSize = False
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
            object lbOMotionFPS: TLabel
              Left = 43
              Top = 51
              Width = 93
              Height = 13
              AutoSize = False
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
            object RxLabel28: TLabel
              Left = 5
              Top = 51
              Width = 23
              Height = 13
              Caption = 'FPS:'
            end
          end
        end
      end
    end
    object tsSounds: TElTabSheet
      OnShow = tsSoundsShow
      PageControl = pcSceneProps
      ImageIndex = -1
      TabVisible = True
      Caption = 'Sounds'
      ParentColor = True
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 303
        Height = 187
        Align = alClient
        BevelOuter = bvLowered
        Color = 10528425
        TabOrder = 0
        object Bevel2: TBevel
          Left = 159
          Top = 1
          Width = 2
          Height = 185
          Align = alRight
          Shape = bsLeftLine
        end
        object tvSounds: TElTree
          Left = 1
          Top = 1
          Width = 158
          Height = 185
          Cursor = crDefault
          LeftPosition = 0
          DragCursor = crDrag
          Align = alClient
          AlwaysKeepSelection = False
          AutoCollapse = False
          DockOrientation = doNoOrient
          DefaultSectionWidth = 120
          BorderStyle = bsNone
          BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
          Ctl3D = True
          CustomPlusMinus = True
          DragAllowed = True
          DrawFocusRect = False
          DragTrgDrawMode = dtdDownColorLine
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
          LinesColor = clBtnShadow
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
          ParentCtl3D = False
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
          ShowCheckboxes = True
          ShowImages = False
          ShowLeafButton = False
          ShowLines = False
          StoragePath = '\Tree'
          TabOrder = 0
          TabStop = True
          Tracking = False
          VertDivLinesColor = 7368816
          VerticalLines = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          VertScrollBarStyles.UseXPThemes = False
          VirtualityLevel = vlNone
          UseXPThemes = False
          TextColor = clBlack
          BkColor = clGray
          OnItemChange = tvSoundsItemChange
        end
        object Panel3: TPanel
          Left = 161
          Top = 1
          Width = 141
          Height = 185
          Align = alRight
          BevelOuter = bvNone
          ParentColor = True
          TabOrder = 1
          object ebSoundAppend: TExtBtn
            Left = 2
            Top = 137
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            OnClick = ebSoundAppendClick
          end
          object ExtBtn2: TExtBtn
            Left = 71
            Top = 137
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
            CloseButton = False
            Caption = 'Rename'
            Enabled = False
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ebSoundDelete: TExtBtn
            Left = 2
            Top = 153
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            OnClick = ebSoundDeleteClick
          end
          object ebSoundClear: TExtBtn
            Left = 71
            Top = 153
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
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
            OnClick = ebSoundClearClick
          end
          object ExtBtn5: TExtBtn
            Left = 2
            Top = 169
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
            CloseButton = False
            Caption = 'Save'
            Enabled = False
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            Transparent = False
            FlatAlwaysEdge = True
          end
          object ExtBtn6: TExtBtn
            Left = 71
            Top = 169
            Width = 68
            Height = 15
            Align = alNone
            BevelShow = False
            BtnColor = 10528425
            CloseButton = False
            Caption = 'Load'
            Enabled = False
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            Transparent = False
            FlatAlwaysEdge = True
          end
          object GroupBox2: TGroupBox
            Left = 1
            Top = 1
            Width = 138
            Height = 53
            Caption = ' Summary Info '
            Ctl3D = True
            ParentCtl3D = False
            TabOrder = 0
            object Label2: TLabel
              Left = 5
              Top = 17
              Width = 31
              Height = 13
              Caption = 'Count:'
            end
            object lbSoundCount: TLabel
              Left = 43
              Top = 17
              Width = 9
              Height = 13
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
            object Label4: TLabel
              Left = 5
              Top = 34
              Width = 33
              Height = 13
              Caption = 'Active:'
            end
            object lbSoundActive: TLabel
              Left = 43
              Top = 34
              Width = 91
              Height = 13
              AutoSize = False
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
          end
          object GroupBox4: TGroupBox
            Left = 1
            Top = 56
            Width = 138
            Height = 51
            Caption = ' Current Sound '
            Ctl3D = True
            ParentCtl3D = False
            TabOrder = 1
            Visible = False
            object Label6: TLabel
              Left = 5
              Top = 17
              Width = 31
              Height = 13
              Caption = 'Name:'
            end
            object lbSoundCurrentName: TLabel
              Left = 43
              Top = 17
              Width = 93
              Height = 13
              AutoSize = False
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
            object Label8: TLabel
              Left = 5
              Top = 34
              Width = 36
              Height = 13
              Caption = 'Lenght:'
            end
            object lbSoundCurrentLenght: TLabel
              Left = 43
              Top = 34
              Width = 93
              Height = 13
              AutoSize = False
              Caption = '...'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clMaroon
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
            end
          end
        end
      end
    end
  end
  object paBottom: TPanel
    Left = 0
    Top = 207
    Width = 307
    Height = 17
    Align = alBottom
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 168
      Top = 0
      Width = 68
      Height = 16
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 237
      Top = 0
      Width = 68
      Height = 16
      Align = alNone
      BevelShow = False
      BtnColor = 10528425
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object fsStorage: TFormStorage
    Options = [fpPosition]
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'pcSceneProps.ActivePage')
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0
      end
      item
        Name = 'EmitterDirY'
        Value = 0
      end
      item
        Name = 'EmitterDirZ'
        Value = 0
      end>
    Left = 2
    Top = 3
  end
  object InplaceMotionEdit: TElTreeInplaceAdvancedEdit
    Tree = tvOMotions
    Types = [sftText]
    OnValidateResult = InplaceMotionEditValidateResult
    Left = 33
    Top = 3
  end
  object pmMotions: TMxPopupMenu
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
    Left = 65
    Top = 3
    object Rename1: TMenuItem
      Caption = 'Rename'
      OnClick = Rename1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object CreateFolder1: TMenuItem
      Caption = 'Create Folder'
      OnClick = CreateFolder1Click
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
end
