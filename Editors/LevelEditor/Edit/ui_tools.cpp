#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_control.h"
#include "cursor3d.h"
#include "UI_CustomTools.h"
#include "LeftBar.h"
#include "TopBar.h"
#include "Scene.h"
#include "ui_main.h"

#include "editlibrary.h"
#include "render.h"
#include "ObjectList.h"

#define DETACH_FRAME(a) if (a){ (a)->Parent = NULL;}
#define ATTACH_FRAME(a,b) if (a){ (a)->Parent = (b);}

TUI_Tools Tools;

TShiftState ssRBOnly;
//---------------------------------------------------------------------------
TUI_Tools::TUI_Tools()
{
    m_Flags.zero();
}
//---------------------------------------------------------------------------
TUI_Tools::~TUI_Tools()
{
    for (u32 i=0; i<etMaxTarget; i++) xr_delete(m_pTools[i]);
}
//---------------------------------------------------------------------------

TFrame*	TUI_Tools::GetFrame(){
	if (pCurTools) return pCurTools->pFrame;
    return 0;
}
//---------------------------------------------------------------------------

bool TUI_Tools::OnCreate()
{
    target          = -1;
    action          = -1;
    sub_target		= -1;
    ZeroMemory      (m_pTools,sizeof(TUI_CustomTools*)*etMaxTarget);
    pCurTools       = 0;
    ssRBOnly << ssRight;
    paParent = fraLeftBar->paFrames;   VERIFY(paParent);
    m_Flags.set(flChangeAction,FALSE);
    m_Flags.set(flChangeTarget,FALSE);
// create tools
    for (int tgt=etFirstTool; tgt<etMaxTarget; tgt++)
		m_pTools[tgt]=NewToolFromTarget(tgt);
    // scene creating
	Scene.OnCreate	();
    // change target to Object
	UI.Command		(COMMAND_CHANGE_TARGET, etObject);
	m_Props 		= TProperties::CreateForm(0,alClient,OnPropsModified,0,OnPropsClose);
    pObjectListForm = TfrmObjectList::CreateForm();
    return true;
}
//---------------------------------------------------------------------------

void TUI_Tools::OnDestroy()
{
    TfrmObjectList::DestroyForm(pObjectListForm);
	TProperties::DestroyForm(m_Props);
    // scene destroing
	Scene.OnDestroy		();
    pCurTools->OnDeactivate();
}
//---------------------------------------------------------------------------
void TUI_Tools::Reset()
{
	SetTarget(GetTarget(),true);
}
//---------------------------------------------------------------------------

