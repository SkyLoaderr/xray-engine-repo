object fraSelectHMap: TfraSelectHMap
  Left = 0
  Top = 0
  Width = 443
  Height = 279
  Align = alClient
  TabOrder = 0
  object Panel4: TPanel
    Left = 0
    Top = 0
    Width = 443
    Height = 160
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
      Caption = 'Properties:'
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
    object Label1: TLabel
      Left = 5
      Top = 18
      Width = 79
      Height = 13
      Caption = 'Map Left Corner:'
    end
    object Label3: TLabel
      Left = 18
      Top = 34
      Width = 10
      Height = 13
      Caption = 'X:'
    end
    object Label4: TLabel
      Left = 18
      Top = 53
      Width = 10
      Height = 13
      Caption = 'Y:'
    end
    object Label5: TLabel
      Left = 18
      Top = 72
      Width = 10
      Height = 13
      Caption = 'Z:'
    end
    object Label6: TLabel
      Left = 98
      Top = 72
      Width = 14
      Height = 13
      Caption = '(m)'
    end
    object Label7: TLabel
      Left = 98
      Top = 53
      Width = 14
      Height = 13
      Caption = '(m)'
    end
    object Label8: TLabel
      Left = 98
      Top = 34
      Width = 14
      Height = 13
      Caption = '(m)'
    end
    object Label9: TLabel
      Left = 5
      Top = 90
      Width = 76
      Height = 13
      Caption = 'Map Dimension:'
    end
    object Label11: TLabel
      Left = 13
      Top = 106
      Width = 15
      Height = 13
      Caption = 'Alt:'
    end
    object Label14: TLabel
      Left = 98
      Top = 106
      Width = 14
      Height = 13
      Caption = '(m)'
    end
    object Label16: TLabel
      Left = 5
      Top = 125
      Width = 64
      Height = 13
      Caption = 'Min Slot Size:'
    end
    object Label17: TLabel
      Left = 98
      Top = 141
      Width = 14
      Height = 13
      Caption = '(m)'
    end
    object seMCenterX: TRxSpinEdit
      Left = 32
      Top = 32
      Width = 65
      Height = 18
      Decimal = 1
      ValueType = vtFloat
      AutoSize = False
      Color = 15132390
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Constraints.MaxHeight = 18
      ParentFont = False
      TabOrder = 0
      OnChange = seMCenterXChange
    end
    object seMCenterY: TRxSpinEdit
      Left = 32
      Top = 51
      Width = 65
      Height = 18
      Decimal = 1
      ValueType = vtFloat
      AutoSize = False
      Color = 15132390
      Constraints.MaxHeight = 18
      TabOrder = 1
      OnChange = seMCenterYChange
    end
    object seMCenterZ: TRxSpinEdit
      Left = 32
      Top = 70
      Width = 65
      Height = 18
      Decimal = 1
      ValueType = vtFloat
      AutoSize = False
      Color = 15132390
      Constraints.MaxHeight = 18
      TabOrder = 2
      OnChange = seMCenterZChange
    end
    object seMDimAlt: TRxSpinEdit
      Left = 32
      Top = 104
      Width = 65
      Height = 18
      Decimal = 1
      ValueType = vtFloat
      AutoSize = False
      Color = 15132390
      Constraints.MaxHeight = 18
      TabOrder = 3
      OnChange = seMDimAltChange
    end
    object seMSlotSize: TRxSpinEdit
      Left = 32
      Top = 139
      Width = 65
      Height = 18
      Decimal = 1
      Increment = 0.1
      ValueType = vtFloat
      AutoSize = False
      Color = 15132390
      Constraints.MaxHeight = 18
      TabOrder = 4
      OnChange = seMSlotSizeChange
    end
  end
end
