object fraTopBar: TfraTopBar
  Left = 0
  Top = 0
  Width = 443
  Height = 277
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Align = alClient
  Constraints.MaxHeight = 18
  Constraints.MinHeight = 18
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clBlack
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  ParentFont = False
  TabOrder = 0
  object paView: TPanel
    Left = 22
    Top = 0
    Width = 159
    Height = 277
    Align = alLeft
    BevelOuter = bvNone
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    object ebViewFront: TExtBtn
      Left = 0
      Top = 0
      Width = 15
      Height = 18
      Hint = 'Front View'
      Align = alNone
      HotTrack = True
      CloseButton = False
      Caption = 'F'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      Margin = 2
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebViewClick
    end
    object ebViewLeft: TExtBtn
      Left = 30
      Top = 0
      Width = 15
      Height = 18
      Hint = 'Left View'
      Align = alNone
      HotTrack = True
      CloseButton = False
      Caption = 'L'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      Margin = 2
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebViewClick
    end
    object ebViewTop: TExtBtn
      Left = 60
      Top = 0
      Width = 15
      Height = 18
      Hint = 'Top View'
      Align = alNone
      HotTrack = True
      CloseButton = False
      Caption = 'T'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      Margin = 2
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebViewClick
    end
    object ebViewBack: TExtBtn
      Left = 15
      Top = 0
      Width = 15
      Height = 18
      Hint = 'Back View'
      Align = alNone
      HotTrack = True
      CloseButton = False
      Caption = 'B'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      Margin = 2
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebViewClick
    end
    object ebViewRight: TExtBtn
      Left = 45
      Top = 0
      Width = 15
      Height = 18
      Hint = 'Right View'
      Align = alNone
      HotTrack = True
      CloseButton = False
      Caption = 'R'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      Margin = 2
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebViewClick
    end
    object ebViewBottom: TExtBtn
      Left = 75
      Top = 0
      Width = 15
      Height = 18
      Hint = 'Bottom View'
      Align = alNone
      HotTrack = True
      CloseButton = False
      Caption = 'B'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      Margin = 2
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebViewClick
    end
    object ebCameraPlane: TExtBtn
      Left = 91
      Top = 0
      Width = 20
      Height = 18
      Hint = 'Zoom Extents'
      Align = alNone
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Down = True
      Caption = 'P'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 1
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebCameraClick
    end
    object ebCameraArcBall: TExtBtn
      Tag = 1
      Left = 111
      Top = 0
      Width = 20
      Height = 18
      Hint = 'Zoom Extents Selected'
      Align = alNone
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Caption = 'A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 1
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebCameraClick
    end
    object ebCameraFly: TExtBtn
      Tag = 2
      Left = 131
      Top = 0
      Width = 20
      Height = 18
      Hint = 'Zoom Extents Selected'
      Align = alNone
      HotTrack = True
      CloseButton = False
      GroupIndex = 1
      Caption = 'F'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Margin = 1
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebCameraClick
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 22
    Height = 277
    Align = alLeft
    BevelOuter = bvNone
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    object ebZoomExtents: TExtBtn
      Left = 0
      Top = 0
      Width = 20
      Height = 18
      Hint = 'Zoom Extents'
      Align = alNone
      HotTrack = True
      CloseButton = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      Glyph.Data = {
        36060000424D3606000000000000360000002800000020000000100000000100
        18000000000000060000120B0000120B0000000000000000000035356C292993
        3030981F1F86585860C1C1BEBDBCBDC2C8C7AEB5B4A6A7A7C5C5C252525F2020
        8A2F2F962626903F3F728A8A869191919393938B8B8B8F8F8FBCBCBCBBBBBBBF
        BFBFB6B6B6B1B1B1BEBEBE8D8D8D8C8C8C9393939090908D8D8B2525892323EC
        1818D63C3C7DBEC0B8C3C8C8B0B3B38C777A77373E35191CB1B2B3BBBBB43434
        7C1919D92020E62E2E8C8C8C86A3A3A39C9C9C8F8F8FBBBBBBBFBFBFB6B6B6A0
        A0A08D8D8D787878B6B6B6B8B8B88E8E8E9C9C9CA1A1A190908B2D2D901918D5
        4B4B75C0C8B8AAA9A9805F63733B43805357827273311D1E6B474BC3C9C8BCBC
        AA4242731717D83535928F8F899C9C9B929292BBBBBBB2B2B29898988D8D8D95
        95959D9D9D7878788F8F8FBFBFBFB7B7B78F8F8F9C9C9C93938E1B1B7F414283
        A8A997815B60753A42825C5F90848394908F9086853F34355F3F42807375CACD
        CEB9B9A833337B272786878784929292AFAFAF9898988D8D8D979797A4A4A4A8
        A8A8A5A5A58080808A8A8A9D9D9DC2C2C2B5B5B58E8E8E8C8C8A58585FB9BDB6
        84525A885F63918A8997908F9B9090A09393A093934D33357067675B4144A3A3
        A3C7C7C6B2B2AB5A5A638B8B8BBABAB99696969A9A9AA5A5A5A8A8A8A9A9A9AB
        ABABABABAB838383979797898989B0B0B0BFBFBFB5B5B58E8E8FC5C4C1B2B6B6
        834E569A91919C9090A29494A69898A99D9DACA09F5E383C6E656576696A6558
        58BDBFBFBDBDBEC2C2BFBEBEBEB7B7B7959595A9A9A9A9A9A9ABABABADADADB0
        B0B0B1B1B1888888959595989898909090BBBBBBBBBBBBBCBCBCBEBFBFA4A0A1
        8A555BAA9F9FA99B9BADA0A0B1A2A2B4A6A6B8ABAB713D416E6161847B7A6959
        59ADAEAEBDBDBDBABABABCBCBCAFAFAF989898B0B0B0AFAFAFB1B1B1B2B2B2B4
        B4B4B6B6B68D8D8D9494949F9F9F919191B4B4B4BBBBBBB9B9B9C1C3C3998B8E
        9D6A70B1ABA9B4A6A6B8A9AABBABABBFAFAEC6BEBD80454B6F6364877C7C6E5C
        5EA5A3A3BEBFBFB9B9B9BDBDBDA9A9A9A1A1A1B4B4B4B3B3B3B5B5B5B7B7B7B8
        B8B8BDBDBD929292969696A1A1A1939393AFAFAFBCBCBCB9B9B9C2C6C590787C
        A88084BEB2B1C1B0B0C7BCBBC6C1BFC1BAB8B8A3A5922F3B7467688B84837764
        64979293C0C1C1B8B8B8BEBEBEA1A1A1A9A9A9B8B8B8B9B9B9BDBDBDBEBEBEBB
        BBBBB4B4B4929292979797A3A3A3979797A9A9A9BDBDBDB9B8B8BFC6C5826568
        BBA7A8C3BCB9C2ADADBA9295B9747FC26574D67887EC939EB04754847E7D7C70
        6F8B8485C0C1C1BABABABEBEBE9A9A9AB6B6B6BCBCBCB8B8B8B1B1B1AAAAAAA8
        A8A8B1B1B1BDBDBD9E9E9EA0A0A09B9B9BA3A3A3BCBCBCBABABAC2C9C672484D
        C27882C96E7BDB7988EEA0ACFCCBD2FFE6E9FFF2F1FCF7F6F8BEC3C554637F6C
        6D807878C3C4C3C2C2C0BEBEBE909090ADADADACACACB3B3B3C0C0C0CCCCCCD4
        D4D4D6D6D6D7D7D7C9C9C9A5A5A59B9B9B9D9D9DBEBEBEBFBFBF5D61657F6561
        994651E8BDC1FFFFFEFFFDFBF9EBEAF5E2E2F1DEDEEDD9D9E8E1E0EEC2C6B646
        55785C5FBABFB75D5C648D8D8F989898989898C5C5C5DADADAD9D9D9D3D3D3D0
        D0D0CECECECDCDCDCDCDCDC7C7C79F9F9F969696B9B9B98F8F901C1C7E4A4E8C
        A8B09E857A7D926E71CAA6AAE6D1D1DDC7C8CBB4B5BAA0A2A4898A927E7F8D67
        6B8671633E4186272684888885969696B1B1B1A0A0A09F9F9FB9B9B9C9C9C9C5
        C5C5BCBCBCB4B4B4AAAAAAA3A3A39D9D9D9A9A9A9393938C8C8A2E2E941A1ADA
        4D4C73C8CBBBADAFB086777A8B6B6F8B76788B7C7E8F87889B9597AEADAEB7BD
        AB4A4F7B1717DB36369591918B9D9D9D919191BDBDBDB4B4B49F9F9F9D9D9DA0
        A0A0A1A1A1A5A5A5AAAAAAB5B5B5B6B6B69393939D9D9D94948F2A2A902929F7
        1D1DDE454584C2C2BAC1C4C4B4B7B7BABCBCBDC0C0BDBEBFC1C3C3BEBEB64141
        871F1FE12525F23131928E8E88A7A7A79F9F9F939393BCBCBCBDBDBDB8B8B8BA
        BABABCBCBCBCBCBCBEBEBEBABABA9393939F9F9FA5A5A592928D2D2D661F1F8F
        2424920B0B764F4F56C1C1BEBBBABAB8B8B8B8B8B8BABABBC0C0BD4949540C0C
        7B2323901D1D8C36366D8686838D8D8D8F8F8F8383838B8B8BBCBCBCBABABAB9
        B9B9B9B9B9BABABABCBCBC8989898585858E8E8E8C8C8C8B8B88}
      Margin = 1
      NumGlyphs = 2
      ParentFont = False
      Spacing = 15
      Transparent = False
      OnClick = ebZoomExtentsClick
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'Top Bar'
    Options = []
    RegistryRoot = prLocalMachine
    OnRestorePlacement = fsStorageRestorePlacement
    StoredProps.Strings = (
      'ebCameraArcBall.Down'
      'ebCameraFly.Down'
      'ebCameraPlane.Down')
    StoredValues = <>
    Left = 65522
    Top = 65520
  end
end
