//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "topbar.h"
#include "xr_trims.h"
#include "library.h"
#include "PropertiesList.h"

//------------------------------------------------------------------------------
CActorTools Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

CActorTools::CActorTools()
{
	m_EditObject		= 0;
    m_bModified			= false;
    m_bReady			= false;
}
//---------------------------------------------------------------------------

CActorTools::~CActorTools()
{
}
//---------------------------------------------------------------------------

bool CActorTools::OnCreate(){
	// shader test locking
//	AnsiString fn = "particles.xr"; FS.m_GameRoot.Update(fn);
//	if (FS.CheckLocking(0,fn.c_str(),false,true))
//    	return false;

    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);

    // props
    m_Props = TfrmProperties::CreateProperties(fraLeftBar->paPSProps,alClient);

    m_bReady = true;

    Load();
//	m_PSProps->SetCurrent(0);
    ChangeAction(eaSelect);

	// lock
//    FS.LockFile(0,fn.c_str());
    return true;
}

void CActorTools::OnDestroy(){
	VERIFY(m_bReady);
    m_bReady			= false;
	// unlock
//    FS.UnlockFile(&FS.m_GameRoot,"particles.xr");
	TfrmProperties::DestroyProperties(m_Props);

	Lib.RemoveEditObject(m_EditObject);
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
}
//---------------------------------------------------------------------------

