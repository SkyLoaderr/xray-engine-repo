object frmKeyBar: TfrmKeyBar
  Left = 253
  Top = 400
  Align = alTop
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 21
  ClientWidth = 747
  Color = 10528425
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 640
    Top = 0
    Width = 107
    Height = 21
    Align = alRight
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 0
    object Label1: TLabel
      Left = 5
      Top = 4
      Width = 25
      Height = 13
      Caption = 'LOD:'
    end
    object Bevel1: TBevel
      Left = 33
      Top = 3
      Width = 70
      Height = 16
    end
    object seLOD: TMultiObjSpinEdit
      Left = 35
      Top = 4
      Width = 67
      Height = 14
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 1
      ValueType = vtFloat
      Value = 1
      BorderStyle = bsNone
      Color = 10526880
      TabOrder = 0
      OnLWChange = seLODLWChange
      OnExit = seLODExit
      OnKeyPress = seLODKeyPress
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 640
    Height = 21
    Align = alClient
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 1
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 59
      Height = 21
      Align = alLeft
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 0
      object stStartTime: TStaticText
        Left = 3
        Top = 4
        Width = 55
        Height = 16
        AutoSize = False
        BorderStyle = sbsSunken
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = 12582911
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentColor = False
        ParentFont = False
        TabOrder = 0
      end
    end
    object Panel4: TPanel
      Left = 59
      Top = 0
      Width = 522
      Height = 21
      Align = alClient
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 1
      object Gradient1: TGradient
        Left = 0
        Top = 0
        Width = 522
        Height = 21
        BeginColor = 6118749
        EndColor = 11777977
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
        BorderWidth = 1
        BorderColor = 8226442
        Align = alClient
        object lbCurrentTime: TMxLabel
          Left = 1
          Top = 6
          Width = 520
          Height = 14
          Align = alBottom
          Alignment = taCenter
          AutoSize = False
          Caption = '...'
          Color = 10528425
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          ShadowColor = 13158600
          ShadowPos = spRightBottom
          Transparent = True
        end
      end
    end
    object Panel5: TPanel
      Left = 581
      Top = 0
      Width = 59
      Height = 21
      Align = alRight
      BevelOuter = bvNone
      Color = 10528425
      TabOrder = 2
      object stEndTime: TStaticText
        Left = 3
        Top = 4
        Width = 55
        Height = 16
        AutoSize = False
        BorderStyle = sbsSunken
        Color = 10526880
        Font.Charset = DEFAULT_CHARSET
        Font.Color = 12582911
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentColor = False
        ParentFont = False
        TabOrder = 0
      end
    end
  end
end
