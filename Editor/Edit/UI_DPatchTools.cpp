#include "stdafx.h"
#pragma hdrstop

#include "ui_DPatchtools.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "DPatch.h"
#include "frameDPatch.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "topbar.h"
#include "Cursor3D.h"
#include "texture.h"
//---------------------------------------------------------------------------
TUI_DPatchTools::TUI_DPatchTools():TUI_CustomTools(OBJCLASS_DPATCH){
    AddControlCB(new TUI_ControlDPatchSelect (estSelf,eaSelect,this));
    AddControlCB(new TUI_ControlDPatchAdd    (estSelf,eaAdd,this));
    srand((unsigned)time(NULL));
}

void TUI_DPatchTools::OnActivate  (){
    pFrame = new TfraDPatch(0);
    ((TfraDPatch*)pFrame)->fsStorage->RestoreFormPlacement();
	TUI_CustomTools::OnActivate();
}
void TUI_DPatchTools::OnDeactivate(){
    ((TfraDPatch*)pFrame)->fsStorage->SaveFormPlacement();
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlDPatchSelect::TUI_ControlDPatchSelect(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}

bool __fastcall TUI_ControlDPatchSelect::Start(TShiftState Shift)
{
    if (!Shift.Contains(ssCtrl)) Scene->m_DetailPatches->Select( false );

    float d0 = 0;
    bool bPick = Scene->m_DetailPatches->RayPickSelect(d0, UI->m_CurrentRStart,UI->m_CurrentRNorm);
    bBoxSelection    = (bPick && Shift.Contains(ssCtrl)) || !bPick;

    if( bBoxSelection ){
        UI->EnableSelectionRect( true );
        UI->UpdateSelectionRect(UI->m_StartCp,UI->m_CurrentCp);
        return true;
    }else
		Scene->UndoSave();
    return false;
}

void __fastcall TUI_ControlDPatchSelect::Move(TShiftState _Shift)
{
    if (bBoxSelection) UI->UpdateSelectionRect(UI->m_StartCp,UI->m_CurrentCp);
}

bool __fastcall TUI_ControlDPatchSelect::End(TShiftState _Shift)
{
    if (bBoxSelection){
        UI->EnableSelectionRect( false );
        bBoxSelection = false;
        Scene->m_DetailPatches->FrustumSelect(true) ;
        Scene->UndoSave();
    }
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlDPatchAdd::TUI_ControlDPatchAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent)
{
}
void TUI_ControlDPatchAdd::OnEnter(){
    UI->m_Cursor->Style(csLasso);
    UI->m_Cursor->Visible(true);
    fraDPatch = (TfraDPatch*)parent_tool->pFrame; VERIFY(fraDPatch);
}
void TUI_ControlDPatchAdd::OnExit(){
    UI->m_Cursor->Visible(false);
	fraDPatch = 0;
}
void TUI_ControlDPatchAdd::GenerateDPatch(){
    Fvector P,N;
    if (UI->m_Cursor->PrepareBrush()){
        for ( DWORD i = 0; i < fraDPatch->seBrushDPatchAmount->Value; i++ ){
            UI->m_Cursor->GetRandomBrushPos(P,N);
            // set texture
//            int r;
//            do{ r=(rnd()<0.5f)?0:1; }while(!fraDPatch->m_Texture[r]);

            float R = Random.randF(fraDPatch->seDPatchMin->Value,fraDPatch->seDPatchMax->Value);
            N.mul(R);
            P.add(N);
            AStringVec TN;
            TN.push_back(fraDPatch->sTexture);
            st_DPSurface* surf = Scene->m_DetailPatches->CreateSurface(TN,fraDPatch->sShader.c_str());
            Scene->m_DetailPatches->AddDPatch(surf,R,P,N);
        }
		Scene->UndoSave();
    }
}

bool __fastcall TUI_ControlDPatchAdd::Start(TShiftState Shift)
{
    SRayPickInfo pinf;
    bool bPickObject, bPickGround;
    if (fraDPatch->sTexture=="..."){
        Log->DlgMsg(mtError, "Link texture before add DPatch!");
        return false;
    }
    if (fraDPatch->sShader=="..."){
        Log->DlgMsg(mtError, "Set shader before add DPatch!");
        return false;
    }
    bPickObject = !!Scene->RayPick( UI->m_CurrentRStart,UI->m_CurrentRNorm, OBJCLASS_EDITOBJECT, &pinf, false, true);
    if (!bPickObject) bPickGround = UI->PickGround(pinf.pt,UI->m_CurrentRStart,UI->m_CurrentRNorm);
    if (bPickObject||bPickGround){
        Scene->m_DetailPatches->Select(false);
        GenerateDPatch();
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
    }
    return false;
}

void __fastcall TUI_ControlDPatchAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlDPatchAdd::End(TShiftState _Shift)
{
    return true;
}

