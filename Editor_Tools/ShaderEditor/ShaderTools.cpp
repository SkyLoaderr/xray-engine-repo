//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ShaderTools.h"
#include "EditObject.h"
#include "bottombar.h"
#include "ui_main.h"

CShaderTools::CShaderTools(){
	m_EditObject = 0;
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);
}

CShaderTools::~CShaderTools(){
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
	Clear();
}
//---------------------------------------------------------------------------

void CShaderTools::Init(){
}

void CShaderTools::Clear(){
	_DELETE(m_EditObject);
    m_Modified = FALSE;
}

bool CShaderTools::IfModified(){
    if (m_Modified && UI->GetEditFileName()[0]){
        int mr = ELog.DlgMsg(mtConfirmation, "The object has been modified. Do you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI->Command(COMMAND_SAVE)) return false; else m_Modified = false; break;
        case mrNo: m_Modified = false; break;
        case mrCancel: return false;
        }
    }
    return true;
}

bool CShaderTools::Load(LPCSTR name){
	Clear();
    m_EditObject = new CEditableObject(0);
	if (!m_EditObject->LoadObject(name)){
    	ELog.DlgMsg(mtError, "Can't load object.");
        _DELETE(m_EditObject);
        return false;
    }
    ZoomObject();
    return true;
}

bool CShaderTools::Save(LPCSTR name){
	if (m_EditObject){
    }
    m_Modified = FALSE;
    return true;
}

void CShaderTools::Render(){
	if (m_EditObject)
    	m_EditObject->RenderSingle(precalc_identity);
}

void CShaderTools::Update(){
	if (m_EditObject)
    	m_EditObject->RTL_Update(Device.fTimeDelta);
}

void CShaderTools::ZoomObject(){
	if (m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
    }else{
		Fbox BB;
        BB.set(-5,-5,-5,5,5,5);
        Device.m_Camera.ZoomExtents(BB);
    }
}

void CShaderTools::OnDeviceCreate(){
	if (m_EditObject) m_EditObject->OnDeviceCreate();
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

void CShaderTools::OnDeviceDestroy(){
	if (m_EditObject) m_EditObject->OnDeviceDestroy();
}

