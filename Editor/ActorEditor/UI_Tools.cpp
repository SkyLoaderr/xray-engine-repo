//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "topbar.h"
#include "xr_trims.h"
#include "PropertiesList.h"
#include "motion.h"
#include "bone.h"
#include "xr_tokens.h"

//------------------------------------------------------------------------------
CActorTools Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}
#define MOTION_SECTION "Motion"

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

	_DELETE(m_EditObject);
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
}
//---------------------------------------------------------------------------

bool CActorTools::IfModified(){
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The '%s' has been modified.\nDo you want to save your changes?",UI.GetEditFileName());
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
    	m_EditObject->RenderSkeletonSingle(precalc_identity);
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

    if (m_EditObject) m_EditObject->OnDeviceCreate();
}

void CActorTools::OnDeviceDestroy(){
    if (m_EditObject) m_EditObject->OnDeviceDestroy();
}

void CActorTools::Clear(){
	VERIFY(m_bReady);

//S    _DELETE();

    UI.RedrawScene();
}

bool CActorTools::Load(LPCSTR name)
{
	VERIFY(m_bReady);
	CEditableObject* O = new CEditableObject(name);
	if (O->Load(name)&&O->IsDynamic()){
    	_DELETE(m_EditObject);
        m_EditObject = O;
        return true;
    }
    _DELETE(O);

    return false;
}

bool CActorTools::Save(LPCSTR name)
{
	VERIFY(m_bReady);
    ApplyChanges();
    if (m_EditObject){
    	m_EditObject->SaveObject(name);
		m_bModified = false;
        return true;
    }
	return false;
}

bool CActorTools::Export(LPCSTR name)
{
	VERIFY(m_bReady);
    if (m_EditObject&&m_EditObject->ExportSkeletonOGF(name)) return true;
    return false;
}

void CActorTools::Reload()
{
	VERIFY(m_bReady);
    // visual part
}

bool CActorTools::AppendMotion(LPCSTR name, LPCSTR fn)
{
	VERIFY(m_EditObject);
    if (m_EditObject->AppendSMotion(name,fn)){
		Modified();
		return true;
    }
	return false;
}

bool CActorTools::RemoveMotion(LPCSTR name)
{
	VERIFY(m_EditObject);
    if (m_EditObject->RemoveSMotion(name)){
		Modified();
		return true;
    }
	return false;
}

bool CActorTools::LoadMotions(LPCSTR name)
{
	VERIFY(m_EditObject);
    if (m_EditObject->LoadSMotions(name)){
		Modified();
		return true;
    }
	return false;
}

bool CActorTools::SaveMotions(LPCSTR name)
{
	VERIFY(m_EditObject);
    return (m_EditObject->SaveSMotions(name));
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
	if (m_EditObject)
    	m_EditObject->SetActiveSMotion(m_EditObject->FindSMotionByName(name));
}

void CActorTools::FillBaseProperties()
{
	R_ASSERT(m_EditObject);
    m_Props->BeginFillMode();
    TElTreeItem* M=0;
    TElTreeItem* N=0;
    M = m_Props->AddItem(0,PROP_MARKER,		"Surfaces");
    {
        for (SurfaceIt& it=m_EditObject->FirstSurface(); it!=m_EditObject->LastSurface(); it++){
            CSurface* S=*it;
            N = m_Props->AddItem(M,PROP_MARKER,	S->_Name());
            {
                m_Props->AddItem(N,PROP_S_SH_ENGINE,	"Shader",	&S->m_ShaderName);
                m_Props->AddItem(N,PROP_S_SH_COMPILE,	"Compile",	&S->m_ShaderXRLCName);
                m_Props->AddItem(N,PROP_S_TEXTURE,		"Texture",	&S->m_Texture);
            }
        }
    }
    M = m_Props->AddItem(0,PROP_MARKER,	"Geometry");
    {
        m_Props->AddItem(M,PROP_MARKER2,"Vertex Count",	AnsiString(m_EditObject->GetVertexCount()).c_str());
        m_Props->AddItem(M,PROP_MARKER2,"Face Count",	AnsiString(m_EditObject->GetFaceCount()).c_str());
        for (EditMeshIt& it=m_EditObject->FirstMesh(); it!=m_EditObject->LastMesh(); it++){
            CEditableMesh* S=*it;
            N = m_Props->AddItem(M,PROP_MARKER,	S->GetName());
            {
                m_Props->AddItem(N,PROP_MARKER2,"Vertex Count",	AnsiString(S->GetVertexCount()).c_str());
                m_Props->AddItem(N,PROP_MARKER2,"Face Count",	AnsiString(S->GetFaceCount()).c_str());
            }
        }
    }
	m_Props->EndFillMode(false);
}

