object Form1: TForm1
  Left = 255
  Top = 287
  BorderStyle = bsSingle
  Caption = 'Translator V2.0'
  ClientHeight = 513
  ClientWidth = 843
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnKeyUp = FormKeyUp
  OnShow = FormShow
  DesignSize = (
    843
    513)
  PixelsPerInch = 96
  TextHeight = 13
  object RxLabel1: TRxLabel
    Left = 0
    Top = 32
    Width = 47
    Height = 13
    Caption = 'RxLabel1'
  end
  object RxLabel2: TRxLabel
    Left = 0
    Top = 264
    Width = 47
    Height = 13
    Caption = 'RxLabel2'
  end
  object SrcText: TRichEdit
    Left = 0
    Top = 48
    Width = 589
    Height = 213
    Anchors = [akLeft, akTop, akRight]
    BevelEdges = [beLeft, beTop, beRight]
    Color = clBtnFace
    MaxLength = 4000
    ReadOnly = True
    TabOrder = 0
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 843
    Height = 29
    ButtonHeight = 21
    ButtonWidth = 73
    Caption = 'ToolBar1'
    EdgeBorders = [ebTop, ebBottom]
    ShowCaptions = True
    TabOrder = 1
    object ToolButton5: TToolButton
      Left = 0
      Top = 2
      Hint = 'Shows the strings added since the last base save'
      Caption = 'Added strings'
      ImageIndex = 4
      ParentShowHint = False
      ShowHint = True
      OnClick = ToolButton5Click
    end
    object ToolButton1: TToolButton
      Left = 73
      Top = 2
      Width = 8
      Caption = 'ToolButton1'
      ImageIndex = 5
      Style = tbsSeparator
    end
    object DiffButton: TToolButton
      Left = 81
      Top = 2
      Hint = 
        'Shows the strings that contain differences between source and ta' +
        'rget languages'
      Caption = 'Differences'
      ImageIndex = 0
      ParentShowHint = False
      ShowHint = True
      Style = tbsCheck
      OnClick = DiffButtonClick
    end
    object ToolButton2: TToolButton
      Left = 154
      Top = 2
      Width = 8
      Caption = 'ToolButton2'
      ImageIndex = 1
      Style = tbsSeparator
    end
    object NotransButton: TToolButton
      Left = 162
      Top = 2
      Hint = 'Shows strings which hasn'#8217't been translated'
      Caption = 'Not translated'
      ImageIndex = 6
      ParentShowHint = False
      ShowHint = True
      Style = tbsCheck
      OnClick = NotransButtonClick
    end
    object ToolButton12: TToolButton
      Left = 235
      Top = 2
      Width = 8
      Caption = 'ToolButton12'
      ImageIndex = 6
      Style = tbsSeparator
    end
    object ToolButton6: TToolButton
      Left = 243
      Top = 2
      Hint = 'Changes source and target languages'
      Caption = 'Change lang'
      ImageIndex = 2
      ParentShowHint = False
      ShowHint = True
      OnClick = ToolButton6Click
    end
    object ToolButton7: TToolButton
      Left = 316
      Top = 2
      Width = 8
      Caption = 'ToolButton7'
      ImageIndex = 3
      Style = tbsSeparator
    end
    object ToolButton8: TToolButton
      Left = 324
      Top = 2
      Hint = 'Adds new language'
      Caption = 'Add lang'
      ImageIndex = 3
      ParentShowHint = False
      ShowHint = True
      OnClick = ToolButton8Click
    end
    object ToolButton9: TToolButton
      Left = 397
      Top = 2
      Width = 8
      Caption = 'ToolButton9'
      ImageIndex = 4
      Style = tbsSeparator
    end
    object CheckButton: TToolButton
      Left = 405
      Top = 2
      Hint = 'Checks for redundant strings in the target language'
      Caption = 'Check'
      ImageIndex = 5
      ParentShowHint = False
      ShowHint = True
      OnClick = CheckButtonClick
    end
    object ToolButton11: TToolButton
      Left = 478
      Top = 2
      Width = 8
      Caption = 'ToolButton11'
      ImageIndex = 5
      Style = tbsSeparator
    end
    object ToolButton10: TToolButton
      Left = 486
      Top = 2
      Hint = 
        'Shows the statistics on the bases for source and target language' +
        's'
      Caption = 'Compare'
      ImageIndex = 4
      ParentShowHint = False
      ShowHint = True
      OnClick = ToolButton10Click
    end
    object ToolButton13: TToolButton
      Left = 559
      Top = 2
      Width = 8
      Caption = 'ToolButton13'
      ImageIndex = 5
      Style = tbsSeparator
    end
    object SaveAllButton: TToolButton
      Left = 567
      Top = 2
      Hint = 'Save all'
      Caption = 'Save all'
      ImageIndex = 6
      ParentShowHint = False
      ShowHint = True
      OnClick = SaveAllButtonClick
    end
    object ToolButton15: TToolButton
      Left = 640
      Top = 2
      Width = 8
      Caption = 'ToolButton15'
      ImageIndex = 7
      Style = tbsSeparator
    end
    object DumpButton: TToolButton
      Left = 648
      Top = 2
      Caption = 'Dump'
      ImageIndex = 5
      OnClick = DumpButtonClick
    end
  end
  object DestText: TRichEdit
    Left = 0
    Top = 280
    Width = 589
    Height = 213
    Anchors = [akLeft, akTop, akRight]
    BevelEdges = [beLeft, beRight, beBottom]
    MaxLength = 4000
    TabOrder = 2
    OnChange = DestTextChange
    OnEnter = DestTextEnter
    OnExit = DestTextExit
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 494
    Width = 843
    Height = 19
    Panels = <>
    SimplePanel = False
  end
  object FilesList: TListBox
    Left = 592
    Top = 48
    Width = 249
    Height = 213
    Style = lbOwnerDrawFixed
    Anchors = [akTop, akRight]
    BevelEdges = [beTop, beRight]
    ItemHeight = 13
    PopupMenu = LockMenu
    TabOrder = 4
    OnClick = FilesListClick
    OnDrawItem = FilesListDrawItem
  end
  object StringsList: TListBox
    Left = 592
    Top = 280
    Width = 249
    Height = 213
    Anchors = [akTop, akRight]
    BevelEdges = [beRight, beBottom]
    ItemHeight = 13
    TabOrder = 5
    OnClick = StringsListClick
  end
  object LockMenu: TPopupMenu
    Left = 600
    Top = 56
    object LockFileMenuItem: TMenuItem
      Caption = 'Lock file'
      OnClick = LockFileMenuItemClick
    end
    object UnlockFileMenuItem: TMenuItem
      Caption = 'Unlock file'
      OnClick = UnlockFileMenuItemClick
    end
  end
  object Timer: TTimer
    Interval = 5000
    OnTimer = TimerTimer
    Left = 4
    Top = 52
  end
end
