#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_control.h"
#include "UI_CustomTools.h"
#include "LeftBar.h"
#include "TopBar.h"
#include "Scene.h"
#include "ui_main.h"

#include "editlibrary.h"

#define DETACH_FRAME(a) if (a){ (a)->Parent = NULL;}
#define ATTACH_FRAME(a,b) if (a){ (a)->Parent = (b);}

TUI_Tools Tools;

TShiftState ssRBOnly;
//---------------------------------------------------------------------------
TUI_Tools::TUI_Tools()
{
	m_Props = TProperties::CreateForm(0,alClient,OnPropsModified,0,OnPropsClose);
}
//---------------------------------------------------------------------------
TUI_Tools::~TUI_Tools()
{
	TProperties::DestroyForm(m_Props);
    for (DWORD i=0; i<etMaxTarget; i++) _DELETE(m_pTools[i]);
}
//---------------------------------------------------------------------------

TFrame*	TUI_Tools::GetFrame(){
	if (pCurTools) return pCurTools->pFrame;
    return 0;
}
//---------------------------------------------------------------------------

void TUI_Tools::OnCreate(){
    target          = -1;
    action          = -1;
    ZeroMemory      (m_pTools,sizeof(TUI_CustomTools*)*etMaxTarget);
    pCurTools       = 0;
    ssRBOnly << ssRight;
    paParent = fraLeftBar->paFrames;   VERIFY(paParent);
    bNeedChangeAction=false;
    bNeedChangeTarget=false;
// create tools
    for (int tgt=etFirstTool; tgt<etMaxTarget; tgt++)
		m_pTools[tgt]=NewToolFromTarget(tgt);
}
//---------------------------------------------------------------------------

void TUI_Tools::OnDestroy(){
    pCurTools->OnDeactivate();
}
//---------------------------------------------------------------------------
void TUI_Tools::Reset(){
	SetTarget(GetTarget(),true);
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
void __fastcall TUI_Tools::OnFrame(){
	if(!UI.IsMouseCaptured()){
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
        case eaSelect:  UI.GetD3DWindow()->Cursor = crCross;     break;
        case eaAdd:     UI.GetD3DWindow()->Cursor = crArrow;     break;
        case eaMove:    UI.GetD3DWindow()->Cursor = crSizeAll;   break;
        case eaRotate:  UI.GetD3DWindow()->Cursor = crSizeWE;    break;
        case eaScale:   UI.GetD3DWindow()->Cursor = crVSplit;    break;
        default:        UI.GetD3DWindow()->Cursor = crHelp;
    }
    if (pCurTools) pCurTools->SetAction(action);
    UI.RedrawScene();
    fraTopBar->ChangeAction(act);
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}

void __fastcall TUI_Tools::ChangeAction(int act){
	// если мышь захвачена - изменим action после того как она освободится
	if (UI.IsMouseCaptured()||UI.IsMouseInUse()){
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
    UI.RedrawScene();
    fraLeftBar->ChangeTarget(tgt);
    UI.Command(COMMAND_UPDATE_TOOLBAR);
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
	if (UI.IsMouseCaptured()||UI.IsMouseInUse()){
		bNeedChangeTarget=true;
        iNeedTarget=tgt;
    }else
    	SetTarget(tgt);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumPosition(CCustomObject* O){
	if (pCurTools) pCurTools->SetNumPosition(O);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumRotation(CCustomObject* O){
	if (pCurTools) pCurTools->SetNumRotation(O);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumScale(CCustomObject* O){
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
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}
//---------------------------------------------------------------------------

EObjClass TUI_Tools::CurrentClassID(){
	return (fraLeftBar->ebIgnoreMode->Down)?OBJCLASS_DUMMY:GetTargetClassID();
}
//---------------------------------------------------------------------------

void TUI_Tools::OnShowHint(AStringVec& ss){
	Scene.OnShowHint(ss);
}
//---------------------------------------------------------------------------

bool TUI_Tools::Pick()
{
    if( Scene.locked() && (esEditLibrary==UI.GetEState())){
        UI.iGetMousePosReal(Device.m_hRenderWnd, UI.m_CurrentCp);
        UI.m_StartCp = UI.m_CurrentCp;
        Device.m_Camera.MouseRayFromPoint(UI.m_StartRStart, UI.m_StartRNorm, UI.m_StartCp );
        Device.m_Camera.MouseRayFromPoint(UI.m_CurrentRStart, UI.m_CurrentRNorm, UI.m_CurrentCp );
        SRayPickInfo pinf;
        TfrmEditLibrary::RayPick(UI.m_CurrentRStart,UI.m_CurrentRNorm,&pinf);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

#include "PropertiesLight.h"
#include "PropertiesSector.h"
#include "PropertiesPortal.h"
#include "PropertiesEvent.h"
#include "PropertiesPS.h"
#include "PropertiesWayPoint.h"

void TUI_Tools::ShowProperties()
{
    ObjectList lst;
    EObjClass cls_id				= CurrentClassID();
    if (Scene.GetQueryObjects		(lst,cls_id)){
        bool bChange				= false;
        switch(cls_id){
        case OBJCLASS_LIGHT:    	frmPropertiesLightRun(&lst,bChange);	   	break;
        case OBJCLASS_SECTOR:   	frmPropertiesSectorRun(&lst,bChange); 		break;
        case OBJCLASS_EVENT:   		frmPropertiesEventRun(&lst,bChange);		break;
        case OBJCLASS_WAY:   		TfrmPropertiesWayPoint::Run(&lst,bChange);	break;
        case OBJCLASS_PS:			TfrmPropertiesPS::Run(&lst,bChange);		break;
        default:
            m_Props->ShowProperties	();
            UpdateProperties		();
        }
        if (bChange) Scene.UndoSave();
        UI.RedrawScene();
    }
}
//---------------------------------------------------------------------------

void TUI_Tools::HideProperties()
{
	m_Props->HideProperties			();
}
//---------------------------------------------------------------------------

void TUI_Tools::UpdateProperties()
{
	if (m_Props->Visible){
		if (m_Props->IsModified()) Scene.UndoSave();
        ObjectList lst;
        EObjClass cls_id				= CurrentClassID();
        PropItemVec values;
        if (Scene.GetQueryObjects(lst,cls_id)){
            for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
                LPCSTR pref				= GetClassNameByClassID((*it)->ClassID);
                (*it)->FillProp	 		(pref,values);
            }
        }
		m_Props->AssignItems		(values,true,"Object Inspector");
    }
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::OnPropsClose()
{
	if (m_Props->IsModified()) Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::OnPropsModified()
{
	Scene.Modified();
//	Scene.UndoSave();
}
//---------------------------------------------------------------------------


