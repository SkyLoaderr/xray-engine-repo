#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_control.h"
#include "cursor3d.h"
#include "LeftBar.h"
#include "TopBar.h"
#include "Scene.h"
#include "ui_main.h"

#include "editlibrary.h"
#include "render.h"
#include "ObjectList.h"

#include "igame_persistent.h"

#define DETACH_FRAME(a) 	if (a){ (a)->Hide(); 	(a)->Parent = NULL; }
#define ATTACH_FRAME(a,b) 	if (a){ (a)->Parent=(b);(a)->Show(); 		}

TUI_Tools Tools;

TShiftState ssRBOnly;
//---------------------------------------------------------------------------
TUI_Tools::TUI_Tools()
{
    fFogness	= 0.9f;
    dwFogColor	= 0xffffffff;
    m_Flags.zero();
}
//---------------------------------------------------------------------------
TUI_Tools::~TUI_Tools()
{
}
//---------------------------------------------------------------------------

TForm*	TUI_Tools::GetFrame()
{
	if (pCurTools) return pCurTools->pFrame;
    return 0;
}
//---------------------------------------------------------------------------
bool TUI_Tools::OnCreate()
{
    target          = OBJCLASS_DUMMY;//-1;
    action          = eaSelect;//-1;
    sub_target		= -1;
    pCurTools       = 0;
    ssRBOnly << ssRight;
    paParent = fraLeftBar->paFrames;   VERIFY(paParent);
    m_Flags.set(flChangeAction,FALSE);
    m_Flags.set(flChangeTarget,FALSE);
    // scene creating
	Scene.OnCreate	();
    // change target to Object
	UI.Command		(COMMAND_CHANGE_TARGET, OBJCLASS_SCENEOBJECT);
	m_Props 		= TProperties::CreateForm("Object Inspector",0,alClient,OnPropsModified,0,OnPropsClose);
    pObjectListForm = TfrmObjectList::CreateForm();
    return true;
}
//---------------------------------------------------------------------------

