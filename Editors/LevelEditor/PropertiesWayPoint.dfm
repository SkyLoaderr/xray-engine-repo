object frmPropertiesWayPoint: TfrmPropertiesWayPoint
  Left = 645
  Top = 274
  BorderStyle = bsDialog
  Caption = 'Way properties'
  ClientHeight = 175
  ClientWidth = 215
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
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object paBottom: TPanel
    Left = 0
    Top = 153
    Width = 215
    Height = 22
    Align = alBottom
    BevelOuter = bvNone
    Color = 10528425
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 87
      Top = 3
      Width = 61
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 148
      Top = 3
      Width = 61
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object pcProps: TElPageControl
    Left = 0
    Top = 0
    Width = 215
    Height = 153
    ActiveTabColor = 10528425
    BorderWidth = 0
    Color = 10528425
    DrawFocus = False
    Flat = True
    HotTrack = False
    InactiveTabColor = 10528425
    Multiline = False
    RaggedRight = False
    ScrollOpposite = False
    Style = etsAngledTabs
    TabHeight = 15
    TabIndex = 0
    TabPosition = etpBottom
    HotTrackFont.Charset = DEFAULT_CHARSET
    HotTrackFont.Color = clBlue
    HotTrackFont.Height = -11
    HotTrackFont.Name = 'MS Sans Serif'
    HotTrackFont.Style = []
    TabBkColor = 10528425
    ActivePage = tsBasic
    FlatTabBorderColor = clBtnShadow
    Align = alClient
    ParentColor = False
    TabOrder = 1
    UseXPThemes = False
    object tsBasic: TElTabSheet
      PageControl = pcProps
      ImageIndex = -1
      TabVisible = True
      Caption = 'Basic'
      Color = 10528425
      object paBasic: TPanel
        Left = 0
        Top = 0
        Width = 211
        Height = 134
        Align = alClient
        BevelOuter = bvLowered
        Color = 10528425
        TabOrder = 0
        object RxLabel1: TLabel
          Left = 6
          Top = 8
          Width = 31
          Height = 13
          Caption = 'Name:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object edName: TEdit
          Left = 64
          Top = 5
          Width = 142
          Height = 18
          AutoSize = False
          Color = 14671839
          TabOrder = 0
          OnChange = OnModified
        end
        object rgType: TRadioGroup
          Left = 1
          Top = 40
          Width = 209
          Height = 93
          Align = alBottom
          Caption = ' Type '
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ItemIndex = 0
          Items.Strings = (
            'Path')
          ParentFont = False
          TabOrder = 1
          OnClick = rgTypeClick
        end
      end
    end
    object tsWayPoint: TElTabSheet
      PageControl = pcProps
      ImageIndex = -1
      TabVisible = True
      Caption = 'Way Point'
      Color = 10528425
      Visible = False
      object paSurfaces: TPanel
        Left = 0
        Top = 0
        Width = 211
        Height = 134
        Align = alClient
        BevelOuter = bvLowered
        Color = 10528425
        TabOrder = 0
        object cb00000001: TMultiObjCheck
          Left = 1
          Top = 1
          Width = 97
          Height = 17
          Caption = '0x00000001'
          TabOrder = 0
          OnClick = OnModified
        end
        object cb00000002: TMultiObjCheck
          Left = 1
          Top = 17
          Width = 97
          Height = 17
          Caption = '0x00000002'
          TabOrder = 1
          OnClick = OnModified
        end
        object cb00000004: TMultiObjCheck
          Left = 1
          Top = 33
          Width = 97
          Height = 17
          Caption = '0x00000004'
          TabOrder = 2
          OnClick = OnModified
        end
        object cb00000008: TMultiObjCheck
          Left = 1
          Top = 49
          Width = 97
          Height = 17
          Caption = '0x00000008'
          TabOrder = 3
          OnClick = OnModified
        end
        object cb00000010: TMultiObjCheck
          Left = 1
          Top = 65
          Width = 97
          Height = 17
          Caption = '0x00000010'
          TabOrder = 4
          OnClick = OnModified
        end
        object cb00000020: TMultiObjCheck
          Left = 1
          Top = 81
          Width = 97
          Height = 17
          Caption = '0x00000020'
          TabOrder = 5
          OnClick = OnModified
        end
        object cb00000040: TMultiObjCheck
          Left = 1
          Top = 97
          Width = 97
          Height = 17
          Caption = '0x00000040'
          TabOrder = 6
          OnClick = OnModified
        end
        object cb00000080: TMultiObjCheck
          Left = 1
          Top = 113
          Width = 97
          Height = 17
          Caption = '0x00000080'
          TabOrder = 7
          OnClick = OnModified
        end
        object cb00000100: TMultiObjCheck
          Left = 105
          Top = 1
          Width = 97
          Height = 17
          Caption = '0x00000100'
          TabOrder = 8
          OnClick = OnModified
        end
        object cb00000200: TMultiObjCheck
          Left = 105
          Top = 17
          Width = 97
          Height = 17
          Caption = '0x00000200'
          TabOrder = 9
          OnClick = OnModified
        end
        object cb00000400: TMultiObjCheck
          Left = 105
          Top = 33
          Width = 97
          Height = 17
          Caption = '0x00000400'
          TabOrder = 10
          OnClick = OnModified
        end
        object cb00000800: TMultiObjCheck
          Left = 105
          Top = 49
          Width = 97
          Height = 17
          Caption = '0x00000800'
          TabOrder = 11
          OnClick = OnModified
        end
        object cb00001000: TMultiObjCheck
          Left = 105
          Top = 65
          Width = 97
          Height = 17
          Caption = '0x00001000'
          TabOrder = 12
          OnClick = OnModified
        end
        object cb00002000: TMultiObjCheck
          Left = 105
          Top = 81
          Width = 97
          Height = 17
          Caption = '0x00002000'
          TabOrder = 13
          OnClick = OnModified
        end
        object cb00004000: TMultiObjCheck
          Left = 105
          Top = 97
          Width = 97
          Height = 17
          Caption = '0x00004000'
          TabOrder = 14
          OnClick = OnModified
        end
        object cb00008000: TMultiObjCheck
          Left = 105
          Top = 113
          Width = 97
          Height = 17
          Caption = '0x00008000'
          TabOrder = 15
          OnClick = OnModified
        end
      end
    end
  end
end
