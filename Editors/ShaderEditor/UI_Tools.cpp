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

void CShaderTools::OnChangeEditor(ISHTools* tools)
{
	if (m_Current) m_Current->OnDeactivate();
	m_Current = tools; R_ASSERT(m_Current);
	m_Current->OnActivate();
	Current()->UpdateProperties();
}
//---------------------------------------------------------------------------

bool CShaderTools::IfModified()
{	
	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (!(*it)->IfModified()) return false;
	return true;
}

bool CShaderTools::IsModified()
{
	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if ((*it)->IsModified()) return true;
	return false;
}

void CShaderTools::Modified()
{
	Current()->Modified();
}
//---------------------------------------------------------------------------

bool CShaderTools::OnCreate(){
    // create props
    m_Props = TProperties::CreateForm(fraLeftBar->paShaderProps,alClient);
	// shader test locking
	AnsiString sh_fn;
    FS.update_path		(sh_fn,_game_data_,"shaders.xr");
	if (EFS.CheckLocking(0,sh_fn.c_str(),false,true))
    	return false;
	// shader test locking
	AnsiString lc_fn;
    FS.update_path		(lc_fn,_game_data_,"shaders_xrlc.xr");
	if (EFS.CheckLocking(0,lc_fn.c_str(),false,true))
    	return false;
	// material test locking
	AnsiString gm_fn;
    FS.update_path		(gm_fn,_game_data_,"gamemtl.xr");
	if (EFS.CheckLocking(0,gm_fn.c_str(),false,true))
    	return false;
	//
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);

    // create tools
    RegisterTools		();

	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (!(*it)->OnCreate()) return false;

   	// lock
    EFS.LockFile(0,sh_fn.c_str());
    EFS.LockFile(0,lc_fn.c_str());
    EFS.LockFile(0,gm_fn.c_str());
    return true;
}

void CShaderTools::OnDestroy()
{
	// destroy props
	TProperties::DestroyForm(m_Props);
	// unlock
    EFS.UnlockFile(_game_data_,"shaders.xr");
    EFS.UnlockFile(_game_data_,"shaders_xrlc.xr");
    EFS.UnlockFile(_game_data_,"gamemtl.xr");
	//
    Lib.RemoveEditObject(m_EditObject);
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	(*it)->OnDestroy();
}

void CShaderTools::Render()
{
	Current()->OnRender();
	if (m_EditObject) m_EditObject->RenderSingle(Fidentity);
}

void CShaderTools::OnFrame()
{
	Current()->OnFrame();
	if (m_EditObject) m_EditObject->OnFrame();
}

void CShaderTools::ZoomObject(bool bOnlySel)
{
	if (m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
    }else{
		Fbox BB;
        BB.set(-5,-5,-5,5,5,5);
        Device.m_Camera.ZoomExtents(BB);
    }
}

void CShaderTools::OnDeviceCreate()
{
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

	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	(*it)->OnDeviceCreate();
//.    SEngine.ResetShaders();
}

void CShaderTools::OnDeviceDestroy()
{
	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	(*it)->OnDeviceDestroy();
}

void CShaderTools::SelectPreviewObject(int p)
{
    LPCSTR fn;
    m_bCustomEditObject	= false;
    switch(p){
        case 0: fn="editor\\ShaderTest_Plane"; 	break;
        case 1: fn="editor\\ShaderTest_Box"; 	break;
        case 2: fn="editor\\ShaderTest_Sphere"; break;
        case 3: fn="editor\\ShaderTest_Teapot";	break;
        case 4: fn=0;							break;
        case -1: fn=m_EditObject?m_EditObject->GetName():""; if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,fn)) return; m_bCustomEditObject = true; break;
        default: THROW2("Failed select test object.");
    }
    Lib.RemoveEditObject(m_EditObject);
    if (fn){
	    m_EditObject = Lib.CreateEditObject(fn);
	    if (!m_EditObject)
    	    ELog.DlgMsg(mtError,"Object '%s.object' can't find in object library. Preview disabled.",fn);
    }
	ZoomObject();
    UpdateObjectShader();
    UI.RedrawScene();
}

