#include "stdafx.h"
#pragma hdrstop

#include "ui_Sectortools.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "Sector.h"
#include "editmesh.h"
#include "EditObject.h"
#include "frameSector.h"
#include "topbar.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "frustum.h"

//---------------------------------------------------------------------------
TUI_SectorTools::TUI_SectorTools():TUI_CustomTools(OBJCLASS_SECTOR){
    AddControlCB(new TUI_ControlSectorSelect(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlSectorAdd 	(estSelf,eaAdd,		this));
}
void TUI_SectorTools::OnObjectsUpdate(){
    TfraSector* fraSector = (TfraSector*)pFrame; VERIFY(fraSector);
    fraSector->OnChange();
}
void TUI_SectorTools::OnActivate  (){
    pFrame = new TfraSector(0);
	TUI_CustomTools::OnActivate();
}
void TUI_SectorTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//------------------------------------------------------------------------------
// add
//------------------------------------------------------------------------------
__fastcall TUI_ControlSectorAdd::TUI_ControlSectorAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

void __fastcall TUI_ControlSectorAdd::OnEnter()
{
    m_Action = saNone;
    fraSector = (TfraSector*)parent_tool->pFrame; VERIFY(fraSector);
    fraSector->paSectorActions->Show();
}

void __fastcall TUI_ControlSectorAdd::OnExit()
{
    fraSector->paSectorActions->Hide();
	fraSector = 0;
}

void TUI_ControlSectorAdd::AddFace(){
    m_Action = saAddFace;
    CSector* sector=(CSector*)fraSector->cbItems->Items->Objects[fraSector->cbItems->ItemIndex];
    VERIFY(sector);
    SRayPickInfo pinf;
    if (Scene->RayPick( UI->m_CurrentRStart,UI->m_CurrentRNorm, OBJCLASS_EDITOBJECT, &pinf, false, false)){
		if (fraSector->ebUseTies->Down){
            DWORDVec fl;
            pinf.mesh->GetTiesFaces(pinf.rp_inf.id, fl, fraSector->seSoftAngle->Value, fraSector->ebSoftRecursive->Down);
            sector->AddFaces(pinf.obj,pinf.mesh,fl);
        }else{
            sector->AddFace(pinf.obj,pinf.mesh,pinf.rp_inf.id);
        }
    }
}

void TUI_ControlSectorAdd::DelFace(){
    m_Action = saDelFace;
    CSector* sector=(CSector*)fraSector->cbItems->Items->Objects[fraSector->cbItems->ItemIndex];
    SRayPickInfo pinf;
    if (Scene->RayPick( UI->m_CurrentRStart,UI->m_CurrentRNorm, OBJCLASS_EDITOBJECT, &pinf, false, false)){
		if (fraSector->ebUseTies->Down){
            DWORDVec fl;
            pinf.mesh->GetTiesFaces(pinf.rp_inf.id, fl, fraSector->seSoftAngle->Value, fraSector->ebSoftRecursive->Down);
            sector->DelFaces(pinf.obj,pinf.mesh,fl);
        }else{
            sector->DelFace(pinf.obj,pinf.mesh,pinf.rp_inf.id);
        }
    }
}

bool __fastcall TUI_ControlSectorAdd::Start(TShiftState Shift)
{
    if (Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_SECTOR); return false;}
	if (fraSector->ebAddFace->Down||fraSector->ebDelFace->Down){
		if (fraSector->cbItems->ItemIndex==-1) return false;
		bool bBoxSelection = Shift.Contains(ssCtrl) || fraSector->ebFaceBoxPick->Down;

        if( bBoxSelection ){
            UI->EnableSelectionRect( true );
            UI->UpdateSelectionRect(UI->m_StartCp,UI->m_CurrentCp);
			m_Action = saFaceBoxSelection;
            return true;
        } else {
            if (fraSector->ebAddFace->Down)	AddFace();
            if (fraSector->ebDelFace->Down)	DelFace();
            return true;
        }
    }
    return false;
}

void __fastcall TUI_ControlSectorAdd::Move(TShiftState _Shift)
{
    switch (m_Action){
    case saAddFace:	AddFace();	break;
    case saDelFace:	DelFace();	break;
    case saFaceBoxSelection:UI->UpdateSelectionRect(UI->m_StartCp,UI->m_CurrentCp); break;
    }
}

bool __fastcall TUI_ControlSectorAdd::End(TShiftState _Shift)
{
	if (fraSector->cbItems->ItemIndex>-1){
        CSector* sector=(CSector*)fraSector->cbItems->Items->Objects[fraSector->cbItems->ItemIndex];
        if (m_Action==saFaceBoxSelection){
            UI->EnableSelectionRect( false );
            DWORDVec fl;
            Fmatrix matrix;
            CEditObject *O_ref=NULL, *O_lib=NULL;;

            CFrustum frustum;
            ObjectList lst;
            if (UI->SelectionFrustum(frustum)){;
                Scene->FrustumPick(frustum, OBJCLASS_EDITOBJECT, lst);
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                    O_ref = (CEditObject*)(*_F); O_lib=(O_ref->IsReference())?O_ref->GetRef():O_ref;
                    for(EditMeshIt m_def = O_lib->m_Meshes.begin();m_def!=O_lib->m_Meshes.end();m_def++){
                        fl.clear();
                        O_ref->GetFullTransformToWorld(matrix);
                        (*m_def)->FrustumPickFaces(frustum,matrix,fl);
                        if (fraSector->ebAddFace->Down)	sector->AddFaces(O_ref,*m_def,fl);
                        if (fraSector->ebDelFace->Down)	sector->DelFaces(O_ref,*m_def,fl);
                    }
                }
            }
        }
        switch (m_Action){
        case saAddFace:
        case saDelFace:
        case saFaceBoxSelection:
        	sector->SectorChanged(fraSector->ebAutoUpdateConvex->Down);
            fraSector->OnSectorUpdate();
			Scene->UndoSave();
        break;
        }
    }
	m_Action = saNone;
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlSectorSelect::TUI_ControlSectorSelect(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
    pFrame 	= 0;
}
void TUI_ControlSectorSelect::OnEnter(){
    pFrame 	= (TfraSector*)parent_tool->pFrame; VERIFY(pFrame);
}
void TUI_ControlSectorSelect::OnExit (){
	pFrame = 0;
}
bool __fastcall TUI_ControlSectorSelect::Start(TShiftState Shift){
	bool bRes = SelectStart(Shift);
	if(!bBoxSelection) pFrame->OnChange();
    return bRes;
}
void __fastcall TUI_ControlSectorSelect::Move(TShiftState Shift){
	SelectProcess(Shift);
}

bool __fastcall TUI_ControlSectorSelect::End(TShiftState Shift){
	bool bRes = SelectEnd(Shift);
	if (bBoxSelection) pFrame->OnChange();
    return bRes;
}

