object frmPropertiesObject: TfrmPropertiesObject
  Left = 551
  Top = 360
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Object properties'
  ClientHeight = 365
  ClientWidth = 372
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
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
      ActivePage = tsMainOptions
      Align = alClient
      MultiLine = True
      TabIndex = 0
      TabOrder = 0
      OnChange = pcObjectsChange
      object tsMainOptions: TTabSheet
        Caption = 'Main options'
        ImageIndex = 4
        OnShow = tsMainOptionsShow
        object cbMakeDynamic: TMultiObjCheck
          Left = 4
          Top = 12
          Width = 209
          Height = 17
          Caption = 'Dynamic behaviour ("dynamic object")'
          Checked = True
          State = cbChecked
          TabOrder = 0
          OnClick = cbMakeDynamicClick
        end
      end
      object tsInfo: TTabSheet
        Caption = 'Transformation'
        ImageIndex = 1
        OnShow = tsInfoShow
        object RxLabel4: TLabel
          Left = 0
          Top = 10
          Width = 50
          Height = 13
          Caption = 'Transform:'
        end
        object RxLabel5: TLabel
          Left = 1
          Top = 110
          Width = 69
          Height = 13
          Caption = 'Bounding Box:'
        end
        object RxLabel11: TLabel
          Left = 48
          Top = 154
          Width = 142
          Height = 13
          Caption = 'Noa available in multiselection'
        end
        object RxLabel10: TLabel
          Left = 10
          Top = 29
          Width = 40
          Height = 13
          Caption = 'Position:'
        end
        object RxLabel13: TLabel
          Left = 10
          Top = 48
          Width = 43
          Height = 13
          Caption = 'Rotation:'
        end
        object RxLabel25: TLabel
          Left = 10
          Top = 67
          Width = 30
          Height = 13
          Caption = 'Scale:'
        end
        object paBB: TPanel
          Left = 3
          Top = 125
          Width = 262
          Height = 64
          BevelOuter = bvLowered
          TabOrder = 0
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
            ActiveFilterColor = clBlack
            AllowDrag = True
            Align = alTop
            Color = clBtnFace
            Flat = True
            DockOrientation = doNoOrient
            DoubleBuffered = False
            MoveOnDrag = False
            FilterColor = clBtnText
            LockHeight = False
            ResizeOnDrag = False
            RightAlignedText = False
            RightAlignedOrder = False
            Sections.Data = {
              F4FFFFFF05000000C05B0207003E0000FFFFFFFF000001000000486F14000000
              000000001027000000000000EC43B90504000000000065530000000000000100
              000000000000000000010000000000007753656C6563740064671901C05B0207
              843E000002F211402CF2114054F211407AF21140A0F21140C4F21140E7F21140
              0DF3114033F3114064671901C05B0207543E0000010000000001000000000000
              000000000000C05B0207003E0000FFFFFFFF000001000000486F3C0000000000
              000010270000000001008048B905000000000000655300000000000001000000
              00000000000000010000000000007753656C6563740064671901C05B0207843E
              000002F211402CF2114054F211407AF21140A0F21140C4F21140E7F211400DF3
              114033F3114064671901C05B0207543E0000080000004D696E696D756D000100
              0000000000000000000000C05B0207003E0000FFFFFFFF000001000000486F3C
              0000000000000010270000000001004449B90501000000000065530000000000
              000100000000000000000000010000000000007753656C6563740064671901C0
              5B0207843E000002F211402CF2114054F211407AF21140A0F21140C4F21140E7
              F211400DF3114033F3114064671901C05B0207543E0000080000004D6178696D
              756D0001000000000000000000000000C05B0207003E0000FFFFFFFF00000100
              0000486F3C000000000000001027000000000100084AB9050200000000006553
              0000000000000100000000000000000000010000000000007753656C65637400
              64671901C05B0207843E000002F211402CF2114054F211407AF21140A0F21140
              C4F21140E7F211400DF3114033F3114064671901C05B0207543E000005000000
              53697A650001000000000000000000000000C05B0207003E0000FFFFFFFF0000
              01000000486F3C000000000000001027000000000100CC4AB905030000000000
              65530000000000000100000000000000000000010000000000007753656C6563
              740064671901C05B0207843E000002F211402CF2114054F211407AF21140A0F2
              1140C4F21140E7F211400DF3114033F3114064671901C05B0207543E00000700
              000043656E7465720001000000000000000000000000}
            StickySections = False
            Tracking = True
            WrapCaptions = False
          end
        end
        object sePositionX: TMultiObjSpinEdit
          Left = 57
          Top = 27
          Width = 66
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 1
          OnChange = seTransformChange
        end
        object sePositionY: TMultiObjSpinEdit
          Left = 123
          Top = 27
          Width = 66
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 2
          OnChange = seTransformChange
        end
        object sePositionZ: TMultiObjSpinEdit
          Left = 189
          Top = 27
          Width = 66
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          ValueType = vtFloat
          AutoSize = False
          TabOrder = 3
          OnChange = seTransformChange
        end
        object seScaleX: TMultiObjSpinEdit
          Left = 57
          Top = 65
          Width = 66
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 100
          MinValue = 0.01
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          TabOrder = 4
          OnChange = seTransformChange
        end
        object seScaleY: TMultiObjSpinEdit
          Left = 123
          Top = 65
          Width = 66
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 100
          MinValue = 0.01
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          TabOrder = 5
          OnChange = seTransformChange
        end
        object seScaleZ: TMultiObjSpinEdit
          Left = 189
          Top = 65
          Width = 66
          Height = 18
          LWSensitivity = 0.1
          ButtonKind = bkLightWave
          Increment = 0.01
          MaxValue = 100
          MinValue = 0.01
          ValueType = vtFloat
          Value = 1
          AutoSize = False
          TabOrder = 6
          OnChange = seTransformChange
        end
        object seRotateX: TMultiObjSpinEdit
          Left = 57
          Top = 46
          Width = 66
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          MaxValue = 180
          MinValue = -180
          AutoSize = False
          TabOrder = 7
          OnChange = seTransformChange
        end
        object seRotateY: TMultiObjSpinEdit
          Left = 123
          Top = 46
          Width = 66
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          MaxValue = 180
          MinValue = -180
          AutoSize = False
          TabOrder = 8
          OnChange = seTransformChange
        end
        object seRotateZ: TMultiObjSpinEdit
          Left = 189
          Top = 46
          Width = 66
          Height = 18
          LWSensitivity = 0.01
          ButtonKind = bkLightWave
          MaxValue = 180
          MinValue = -180
          AutoSize = False
          TabOrder = 9
          OnChange = seTransformChange
        end
      end
      object tsMeshes: TTabSheet
        Caption = 'Meshes'
        ImageIndex = 3
        OnShow = tsMeshesShow
        object Panel2: TPanel
          Left = 227
          Top = 0
          Width = 137
          Height = 314
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          object GroupBox4: TGroupBox
            Left = 0
            Top = 0
            Width = 137
            Height = 53
            Align = alTop
            Caption = ' Summary Info '
            TabOrder = 0
            object RxLabel1: TLabel
              Left = 5
              Top = 18
              Width = 41
              Height = 13
              Caption = 'Vertices:'
            end
            object RxLabel3: TLabel
              Left = 5
              Top = 32
              Width = 32
              Height = 13
              Caption = 'Faces:'
            end
            object lbVertices: TLabel
              Left = 67
              Top = 18
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
            object lbFaces: TLabel
              Left = 67
              Top = 32
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
        end
        object tvMeshes: TElTree
          Left = 0
          Top = 0
          Width = 227
          Height = 314
          Cursor = crDefault
          LeftPosition = 0
          DragCursor = crDrag
          Align = alLeft
          AutoCollapse = False
          DockOrientation = doNoOrient
          DefaultSectionWidth = 120
          BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
          CustomPlusMinus = True
          DoInplaceEdit = False
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
          ShowImages = False
          ShowLeafButton = False
          ShowLines = False
          StoragePath = '\Tree'
          TabOrder = 1
          TabStop = True
          Tracking = False
          TrackColor = clBlack
          VertDivLinesColor = 7368816
          VerticalLines = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          VertScrollBarStyles.UseXPThemes = False
          VirtualityLevel = vlNone
          UseXPThemes = False
          BkColor = clGray
          OnItemFocused = tvMeshesItemFocused
        end
      end
      object tsSurfaces: TTabSheet
        Caption = 'Surfaces'
        ImageIndex = 2
        OnShow = tsSurfacesShow
        object paTex_Surf: TPanel
          Left = 210
          Top = 0
          Width = 154
          Height = 314
          Align = alRight
          BevelOuter = bvNone
          TabOrder = 0
          object paTexture: TPanel
            Left = 0
            Top = 0
            Width = 154
            Height = 277
            BevelOuter = bvNone
            TabOrder = 0
            Visible = False
            object GroupBox2: TGroupBox
              Left = 0
              Top = 0
              Width = 154
              Height = 277
              Align = alClient
              Caption = ' Texture Properties '
              TabOrder = 0
              object RxLabel7: TLabel
                Left = 4
                Top = 18
                Width = 31
                Height = 13
                Caption = 'Width:'
              end
              object RxLabel8: TLabel
                Left = 4
                Top = 33
                Width = 34
                Height = 13
                Caption = 'Height:'
              end
              object RxLabel9: TLabel
                Left = 4
                Top = 48
                Width = 30
                Height = 13
                Caption = 'Alpha:'
              end
              object lbWidth: TLabel
                Left = 45
                Top = 18
                Width = 13
                Height = 13
                Caption = '...'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = [fsBold]
                ParentFont = False
              end
              object lbHeight: TLabel
                Left = 45
                Top = 33
                Width = 13
                Height = 13
                Caption = '...'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = [fsBold]
                ParentFont = False
              end
              object lbAlpha: TLabel
                Left = 45
                Top = 48
                Width = 13
                Height = 13
                Caption = '...'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = [fsBold]
                ParentFont = False
              end
              object paImage: TPanel
                Left = 3
                Top = 67
                Width = 147
                Height = 147
                BevelOuter = bvLowered
                Caption = '<image>'
                TabOrder = 0
                object pbImage: TPaintBox
                  Left = 1
                  Top = 1
                  Width = 145
                  Height = 145
                  Align = alClient
                  OnPaint = pbImagePaint
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
              object RxLabel18: TLabel
                Left = 4
                Top = 31
                Width = 64
                Height = 13
                Caption = 'Double Sided'
              end
              object lbSurfSideFlag: TLabel
                Left = 95
                Top = 31
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
              object RxLabel35: TLabel
                Left = 5
                Top = 15
                Width = 32
                Height = 13
                Caption = 'Faces:'
              end
              object lbSurfFaces: TLabel
                Left = 95
                Top = 15
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
              object ebSelectShader: TExtBtn
                Left = 4
                Top = 52
                Width = 66
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
              object lbShader: TLabel
                Left = 6
                Top = 69
                Width = 146
                Height = 13
                AutoSize = False
                Caption = 'default'
                Color = clBtnFace
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clNavy
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentColor = False
                ParentFont = False
              end
              object ebSelectTexture: TExtBtn
                Left = 4
                Top = 118
                Width = 66
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
              object lbTexture: TLabel
                Left = 6
                Top = 135
                Width = 146
                Height = 13
                AutoSize = False
                Caption = 'default'
                Color = clBtnFace
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clNavy
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentColor = False
                ParentFont = False
              end
              object ebSelectShaderXRLC: TExtBtn
                Left = 4
                Top = 85
                Width = 66
                Height = 16
                Align = alNone
                BevelShow = False
                CloseButton = False
                Caption = 'Compile'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentFont = False
                Transparent = False
                FlatAlwaysEdge = True
                OnClick = ebSelectShaderXRLCClick
              end
              object lbShaderXRLC: TLabel
                Left = 6
                Top = 102
                Width = 146
                Height = 13
                AutoSize = False
                Caption = 'default'
                Color = clBtnFace
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clNavy
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentColor = False
                ParentFont = False
              end
              object gbFVF: TGroupBox
                Left = 2
                Top = 174
                Width = 150
                Height = 101
                Align = alBottom
                Caption = ' Surface FVF '
                TabOrder = 0
                object RxLabel14: TLabel
                  Left = 4
                  Top = 82
                  Width = 66
                  Height = 13
                  Caption = 'UV'#39's channel:'
                end
                object lbSurfFVF_TCCount: TLabel
                  Left = 90
                  Top = 82
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
                object RxLabel6: TLabel
                  Left = 4
                  Top = 15
                  Width = 24
                  Height = 13
                  Caption = 'XYZ:'
                end
                object RxLabel12: TLabel
                  Left = 4
                  Top = 28
                  Width = 36
                  Height = 13
                  Caption = 'Normal:'
                end
                object RxLabel15: TLabel
                  Left = 4
                  Top = 41
                  Width = 36
                  Height = 13
                  Caption = 'Diffuse:'
                end
                object RxLabel16: TLabel
                  Left = 4
                  Top = 54
                  Width = 45
                  Height = 13
                  Caption = 'Specular:'
                end
                object RxLabel17: TLabel
                  Left = 4
                  Top = 69
                  Width = 51
                  Height = 13
                  Caption = 'XYZRHW:'
                end
                object lbSurfFVF_XYZ: TLabel
                  Left = 90
                  Top = 15
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
                object lbSurfFVF_Normal: TLabel
                  Left = 90
                  Top = 28
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
                object lbSurfFVF_Diffuse: TLabel
                  Left = 90
                  Top = 41
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
                object lbSurfFVF_Specular: TLabel
                  Left = 90
                  Top = 54
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
                object lbSurfFVF_XYZRHW: TLabel
                  Left = 90
                  Top = 69
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
            end
          end
          object Panel5: TPanel
            Left = 0
            Top = 297
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
        object tvSurfaces: TElTree
          Left = 0
          Top = 0
          Width = 209
          Height = 314
          Cursor = crDefault
          LeftPosition = 0
          DragCursor = crDrag
          Align = alLeft
          AutoCollapse = False
          DockOrientation = doNoOrient
          DefaultSectionWidth = 120
          BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
          CustomPlusMinus = True
          DoInplaceEdit = False
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
          ShowImages = False
          ShowLeafButton = False
          ShowLines = False
          StoragePath = '\Tree'
          TabOrder = 1
          TabStop = True
          Tracking = False
          TrackColor = clBlack
          VertDivLinesColor = 7368816
          VerticalLines = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          VertScrollBarStyles.UseXPThemes = False
          VirtualityLevel = vlNone
          UseXPThemes = False
          BkColor = clGray
          OnItemFocused = tvSurfacesItemFocused
        end
      end
      object tsScript: TTabSheet
        Caption = 'LTX Script'
        Enabled = False
        object mmScript: TMemo
          Left = 0
          Top = 0
          Width = 263
          Height = 314
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
          Left = 263
          Top = 0
          Width = 101
          Height = 314
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
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
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
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
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
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
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
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
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
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
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
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
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
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
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
      Left = 208
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
    object ebCancel: TExtBtn
      Left = 290
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
    object ebApply: TExtBtn
      Left = 126
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Apply'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebApplyClick
    end
  end
  object fsStorage: TFormStorage
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
    Top = 307
  end
end
