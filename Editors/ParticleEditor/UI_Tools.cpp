//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "topbar.h"
#include "PSObject.h"
#include "PSLibrary.h"
#include "ParticleSystem.h"
#include "PropertiesPSDef.h"
#include "xr_trims.h"
#include "library.h"

//------------------------------------------------------------------------------
CParticleTools Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

CParticleTools::CParticleTools()
{
	m_EditObject		= 0;
	m_TestObject 		= 0;
    m_LibPS				= 0;
    m_PSProps			= 0;
    m_bModified			= false;
    m_bReady			= false;
}
//---------------------------------------------------------------------------

CParticleTools::~CParticleTools()
{
}
//---------------------------------------------------------------------------

bool CParticleTools::OnCreate(){
	// shader test locking
	AnsiString fn = "particles.xr"; Engine.FS.m_GameRoot.Update(fn);
	if (Engine.FS.CheckLocking(0,fn.c_str(),false,true))
    	return false;

    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);
    m_PSProps = new TfrmPropertiesPSDef(fraLeftBar->paPSProps);
	m_PSProps->Parent = fraLeftBar->paPSProps;
    m_PSProps->Align = alClient;
    m_PSProps->BorderStyle = bsNone;
    m_PSProps->ShowProperties();

    m_bReady = true;

    Load();
	m_PSProps->SetCurrent(0);
    ChangeAction(eaSelect);

	// lock
    Engine.FS.LockFile(0,fn.c_str());
    return true;
}

void CParticleTools::OnDestroy(){
	VERIFY(m_bReady);
    m_bReady			= false;
	// unlock
    Engine.FS.UnlockFile(&Engine.FS.m_GameRoot,"particles.xr");

	Lib.RemoveEditObject(m_EditObject);
	_DELETE(m_TestObject);
    m_LibPS				= 0;
    m_PSProps->HideProperties();
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
}
//---------------------------------------------------------------------------

