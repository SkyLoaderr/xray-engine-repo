//---------------------------------------------------------------------------
#include "pch.h"
#pragma hdrstop

#include "ui_texturingtools.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "scene.h"
#include "selecttexturing.h"
#include "texturizer.h"
#include "SObject2.h"

TUI_ControlTexturingSelect  MSC_TexturingSelect;

void InitMSCTexturing()
{
    UI.m_Tools->AddControlCB(etTexturing,eaSelect,   &MSC_TexturingSelect);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlTexturingSelect::TUI_ControlTexturingSelect()
{
}

void __fastcall TUI_ControlTexturingSelect::OnEnter()
{
    Texturizer.Activate();
    Scene.SelectObjects( false, OBJCLASS_SOBJECT2 );
    m_TAction = taNone;
}

void __fastcall TUI_ControlTexturingSelect::OnExit()
{
    Texturizer.Deactivate();
}

bool __fastcall TUI_ControlTexturingSelect::Start(TShiftState Shift)
{
    if (Shift==ssRBOnly){
        SPickInfo pinf;
        Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf);
        if( pinf.obj ){
            fraSelectTexturing->SelectLayer(pinf.rp_inf.id);
        }
        return false;
    }
    if (fraSelectTexturing->ebPickTarget->Down){
    // picking object & mesh
        SPickInfo pinf;
        Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf);
        if( pinf.obj ){
            Texturizer.SetEditObject(pinf.obj);
            Texturizer.SetEditMesh(pinf.mesh);
        }else{
            Texturizer.SetEditObject(0);
        }
        return false;
    }else{
        if (fraSelectTexturing->ebLayerAddFace->Down){
            if (Texturizer.ValidLayer()){
                SPickInfo pinf;
                m_TAction = taAddFace;
                if (Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf))
                    Texturizer.GetLayer()->AddFace(pinf.rp_inf.id);
                return true;
            }else{
                MessageDlg("Select layer.", mtError, TMsgDlgButtons() << mbOK, 0);
            }
        }else{
            if (fraSelectTexturing->ebLayerRemoveFace->Down){
                if (Texturizer.ValidLayer()){
                    SPickInfo pinf;
                    m_TAction = taRemoveFace;
                    if (Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf))
                        Texturizer.GetLayer()->DelFace(pinf.rp_inf.id);
                    return true;
                }else{
                    MessageDlg("Select layer.", mtError, TMsgDlgButtons() << mbOK, 0);
                }
                return true;
            }
        }
    }
    return false;
}

void __fastcall TUI_ControlTexturingSelect::Move(TShiftState Shift)
{
    switch (m_TAction){
    case taAddFace:{
        SPickInfo pinf;
        if (Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf))
            Texturizer.GetLayer()->AddFace(pinf.rp_inf.id);
    }break;
    case taRemoveFace:{
        SPickInfo pinf;
        if (Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf))
            Texturizer.GetLayer()->DelFace(pinf.rp_inf.id);
    }break;
    }
}

bool __fastcall TUI_ControlTexturingSelect::End(TShiftState Shift)
{
    return true;
}

bool __fastcall TUI_ControlTexturingSelect::KeyPress(WORD Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE){
        if (fraSelectTexturing->ebPickTarget->Down)     fraSelectTexturing->ebPickTarget->Down      = false;
        if (fraSelectTexturing->ebLayerAddFace->Down)   fraSelectTexturing->ebLayerAddFace->Down    = false;
        if (fraSelectTexturing->ebLayerRemoveFace->Down)fraSelectTexturing->ebLayerRemoveFace->Down = false;
        if (fraSelectTexturing->ebNormalAlign->Down)    fraSelectTexturing->ebNormalAlign->Down     = false;
        if (fraSelectTexturing->ebGizmo->Down)          fraSelectTexturing->ebGizmo->Down           = false;
        UI.UpdateScene();
        return true;
    }
    return false;
}

