object frmOneEventAction: TfrmOneEventAction
  Left = 376
  Top = 220
  Align = alTop
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 50
  ClientWidth = 485
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
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 0
    Top = 0
    Width = 485
    Height = 50
    Align = alClient
  end
  object Bevel1: TBevel
    Left = 61
    Top = 4
    Width = 106
    Height = 20
    Style = bsRaised
  end
  object ebMultiRemove: TExtBtn
    Left = 1
    Top = 2
    Width = 23
    Height = 15
    Align = alNone
    CloseButton = False
    Glyph.Data = {
      56010000424D560100000000000036000000280000000A000000090000000100
      18000000000020010000120B0000120B00000000000000000000C8D0D4C8D0D4
      C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4000000
      000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
      000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4C8D0D4
      C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
      C8D0D4C8D0D4000000000000C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
      C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
      000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4000000
      000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
      C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000}
    Spacing = 0
    OnClick = ebMultiRemoveClick
  end
  object Label1: TLabel
    Left = 168
    Top = 6
    Width = 31
    Height = 13
    Caption = 'Count:'
  end
  object Label2: TLabel
    Left = 280
    Top = 6
    Width = 62
    Height = 13
    Caption = 'Target Class:'
  end
  object ebTypeEnter: TExtBtn
    Left = 62
    Top = 5
    Width = 52
    Height = 17
    Align = alNone
    BevelShow = False
    HotTrack = True
    HotColor = 15790320
    CloseButton = False
    GroupIndex = 1
    Down = True
    Caption = 'Enter'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Glyph.Data = {
      6E040000424D6E04000000000000360000002800000028000000090000000100
      18000000000038040000120B0000120B00000000000000000000FF00FF000000
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C40000000000000000000000000000000000
      00000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000999999999999999999999999999999999999C4C4C4FF00FF
      FF00FF000000999999999999999999999999999999999999C4C4C4FF00FFFF00
      FF0000009999999999999999999999995A699C999999C4C4C4FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FF000000
      999999999999999999999999999999999999C4C4C4FF00FFFF00FF0000009999
      999999995A699C999999999999999999C4C4C4FF00FFFF00FF0000009999995A
      699C999999999999999999999999C4C4C4FF00FFC4C4C4999999999999999999
      999999999999999999000000FF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C49999999999999999999999999999999999
      99000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000000000000000000000000000000000000000000000FF00FF
      FF00FF000000000000000000000000000000000000000000000000FF00FFFF00
      FF000000000000000000000000000000000000000000000000FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC4C4C4C4C4C4C4C4C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF}
    Margin = 1
    NumGlyphs = 4
    ParentFont = False
    OnClick = OnModified
  end
  object ebTypeLeave: TExtBtn
    Left = 114
    Top = 5
    Width = 52
    Height = 17
    Align = alNone
    BevelShow = False
    HotTrack = True
    HotColor = 15790320
    CloseButton = False
    GroupIndex = 1
    Caption = 'Leave'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Glyph.Data = {
      6E040000424D6E04000000000000360000002800000028000000090000000100
      18000000000038040000120B0000120B00000000000000000000FF00FF000000
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF000000C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C40000000000000000000000000000000000
      00000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000999999999999999999999999999999999999C4C4C4FF00FF
      FF00FF000000999999999999999999999999999999999999C4C4C4FF00FFFF00
      FF0000009999999999999999999999995A699C999999C4C4C4FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FF000000
      999999999999999999999999999999999999C4C4C4FF00FFFF00FF0000009999
      999999995A699C999999999999999999C4C4C4FF00FFFF00FF0000009999995A
      699C999999999999999999999999C4C4C4FF00FFC4C4C4999999999999999999
      999999999999999999000000FF00FFFF00FFFF00FF0000009999999999999999
      99999999999999999999C4C4C4FF00FFFF00FF00000099999999999999999999
      9999999999999999C4C4C4FF00FFFF00FF000000999999999999999999999999
      999999999999C4C4C4FF00FFC4C4C49999999999999999999999999999999999
      99000000FF00FFFF00FFFF00FF00000099999999999999999999999999999999
      9999C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999
      C4C4C4FF00FFFF00FF000000999999999999999999999999999999999999C4C4
      C4FF00FFC4C4C4999999999999999999999999999999999999000000FF00FFFF
      00FFFF00FF000000000000000000000000000000000000000000000000FF00FF
      FF00FF000000000000000000000000000000000000000000000000FF00FFFF00
      FF000000000000000000000000000000000000000000000000FF00FFC4C4C499
      9999999999999999999999999999999999000000FF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC4C4C4C4C4C4C4C4C4C4C4C4
      C4C4C4C4C4C4C4C4C4C4C4C4FF00FFFF00FF}
    Margin = 1
    NumGlyphs = 4
    ParentFont = False
    OnClick = OnModified
  end
  object Label3: TLabel
    Left = 28
    Top = 6
    Width = 27
    Height = 13
    Caption = 'Type:'
  end
  object Label4: TLabel
    Left = 28
    Top = 27
    Width = 31
    Height = 13
    Caption = 'Event:'
  end
  object ebCLSID: TExtBtn
    Left = 345
    Top = 4
    Width = 135
    Height = 18
    Align = alNone
    BevelShow = False
    BtnColor = 10528425
    CloseButton = False
    Caption = '<none>'
    Margin = 1
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = ebCLSIDClick
  end
  object seCount: TMultiObjSpinEdit
    Left = 200
    Top = 2
    Width = 73
    Height = 21
    LWSensitivity = 0.01
    ButtonKind = bkLightWave
    MaxValue = 65535
    MinValue = -1
    Color = 8422020
    TabOrder = 0
    OnChange = OnModified
  end
  object edEvent: TEdit
    Left = 61
    Top = 25
    Width = 420
    Height = 21
    Color = 8422020
    TabOrder = 1
    OnChange = OnModified
  end
end