bool CParticleTools::IfModified(){
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The particles has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CParticleTools::Modified(){
	m_bModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CParticleTools::Render(){
	if (!m_bReady) return;
    if (m_EditObject)	m_EditObject->RenderSingle(Fidentity);
	if (m_TestObject)	m_TestObject->RenderSingle();
}

void CParticleTools::Update(){
	if (!m_bReady) return;
	if (m_TestObject){
    	m_TestObject->OnFrame();
        if (m_TestObject->IsPlaying())	fraLeftBar->lbCurState->Caption = "generate&&playing";
        else 							fraLeftBar->lbCurState->Caption = m_TestObject->ParticleCount()?"playing":"stopped";
        fraLeftBar->lbParticleCount->Caption 	= m_TestObject->ParticleCount();
    }
	if (m_EditObject)
    	m_EditObject->OnFrame();
}

void CParticleTools::ZoomObject(BOOL bObjectOnly){
	VERIFY(m_bReady);
    if (bObjectOnly&&m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
	}else{
    	if (m_TestObject){
			Fbox BB;
	        m_TestObject->GetBox(BB);
    	    Device.m_Camera.ZoomExtents(BB);
        }else{
			Fbox BB;
    	    BB.set(-5,-5,-5,5,5,5);
        	Device.m_Camera.ZoomExtents(BB);
        }
    }
}

void CParticleTools::OnDeviceCreate(){
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
	if (m_TestObject) m_TestObject->OnDeviceCreate();
}

void CParticleTools::OnDeviceDestroy(){
	if (m_TestObject) m_TestObject->OnDeviceDestroy();
}

void CParticleTools::SelectPreviewObject(int p){
    LPCSTR fn=m_EditObject?m_EditObject->GetName():"";
    fn=TfrmChoseItem::SelectObject(false,0,fn);
    if (!fn) return;
    Lib.RemoveEditObject(m_EditObject);
    m_EditObject = Lib.CreateEditObject(fn);
    if (!m_EditObject)
        ELog.DlgMsg(mtError,"Object '%s' can't find in object library.",fn);
	ZoomObject(TRUE);
    UI.RedrawScene();
}

void CParticleTools::ResetPreviewObject(){
	VERIFY(m_bReady);
    m_LibPS 	= 0;
    UI.RedrawScene();
}

void CParticleTools::Load()
{
	VERIFY(m_bReady);
    PS::PSIt P = PSLib.FirstPS();
    PS::PSIt E = PSLib.LastPS();
    for (; P!=E; P++) fraLeftBar->AddPS(P->m_Name,true);
}

void CParticleTools::Save()
{
	VERIFY(m_bReady);
    ApplyChanges();
	m_bModified = false;
	// backup
    Engine.FS.BackupFile(&Engine.FS.m_GameRoot,"particles.xr");
	// save   
    Engine.FS.UnlockFile(&Engine.FS.m_GameRoot,"particles.xr",false);
	PSLib.Save();
    Engine.FS.LockFile(&Engine.FS.m_GameRoot,"particles.xr",false);
}

void CParticleTools::Reload()
{
	VERIFY(m_bReady);
	PSLib.Reload();
    // visual part
    fraLeftBar->ClearParticleList();
    PS::PSIt P = PSLib.FirstPS();
    PS::PSIt E = PSLib.LastPS();
    for (; P!=E; P++) fraLeftBar->AddPS(P->m_Name,true);
    ResetCurrentPS();
}


void CParticleTools::RenamePS(LPCSTR old_full_name, LPCSTR ren_part, int level){
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    RenamePS(old_full_name, new_full_name);
}

void CParticleTools::RenamePS(LPCSTR old_full_name, LPCSTR new_full_name){
	VERIFY(m_bReady);
    ApplyChanges();
	PS::SDef* S = PSLib.FindPS(old_full_name); R_ASSERT(S);
    PSLib.RenamePS(S,new_full_name);
	if (S==m_LibPS){
    	m_EditPS = *S;
    }
}

PS::SDef* CParticleTools::AppendPS(LPCSTR folder_name, PS::SDef* src)
{
	VERIFY(m_bReady);
    char old_name[128]; if (src) strcpy(old_name,src->m_Name);
    PS::SDef* S = PSLib.AppendPS(src);
    char new_name[128]; new_name[0]=0;
    if (folder_name) strcpy(new_name,folder_name);
    PSLib.GenerateName(new_name,src?old_name:0);
    PSLib.RenamePS(S,new_name);
	fraLeftBar->AddPS(S->m_Name,false);
    return S;
}

void CParticleTools::RemovePS(LPCSTR name)
{
	VERIFY(m_bReady);
	PSLib.RemovePS(name);
}

void CParticleTools::ResetCurrentPS()
{
	VERIFY(m_bReady);
	m_LibPS = 0;
	_DELETE(m_TestObject);
}

void CParticleTools::SetCurrentPS(PS::SDef* P)
{
	VERIFY(m_bReady);
    // save changes
    if (m_LibPS)
    	*m_LibPS = m_EditPS;
    // load shader
	if (m_LibPS!=P){
        m_LibPS = P;
        if (m_LibPS) m_EditPS = *m_LibPS;
        m_PSProps->SetCurrent(m_LibPS?&m_EditPS:0);
        // update visual
        _DELETE(m_TestObject);
		m_TestObject = new CPSObject(0,"Test");
		m_TestObject->Compile(&m_EditPS);
    }
}

void CParticleTools::SetCurrentPS(LPCSTR name)
{
    SetCurrentPS(PSLib.FindPS(name));
}

void CParticleTools::UpdateCurrent(){
	VERIFY(m_bReady);
    VERIFY(m_LibPS);
	_DELETE(m_TestObject);
    m_TestObject = new CPSObject(0,"Test");
    m_TestObject->Compile(&m_EditPS);
}
//---------------------------------------------------------------------------

void CParticleTools::UpdateEmitter(){
	VERIFY(m_bReady);
    if (!m_LibPS) return;
    if (m_TestObject) m_TestObject->UpdateEmitter(&m_EditPS.m_DefaultEmitter);
}
//---------------------------------------------------------------------------

PS::SDef* CParticleTools::ClonePS(LPCSTR name)
{
	VERIFY(m_bReady);
	PS::SDef* S = PSLib.FindPS(name); R_ASSERT(S);
	return AppendPS(0,S);
}

void CParticleTools::ApplyChanges(){
	VERIFY(m_bReady);
    if (m_LibPS){
		*m_LibPS = m_EditPS;
		Modified();
    }
}

void CParticleTools::PlayCurrentPS()
{
	VERIFY(m_bReady);
	if (m_TestObject) m_TestObject->Play();
}

void CParticleTools::StopCurrentPS()
{
	VERIFY(m_bReady);
	if (m_TestObject) m_TestObject->Stop();
}

void CParticleTools::OnShowHint(AStringVec& SS)
{
}

void CParticleTools::ChangeAction(EAction action)
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

bool __fastcall CParticleTools::MouseStart(TShiftState Shift)
{
	if (!m_TestObject) return false;
	switch(m_Action){
    case eaSelect: return false;
    case eaAdd:{
        if (m_EditObject){
            Fvector p;
            float dist=UI.ZFar();
            SRayPickInfo pinf;
            if (m_EditObject->RayPick(dist,UI.m_CurrentRStart,UI.m_CurrentRNorm,Fidentity,&pinf)){
                R_ASSERT(pinf.e_mesh);
                m_EditPS.m_DefaultEmitter.m_Position.set(pinf.pt);
            }
        }
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
        m_RotateCenter.set(0,0,0);
        m_RotateVector.set(0,0,0);
        if (fraTopBar->ebAxisX->Down) m_RotateVector.set(0,1,0);
        else if (fraTopBar->ebAxisY->Down) m_RotateVector.set(1,0,0);
        else if (fraTopBar->ebAxisZ->Down) m_RotateVector.set(0,0,1);
        m_fRotateSnapAngle = 0;
    }break;
    case eaScale:{
		m_ScaleCenter.set(0,0,0);
    }break;
    }
    UpdateEmitter();
    m_PSProps->fraEmitter->GetInfoFirst(m_EditPS.m_DefaultEmitter);
	return m_bHiddenMode;
}

bool __fastcall CParticleTools::MouseEnd(TShiftState Shift)
{
	return true;
}

void __fastcall CParticleTools::MouseMove(TShiftState Shift)
{
	if (!m_TestObject) return;
	switch(m_Action){
    case eaSelect: return;
    case eaAdd: break;
    case eaMove:{
    	Fvector amount;
        amount.mul( m_MovingXVector, UI.m_MouseSM * UI.m_DeltaCpH.x );
        amount.mad( amount, m_MovingYVector, -UI.m_MouseSM * UI.m_DeltaCpH.y );

        if( fraTopBar->ebMSnap->Down ){
        	CHECK_SNAP(m_MovingReminder.x,amount.x,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.y,amount.y,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.z,amount.z,UI.movesnap());
        }

        if (!fraTopBar->ebAxisX->Down&&!fraTopBar->ebAxisZX->Down) amount.x = 0.f;
        if (!fraTopBar->ebAxisZ->Down&&!fraTopBar->ebAxisZX->Down) amount.z = 0.f;
        if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
		m_EditPS.m_DefaultEmitter.m_Position.add(amount);
    }break;
    case eaRotate:{
        float amount = -UI.m_DeltaCpH.x * UI.m_MouseSR;
        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI.anglesnap());
        switch (m_EditPS.m_DefaultEmitter.m_EmitterType){
        case PS::SEmitterDef::emPoint:
        case PS::SEmitterDef::emBox:
        case PS::SEmitterDef::emSphere: 	break;
        case PS::SEmitterDef::emCone:
        	m_EditPS.m_DefaultEmitter.m_ConeHPB.mad(m_TestObject->m_Emitter.m_ConeHPB,m_RotateVector,amount);
            m_EditPS.m_DefaultEmitter.UpdateConeOrientation();
        break;
        }
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

        switch (m_EditPS.m_DefaultEmitter.m_EmitterType){
        case PS::SEmitterDef::emCone:
        case PS::SEmitterDef::emPoint:		break;
        case PS::SEmitterDef::emBox:
	        m_EditPS.m_DefaultEmitter.m_BoxSize.add(amount);
        break;
        case PS::SEmitterDef::emSphere:
	        m_EditPS.m_DefaultEmitter.m_SphereRadius += dy;
        break;
        }
    }break;
    }
    UpdateEmitter();
    m_PSProps->fraEmitter->GetInfoFirst(m_EditPS.m_DefaultEmitter);
}

