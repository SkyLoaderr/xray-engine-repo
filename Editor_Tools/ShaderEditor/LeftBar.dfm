object fraLeftBar: TfraLeftBar
  Left = 0
  Top = 0
  Width = 215
  Height = 188
  HorzScrollBar.Visible = False
  VertScrollBar.Increment = 34
  VertScrollBar.Size = 13
  VertScrollBar.Style = ssHotTrack
  VertScrollBar.Tracking = True
  Align = alClient
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentFont = False
  TabOrder = 0
  object paLeftBar: TPanel
    Left = 0
    Top = 0
    Width = 195
    Height = 97
    BevelInner = bvLowered
    BevelOuter = bvNone
    Constraints.MaxWidth = 195
    Constraints.MinWidth = 195
    TabOrder = 0
    object paScene: TPanel
      Left = 1
      Top = 1
      Width = 193
      Height = 67
      Hint = 'Scene commands'
      Align = alTop
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      object APHeadLabel2: TLabel
        Left = 1
        Top = 1
        Width = 191
        Height = 13
        Align = alTop
        Alignment = taCenter
        Caption = 'Scene'
        Color = clGray
        ParentColor = False
        OnClick = PanelMaximizeClick
      end
      object sbSceneMin: TExtBtn
        Left = 180
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
        Width = 190
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
        Width = 190
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
        Width = 190
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
  end
  object paFrames: TPanel
    Left = 0
    Top = 107
    Width = 195
    Height = 31
    BevelOuter = bvLowered
    Constraints.MaxWidth = 195
    Constraints.MinWidth = 195
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
  object pmSceneFile: TRxPopupMenu
    Alignment = paRight
    AutoPopup = False
    TrackButton = tbLeftButton
    LeftMargin = 10
    Style = msBtnLowered
    Left = 165
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
  object pmSceneCommands: TRxPopupMenu
    AutoPopup = False
    TrackButton = tbLeftButton
    LeftMargin = 10
    Style = msBtnLowered
    Left = 165
    Top = 42
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
