object frmPropertiesPS: TfrmPropertiesPS
  Left = 400
  Top = 431
  BorderStyle = bsDialog
  Caption = 'PS properties'
  ClientHeight = 210
  ClientWidth = 204
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
    Top = 188
    Width = 204
    Height = 22
    Align = alBottom
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object ebOk: TExtBtn
      Left = 75
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
      Left = 136
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
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 204
    Height = 187
    Align = alTop
    Caption = ' Emitter '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    inline fraEmitter: TfraEmitter
      Left = 2
      Top = 15
      Width = 200
      Height = 170
      Align = alClient
      Color = 10528425
      ParentColor = False
      TabOrder = 0
      inherited ebBirthFunc: TExtBtn
        OnClick = OnModified
        OnMouseUp = ebBirthFuncMouseUp
      end
      inherited seBirthRate: TMultiObjSpinEdit
        OnLWChange = seEmitterLWChange
        OnExit = OnModified
        OnKeyDown = seEmitterKeyDown
      end
      inherited seParticleLimit: TMultiObjSpinEdit
        OnLWChange = seEmitterLWChange
        OnExit = OnModified
        OnKeyDown = seEmitterKeyDown
      end
      inherited cbBurst: TMultiObjCheck
        OnClick = OnModified
      end
      inherited cbPlayOnce: TMultiObjCheck
        OnClick = OnModified
      end
      inherited pcEmitterType: TElPageControl
        OnChange = OnModified
        inherited tsCone: TElTabSheet
          inherited Panel1: TPanel
            Width = 193
            inherited seConeAngle: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
            inherited seConeDirH: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
            inherited seConeDirP: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
            inherited seConeDirB: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
          end
        end
        inherited tsSphere: TElTabSheet
          inherited Panel5: TPanel
            Width = 193
            inherited seSphereRadius: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
          end
        end
        inherited tsBox: TElTabSheet
          inherited Panel4: TPanel
            inherited seBoxSizeX: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
            inherited seBoxSizeY: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
            inherited seBoxSizeZ: TMultiObjSpinEdit
              OnLWChange = seEmitterLWChange
              OnExit = OnModified
              OnKeyDown = seEmitterKeyDown
            end
          end
        end
      end
    end
  end
end
