object frmAddNewObject: TfrmAddNewObject
  Left = 422
  Top = 270
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Object Description'
  ClientHeight = 214
  ClientWidth = 276
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object sbNewObject: TExtBtn
    Left = 193
    Top = 31
    Width = 81
    Height = 22
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Add Mesh'
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = sbNewObjectClick
  end
  object ebAdd: TExtBtn
    Left = 1
    Top = 191
    Width = 128
    Height = 22
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Add'
    Enabled = False
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = ebAddClick
  end
  object ExtBtn2: TExtBtn
    Left = 144
    Top = 191
    Width = 130
    Height = 22
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Cancel'
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = ExtBtn2Click
  end
  object RxLabel1: TRxLabel
    Left = 1
    Top = 6
    Width = 64
    Height = 13
    Caption = 'Object Name'
  end
  object ExtBtn1: TExtBtn
    Left = 193
    Top = 57
    Width = 81
    Height = 22
    Align = alNone
    BevelShow = False
    CloseButton = False
    Caption = 'Delete Mesh'
    Transparent = False
    FlatAlwaysEdge = True
    OnClick = ExtBtn1Click
  end
  object lbMeshes: TListBox
    Left = 1
    Top = 31
    Width = 184
    Height = 154
    ItemHeight = 13
    MultiSelect = True
    Sorted = True
    TabOrder = 0
  end
  object edName: TEdit
    Left = 73
    Top = 3
    Width = 201
    Height = 21
    TabOrder = 1
    OnChange = edNameChange
  end
  object odMesh: TOpenDialog
    Filter = 'Mesh file|*.*'
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Title = 'Open Mesh file'
    Left = 2
    Top = 35
  end
end
