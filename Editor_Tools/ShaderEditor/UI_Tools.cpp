//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ShaderTools.h"
#include "EditObject.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "PropertiesShader.h"

//------------------------------------------------------------------------------
CShaderTools SHTools;
//------------------------------------------------------------------------------

CShaderTools::CShaderTools()
{
	m_LibObject 		= 0;
	m_EditObject 		= 0;
}
//---------------------------------------------------------------------------

CShaderTools::~CShaderTools()
{
}
//---------------------------------------------------------------------------

void CShaderTools::OnChangeEditor()
{
    TfrmShaderProperties::InitProperties();
}
//---------------------------------------------------------------------------

EActiveEditor CShaderTools::ActiveEditor()
{
	if (fraLeftBar->pcShaders->ActivePage==fraLeftBar->tsEngine) return aeEngine;
	else if (fraLeftBar->pcShaders->ActivePage==fraLeftBar->tsCompiler)	return aeCompiler;
    return -1;
}
//---------------------------------------------------------------------------

void CShaderTools::Modified(){
    switch (ActiveEditor()){
    case aeEngine: 		Engine.Modified(); 		break;
    case aeCompiler: 	Compiler.Modified(); 	break;
    }
}
//---------------------------------------------------------------------------

void CShaderTools::OnCreate(){
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);
	Engine.OnCreate();
    Compiler.OnCreate();
}

void CShaderTools::OnDestroy(){
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
	Engine.OnDestroy();
    Compiler.OnDestroy();
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
}

void CShaderTools::SelectPreviewObject(int p){
    LPCSTR fn;
    switch(p){
        case 0: fn="$ShaderTest_Plane"; 	break;
        case 1: fn="$ShaderTest_Box"; 		break;
        case 2: fn="$ShaderTest_Sphere"; 	break;
        case 3: fn="$ShaderTest_Teapot";	break;
        case -1: fn=m_EditObject?m_EditObject->GetName():""; fn=TfrmChoseItem::SelectObject(false,true,0,fn); if (!fn) return; break;
        default: THROW2("Failed select test object.");
    }
    m_LibObject = Lib->SearchObject(fn);
    m_EditObject = 0;
    if (m_LibObject) m_EditObject = m_LibObject->GetReference();
    if (!m_LibObject||!m_EditObject)
        ELog.DlgMsg(mtError,"System object '%s.object' can't find in object library. Preview disabled.",fn);
	ZoomObject();
    UI.RedrawScene();
}

void CShaderTools::ResetPreviewObject(){
    m_LibObject 	= 0;
    m_EditObject 	= 0;
    UI.RedrawScene();
}


