object frmOneEnvironment: TfrmOneEnvironment
  Left = 365
  Top = 196
  Align = alTop
  BorderIcons = []
  BorderStyle = bsNone
  Caption = 'Environment'
  ClientHeight = 58
  ClientWidth = 337
  Color = clBtnFace
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
  object gbEnv: TGroupBox
    Left = 0
    Top = 0
    Width = 337
    Height = 58
    Align = alTop
    Caption = ' Environment #0 '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object RxLabel29: TLabel
      Left = 2
      Top = 19
      Width = 86
      Height = 13
      Caption = 'View distance (m):'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object RxLabel30: TLabel
      Left = 2
      Top = 38
      Width = 43
      Height = 13
      Caption = 'Fogness:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object RxLabel31: TLabel
      Left = 154
      Top = 19
      Width = 41
      Height = 13
      Caption = 'Ambient:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object mcAmbColor: TMultiObjColor
      Left = 199
      Top = 19
      Width = 48
      Height = 16
      Brush.Color = clBlack
      OnMouseDown = mcAmbColorMouseDown
    end
    object RxLabel32: TLabel
      Left = 154
      Top = 38
      Width = 21
      Height = 13
      Caption = 'Fog:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object mcFogColor: TMultiObjColor
      Left = 199
      Top = 37
      Width = 48
      Height = 16
      Brush.Color = 5592405
      OnMouseDown = mcAmbColorMouseDown
    end
    object RxLabel1: TLabel
      Left = 256
      Top = 20
      Width = 21
      Height = 13
      Caption = 'Sky:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object mcSkyColor: TMultiObjColor
      Left = 281
      Top = 19
      Width = 48
      Height = 16
      Brush.Color = 15066597
      OnMouseDown = mcAmbColorMouseDown
    end
    object seViewDistance: TMultiObjSpinEdit
      Left = 90
      Top = 17
      Width = 58
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Decimal = 1
      Increment = 0.1
      MaxValue = 10000
      MinValue = 0.1
      ValueType = vtFloat
      Value = 500
      AutoSize = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnLWChange = seFognessLWChange
      OnExit = UpdateClick
      OnKeyDown = seFognessKeyDown
    end
    object seFogness: TMultiObjSpinEdit
      Left = 90
      Top = 36
      Width = 58
      Height = 18
      LWSensitivity = 0.01
      ButtonKind = bkLightWave
      Decimal = 1
      Increment = 0.1
      MaxValue = 100
      ValueType = vtFloat
      Value = 0.9
      AutoSize = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnLWChange = seFognessLWChange
      OnExit = UpdateClick
      OnKeyDown = seFognessKeyDown
    end
    object seSkyFogness: TMultiObjSpinEdit
      Left = 281
      Top = 36
      Width = 50
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 1
      ValueType = vtFloat
      Value = 0.9
      AutoSize = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnLWChange = seFognessLWChange
      OnExit = UpdateClick
      OnKeyDown = seFognessKeyDown
    end
  end
end
