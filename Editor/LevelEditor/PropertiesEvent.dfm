object frmPropertiesEvent: TfrmPropertiesEvent
  Left = 675
  Top = 404
  BorderStyle = bsDialog
  Caption = 'Event properties'
  ClientHeight = 367
  ClientWidth = 492
  Color = 10528425
  Constraints.MaxWidth = 500
  Constraints.MinWidth = 500
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
  object Bevel1: TBevel
    Left = 0
    Top = 343
    Width = 492
    Height = 3
    Align = alBottom
    Shape = bsTopLine
  end
  object paBottom: TPanel
    Left = 0
    Top = 346
    Width = 492
    Height = 21
    Align = alBottom
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 327
      Top = 1
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      FlatAlwaysEdge = True
      OnClick = ebOkClick
    end
    object ebCancel: TExtBtn
      Left = 409
      Top = 1
      Width = 82
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      FlatAlwaysEdge = True
      OnClick = ebCancelClick
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 492
    Height = 19
    Align = alTop
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object ebAppendAction: TExtBtn
      Left = 28
      Top = 0
      Width = 464
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Append Action'
      FlatAlwaysEdge = True
      OnClick = ebAppendActionClick
    end
    object ebMultiClear: TExtBtn
      Left = 1
      Top = 0
      Width = 26
      Height = 18
      Align = alNone
      CloseButton = False
      Glyph.Data = {
        C2010000424DC20100000000000036000000280000000C0000000B0000000100
        1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF000000000000
        FFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF0000000000000000000000
        00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF00000000
        0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000
        000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000
        00000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
        0000000000000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF
        000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        FFFFFFFFFFFF}
      Spacing = 0
      FlatAlwaysEdge = True
      OnClick = ebMultiClearClick
    end
  end
  object sbActions: TScrollBox
    Left = 0
    Top = 19
    Width = 492
    Height = 324
    VertScrollBar.ButtonSize = 11
    VertScrollBar.Increment = 10
    VertScrollBar.Size = 11
    VertScrollBar.Style = ssHotTrack
    VertScrollBar.Tracking = True
    Align = alClient
    BorderStyle = bsNone
    TabOrder = 2
  end
end
