object SplashForm: TSplashForm
  Left = 422
  Top = 369
  BorderStyle = bsNone
  Caption = 'SplashForm'
  ClientHeight = 285
  ClientWidth = 499
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 499
    Height = 285
    Align = alClient
    AutoSize = True
  end
  object lbStatus: TMxLabel
    Left = 1
    Top = 270
    Width = 497
    Height = 12
    Alignment = taCenter
    AutoSize = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWhite
    Font.Height = -11
    Font.Name = 'Lucida Console'
    Font.Style = []
    ParentFont = False
    ShadowColor = clBlack
    ShadowPos = spRightBottom
    Transparent = True
  end
end
