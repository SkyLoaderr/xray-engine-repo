object fraScalePClipper: TfraScalePClipper
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
    Height = 47
    Align = alTop
    ParentColor = True
    TabOrder = 0
    object Label2: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Scale p-clipper:'
      Color = clBtnShadow
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      OnClick = Label2Click
    end
    object SpeedButton19: TExtBtn
      Left = 102
      Top = 2
      Width = 11
      Height = 11
      Align = alNone
      CloseButton = False
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
    object Label1: TLabel
      Left = 4
      Top = 18
      Width = 22
      Height = 13
      Caption = 'Axis:'
    end
    object sbAxisW: TExtBtn
      Left = 38
      Top = 16
      Width = 37
      Height = 14
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = 'W'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object sbAxisH: TExtBtn
      Left = 75
      Top = 16
      Width = 38
      Height = 14
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 2
      Down = True
      Caption = 'H'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object sbCSLocal: TExtBtn
      Left = 38
      Top = 30
      Width = 75
      Height = 14
      Align = alNone
      AllowAllUp = True
      BevelShow = False
      HotTrack = True
      CloseButton = False
      GroupIndex = 5
      Down = True
      Caption = 'Local'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label3: TLabel
      Left = 3
      Top = 30
      Width = 17
      Height = 13
      Caption = 'CS:'
    end
  end
end