bool __fastcall TUI_Tools::MouseStart(TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->Start(Shift);
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::MouseMove(TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) pCurTools->pCurControl->Move(Shift);
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::MouseEnd(TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl)	return pCurTools->pCurControl->End(Shift);
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::OnObjectsUpdate()
{
	UpdateProperties(false);
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->OnObjectsUpdate();
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::HiddenMode()
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->HiddenMode();
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyDown   (WORD Key, TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->KeyDown(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyUp     (WORD Key, TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->KeyUp(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyPress  (WORD Key, TShiftState Shift)
{
    if(pCurTools&&pCurTools->pCurControl) return pCurTools->pCurControl->KeyPress(Key,Shift);
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
    m_Flags.set	(flChangeAction,FALSE);
}

void __fastcall TUI_Tools::ChangeAction(int act, bool forced){
	// если мышь захвачена - изменим action после того как она освободится
	if (UI.IsMouseCaptured()||UI.IsMouseInUse()||!forced){
	    m_Flags.set	(flChangeAction,TRUE);
        iNeedAction=act;
    }else
    	SetAction	(act);
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::SetTarget   (int tgt,bool bForced)
{
    if(bForced||(target!=tgt)){
        target 					= tgt;
        sub_target 				= estDefault;
        if (pCurTools){
            DETACH_FRAME(pCurTools->pFrame);
            pCurTools->OnDeactivate();
        }
        pCurTools=m_pTools[tgt]; VERIFY(pCurTools);
        pCurTools->OnActivate	();
        
        pCurTools->SetSubTarget	(sub_target);
        pCurTools->SetAction	(action);
        ATTACH_FRAME(pCurTools->pFrame, paParent);
    }
    UI.RedrawScene();
    fraLeftBar->ChangeTarget(tgt);
    UI.Command(COMMAND_UPDATE_TOOLBAR);
    m_Flags.set(flChangeTarget,FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::ResetSubTarget()
{
	VERIFY(pCurTools);
	pCurTools->ResetSubTarget();
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::SetSubTarget(int tgt)
{
	VERIFY(pCurTools);
	sub_target 				= tgt;
    pCurTools->SetSubTarget	(tgt);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::ChangeTarget(int tgt, bool forced)
{
	// если мышь захвачена - изменим target после того как она освободится
	if (UI.IsMouseCaptured()||UI.IsMouseInUse()||!forced){
	    m_Flags.set(flChangeTarget,TRUE);
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

bool TUI_Tools::Pick(TShiftState Shift)
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

#include "PropertiesWayPoint.h"

void TUI_Tools::RefreshProperties()
{
	m_Props->RefreshForm();
}

void TUI_Tools::ShowProperties()
{
    ObjectList lst;
    EObjClass cls_id				= CurrentClassID();
    if (Scene.GetQueryObjects		(lst,cls_id)){
        bool bChange				= false;
        switch(cls_id){
        case OBJCLASS_WAY:   		TfrmPropertiesWayPoint::Run(&lst,bChange);	break;
        default:
            m_Props->ShowProperties	();
            UpdateProperties		(false);
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
void TUI_Tools::RealUpdateProperties()
{
	if (m_Props->Visible){
		if (m_Props->IsModified()) Scene.UndoSave();
        ObjectList lst;
        EObjClass cls_id				= CurrentClassID();
        PropItemVec items;
        if (Scene.GetQueryObjects(lst,cls_id)){
            for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
                LPCSTR pref				= GetClassNameByClassID((*it)->ClassID);
                (*it)->FillProp	 		(pref,items);
            }
        }

		m_Props->AssignItems		(items,true,"Object Inspector");
    }
	m_Flags.set(flUpdateProperties,FALSE);
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

#include "EditLightAnim.h"

bool TUI_Tools::IfModified()
{
    EEditorState est 		= UI.GetEState();
    switch(est){
    case esEditLightAnim: 	return TfrmEditLightAnim::FinalClose();
    case esEditLibrary: 	return TfrmEditLibrary::FinalClose();
    case esEditScene:		return Scene.IfModified();
    default: THROW;
    }
    return false;
}
//---------------------------------------------------------------------------

void TUI_Tools::ZoomObject(bool bSelectedOnly)
{
    if( !Scene.locked() ){
        Scene.ZoomExtents(bSelectedOnly);
    } else {
        if (UI.GetEState()==esEditLibrary){
            TfrmEditLibrary::ZoomObject();
        }
    }
}
//---------------------------------------------------------------------------

void TUI_Tools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
	st_Environment& E 	= Scene.m_LevelOp.m_Envs[Scene.m_LevelOp.m_CurEnv];
	s_fog				= psDeviceFlags.is(rsFog)?(1.0f - E.m_Fogness)* 0.85f * E.m_ViewDist:UI.ZFar();
	e_fog				= psDeviceFlags.is(rsFog)?0.91f * E.m_ViewDist:UI.ZFar();
	fog_color 			=  E.m_FogColor.get();
}
//---------------------------------------------------------------------------

LPCSTR TUI_Tools::GetInfo()
{
	static AnsiString sel;
	int cnt = Scene.SelectionCount(true,Tools.CurrentClassID());
	return sel.sprintf(" Sel: %d",cnt).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::OnFrame()
{
	Scene.OnFrame		(Device.fTimeDelta);
    EEditorState est 	= UI.GetEState();
    if ((est==esEditScene)||(est==esEditLibrary)||(est==esEditLightAnim)){
        if (!UI.IsMouseCaptured()){
            // если нужно изменить target выполняем после того как мышь освободится
            if(m_Flags.is(flChangeTarget)) 		SetTarget(iNeedTarget);
            // если нужно изменить action выполняем после того как мышь освободится
            if(m_Flags.is(flChangeAction)) 		SetAction(iNeedAction);
        }
        if (m_Flags.is(flUpdateProperties)) 	RealUpdateProperties();
        if (m_Flags.is(flUpdateObjectList)) 	RealUpdateObjectList();
        if (est==esEditLightAnim) TfrmEditLightAnim::OnIdle();
    }
}
//---------------------------------------------------------------------------
#include "d3dutils.h"
void __fastcall TUI_Tools::RenderEnvironment()
{
    // draw sky
    EEditorState est 		= UI.GetEState();
    switch(est){
    case esEditLightAnim:
    case esEditScene:		Scene.RenderSky(Device.m_Camera.GetTransform()); break;
    }
}

void __fastcall TUI_Tools::Render()
{
	// Render update
    ::Render->Calculate		();
    ::Render->Render		();

    EEditorState est 		= UI.GetEState();
    // draw scene
    switch(est){
    case esEditLibrary: 	TfrmEditLibrary::OnRender(); break;
    case esEditLightAnim:
    case esEditScene:		Scene.Render(Device.m_Camera.GetTransform()); break;
    }
    // draw cursor
    UI.m_Cursor->Render();
}
//---------------------------------------------------------------------------

void TUI_Tools::ShowObjectList()
{
	if (pObjectListForm) pObjectListForm->ShowObjectList();
}
//---------------------------------------------------------------------------

void TUI_Tools::RealUpdateObjectList()
{
	if (pObjectListForm) pObjectListForm->UpdateObjectList();
	m_Flags.set(flUpdateObjectList,FALSE);
}
//---------------------------------------------------------------------------

bool TUI_Tools::IsModified()
{
	return Scene.IsModified();
}