xr_token					tfx_token		[ ]={
	{ "Cycle",				0				},
	{ "FX",					1				},
	{ 0,					0				}
};

void CActorTools::OnMotionTypeChange	(LPVOID data)
{
	TokenValue* V = (TokenValue*)data;
    if (0==*V->val){
	    m_pCycleNode->Hidden	= false;
    	m_pFXNode->Hidden		= true;
    }else{
	    m_pCycleNode->Hidden	= true;
    	m_pFXNode->Hidden		= false;
    }
}

void CActorTools::FillMotionProperties()
{
	R_ASSERT(m_EditObject);
	CSMotion* SM = m_EditObject->GetActiveSMotion();
    m_Props->BeginFillMode("",MOTION_SECTION);
    if (SM){
        TElTreeItem* M=0;
        M = m_Props->AddItem(0,PROP_MARKER,MOTION_SECTION);
        {
			m_Props->AddItem(M,PROP_MARKER2, 	"Name", 	(LPVOID)SM->Name());
            m_Props->AddItem(M,PROP_FLOAT, 		"Speed", 	m_Props->MakeFloatValue(&SM->fSpeed,	0.f,20.f,0.01f,2));
            m_Props->AddItem(M,PROP_FLOAT, 		"Accrue", 	m_Props->MakeFloatValue(&SM->fAccrue,	0.f,20.f,0.01f,2));
            m_Props->AddItem(M,PROP_FLOAT, 		"Falloff", 	m_Props->MakeFloatValue(&SM->fFalloff,	0.f,20.f,0.01f,2));
            TokenValue* TV = m_Props->MakeTokenValue(&SM->bFX,tfx_token,OnMotionTypeChange);
            m_Props->AddItem(M,PROP_TOKEN, 		"Type", 	TV);
			m_pCycleNode = m_Props->AddItem(M,PROP_MARKER, "Cycle");
            {
            	AStringVec lst;
                lst.push_back("--none--");
                for (BPIt it=m_EditObject->FirstBonePart(); it!=m_EditObject->LastBonePart(); it++) lst.push_back(it->alias);
            	m_Props->AddItem(m_pCycleNode,PROP_LIST, 	"Bone part",m_Props->MakeListValue(&SM->cBonePart,&lst));
	            m_Props->AddItem(m_pCycleNode,PROP_BOOL,	"Stop at end",&SM->bStopAtEnd);
            }
			m_pFXNode = m_Props->AddItem(M,PROP_MARKER, "FX");
            {
            	AStringVec lst;
                for (BoneIt it=m_EditObject->FirstBone(); it!=m_EditObject->LastBone(); it++) lst.push_back((*it)->Name());
            	m_Props->AddItem(m_pFXNode,PROP_LIST, 	"Start bone",m_Props->MakeListValue(&SM->cStartBone,&lst));
	            m_Props->AddItem(m_pFXNode,PROP_FLOAT, 	"Power", 	m_Props->MakeFloatValue(&SM->fPower,	0.f,20.f,0.01f,2));
            }
            OnMotionTypeChange(TV);
        }
        M->Expand(true);
    }else{
		m_pCycleNode=0;
        m_pFXNode	=0;
    }
	m_Props->EndFillMode(false);
}

void CActorTools::PlayMotion()
{
	if (m_EditObject) m_EditObject->SkeletonPlay();
}

void CActorTools::StopMotion()
{
	if (m_EditObject) m_EditObject->SkeletonStop();
}

void CActorTools::PauseMotion()
{
	if (m_EditObject) m_EditObject->SkeletonPause();
}

