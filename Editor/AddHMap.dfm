object fraAddHMap: TfraAddHMap
  Left = 0
  Top = 0
  Width = 443
  Height = 277
  Align = alClient
  TabOrder = 0
  object Panel4: TPanel
    Left = 0
    Top = 0
    Width = 443
    Height = 197
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object Bevel1: TBevel
      Left = 2
      Top = 17
      Width = 113
      Height = 113
    end
    object Label2: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Height Map:'
      Color = clBtnShadow
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object SpeedButton19: TSpeedButton
      Left = 102
      Top = 2
      Width = 11
      Height = 11
      Flat = True
      Glyph.Data = {
        DE000000424DDE00000000000000360000002800000007000000070000000100
        180000000000A8000000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFF00
        0000FFFFFFFFFFFF000000000000000000FFFFFFFFFFFF000000FFFFFF000000
        000000000000000000000000FFFFFF0000000000000000000000000000000000
        00000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000}
      OnClick = PanelMinClick
    end
    object sbLoad: TSpeedButton
      Left = 3
      Top = 164
      Width = 111
      Height = 15
      Caption = 'Load'
      Flat = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = sbLoadClick
    end
    object lbName: TLabel
      Left = 3
      Top = 132
      Width = 111
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'none'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object imHMap: TImage
      Left = 4
      Top = 19
      Width = 109
      Height = 109
      Stretch = True
    end
    object lbSize: TLabel
      Left = 3
      Top = 148
      Width = 111
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '???x???'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object sbClear: TSpeedButton
      Left = 3
      Top = 180
      Width = 111
      Height = 15
      Caption = 'Clear'
      Flat = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = sbClearClick
    end
  end
  object opd: TOpenPictureDialog
    Filter = 'Height Map file (*.bmp)|*.bmp'
    InitialDir = '.'
    Title = 'Load Height Map file'
    Left = 88
    Top = 56
  end
end
