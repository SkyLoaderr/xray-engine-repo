object ClipMaker: TClipMaker
  Left = 396
  Top = 411
  Width = 500
  Height = 256
  HorzScrollBar.Style = ssFlat
  HorzScrollBar.ThumbSize = 12
  BiDiMode = bdRightToLeft
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSizeToolWin
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
  PixelsPerInch = 96
  TextHeight = 13
  object paB: TPanel
    Left = 104
    Top = 0
    Width = 388
    Height = 229
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
      OnResize = paFrameResize
      object sbBase: TScrollBox
        Left = 0
        Top = 0
        Width = 388
        Height = 121
        HorzScrollBar.Style = ssFlat
        VertScrollBar.Visible = False
        Align = alClient
        BorderStyle = bsNone
        TabOrder = 0
        object paFrame: TPanel
          Left = 0
          Top = 0
          Width = 241
          Height = 104
          BevelOuter = bvNone
          ParentColor = True
          TabOrder = 0
          OnResize = paFrameResize
          object Bevel6: TBevel
            Left = 0
            Top = 34
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object Bevel7: TBevel
            Left = 0
            Top = 82
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object Bevel8: TBevel
            Left = 0
            Top = 50
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
            Top = 66
            Width = 241
            Height = 1
            Align = alTop
            Shape = bsBottomLine
            Style = bsRaised
          end
          object paClips: TPanel
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
          object gtClip: TGradient
            Left = 0
            Top = 83
            Width = 241
            Height = 21
            BeginColor = 5460819
            EndColor = 6908265
            FillDirection = fdUpToBottom
            NumberOfColors = 32
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            Caption = ' '
            TextTop = 5
            TextLeft = 12
            Border = True
            BorderWidth = 0
            BorderColor = clGray
            Color = 5460819
            Align = alTop
            OnPaint = gtClipPaint
          end
          object gtBP0: TGradient
            Left = 0
            Top = 19
            Width = 241
            Height = 15
            BeginColor = 5460819
            EndColor = 6908265
            NumberOfColors = 32
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            Caption = ' '
            TextTop = 5
            TextLeft = 12
            Border = True
            BorderWidth = 0
            BorderColor = clGray
            Color = 5460819
            Align = alTop
            OnPaint = BPOnPaint
          end
          object gtBP3: TGradient
            Tag = 3
            Left = 0
            Top = 67
            Width = 241
            Height = 15
            BeginColor = 5460819
            EndColor = 6908265
            NumberOfColors = 32
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            Caption = ' '
            TextTop = 5
            TextLeft = 12
            Border = True
            BorderWidth = 0
            BorderColor = clGray
            Color = 5460819
            Align = alTop
            OnPaint = BPOnPaint
          end
          object gtBP2: TGradient
            Tag = 2
            Left = 0
            Top = 51
            Width = 241
            Height = 15
            BeginColor = 5460819
            EndColor = 6908265
            NumberOfColors = 32
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            Caption = ' '
            TextTop = 5
            TextLeft = 12
            Border = True
            BorderWidth = 0
            BorderColor = clGray
            Color = 5460819
            Align = alTop
            OnPaint = BPOnPaint
          end
          object gtBP1: TGradient
            Tag = 1
            Left = 0
            Top = 35
            Width = 241
            Height = 15
            BeginColor = 5460819
            EndColor = 6908265
            NumberOfColors = 32
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            Caption = ' '
            TextTop = 5
            TextLeft = 12
            Border = True
            BorderWidth = 0
            BorderColor = clGray
            Color = 5460819
            Align = alTop
            OnPaint = BPOnPaint
          end
        end
      end
    end
    object paClipProps: TPanel
      Left = 0
      Top = 121
      Width = 388
      Height = 108
      Align = alClient
      BevelOuter = bvNone
      Color = 6908265
      TabOrder = 1
      object Bevel20: TBevel
        Left = 387
        Top = 1
        Width = 1
        Height = 107
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
    Height = 229
    Align = alLeft
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object ebInsertClip: TExtBtn
      Left = 1
      Top = 150
      Width = 101
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
      Left = 1
      Top = 166
      Width = 101
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
    object Bevel14: TBevel
      Left = 103
      Top = 106
      Width = 1
      Height = 123
      Align = alRight
      Shape = bsRightLine
      Style = bsRaised
    end
    object Bevel16: TBevel
      Left = 0
      Top = 105
      Width = 104
      Height = 1
      Align = alTop
      Shape = bsBottomLine
      Style = bsRaised
    end
    object MxLabel5: TMxLabel
      Left = 0
      Top = 106
      Width = 51
      Height = 15
      Caption = 'Length:'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = 12698049
      Font.Height = -12
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      ShadowColor = clBlack
      ShadowPos = spRightBottom
    end
    object Bevel17: TBevel
      Left = 48
      Top = 107
      Width = 47
      Height = 16
    end
    object MxLabel7: TMxLabel
      Left = 95
      Top = 108
      Width = 7
      Height = 13
      Caption = 's'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWhite
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ShadowColor = clBlack
      ShadowPos = spRightBottom
      Transparent = True
    end
    object MxLabel8: TMxLabel
      Left = 0
      Top = 124
      Width = 37
      Height = 15
      Caption = 'Zoom:'
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = 12698049
      Font.Height = -12
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      ShadowColor = clBlack
      ShadowPos = spRightBottom
    end
    object Bevel18: TBevel
      Left = 48
      Top = 125
      Width = 47
      Height = 16
    end
    object seLength: TMultiObjSpinEdit
      Left = 50
      Top = 109
      Width = 44
      Height = 13
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 1
      MaxValue = 999
      MinValue = 1
      ValueType = vtFloat
      Value = 60
      AutoSize = False
      BorderStyle = bsNone
      Color = 10526880
      TabOrder = 1
      OnChange = seLengthChange
    end
    object Panel1: TPanel
      Left = 0
      Top = 0
      Width = 104
      Height = 105
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
        Height = 105
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
        Top = 86
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
        Height = 105
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
    end
    object seScale: TMultiObjSpinEdit
      Left = 50
      Top = 127
      Width = 44
      Height = 13
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      MaxValue = 100
      MinValue = 2
      Value = 4
      AutoSize = False
      BorderStyle = bsNone
      Color = 10526880
      TabOrder = 2
      OnChange = seScaleChange
    end
  end
  object fsStorage: TFormStorage
    IniSection = 'ClipMaker'
    Options = []
    OnSavePlacement = fsStorageSavePlacement
    OnRestorePlacement = fsStorageRestorePlacement
    StoredValues = <>
    Left = 1
    Top = 65520
  end
  object pmClip: TMxPopupMenu
    Alignment = paCenter
    AutoHotkeys = maManual
    AutoPopup = False
    MarginStartColor = 10921638
    MarginEndColor = 2763306
    BKColor = 10528425
    SelColor = clBlack
    SelFontColor = 10526880
    SepHColor = 1644825
    SepLColor = 13158600
    LeftMargin = 10
    Style = msOwnerDraw
    Left = 30
    Top = 65520
    object MenuItem1: TMenuItem
      Caption = '-'
    end
    object MenuItem2: TMenuItem
      Caption = 'Remove Current'
      OnClick = MenuItem2Click
    end
    object RemoveAll1: TMenuItem
      Caption = 'Remove All'
      OnClick = RemoveAll1Click
    end
    object MenuItem3: TMenuItem
      Caption = '-'
    end
  end
end