void TUI_Tools::OnDestroy()
{
    TfrmObjectList::DestroyForm(pObjectListForm);
	TProperties::DestroyForm(m_Props);
    // scene destroing
    if (pCurTools) 		pCurTools->OnDeactivate();
	Scene.OnDestroy		();
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
	// ���� ���� ��������� - ������� action ����� ���� ��� ��� �����������
	if (UI.IsMouseCaptured()||UI.IsMouseInUse()||!forced){
	    m_Flags.set	(flChangeAction,TRUE);
        iNeedAction=act;
    }else
    	SetAction	(act);
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::SetTarget   (EObjClass tgt,bool bForced)
{
    if(bForced||(target!=tgt)){
        target 					= tgt;
        sub_target 				= estDefault;
        if (pCurTools){
            DETACH_FRAME(pCurTools->pFrame);
            pCurTools->OnDeactivate();
        }
        pCurTools				= Scene.GetMTools(tgt); VERIFY(pCurTools);
        pCurTools->OnActivate	();
        
        pCurTools->SetSubTarget	(sub_target);
        pCurTools->SetAction	(action);
        ATTACH_FRAME(pCurTools->pFrame, paParent);
    }
    UI.RedrawScene();
    fraLeftBar->ChangeTarget(tgt);
    fraLeftBar->UpdateSnapList();
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
void __fastcall TUI_Tools::ChangeTarget(EObjClass tgt, bool forced)
{
	// ���� ���� ��������� - ������� target ����� ���� ��� ��� �����������
	if (UI.IsMouseCaptured()||UI.IsMouseInUse()||!forced){
	    m_Flags.set(flChangeTarget,TRUE);
        iNeedTarget=tgt;
    }else
    	SetTarget(tgt,forced);
}
//---------------------------------------------------------------------------

EObjClass TUI_Tools::CurrentClassID()
{
	return GetTarget();
}
//---------------------------------------------------------------------------

void TUI_Tools::OnShowHint(AStringVec& ss)
{
	Scene.OnShowHint(ss);
}
//---------------------------------------------------------------------------

bool TUI_Tools::Pick(TShiftState Shift)
{
    if( Scene.locked() && (esEditLibrary==UI.GetEState())){
        UI.IR_GetMousePosReal(Device.m_hRenderWnd, UI.m_CurrentCp);
        UI.m_StartCp = UI.m_CurrentCp;
        Device.m_Camera.MouseRayFromPoint(UI.m_CurrentRStart, UI.m_CurrentRNorm, UI.m_CurrentCp );
        SRayPickInfo pinf;
        TfrmEditLibrary::RayPick(UI.m_CurrentRStart,UI.m_CurrentRNorm,&pinf);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

void TUI_Tools::RefreshProperties()
{
	m_Props->RefreshForm();
}

void TUI_Tools::ShowProperties()
{
    m_Props->ShowProperties	();
    UpdateProperties		(false);
    UI.RedrawScene			();
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
        EObjClass cls_id			= CurrentClassID();
        PropItemVec items;
        if (OBJCLASS_DUMMY==cls_id){
            SceneToolsMapPairIt _I 	= Scene.FirstTools();
            SceneToolsMapPairIt _E	= Scene.LastTools();
            for (; _I!=_E; _I++)
                if (_I->second&&(_I->first!=OBJCLASS_DUMMY))	
                	_I->second->FillProp(_I->second->ClassDesc(),items);
        }else{
            ESceneCustomMTools* mt	= Scene.GetMTools(cls_id);
            if (mt) mt->FillProp	(mt->ClassDesc(),items);
        }

		m_Props->AssignItems		(items,true);
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
        Scene.ZoomExtents(CurrentClassID(),bSelectedOnly);
    } else {
        if (UI.GetEState()==esEditLibrary){
            TfrmEditLibrary::ZoomObject();
        }
    }
}
//---------------------------------------------------------------------------

void TUI_Tools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
	if (psDeviceFlags.is(rsEnvironment)&&psDeviceFlags.is(rsFog)){
        s_fog				= g_pGamePersistent->Environment.CurrentEnv.fog_near;
        e_fog				= g_pGamePersistent->Environment.CurrentEnv.fog_far;
        Fvector& f_clr		= g_pGamePersistent->Environment.CurrentEnv.fog_color;
        fog_color 			= color_rgba_f(f_clr.x,f_clr.y,f_clr.z,1.f);
    }else{
        s_fog				= psDeviceFlags.is(rsFog)?(1.0f - fFogness)* 0.85f * UI.ZFar():0.99f*UI.ZFar();
        e_fog				= psDeviceFlags.is(rsFog)?0.91f * UI.ZFar():UI.ZFar();
        fog_color 			= dwFogColor;
    }
/*
//.
    f_near	= g_pGamePersistent->Environment.Current.fog_near;
    f_far	= 1/(g_pGamePersistent->Environment.Current.fog_far - f_near);
*/
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
            // ���� ����� �������� target ��������� ����� ���� ��� ���� �����������
            if(m_Flags.is(flChangeTarget)) 		SetTarget(iNeedTarget);
            // ���� ����� �������� action ��������� ����� ���� ��� ���� �����������
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
    case esEditScene:		if (psDeviceFlags.is(rsEnvironment)) g_pGamePersistent->Environment.RenderFirst	();
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
    case esEditScene:
    	Scene.Render(Device.m_Camera.GetTransform()); 
	    if (psDeviceFlags.is(rsEnvironment)) g_pGamePersistent->Environment.RenderLast	();
    break;
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
	return Scene.IsUnsaved();
}
//---------------------------------------------------------------------------

#include "EditMesh.h"
bool TUI_Tools::RayPick(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n)
{
    if (Scene.ObjCount()&&(UI.GetEState()==esEditScene)){
        SRayPickInfo pinf;
        pinf.inf.range	= dist;
        if (Scene.RayPickObject(dist, start,dir,OBJCLASS_SCENEOBJECT,&pinf,0)){ 
        	dist		= pinf.inf.range;
        	if (pt) 	pt->set(pinf.pt); 
            if (n){	
                const Fvector* PT[3];
                pinf.e_mesh->GetFacePT(pinf.inf.id, PT);
            	n->mknormal(*PT[0],*PT[1],*PT[2]);
            }
            return true;
        }
    }
    Fvector N={0.f,-1.f,0.f};
    Fvector P={0.f,0.f,0.f};
    Fplane PL; PL.build(P,N);
    float d;
    if (PL.intersectRayDist(start,dir,d)&&(d<=dist)){
        dist = d;
        if (pt) pt->mad(start,dir,dist); 
        if (n)	n->set(N);
        return true;
    }else return false;
}

