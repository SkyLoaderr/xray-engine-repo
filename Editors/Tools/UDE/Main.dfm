object MainForm: TMainForm
  Left = 300
  Top = 373
  BorderStyle = bsSingle
  Caption = 'Universal dialog editor'
  ClientHeight = 451
  ClientWidth = 762
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnKeyDown = FormKeyDown
  DesignSize = (
    762
    451)
  PixelsPerInch = 96
  TextHeight = 13
  object AddStringButton: TSpeedButton
    Left = 268
    Top = 168
    Width = 23
    Height = 22
    Caption = 'Add'
    OnClick = AddStringButtonClick
  end
  object DeleteStringButton: TSpeedButton
    Left = 292
    Top = 168
    Width = 23
    Height = 22
    Caption = 'Del'
    OnClick = DeleteStringButtonClick
  end
  object StringList: TListBox
    Left = 0
    Top = 4
    Width = 317
    Height = 161
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 0
    OnClick = StringListClick
  end
  object Text: TMemo
    Left = 0
    Top = 192
    Width = 761
    Height = 260
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    OnChange = CategoryChange
  end
  object AddString: TEdit
    Left = 0
    Top = 168
    Width = 265
    Height = 21
    TabOrder = 1
  end
  object Page: TTabbedNotebook
    Left = 320
    Top = 4
    Width = 441
    Height = 185
    TabFont.Charset = DEFAULT_CHARSET
    TabFont.Color = clBtnText
    TabFont.Height = -11
    TabFont.Name = 'MS Sans Serif'
    TabFont.Style = []
    TabOrder = 3
    OnChange = PageChange
    object TTabPage
      Left = 4
      Top = 24
      Caption = 'Main'
      DesignSize = (
        433
        157)
      object GroupBox1: TGroupBox
        Left = 0
        Top = 4
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
        object Category: TComboBox
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
        object NPCCommunity: TComboBox
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
        object Relation: TComboBox
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
        object NPCRank: TRxSpinEdit
          Left = 300
          Top = 52
          Width = 125
          Height = 21
          Alignment = taRightJustify
          MaxValue = 100
          TabOrder = 4
        end
        object Level: TComboBox
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
        object Condlist: TEdit
          Left = 88
          Top = 100
          Width = 337
          Height = 21
          TabOrder = 6
        end
        object SmartTerrain: TEdit
          Left = 88
          Top = 124
          Width = 337
          Height = 21
          TabOrder = 7
        end
        object Community: TComboBox
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
    end
    object TTabPage
      Left = 4
      Top = 24
      Caption = 'Find'
      DesignSize = (
        433
        157)
      object GroupBox2: TGroupBox
        Left = 0
        Top = 4
        Width = 433
        Height = 153
        Anchors = [akTop, akRight]
        Caption = 'Preconditions'
        TabOrder = 0
        object RxLabel9: TRxLabel
          Left = 220
          Top = 80
          Width = 28
          Height = 13
          Caption = 'Level'
        end
        object RxLabel10: TRxLabel
          Left = 8
          Top = 108
          Width = 39
          Height = 13
          Caption = 'Condlist'
        end
        object RxLabel11: TRxLabel
          Left = 8
          Top = 132
          Width = 61
          Height = 13
          Caption = 'Smart terrain'
        end
        object RxLabel12: TRxLabel
          Left = 8
          Top = 36
          Width = 44
          Height = 13
          Caption = 'Category'
        end
        object RxLabel13: TRxLabel
          Left = 8
          Top = 60
          Width = 53
          Height = 13
          Caption = 'Community'
        end
        object RxLabel14: TRxLabel
          Left = 8
          Top = 84
          Width = 77
          Height = 13
          Caption = 'NPC community'
        end
        object RxLabel15: TRxLabel
          Left = 220
          Top = 36
          Width = 41
          Height = 13
          Caption = 'Relation'
        end
        object RxLabel16: TRxLabel
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
          OnChange = CategoryFChange
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
          OnChange = CategoryFChange
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
          OnChange = CategoryFChange
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
          OnChange = CategoryFChange
        end
        object LevelF: TComboBox
          Left = 300
          Top = 76
          Width = 125
          Height = 21
          Style = csDropDownList
          ItemHeight = 13
          TabOrder = 5
          OnChange = CategoryFChange
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
          OnChange = CategoryFChange
        end
        object SmartTerrainF: TEdit
          Left = 88
          Top = 124
          Width = 337
          Height = 21
          TabOrder = 7
          OnChange = CategoryFChange
        end
        object CommunityF: TComboBox
          Left = 88
          Top = 52
          Width = 125
          Height = 21
          Style = csDropDownList
          ItemHeight = 13
          TabOrder = 1
          OnChange = CategoryFChange
          Items.Strings = (
            ''
            'actor'
            'actor_dolg'
            'actor_freedom')
        end
      end
    end
  end
end
