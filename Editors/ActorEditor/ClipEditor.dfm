object ClipMaker: TClipMaker
  Left = 258
  Top = 246
  Width = 500
  Height = 299
  HorzScrollBar.Style = ssFlat
  HorzScrollBar.ThumbSize = 12
  BiDiMode = bdRightToLeft
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Clip Maker'
  Color = 6908265
  Constraints.MinHeight = 256
  Constraints.MinWidth = 500
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  ParentBiDiMode = False
  Scaled = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paB: TPanel
    Left = 104
    Top = 0
    Width = 388
    Height = 272
    Align = alClient
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object paBase: TPanel
      Left = 0
      Top = 0
      Width = 388
      Height = 121
      Align = alTop
      BevelOuter = bvNone
      Color = 6908265
      TabOrder = 0
      object sbBase: TScrollBox
        Left = 0
        Top = 0
        Width = 388
        Height = 121
        HorzScrollBar.Style = ssFlat
        HorzScrollBar.Tracking = True
        VertScrollBar.Visible = False
        Align = alClient
        BorderStyle = bsNone
        TabOrder = 0
        object paFrame: TMxPanel
          Left = 0
          Top = 0
          Width = 241
          Height = 121
          Align = alLeft
          BevelOuter = bvNone
          ParentColor = True
          TabOrder = 0
          object Bevel6: TBevel
            Left = 0
            Top = 82
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object Bevel7: TBevel
            Left = 0
            Top = 50
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object Bevel8: TBevel
            Left = 0
            Top = 66
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object Bevel1: TBevel
            Left = 0
            Top = 18
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object Bevel3: TBevel
            Left = 0
            Top = 34
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object Bevel18: TBevel
            Left = 0
            Top = 102
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object paClips: TMxPanel
            Tag = -1
            Left = 0
            Top = 0
            Width = 241
            Height = 18
            Align = alTop
            BevelOuter = bvNone
            Color = 6316128
            TabOrder = 0
            OnDragDrop = ClipDragDrop
            OnDragOver = ClipDragOver
            OnMouseDown = ClipMouseDown
            OnMouseMove = ClipMouseMove
            OnPaint = paClipsPaint
            object Bevel9: TBevel
              Left = 0
              Top = 0
              Width = 241
              Height = 1
              Align = alTop
              Shape = bsBottomLine
              Style = bsRaised
            end
          end
          object gtClip: TMxPanel
            Left = 0
            Top = 83
            Width = 241
            Height = 19
            Align = alTop
            BevelOuter = bvNone
            Caption = ' '
            Color = 5460819
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            ParentShowHint = False
            ShowHint = False
            TabOrder = 1
            OnPaint = gtClipPaint
          end
          object paBP3: TMxPanel
            Tag = 3
            Left = 0
            Top = 67
            Width = 241
            Height = 15
            Align = alTop
            BevelOuter = bvNone
            Color = 6316128
            TabOrder = 2
            OnDragDrop = BPDragDrop
            OnDragOver = BPDragOver
            OnMouseDown = BPMouseDown
            OnMouseUp = BPMouseUp
            OnPaint = BPOnPaint
          end
          object paBP2: TMxPanel
            Tag = 2
            Left = 0
            Top = 51
            Width = 241
            Height = 15
            Align = alTop
            BevelOuter = bvNone
            Color = 6316128
            TabOrder = 3
            OnDragDrop = BPDragDrop
            OnDragOver = BPDragOver
            OnMouseDown = BPMouseDown
            OnMouseUp = BPMouseUp
            OnPaint = BPOnPaint
          end
          object paBP1: TMxPanel
            Tag = 1
            Left = 0
            Top = 35
            Width = 241
            Height = 15
            Align = alTop
            BevelOuter = bvNone
            Color = 6316128
            TabOrder = 4
            OnDragDrop = BPDragDrop
            OnDragOver = BPDragOver
            OnMouseDown = BPMouseDown
            OnMouseUp = BPMouseUp
            OnPaint = BPOnPaint
          end
          object paBP0: TMxPanel
            Left = 0
            Top = 19
            Width = 241
            Height = 15
            Align = alTop
            BevelOuter = bvNone
            Color = 6316128
            TabOrder = 5
            OnDragDrop = BPDragDrop
            OnDragOver = BPDragOver
            OnMouseDown = BPMouseDown
            OnMouseUp = BPMouseUp
            OnStartDrag = paBPStartDrag
            OnPaint = BPOnPaint
          end
        end
      end
    end
    object paClipProps: TPanel
      Left = 0
      Top = 121
      Width = 388
      Height = 151
      Align = alClient
      BevelOuter = bvNone
      Color = 6908265
      TabOrder = 1
      object Bevel20: TBevel
        Left = 387
        Top = 1
        Width = 1
        Height = 150
        Align = alRight
        Shape = bsRightLine
        Style = bsRaised
      end
      object Bevel15: TBevel
        Left = 0
        Top = 0
        Width = 388
        Height = 1
        Align = alTop
        Shape = bsBottomLine
        Style = bsRaised
      end
    end
  end
  object paA: TPanel
    Left = 0
    Top = 0
    Width = 104
    Height = 272
    Align = alLeft
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object Bevel16: TBevel
      Left = 0
      Top = 102
      Width = 104
      Height = 1
      Align = alTop
      Shape = bsBottomLine
      Style = bsRaised
    end
    object Panel1: TPanel
      Left = 0
      Top = 0
      Width = 104
      Height = 102
      Align = alTop
      BevelOuter = bvNone
      Color = 6316128
      TabOrder = 0
      object MxLabel1: TMxLabel
        Left = 30
        Top = 2
        Width = 50
        Height = 16
        Caption = 'Clips:'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
      end
      object MxLabel2: TMxLabel
        Left = 3
        Top = 18
        Width = 13
        Height = 64
        AutoSize = False
        Caption = 'B o n e'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
        WordWrap = True
      end
      object MxLabel3: TMxLabel
        Left = 14
        Top = 18
        Width = 11
        Height = 64
        AutoSize = False
        Caption = 'p a r t '
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
        WordWrap = True
      end
      object Bevel2: TBevel
        Left = 28
        Top = 0
        Width = 1
        Height = 102
        Shape = bsRightLine
        Style = bsRaised
      end
      object Bevel4: TBevel
        Left = 0
        Top = 18
        Width = 103
        Height = 1
        Shape = bsBottomLine
        Style = bsRaised
      end
      object Bevel5: TBevel
        Left = 0
        Top = 82
        Width = 103
        Height = 1
        Shape = bsBottomLine
        Style = bsRaised
      end
      object Bevel10: TBevel
        Left = 30
        Top = 34
        Width = 73
        Height = 1
        Shape = bsBottomLine
        Style = bsRaised
      end
      object Bevel11: TBevel
        Left = 30
        Top = 50
        Width = 73
        Height = 1
        Shape = bsBottomLine
        Style = bsRaised
      end
      object Bevel12: TBevel
        Left = 30
        Top = 66
        Width = 73
        Height = 1
        Shape = bsBottomLine
        Style = bsRaised
      end
      object MxLabel4: TMxLabel
        Left = 30
        Top = 85
        Width = 42
        Height = 16
        Caption = 'Time:'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -13
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
      end
      object lbBPName0: TMxLabel
        Left = 38
        Top = 19
        Width = 64
        Height = 15
        AutoSize = False
        Caption = '-'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -11
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
      end
      object Bevel13: TBevel
        Left = 103
        Top = 0
        Width = 1
        Height = 102
        Align = alRight
        Shape = bsRightLine
        Style = bsRaised
      end
      object lbBPName1: TMxLabel
        Left = 38
        Top = 35
        Width = 64
        Height = 15
        AutoSize = False
        Caption = '-'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -11
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
      end
      object lbBPName2: TMxLabel
        Left = 38
        Top = 51
        Width = 64
        Height = 15
        AutoSize = False
        Caption = '-'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -11
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
      end
      object lbBPName3: TMxLabel
        Left = 38
        Top = 67
        Width = 64
        Height = 15
        AutoSize = False
        Caption = '-'
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = 12698049
        Font.Height = -11
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ShadowColor = clBlack
        ShadowPos = spRightBottom
      end
      object ebTrash: TExtBtn
        Tag = -1
        Left = 1
        Top = 84
        Width = 26
        Height = 18
        Hint = 'Trash'
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        BtnColor = 10528425
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          76010000424D7601000000000000760000002800000020000000100000000100
          04000000000000010000120B0000120B00001000000000000000000000000000
          800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333000000000
          3333333777777777F3333330F777777033333337F3F3F3F7F3333330F0808070
          33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
          33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
          333333F7F7F7F7F7F3F33030F080707030333737F7F7F7F7F7333300F0808070
          03333377F7F7F7F773333330F080707033333337F7F7F7F7F333333070707070
          33333337F7F7F7F7FF3333000000000003333377777777777F33330F88877777
          0333337FFFFFFFFF7F3333000000000003333377777777777333333330777033
          3333333337FFF7F3333333333000003333333333377777333333}
        NumGlyphs = 2
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = ebTrashClick
        OnDragOver = ebTrashDragOver
      end
    end
    object Panel2: TPanel
      Left = 0
      Top = 103
      Width = 104
      Height = 19
      Align = alTop
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 1
      object ebPrevClip: TExtBtn
        Left = 0
        Top = 2
        Width = 20
        Height = 15
        Hint = 'Prev Clip'
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        BtnColor = 10528425
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          16020000424D16020000000000003600000028000000100000000A0000000100
          180000000000E0010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBA
          BABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABAFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFBABABA222222BABABAFFFFFFFFFFFFFFFFFFBABA
          BA292929BABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA3434340000000B
          0B0BBABABAFFFFFFBABABA505050000000000000BABABAFFFFFFFFFFFFFFFFFF
          777777454545000000000000000000060606BABABA5656560000000000000000
          00000000BABABAFFFFFFFFFFFFFFFFFF77777745454500000000000000000006
          0606BABABA565656000000000000000000000000BABABAFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFBABABA3434340000000B0B0BBABABAFFFFFFBABABA5050500000
          00000000BABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA22
          2222BABABAFFFFFFFFFFFFFFFFFFBABABA292929BABABAFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFBABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebPrevClipClick
      end
      object ebPlay: TExtBtn
        Left = 21
        Top = 2
        Width = 20
        Height = 15
        Hint = 'Play'
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        BtnColor = 10528425
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFA7A7A7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFBABABA0000005050508E8E8EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFBABABA0000000000000000004F4F4F7B7B7BFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000000000000000000008080828
          2828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA000000000000000000000000
          0000000000001212121F1F1FC6C6C6FFFFFFFFFFFFBABABA0000000000000000
          00000000080808282828898989FFFFFFFFFFFFFFFFFFFFFFFFBABABA00000000
          00000000004F4F4F7B7B7BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
          0000005050508E8E8EFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFA7A7A7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebPlayClick
      end
      object ebPause: TExtBtn
        Left = 41
        Top = 2
        Width = 20
        Height = 15
        Hint = 'Pause'
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        BtnColor = 10528425
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          9E010000424D9E0100000000000036000000280000000B0000000A0000000100
          18000000000068010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000FFFF
          FFFFFFFFA5A5A5ADADADFFFFFFFFFFFFFFFFFFADADADA5A5A5FFFFFFFFFFFF00
          0000FFFFFF969696000000000000969696FFFFFF969696000000000000969696
          FFFFFF000000FFFFFF969696000000000000969696FFFFFF9696960000000000
          00969696FFFFFF000000FFFFFF969696000000000000969696FFFFFF96969600
          0000000000969696FFFFFF000000FFFFFF969696000000000000969696FFFFFF
          969696000000000000969696FFFFFF000000FFFFFF9696960000000000009696
          96FFFFFF969696000000000000969696FFFFFF000000FFFFFF96969600000000
          0000969696FFFFFF969696000000000000969696FFFFFF000000FFFFFFFFFFFF
          A5A5A5ADADADFFFFFFFFFFFFFFFFFFADADADA5A5A5FFFFFFFFFFFF000000FFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebPauseClick
      end
      object ebStop: TExtBtn
        Left = 61
        Top = 2
        Width = 20
        Height = 15
        Hint = 'Stop'
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        BtnColor = 10528425
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          9E010000424D9E0100000000000036000000280000000C0000000A0000000100
          18000000000068010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFA5A5A5ADADADADADADADADADADADADA5A5A5FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFF969696000000000000000000000000000000000000969696
          FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000000000000000000000000000
          00969696FFFFFFFFFFFFFFFFFFFFFFFF96969600000000000000000000000000
          0000000000969696FFFFFFFFFFFFFFFFFFFFFFFF969696000000000000000000
          000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF9696960000000000
          00000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF96969600
          0000000000000000000000000000000000969696FFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFA5A5A5ADADADADADADADADADADADADA5A5A5FFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFF}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebStopClick
      end
      object ebNextClip: TExtBtn
        Left = 81
        Top = 2
        Width = 20
        Height = 15
        Hint = 'Next Clip'
        Align = alNone
        BevelShow = False
        HotTrack = True
        HotColor = 15790320
        BtnColor = 10528425
        CloseButton = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Glyph.Data = {
          16020000424D16020000000000003600000028000000100000000A0000000100
          180000000000E0010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABAFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFBABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
          292929BABABAFFFFFFFFFFFFFFFFFFBABABA222222BABABAFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA000000000000505050BABABAFFFFFFBA
          BABA0B0B0B000000343434BABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABA
          000000000000000000000000565656BABABA0606060000000000000000004545
          45BABABAFFFFFFFFFFFFFFFFFFBABABA000000000000000000000000565656BA
          BABA060606000000000000000000454545BABABAFFFFFFFFFFFFFFFFFFBABABA
          000000000000505050BABABAFFFFFFBABABA0B0B0B000000343434BABABAFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFBABABA292929BABABAFFFFFFFFFFFFFFFFFFBA
          BABA222222BABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          BABABAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBABABAFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        Transparent = False
        FlatAlwaysEdge = True
        OnClick = ebNextClipClick
      end
      object Bevel17: TBevel
        Left = 0
        Top = 18
        Width = 104
        Height = 1
        Shape = bsBottomLine
        Style = bsRaised
      end
      object Bevel19: TBevel
        Left = 103
        Top = 0
        Width = 1
        Height = 19
        Align = alRight
        Shape = bsRightLine
        Style = bsRaised
      end
    end
    object paClipList: TPanel
      Left = 0
      Top = 122
      Width = 103
      Height = 150
      Align = alClient
      BevelOuter = bvNone
      ParentColor = True
      TabOrder = 2
      object Panel4: TPanel
        Left = 0
        Top = 135
        Width = 103
        Height = 15
        Align = alBottom
        BevelOuter = bvNone
        ParentColor = True
        TabOrder = 0
        object ebInsertClip: TExtBtn
          Left = 1
          Top = 0
          Width = 51
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
          BtnColor = 10528425
          CloseButton = False
          Caption = 'Insert'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          Transparent = False
          FlatAlwaysEdge = True
          OnClick = ebInsertClipClick
        end
        object ebAppendClip: TExtBtn
          Left = 52
          Top = 0
          Width = 51
          Height = 15
          Align = alNone
          BevelShow = False
          HotTrack = True
          HotColor = 15790320
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
          OnClick = ebAppendClipClick
        end
      end
    end
    object Panel3: TPanel
      Left = 103
      Top = 122
      Width = 1
      Height = 150
      Align = alRight
      BevelOuter = bvNone
      Color = 5460819
      TabOrder = 3
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'ClipMaker'
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 1
    Top = 65520
  end
end
