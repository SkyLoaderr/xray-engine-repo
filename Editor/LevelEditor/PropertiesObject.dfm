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
      ActivePage = tsScript
      Align = alClient
      MultiLine = True
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
              F4FFFFFF05000000CC9B9E04005A0000FFFFFFFF000001000000004114000000
              000000001027000000000041EC43B90504000000000000000000000000000100
              00000000000000000001000000000000CD4D08400F4E0840384E08405D4E0840
              824E0840A64E0840C84E0840E94E08400A4F0840304F0840564F08407A4F0840
              9D4F0840BE4F084024750041CC9B9E04DC590000010000000001000000000000
              000000000000CC9B9E04005A0000FFFFFFFF00000100000000413C0000000000
              000010270000000001418048B905000000000000000000000000000001000000
              0000000000000001000000000000CD4D08400F4E0840384E08405D4E0840824E
              0840A64E0840C84E0840E94E08400A4F0840304F0840564F08407A4F08409D4F
              0840BE4F084024750041CC9B9E04DC590000080000004D696E696D756D000100
              0000000000000000000000CC9B9E04005A0000FFFFFFFF00000100000000413C
              0000000000000010270000000001414449B90501000000000000000000000000
              00010000000000000000000001000000000000CD4D08400F4E0840384E08405D
              4E0840824E0840A64E0840C84E0840E94E08400A4F0840304F0840564F08407A
              4F08409D4F0840BE4F084024750041CC9B9E04DC590000080000004D6178696D
              756D0001000000000000000000000000CC9B9E04005A0000FFFFFFFF00000100
              000000413C000000000000001027000000000141084AB9050200000000000000
              000000000000010000000000000000000001000000000000CD4D08400F4E0840
              384E08405D4E0840824E0840A64E0840C84E0840E94E08400A4F0840304F0840
              564F08407A4F08409D4F0840BE4F084024750041CC9B9E04DC59000005000000
              53697A650001000000000000000000000000CC9B9E04005A0000FFFFFFFF0000
              0100000000413C000000000000001027000000000141CC4AB905030000000000
              0000000000000000010000000000000000000001000000000000CD4D08400F4E
              0840384E08405D4E0840824E0840A64E0840C84E0840E94E08400A4F0840304F
              0840564F08407A4F08409D4F0840BE4F084024750041CC9B9E04DC5900000700
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
          Height = 296
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
          Height = 296
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
          Height = 296
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
        object tvSurfaces: TElTree
          Left = 0
          Top = 0
          Width = 209
          Height = 296
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
          object RxLabel20: TLabel
            Left = 5
            Top = 13
            Width = 31
            Height = 13
            Caption = 'Name:'
          end
          object lbOMotionName: TLabel
            Left = 51
            Top = 13
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
          object RxLabel23: TLabel
            Left = 5
            Top = 27
            Width = 37
            Height = 13
            Caption = 'Frames:'
          end
          object lbOMotionFrames: TLabel
            Left = 51
            Top = 27
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
          object lbOMotionFPS: TLabel
            Left = 51
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
          object RxLabel28: TLabel
            Left = 5
            Top = 41
            Width = 23
            Height = 13
            Caption = 'FPS:'
          end
        end
        object tvOMotions: TElTree
          Left = 0
          Top = 0
          Width = 223
          Height = 235
          Cursor = crDefault
          LeftPosition = 0
          DoubleBuffered = False
          DragCursor = crDrag
          Align = alLeft
          AlwaysKeepFocus = True
          AutoCollapse = False
          DockOrientation = doNoOrient
          DefaultSectionWidth = 120
          BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
          DoInplaceEdit = False
          Enabled = False
          ExplorerEditMode = False
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
          HorzScrollBarStyles.ShowTrackHint = False
          HorzScrollBarStyles.Width = 16
          HorzScrollBarStyles.ButtonSize = 16
          HorzScrollBarStyles.UseSystemMetrics = False
          IgnoreEnabled = False
          IncrementalSearch = False
          KeepSelectionWithinLevel = False
          LineBorderActiveColor = clBlack
          LineBorderInactiveColor = clBlack
          LineHeight = 17
          MouseFrameSelect = True
          MultiSelect = False
          OwnerDrawMask = '~~@~~'
          ScrollbarOpposite = False
          ShowButtons = False
          ShowCheckboxes = True
          ShowImages = False
          ShowLeafButton = False
          ShowRootButtons = False
          StoragePath = '\Tree'
          TabOrder = 1
          TabStop = True
          VertScrollBarStyles.ShowTrackHint = True
          VertScrollBarStyles.Width = 16
          VertScrollBarStyles.ButtonSize = 16
          VertScrollBarStyles.UseSystemMetrics = False
          VirtualityLevel = vlNone
          BkColor = clBtnShadow
          OnItemChange = tvOMotionsItemChange
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
            object RxLabel19: TLabel
              Left = 5
              Top = 13
              Width = 65
              Height = 13
              Caption = 'Motion count:'
            end
            object lbOMotionCount: TLabel
              Left = 75
              Top = 13
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
              Top = 27
              Width = 68
              Height = 13
              Caption = 'Active Motion:'
            end
            object lbActiveOMotion: TLabel
              Left = 75
              Top = 27
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
          Cursor = crDefault
          LeftPosition = 0
          DragCursor = crDrag
          Align = alLeft
          AlwaysKeepFocus = True
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
          ShowCheckboxes = True
          ShowImages = False
          ShowLeafButton = False
          ShowLines = False
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
          OnItemChange = tvSMotionsItemChange
          OnItemFocused = tvSMotionsItemFocused
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
          object RxLabel29: TLabel
            Left = 5
            Top = 13
            Width = 31
            Height = 13
            Caption = 'Name:'
          end
          object lbSMotionName: TLabel
            Left = 51
            Top = 13
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
          object RxLabel24: TLabel
            Left = 5
            Top = 25
            Width = 37
            Height = 13
            Caption = 'Frames:'
          end
          object lbSMotionFrames: TLabel
            Left = 51
            Top = 25
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
          object RxLabel27: TLabel
            Left = 5
            Top = 37
            Width = 23
            Height = 13
            Caption = 'FPS:'
          end
          object lbSMotionFPS: TLabel
            Left = 51
            Top = 37
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
          object Bevel1: TBevel
            Left = 2
            Top = 52
            Width = 135
            Height = 2
          end
          object RxLabel30: TLabel
            Left = 5
            Top = 139
            Width = 34
            Height = 13
            Caption = 'Speed:'
          end
          object RxLabel31: TLabel
            Left = 5
            Top = 156
            Width = 37
            Height = 13
            Caption = 'Accrue:'
          end
          object RxLabel32: TLabel
            Left = 5
            Top = 174
            Width = 31
            Height = 13
            Caption = 'Falloff:'
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
              object RxLabel39: TLabel
                Left = 0
                Top = -2
                Width = 49
                Height = 13
                Caption = 'Bone part:'
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
              object RxLabel40: TLabel
                Left = 0
                Top = -2
                Width = 52
                Height = 13
                Caption = 'Start bone:'
                Font.Charset = DEFAULT_CHARSET
                Font.Color = clWindowText
                Font.Height = -11
                Font.Name = 'MS Sans Serif'
                Font.Style = []
                ParentFont = False
              end
              object RxLabel34: TLabel
                Left = 0
                Top = 35
                Width = 33
                Height = 13
                Caption = 'Power:'
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
            object RxLabel22: TLabel
              Left = 5
              Top = 27
              Width = 65
              Height = 13
              Caption = 'Motion count:'
            end
            object lbSMotionCount: TLabel
              Left = 75
              Top = 27
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
            object RxLabel26: TLabel
              Left = 5
              Top = 41
              Width = 68
              Height = 13
              Caption = 'Active Motion:'
            end
            object lbActiveSMotion: TLabel
              Left = 75
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
            object RxLabel37: TLabel
              Left = 5
              Top = 13
              Width = 58
              Height = 13
              Caption = 'Bone count:'
            end
            object lbSBoneCount: TLabel
              Left = 75
              Top = 13
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
        Enabled = False
        object mmScript: TMemo
          Left = 0
          Top = 0
          Width = 263
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
          Left = 263
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
    Left = 2
    Top = 307
  end
  object InplaceText: TElTreeInplaceEdit
    Tree = tvSMotions
    Types = [sftText]
    OnValidateResult = InplaceTextValidateResult
    Left = 8
    Top = 48
  end
end
