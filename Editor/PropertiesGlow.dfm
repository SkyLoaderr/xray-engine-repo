object frmPropertiesGlow: TfrmPropertiesGlow
  Left = 506
  Top = 302
  BorderStyle = bsDialog
  Caption = 'Glow properties'
  ClientHeight = 109
  ClientWidth = 289
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
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 289
    Height = 86
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object RxLabel4: TMxLabel
      Left = 1
      Top = 9
      Width = 57
      Height = 13
      Caption = 'Glow radius'
    end
    object ebSelectShader: TExtBtn
      Left = 0
      Top = 25
      Width = 40
      Height = 16
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Shader'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSelectShaderClick
    end
    object lbShader: TMxLabel
      Left = 59
      Top = 26
      Width = 142
      Height = 13
      AutoSize = False
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object lbTexture: TMxLabel
      Left = 59
      Top = 45
      Width = 142
      Height = 16
      AutoSize = False
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ebSelectTexture: TExtBtn
      Left = 0
      Top = 44
      Width = 40
      Height = 16
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Texture'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebSelectTextureClick
    end
    object seRange: TMultiObjSpinEdit
      Left = 60
      Top = 6
      Width = 143
      Height = 18
      LWSensitivity = 0.1
      ButtonKind = bkLightWave
      Increment = 0.01
      MaxValue = 1000
      MinValue = 0.01
      ValueType = vtFloat
      Value = 0.5
      AutoSize = False
      TabOrder = 0
      OnLWChange = seRangeLWChange
      OnExit = OnModified
      OnKeyDown = seRangeKeyDown
    end
    object paImage: TPanel
      Left = 208
      Top = 6
      Width = 80
      Height = 80
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 1
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 78
        Height = 78
        Align = alClient
        OnPaint = pbImagePaint
      end
    end
  end
  object paBottom: TPanel
    Left = 0
    Top = 86
    Width = 289
    Height = 23
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ebOk: TExtBtn
      Left = 124
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 206
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
    object ebApply: TExtBtn
      Left = 42
      Top = 3
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Apply'
      Enabled = False
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = ebApplyClick
    end
  end
  object fsStorage: TFormStorage
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0s
      end
      item
        Name = 'EmitterDirY'
        Value = 0s
      end
      item
        Name = 'EmitterDirZ'
        Value = 0s
      end>
    Left = 16
    Top = 65528
  end
end
