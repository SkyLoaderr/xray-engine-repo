object fraLeftBar: TfraLeftBar
  Left = 0
  Top = 0
  Width = 443
  Height = 593
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
    Width = 300
    Height = 593
    Align = alLeft
    BevelInner = bvLowered
    BevelOuter = bvNone
    Color = 10528425
    Constraints.MaxWidth = 300
    Constraints.MinWidth = 300
    TabOrder = 0
    object paBasic: TPanel
      Left = 1
      Top = 1
      Width = 298
      Height = 591
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 0
      object Splitter1: TSplitter
        Left = 0
        Top = 350
        Width = 298
        Height = 2
        Cursor = crVSplit
        Align = alBottom
        Color = clBlack
        ParentColor = False
      end
      object paModel: TPanel
        Left = 0
        Top = 82
        Width = 298
        Height = 79
        Hint = 'Scene commands'
        Align = alTop
        Color = 10528425
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        object Label4: TLabel
          Left = 1
          Top = 1
          Width = 296
          Height = 13
          Align = alTop
          Alignment = taCenter
          Caption = 'Model'
          Color = clGray
          ParentColor = False
          OnClick = PanelMaximizeClick
        end
        object ExtBtn2: TExtBtn
          Left = 286
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
        object ebRenderEditorStyle: TExtBtn
          Left = 84
          Top = 31
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
          Left = 144
          Top = 31
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
          Left = 16
          Top = 32
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
        Top = 161
        Width = 298
        Height = 189
        Align = alClient
        Color = 10528425
        ParentShowHint = False
        ShowHint = False
        TabOrder = 1
        object Label6: TLabel
          Left = 1
          Top = 1
          Width = 296
          Height = 13
          Align = alTop
          Alignment = taCenter
          Caption = 'Object Properties'
          Color = clGray
          ParentColor = False
          OnClick = PanelMaximizeClick
        end
        object Bevel6: TBevel
          Left = 1
          Top = 14
          Width = 296
          Height = 1
          Align = alTop
          Shape = bsLeftLine
        end
        object paObjectProps: TPanel
          Left = 1
          Top = 15
          Width = 296
          Height = 173
          Align = alClient
          BevelOuter = bvNone
          Color = 10528425
          TabOrder = 0
        end
      end
      object paScene: TPanel
        Left = 0
        Top = 0
        Width = 298
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
          Width = 296
          Height = 13
          Align = alTop
          Alignment = taCenter
          Caption = 'Scene'
          Color = clGray
          ParentColor = False
          OnClick = PanelMaximizeClick
        end
        object ebSceneMin: TExtBtn
          Left = 286
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
      object paCurrentMotion: TPanel
        Left = 0
        Top = 352
        Width = 298
        Height = 239
        Align = alBottom
        Color = 10528425
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        object Label1: TLabel
          Left = 1
          Top = 1
          Width = 296
          Height = 13
          Align = alTop
          Alignment = taCenter
          Caption = 'Item Properties'
          Color = clGray
          ParentColor = False
          OnClick = PanelMaximizeClick
        end
        object ExtBtn10: TExtBtn
          Left = 286
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
          Width = 296
          Height = 224
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          object Bevel1: TBevel
            Left = 0
            Top = 0
            Width = 296
            Height = 1
            Align = alTop
            Shape = bsLeftLine
          end
          object paItemProps: TPanel
            Left = 0
            Top = 1
            Width = 296
            Height = 223
            Align = alClient
            BevelOuter = bvNone
            Color = 10528425
            TabOrder = 0
          end
        end
      end
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Left Bar'
    Options = []
    RegistryRoot = prLocalMachine
    Version = 12
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'paScene.Height'
      'paScene.Tag'
      'paModel.Height'
      'paModel.Tag'
      'paCurrentMotion.Height'
      'paCurrentMotion.Tag')
    StoredValues = <>
    Left = 65529
    Top = 65526
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
    object miExportOGF: TMenuItem
      Caption = 'Export OGF'
      OnClick = miExportOGFClick
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
    Left = 10
    Top = 370
    object N11: TMenuItem
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
    end
    object CollapseAll1: TMenuItem
      Caption = 'Collapse All'
    end
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