void CShaderTools::ResetPreviewObject()
{
    Lib.RemoveEditObject(m_EditObject);
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
            ss.push_back(AnsiString("Game Mtl: ")+AnsiString(surf->_GameMtlName()));
            ss.push_back(AnsiString("2 Sided: ")+AnsiString(surf->m_Flags.is(CSurface::sf2Sided)?"on":"off"));
        }
    }
}

void CShaderTools::UpdateObjectShader()
{
//.
/*    // apply this shader to non custom object
	if (m_EditObject&&!m_bCustomEditObject){
    	CSurface* surf = *m_EditObject->FirstSurface(); R_ASSERT(surf);
        string512 tex; strcpy(tex,surf->_Texture());
        for (int i=0; i<7; i++){ strcat(tex,","); strcat(tex,surf->_Texture());}
        if (SEngine.m_CurrentBlender)	surf->SetShader(SEngine.m_CurrentBlender->getName());
        else							surf->SetShader("editor\\wire");
        UI.RedrawScene();
		m_EditObject->OnDeviceDestroy();
    }
*/
}

void CShaderTools::ApplyChanges()
{
	Current()->ApplyChanges();
}

void CShaderTools::ShowProperties()
{
	m_Props->ShowProperties();
}

void CShaderTools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
	s_fog		= UI.ZFar();
	e_fog		= UI.ZFar();
	fog_color	= DEFAULT_CLEARCOLOR;
}

LPCSTR CShaderTools::GetInfo()
{
	return 0;
}

ISHTools* CShaderTools::FindTools(EToolsID id)
{
	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if ((*it)->ID()==id) return *it;
    return 0;
}

ISHTools* CShaderTools::FindTools(TElTabSheet* sheet)
{
	for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if ((*it)->Sheet()==sheet) return *it;
    return 0;
}

void CShaderTools::Save()
{
    for (ToolsIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
        (*it)->Save();
}

void CShaderTools::Reload()
{
	if (!Current()->IfModified()) return;
    if (ELog.DlgMsg(mtConfirmation,"Reload current items?")==mrYes)
        Current()->Reload();
}

#include "SHGameMtlTools.h"
#include "SHGameMtlPairTools.h"
#include "SHCompilerTools.h"
#include "SHEngineTools.h"
#include "SHSoundEnvTools.h"

void CShaderTools::RegisterTools()
{
	for (int k=aeFirstTool; k<aeMaxTools; k++){	
    	ISHTools* tools = 0;
		switch(k){
//		case aeEngine:		tools = xr_new<CSHEngineTools>(tvEngine,aeEngine); break;
    	case aeCompiler:	tools = xr_new<CSHCompilerTools>	(aeCompiler,fraLeftBar->tvCompiler_,	fraLeftBar->pmListCommand,	fraLeftBar->tsCompiler, m_Props);	break;
    	case aeMtl:			tools = xr_new<CSHGameMtlTools>		(aeMtl,		fraLeftBar->tvMtl_,		fraLeftBar->pmListCommand,fraLeftBar->tsMaterial,m_Props);	break;
    	case aeMtlPair:		tools = xr_new<CSHGameMtlPairTools>	(aeMtlPair,	fraLeftBar->tvMtlPair_,	fraLeftBar->pmListCommand,fraLeftBar->tsMaterialPair,m_Props);	break;
    	case aeSoundEnv:	tools = xr_new<CSHSoundEnvTools>	(aeSoundEnv,fraLeftBar->tvSoundEnv_,	fraLeftBar->pmListCommand,fraLeftBar->tsSoundEnv,m_Props);	break;
        }
        
		m_Tools.push_back(tools);
    }
}


