#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_control.h"
#include "ui_main.h"
#include "sceneclasslist.h"
#include "UI_CustomTools.h"
#include "LeftBar.h"
#include "TopBar.h"
#include "Scene.h"

#include "UI_LightTools.h"
#include "UI_ObjectTools.h"
#include "UI_SoundTools.h"
#include "UI_OccluderTools.h"
#include "UI_GlowTools.h"
#include "UI_DPatchTools.h"
#include "UI_RPointTools.h"
#include "UI_AITPointTools.h"
#include "UI_SectorTools.h"
#include "UI_PortalTools.h"
#include "UI_EventTools.h"
#include "UI_PSTools.h"
#include "UI_DOTools.h"

#define DETACH_FRAME(a) if (a){ (a)->Parent = NULL;}
#define ATTACH_FRAME(a,b) if (a){ (a)->Parent = (b);}

TShiftState ssRBOnly;
//---------------------------------------------------------------------------
TUI_Tools::TUI_Tools(TPanel* p)
{
    UI->m_Tools      = this;
    target          = -1;
    action          = -1;
    ZeroMemory      (m_pTools,sizeof(TUI_CustomTools*)*etMaxTarget);
    pCurTools       = 0;
    ssRBOnly << ssRight;
    paParent = p;   VERIFY(p);
    bNeedChangeAction=false;
    bNeedChangeTarget=false;
// create tools
    AddTool			(etLight, 	new TUI_LightTools	());
    AddTool			(etObject,	new TUI_ObjectTools	());
    AddTool			(etSound, 	new TUI_SoundTools	());
    AddTool			(etOccluder,new TUI_OccluderTools());
    AddTool			(etGlow,   	new TUI_GlowTools	());
	AddTool			(etDPatch, 	new TUI_DPatchTools	());
    AddTool			(etRPoint, 	new TUI_RPointTools	());
    AddTool			(etSector, 	new TUI_SectorTools	());
    AddTool			(etPortal, 	new TUI_PortalTools	());
    AddTool			(etEvent, 	new TUI_EventTools	());
    AddTool			(etAITPoint,new TUI_AITPointTools());
    AddTool			(etPS, 		new TUI_PSTools		());
    AddTool			(etDO,		new TUI_DOTools		());
// check tools    
    for (DWORD i=0; i<etMaxTarget; i++) VERIFY2(m_pTools[i], "Can't find specify tools.");
}
//---------------------------------------------------------------------------
TUI_Tools::~TUI_Tools()
{
    for (DWORD i=0; i<etMaxTarget; i++) _DELETE(m_pTools[i]);
}
//---------------------------------------------------------------------------

TFrame*	TUI_Tools::GetFrame(){
	if (pCurTools) return pCurTools->pFrame;
    return 0;
}
//---------------------------------------------------------------------------

void TUI_Tools::Clear(){
    pCurTools->OnDeactivate();
}
//---------------------------------------------------------------------------
void TUI_Tools::Reset(){
	SendMessage(fraLeftBar->Handle,WM_SETREDRAW,0,0);
	SetTarget(GetTarget(),true);
	SendMessage(fraLeftBar->Handle,WM_SETREDRAW,1,0);
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::AddTool(ETarget tgt, TUI_CustomTools* t){
    VERIFY(t);
    m_pTools[tgt]=t;
}
//---------------------------------------------------------------------------

bool __fastcall TUI_Tools::MouseStart(TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->Start(Shift);
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::MouseMove(TShiftState Shift){
    if(pCurTools->pCurControl) pCurTools->pCurControl->Move(Shift);
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::MouseEnd(TShiftState Shift){
    if(pCurTools->pCurControl)	return pCurTools->pCurControl->End(Shift);
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::Update(){
	if(!UI->IsMouseCaptured()){
        // если нужно изменить target выполняем после того как мышь освободится
        if(bNeedChangeTarget){
            SetTarget(iNeedTarget);
            bNeedChangeTarget=false;
        }
        // если нужно изменить action выполняем после того как мышь освободится
        if(bNeedChangeAction){
            SetAction(iNeedAction);
            bNeedChangeAction=false;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::OnObjectsUpdate(){
    if(pCurTools->pCurControl) return pCurTools->OnObjectsUpdate();
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::HiddenMode(){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->HiddenMode();
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyDown   (WORD Key, TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->KeyDown(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyUp     (WORD Key, TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->KeyUp(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyPress  (WORD Key, TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->KeyPress(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::SetAction   (int act)
{
    action = act;
    switch(act){
        case eaSelect:  UI->GetD3DWindow()->Cursor = crCross;     break;
        case eaAdd:     UI->GetD3DWindow()->Cursor = crArrow;     break;
        case eaMove:    UI->GetD3DWindow()->Cursor = crSizeAll;   break;
        case eaRotate:  UI->GetD3DWindow()->Cursor = crSizeWE;    break;
        case eaScale:   UI->GetD3DWindow()->Cursor = crVSplit;    break;
        default:        UI->GetD3DWindow()->Cursor = crHelp;
    }
    if (pCurTools) pCurTools->SetAction(action);
    UI->RedrawScene();
    fraTopBar->ChangeAction(act);
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}

void __fastcall TUI_Tools::ChangeAction(int act){
	// если мышь захвачена - изменим action после того как она освободится
	if (UI->IsMouseCaptured()||UI->IsMouseInUse()){
		bNeedChangeAction=true;
        iNeedAction=act;
    }else
    	SetAction(act);
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::SetTarget   (int tgt,bool bForced)
{
    if(bForced||(target!=tgt)){
        target = tgt;
        if (pCurTools){
            DETACH_FRAME(pCurTools->pFrame);
            pCurTools->OnDeactivate();
        }
        pCurTools=m_pTools[tgt]; VERIFY(pCurTools);
        pCurTools->OnActivate();
        pCurTools->SetAction(action);
        pCurTools->ResetSubTarget();
        ATTACH_FRAME(pCurTools->pFrame, paParent);
    }
    UI->RedrawScene();
    fraLeftBar->ChangeTarget(tgt);
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::SetSubTarget(int tgt)
{
    pCurTools->SetSubTarget(tgt);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::UnsetSubTarget(int tgt)
{
    pCurTools->UnsetSubTarget(tgt);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::ChangeTarget(int tgt){
	// если мышь захвачена - изменим target после того как она освободится
	if (UI->IsMouseCaptured()||UI->IsMouseInUse()){
		bNeedChangeTarget=true;
        iNeedTarget=tgt;
    }else
    	SetTarget(tgt);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::ShowProperties(){
	if (pCurTools) pCurTools->ShowProperties();
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumPosition(SceneObject* O){
	if (pCurTools) pCurTools->SetNumPosition(O);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumRotation(SceneObject* O){
	if (pCurTools) pCurTools->SetNumRotation(O);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumScale(SceneObject* O){
	if (pCurTools) pCurTools->SetNumScale(O);
}
//---------------------------------------------------------------------------


#define MIN_PANEL_HEIGHT 15
void __fastcall PanelMinimizeClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }else{
        pa->Tag    = pa->Height;
        pa->Height = MIN_PANEL_HEIGHT;
    }
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}
//---------------------------------------------------------------------------


