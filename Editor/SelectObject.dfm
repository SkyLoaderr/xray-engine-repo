object fraSelectObject: TfraSelectObject
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
    Height = 60
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
      Caption = 'Select:'
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
      OnClick = PaneMinClick
    end
    object Label1: TLabel
      Left = 4
      Top = 17
      Width = 14
      Height = 13
      Caption = 'All:'
    end
    object sbInvert: TExtBtn
      Left = 21
      Top = 15
      Width = 30
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '*'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = sbInvertClick
    end
    object sbSelectAll: TExtBtn
      Left = 52
      Top = 15
      Width = 30
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '+'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = sbSelectAllClick
    end
    object sbDeselectAll: TExtBtn
      Left = 83
      Top = 15
      Width = 30
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = sbDeselectAllClick
    end
    object Label3: TLabel
      Left = 2
      Top = 30
      Width = 28
      Height = 13
      Caption = 'Class:'
    end
    object sbSelectByClass: TExtBtn
      Left = 38
      Top = 30
      Width = 38
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '+'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = sbSelectByClassClick
    end
    object sbDeselectByClass: TExtBtn
      Left = 77
      Top = 30
      Width = 36
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = sbDeselectByClassClick
    end
    object Label5: TLabel
      Left = 2
      Top = 45
      Width = 25
      Height = 13
      Caption = 'Refs:'
    end
    object sbSelectByRefs: TExtBtn
      Left = 38
      Top = 45
      Width = 38
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '+'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = sbSelectByRefsClick
    end
    object sbDeselectByRefs: TExtBtn
      Left = 77
      Top = 45
      Width = 36
      Height = 14
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = sbDeselectByRefsClick
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 60
    Width = 443
    Height = 63
    Align = alTop
    ParentColor = True
    TabOrder = 1
    object Label4: TLabel
      Left = 1
      Top = 1
      Width = 441
      Height = 13
      Align = alTop
      Alignment = taCenter
      Caption = 'Commands:'
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
    object SpeedButton1: TExtBtn
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
      OnClick = PaneMinClick
    end
    object sbResolve: TExtBtn
      Left = 2
      Top = 15
      Width = 111
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Resolve'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = sbResolveClick
    end
    object sbProperties: TExtBtn
      Left = 2
      Top = 31
      Width = 111
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Properties..'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = sbPropertiesClick
    end
    object sbRemove: TExtBtn
      Left = 2
      Top = 47
      Width = 111
      Height = 15
      Align = alNone
      BevelShow = False
      HotTrack = True
      CloseButton = False
      Caption = 'Remove'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = sbRemoveClick
    end
  end
end
