#include "stdafx.h"
#pragma hdrstop

#include "ui_Portaltools.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "Portal.h"
#include "framePortal.h"
#include "topbar.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "frustum.h"
//---------------------------------------------------------------------------
TUI_PortalTools::TUI_PortalTools():TUI_CustomTools(OBJCLASS_PORTAL){
    AddControlCB(new TUI_ControlPortalSelect(estSelf,eaSelect,	this));
    AddControlCB(new TUI_ControlPortalAdd 	(estSelf,eaAdd,		this));
}
void TUI_PortalTools::OnObjectsUpdate(){
    TfraPortal* fraPortal = (TfraPortal*)pFrame; VERIFY(fraPortal);
    fraPortal->OnChange();
}
void TUI_PortalTools::OnActivate  (){
    pFrame = new TfraPortal(0);
	TUI_CustomTools::OnActivate();
}
void TUI_PortalTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}
//------------------------------------------------------------------------------
// add
//------------------------------------------------------------------------------
__fastcall TUI_ControlPortalAdd::TUI_ControlPortalAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

void __fastcall TUI_ControlPortalAdd::OnEnter()
{
    TfraPortal* fraPortal = (TfraPortal*)parent_tool->pFrame; VERIFY(fraPortal);
    fraPortal->ShowCompute(true);
}

void __fastcall TUI_ControlPortalAdd::OnExit()
{
    TfraPortal* fraPortal = (TfraPortal*)parent_tool->pFrame; VERIFY(fraPortal);
    fraPortal->ShowCompute(false);
}

bool __fastcall TUI_ControlPortalAdd::Start(TShiftState Shift)
{
    if (Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,OBJCLASS_PORTAL); return false;}
    TfraPortal* fraPortal = (TfraPortal*)parent_tool->pFrame; VERIFY(fraPortal);

    if (fraPortal->ebPickSectors->Down){
	    SceneObject *obj = Scene->RayPick( UI->m_CurrentRStart,UI->m_CurrentRNorm, OBJCLASS_SECTOR, 0, false, false);
		if( obj ){
     		obj->Select(true);
	        fraPortal->SetSector((CSector*)obj);
    	}
    }

    return false;
}

void __fastcall TUI_ControlPortalAdd::Move(TShiftState _Shift)
{
}

bool __fastcall TUI_ControlPortalAdd::End(TShiftState _Shift)
{
    return true;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPortalSelect::TUI_ControlPortalSelect(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
    pFrame 	= 0;
}
void TUI_ControlPortalSelect::OnEnter(){
    pFrame 	= (TfraPortal*)parent_tool->pFrame; VERIFY(pFrame);
}
void TUI_ControlPortalSelect::OnExit (){
	pFrame = 0;
}
bool __fastcall TUI_ControlPortalSelect::Start(TShiftState Shift){
	bool bRes = SelectStart(Shift);
	if(!bBoxSelection) pFrame->OnChange();
    return bRes;
}
void __fastcall TUI_ControlPortalSelect::Move(TShiftState Shift){
	SelectProcess(Shift);
}

bool __fastcall TUI_ControlPortalSelect::End(TShiftState Shift){
	bool bRes = SelectEnd(Shift);
	if (bBoxSelection) pFrame->OnChange();
    return bRes;
}


