//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "PropertiesList.h"
#include "Blender.h"

//------------------------------------------------------------------------------
CShaderTools Tools;
//------------------------------------------------------------------------------

CShaderTools::CShaderTools()
{
	m_EditObject 		= 0;
    m_bCustomEditObject	= false;
    m_Props				= 0;
}
//---------------------------------------------------------------------------

CShaderTools::~CShaderTools()
{
}
//---------------------------------------------------------------------------

void CShaderTools::OnChangeEditor()
{
	switch (ActiveEditor()){
    case aeEngine: 		SEngine.UpdateProperties(); 	break;
    case aeCompiler: 	SCompiler.UpdateProperties(); 	break;
    };
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
    case aeEngine: 		SEngine.Modified(); 	break;
    case aeCompiler: 	SCompiler.Modified(); 	break;
    }
}
//---------------------------------------------------------------------------

bool CShaderTools::OnCreate(){
    // create props
    m_Props = TProperties::CreateForm(fraLeftBar->paShaderProps,alClient);
	// shader test locking
	AnsiString sh_fn = "shaders.xr"; Engine.FS.m_GameRoot.Update(sh_fn);
	if (Engine.FS.CheckLocking(0,sh_fn.c_str(),false,true))
    	return false;
	// shader test locking
	AnsiString lc_fn = "shaders_xrlc.xr"; Engine.FS.m_GameRoot.Update(lc_fn);
	if (Engine.FS.CheckLocking(0,lc_fn.c_str(),false,true))
    	return false;
	//
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);
	SEngine.OnCreate();
    SCompiler.OnCreate();
	// lock
    Engine.FS.LockFile(0,sh_fn.c_str());
    Engine.FS.LockFile(0,lc_fn.c_str());
    return true;
}

void CShaderTools::OnDestroy(){
	// unlock
    Engine.FS.UnlockFile(&Engine.FS.m_GameRoot,"shaders.xr");
    Engine.FS.UnlockFile(&Engine.FS.m_GameRoot,"shaders_xrlc.xr");
	//
    Lib.RemoveEditObject(m_EditObject);
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
	SEngine.OnDestroy();
    SCompiler.OnDestroy();
	// destroy props
	TProperties::DestroyForm(m_Props);
}

void CShaderTools::Render(){
	if (m_EditObject) m_EditObject->RenderSingle(Fidentity);
}

void CShaderTools::Update(){
	switch (ActiveEditor()){
	case aeEngine:
    	if (m_EditObject) m_EditObject->OnFrame();
	    SEngine.Update();
    break;
	case aeCompiler:
    	SCompiler.Update();
    break;
    };
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

    SEngine.ResetShaders();
}

void CShaderTools::OnDeviceDestroy(){
}

void CShaderTools::SelectPreviewObject(int p){
    LPCSTR fn;
    m_bCustomEditObject	= false;
    switch(p){
        case 0: fn="editor\\ShaderTest_Plane"; 	break;
        case 1: fn="editor\\ShaderTest_Box"; 	break;
        case 2: fn="editor\\ShaderTest_Sphere"; break;
        case 3: fn="editor\\ShaderTest_Teapot";	break;
        case -1: fn=m_EditObject?m_EditObject->GetName():""; fn=TfrmChoseItem::SelectObject(false,0,fn); if (!fn) return; m_bCustomEditObject = true; break;
        default: THROW2("Failed select test object.");
    }
    Lib.RemoveEditObject(m_EditObject);
    m_EditObject = Lib.CreateEditObject(fn);
    if (!m_EditObject)
        ELog.DlgMsg(mtError,"Object '%s.object' can't find in object library. Preview disabled.",fn);
	ZoomObject();
    UpdateObjectShader();
    UI.RedrawScene();
}

void CShaderTools::ResetPreviewObject(){
    m_EditObject 	= 0;
    UI.RedrawScene();
}

void CShaderTools::OnShowHint(AStringVec& ss){
	if (m_EditObject){
	    Fvector p;
        float dist=UI.ZFar();
        SRayPickInfo pinf;
    	if (m_EditObject->RayPick(dist,UI.m_CurrentRStart,UI.m_CurrentRNorm,Fidentity,&pinf)){
        	R_ASSERT(pinf.e_mesh);
            CSurface* surf=pinf.e_mesh->GetSurfaceByFaceID(pinf.inf.id);
            ss.push_back(AnsiString("Surface: ")+AnsiString(surf->_Name()));
            ss.push_back(AnsiString("Texture: ")+AnsiString(surf->_Texture()));
            ss.push_back(AnsiString("Shader: ")+AnsiString(surf->_ShaderName()));
            ss.push_back(AnsiString("LC Shader: ")+AnsiString(surf->_ShaderXRLCName()));
            ss.push_back(AnsiString("2 Sided: ")+AnsiString(surf->_2Sided()?"on":"off"));
        }
    }
}

void CShaderTools::UpdateObjectShader(){
    // apply this shader to non custom object
	if (m_EditObject&&!m_bCustomEditObject){
    	CSurface* surf = *m_EditObject->FirstSurface(); R_ASSERT(surf);
        string512 tex; strcpy(tex,surf->_Texture());
        for (int i=0; i<7; i++){ strcat(tex,","); strcat(tex,surf->_Texture());}
        if (SEngine.m_CurrentBlender)	surf->ED_SetShader(SEngine.m_CurrentBlender->getName());
        else							surf->ED_SetShader("editor\\wire");
        UI.RedrawScene();
		m_EditObject->OnDeviceDestroy();
    }
}

void CShaderTools::ApplyChanges()
{
    if (ActiveEditor()==aeEngine)		SEngine.ApplyChanges();
    else if (ActiveEditor()==aeCompiler)SCompiler.ApplyChanges();
}

void CShaderTools::ShowProperties()
{
	m_Props->ShowProperties();
}