bool CActorTools::IfModified(){
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The ... has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CActorTools::Modified(){
	m_bModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CActorTools::Render(){
	if (!m_bReady) return;
	if (m_EditObject)
    	m_EditObject->RenderSingle(precalc_identity);
}

void CActorTools::Update(){
	if (!m_bReady) return;
	if (m_EditObject)
    	m_EditObject->RTL_Update(Device.fTimeDelta);
}

void CActorTools::ZoomObject(){
	VERIFY(m_bReady);
    if (m_EditObject)
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
}

void CActorTools::OnDeviceCreate(){
	VERIFY(m_bReady);
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	Device.SetLight(0,L);
	Device.LightEnable(0,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,-1); L.direction.normalize();
	Device.SetLight(1,L);
	Device.LightEnable(1,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	Device.SetLight(2,L);
	Device.LightEnable(2,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,1); L.direction.normalize();
	Device.SetLight(3,L);
	Device.LightEnable(3,true);

	L.diffuse.set(1.0,0.8,0.7,1);
    L.direction.set(0,1,0); L.direction.normalize();
	Device.SetLight(4,L);
	Device.LightEnable(4,true);
}

void CActorTools::OnDeviceDestroy(){
}

void CActorTools::Clear(){
	VERIFY(m_bReady);

//S    _DELETE();

    UI.RedrawScene();
}

void CActorTools::Load(LPCSTR name)
{
	VERIFY(m_bReady);
}

void CActorTools::Save()
{
	VERIFY(m_bReady);
    ApplyChanges();
	m_bModified = false;
}

void CActorTools::Reload()
{
	VERIFY(m_bReady);
    // visual part
}

void CActorTools::ApplyChanges()
{
	VERIFY(m_bReady);
}

void CActorTools::OnShowHint(AStringVec& SS)
{
}

void CActorTools::ChangeAction(EAction action)
{
	switch(action){
    case eaSelect: m_bHiddenMode=false; break;
    case eaAdd:
    case eaMove:
    case eaRotate:
    case eaScale:  m_bHiddenMode=true; break;
    }
    m_Action = action;
    switch(m_Action){
        case eaSelect:  UI.GetD3DWindow()->Cursor = crCross;     break;
        case eaAdd:     UI.GetD3DWindow()->Cursor = crArrow;     break;
        case eaMove:    UI.GetD3DWindow()->Cursor = crSizeAll;   break;
        case eaRotate:  UI.GetD3DWindow()->Cursor = crSizeWE;    break;
        case eaScale:   UI.GetD3DWindow()->Cursor = crVSplit;    break;
        default:        UI.GetD3DWindow()->Cursor = crHelp;
    }
    UI.RedrawScene();
    fraTopBar->ChangeAction(m_Action);
}

bool __fastcall CActorTools::MouseStart(TShiftState Shift)
{
	switch(m_Action){
    case eaSelect: return false;
    case eaAdd:{
        if (m_EditObject){
/*            Fvector p;
            float dist=UI.ZFar();
            SRayPickInfo pinf;
            if (m_EditObject->RayPick(dist,UI.m_CurrentRStart,UI.m_CurrentRNorm,precalc_identity,&pinf)){
                R_ASSERT(pinf.e_mesh);
                m_EditPS.m_DefaultEmitter.m_Position.set(pinf.pt);
            }
*/        }
    }break;
    case eaMove:{
        if (fraTopBar->ebAxisY->Down){
            m_MovingXVector.set(0,0,0);
            m_MovingYVector.set(0,1,0);
        }else{
            m_MovingXVector.set( Device.m_Camera.GetRight() );
            m_MovingXVector.y = 0;
            m_MovingYVector.set( Device.m_Camera.GetDirection() );
            m_MovingYVector.y = 0;
            m_MovingXVector.normalize_safe();
            m_MovingYVector.normalize_safe();
        }
        m_MovingReminder.set(0,0,0);
    }break;
    case eaRotate:{
        m_RotateCenter.set( UI.pivot() );
        m_RotateVector.set(0,0,0);
        if (fraTopBar->ebAxisX->Down) m_RotateVector.set(0,1,0);
        else if (fraTopBar->ebAxisY->Down) m_RotateVector.set(1,0,0);
        else if (fraTopBar->ebAxisZ->Down) m_RotateVector.set(0,0,1);
        m_fRotateSnapAngle = 0;
    }break;
    case eaScale:{
		m_ScaleCenter.set( UI.pivot() );
    }break;
    }
//    UpdateEmitter();
//    m_PSProps->fraEmitter->GetInfoFirst(m_EditPS.m_DefaultEmitter);
	return m_bHiddenMode;
}

bool __fastcall CActorTools::MouseEnd(TShiftState Shift)
{
	return true;
}

void __fastcall CActorTools::MouseMove(TShiftState Shift)
{
	switch(m_Action){
    case eaSelect: return;
    case eaAdd: break;
    case eaMove:{
    	Fvector amount;
        amount.mul( m_MovingXVector, UI.m_MouseSM * UI.m_DeltaCpH.x );
        amount.direct( amount, m_MovingYVector, -UI.m_MouseSM * UI.m_DeltaCpH.y );

        if( fraTopBar->ebMSnap->Down ){
        	CHECK_SNAP(m_MovingReminder.x,amount.x,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.y,amount.y,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.z,amount.z,UI.movesnap());
        }

        if (!fraTopBar->ebAxisX->Down&&!fraTopBar->ebAxisZX->Down) amount.x = 0.f;
        if (!fraTopBar->ebAxisZ->Down&&!fraTopBar->ebAxisZX->Down) amount.z = 0.f;
        if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
//		amount
    }break;
    case eaRotate:{
        float amount = -UI.m_DeltaCpH.x * UI.m_MouseSR;
        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI.anglesnap());
		//m_RotateVector, amount
    }break;
    case eaScale:{
        float dy = UI.m_DeltaCpH.x * UI.m_MouseSS;
        if (dy>1.f) dy=1.f; else if (dy<-1.f) dy=-1.f;

        Fvector amount;
        amount.set( dy, dy, dy );

        if (fraTopBar->ebNonUniformScale->Down){
            if (!fraTopBar->ebAxisX->Down && !fraTopBar->ebAxisZX->Down) amount.x = 0.f;
            if (!fraTopBar->ebAxisZ->Down && !fraTopBar->ebAxisZX->Down) amount.z = 0.f;
            if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
        }

        //amount
    }break;
    }
}

void CActorTools::SetCurrentMotion(LPCSTR name)
{
	//
}

