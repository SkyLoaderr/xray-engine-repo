#include "stdafx.h"
#pragma hdrstop

#include "ui_Sectortools.h"
#include "scene.h"
#include "Sector.h"
#include "editmesh.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "frameSector.h"
#include "topbar.h"
#include "ui_tools.h"
#include "frustum.h"
#include "ui_main.h"

//---------------------------------------------------------------------------
TUI_SectorTools::TUI_SectorTools():TUI_CustomTools(OBJCLASS_SECTOR){
    AddControlCB(new TUI_ControlSectorSelect(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlSectorAdd 	(estSelf,eaAdd,		this));
}
void TUI_SectorTools::OnObjectsUpdate(){
    TfraSector* fraSector = (TfraSector*)pFrame; VERIFY(fraSector);
//    fraSector->OnChange();
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

void TUI_ControlSectorAdd::AddMesh(){
    m_Action = saAddMesh;
    CSector* sector=PortalUtils.GetSelectedSector();
    if (!sector) return;
    SRayPickInfo pinf;
    if (Scene.RayPick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SCENEOBJECT, &pinf, false, 0))
		sector->AddMesh(pinf.s_obj,pinf.e_mesh);
}

void TUI_ControlSectorAdd::DelMesh(){
    m_Action = saDelMesh;
    CSector* sector=PortalUtils.GetSelectedSector();
    if (!sector) return;
    SRayPickInfo pinf;
    if (Scene.RayPick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SCENEOBJECT, &pinf, false, 0))
		sector->DelMesh(pinf.s_obj,pinf.e_mesh);
}

bool TUI_ControlSectorAdd::AddSector(){
	char namebuffer[MAX_OBJ_NAME];
	Scene.GenObjectName( OBJCLASS_SECTOR, namebuffer );
	CSector* _O = new CSector(0,namebuffer);
    SRayPickInfo pinf;
    if (Scene.RayPick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SCENEOBJECT, &pinf, false, 0)&&
    	(_O->AddMesh(pinf.s_obj,pinf.e_mesh)))
    {
        Scene.SelectObjects(false,OBJCLASS_SECTOR);
        Scene.AddObject( _O );
		_O->UpdateVolume();
        return true;
    }else{
    	_DELETE(_O);
		return false;
    }
}

bool __fastcall TUI_ControlSectorAdd::Start(TShiftState Shift)
{
    if (Shift==ssRBOnly){ UI.Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_SECTOR); return false;}
    if (fraSector->ebCreateNew->Down){
    	if (AddSector()&&(!Shift.Contains(ssAlt))) fraSector->ebCreateNew->Down=false;
        return false;
    }
	if (fraSector->ebAddMesh->Down||fraSector->ebDelMesh->Down){
		bool bBoxSelection = Shift.Contains(ssCtrl) || fraSector->ebFaceBoxPick->Down;

        if( bBoxSelection ){
            UI.EnableSelectionRect( true );
            UI.UpdateSelectionRect(UI.m_StartCp,UI.m_CurrentCp);
			m_Action = saMeshBoxSelection;
            return true;
        } else {
            if (fraSector->ebAddMesh->Down)	AddMesh();
            if (fraSector->ebDelMesh->Down)	DelMesh();
            return true;
        }
    }
    return false;
}

void __fastcall TUI_ControlSectorAdd::Move(TShiftState _Shift)
{
    switch (m_Action){
    case saAddMesh:	AddMesh();	break;
    case saDelMesh:	DelMesh();	break;
    case saMeshBoxSelection:UI.UpdateSelectionRect(UI.m_StartCp,UI.m_CurrentCp); break;
    }
}

bool __fastcall TUI_ControlSectorAdd::End(TShiftState _Shift)
{
    CSector* sector=PortalUtils.GetSelectedSector();
	if (sector){
        if (m_Action==saMeshBoxSelection){
            UI.EnableSelectionRect( false );
            DWORDVec fl;
            Fmatrix matrix;
            CSceneObject* O_ref=NULL;
            CEditableObject* O_lib=NULL;

            CFrustum frustum;
            ObjectList lst;
            if (UI.SelectionFrustum(frustum)){;
                Scene.FrustumPick(frustum, OBJCLASS_SCENEOBJECT, lst);
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                    O_ref = (CSceneObject*)(*_F);
                    O_lib = O_ref->GetReference();
                    for(EditMeshIt m_def = O_lib->m_Meshes.begin();m_def!=O_lib->m_Meshes.end();m_def++){
                        fl.clear();
                        O_ref->GetFullTransformToWorld(matrix);
                        if (fraSector->ebAddMesh->Down)	sector->AddMesh(O_ref,*m_def);
                        if (fraSector->ebDelMesh->Down)	if (!sector->DelMesh(O_ref,*m_def)) break;
                    }
                }
            }
        }
        switch (m_Action){
        case saAddMesh:
        case saDelMesh:
        case saMeshBoxSelection:
			Scene.UndoSave();
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
//	if(!bBoxSelection) pFrame->OnChange();
    return bRes;
}
void __fastcall TUI_ControlSectorSelect::Move(TShiftState Shift){
	SelectProcess(Shift);
}

bool __fastcall TUI_ControlSectorSelect::End(TShiftState Shift){
	bool bRes = SelectEnd(Shift);
//	if (bBoxSelection) pFrame->OnChange();
    return bRes;
}

