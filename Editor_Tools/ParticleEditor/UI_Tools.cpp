//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "PSObject.h"
#include "PSLibrary.h"
#include "PropertiesPSDef.h"
#include "xr_trims.h"

//------------------------------------------------------------------------------
CParticleTools Tools;
//------------------------------------------------------------------------------

CParticleTools::CParticleTools()
{
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

void CParticleTools::OnCreate(){
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
}

void CParticleTools::OnDestroy(){
	VERIFY(m_bReady);
    m_bReady			= false;
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
	VERIFY(m_bReady);
	if (m_TestObject)
    	m_TestObject->RenderSingle();
}

void CParticleTools::Update(){
	VERIFY(m_bReady);
	if (m_TestObject){
    	m_TestObject->RTL_Update(Device.fTimeDelta);
        if (m_TestObject->IsPlaying())	fraLeftBar->lbCurState->Caption = "generate&&playing";
        else 							fraLeftBar->lbCurState->Caption = m_TestObject->ParticleCount()?"playing":"stopped";
        fraLeftBar->lbParticleCount->Caption 	= m_TestObject->ParticleCount();
    }
}

void CParticleTools::ZoomObject(){
	VERIFY(m_bReady);
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
/*    LPCSTR fn;
    switch(p){
        case 0: fn="editor\\ShaderTest_Plane"; 	break;
        case 1: fn="editor\\ShaderTest_Box"; 	break;
        case 2: fn="editor\\ShaderTest_Sphere"; break;
        case 3: fn="editor\\ShaderTest_Teapot";	break;
        case -1: fn=m_EditObject?m_EditObject->GetName():""; fn=TfrmChoseItem::SelectObject(false,true,0,fn); if (!fn) return; break;
        default: THROW2("Failed select test object.");
    }
    m_EditObject = Lib.GetEditObject(fn);
    if (!m_EditObject)
        ELog.DlgMsg(mtError,"Object '%s.object' can't find in object library. Preview disabled.",fn);
	ZoomObject();
    UI.RedrawScene();
*/
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
	PSLib.Save();
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
		m_TestObject = new CPSObject("Test");
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
    m_TestObject = new CPSObject("Test");
    m_TestObject->Compile(&m_EditPS);
}
//---------------------------------------------------------------------------

void CParticleTools::UpdateEmitter(){
	VERIFY(m_bReady);
    VERIFY(m_LibPS);
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

