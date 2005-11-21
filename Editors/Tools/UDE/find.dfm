object FindStringDialog: TFindStringDialog
  Left = 333
  Top = 406
  BorderStyle = bsDialog
  Caption = 'Find'
  ClientHeight = 357
  ClientWidth = 433
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    433
    357)
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 433
    Height = 153
    Anchors = [akTop, akRight]
    Caption = 'Preconditions'
    TabOrder = 0
    object RxLabel1: TRxLabel
      Left = 220
      Top = 80
      Width = 28
      Height = 13
      Caption = 'Level'
    end
    object RxLabel2: TRxLabel
      Left = 8
      Top = 108
      Width = 39
      Height = 13
      Caption = 'Condlist'
    end
    object RxLabel3: TRxLabel
      Left = 8
      Top = 132
      Width = 61
      Height = 13
      Caption = 'Smart terrain'
    end
    object RxLabel4: TRxLabel
      Left = 8
      Top = 36
      Width = 44
      Height = 13
      Caption = 'Category'
    end
    object RxLabel5: TRxLabel
      Left = 8
      Top = 60
      Width = 53
      Height = 13
      Caption = 'Community'
    end
    object RxLabel6: TRxLabel
      Left = 8
      Top = 84
      Width = 77
      Height = 13
      Caption = 'NPC community'
    end
    object RxLabel7: TRxLabel
      Left = 220
      Top = 36
      Width = 41
      Height = 13
      Caption = 'Relation'
    end
    object RxLabel8: TRxLabel
      Left = 220
      Top = 60
      Width = 53
      Height = 13
      Caption = 'NPC Rank'
    end
    object CategoryF: TComboBox
      Left = 88
      Top = 28
      Width = 125
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      Items.Strings = (
        ''
        'intro'
        'trade_info'
        'action_info'
        'cool_info')
    end
    object NPCCommunityF: TComboBox
      Left = 88
      Top = 76
      Width = 125
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 2
      Items.Strings = (
        ''
        'stalker'
        'monolith'
        'military'
        'killer'
        'ecolog'
        'dolg'
        'freedom'
        'bandit'
        'zombied'
        'stranger'
        'trader'
        'arena_enemy')
    end
    object RelationF: TComboBox
      Left = 300
      Top = 28
      Width = 125
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 3
      Items.Strings = (
        ''
        'enemy'
        'neutral'
        'friend')
    end
    object NPCRankF: TRxSpinEdit
      Left = 300
      Top = 52
      Width = 125
      Height = 21
      Alignment = taRightJustify
      MaxValue = 100
      TabOrder = 4
    end
    object LevelF: TComboBox
      Left = 300
      Top = 76
      Width = 125
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 5
      Items.Strings = (
        ''
        'l01_escape'
        'l02_garbage'
        'l03_agroprom'
        'l03u_agr_underground'
        'l04_darkvalley'
        'l04u_labx18'
        'l05_bar'
        'l06_Rostok'
        'l07_military'
        'l10_radar'
        'l11_pripyat'
        'l12_stancia')
    end
    object CondlistF: TEdit
      Left = 88
      Top = 100
      Width = 337
      Height = 21
      TabOrder = 6
    end
    object SmartTerrainF: TEdit
      Left = 88
      Top = 124
      Width = 337
      Height = 21
      TabOrder = 7
    end
    object CommunityF: TComboBox
      Left = 88
      Top = 52
      Width = 125
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 1
      Items.Strings = (
        ''
        'actor'
        'actor_dolg'
        'actor_freedom')
    end
  end
  object Button1: TButton
    Left = 0
    Top = 332
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 356
    Top = 332
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object StringList: TListBox
    Left = 0
    Top = 156
    Width = 433
    Height = 173
    ItemHeight = 13
    TabOrder = 3
    OnDblClick = Button1Click
  end
end
