//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
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
    m_ItemProps			= 0;
    m_PreviewProps		= 0;
    fFogness			= 0.9f;
    dwFogColor			= 0xffffffff;
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
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

bool CShaderTools::IfModified()
{	
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (!it->second->IfModified()) return false;
	return true;
}

bool CShaderTools::IsModified()
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (it->second->IsModified()) return true;
	return false;
}

void CShaderTools::Modified()
{
	Current()->Modified();
}
//---------------------------------------------------------------------------

bool CShaderTools::OnCreate()
{
    // create props
    m_ItemProps 	= TProperties::CreateForm(fraLeftBar->paShaderProps,alClient);
    m_PreviewProps  = TProperties::CreateForm(fraLeftBar->paPreviewProps,alClient);
	// shader test locking
	AnsiString sh_fn;
    FS.update_path		(sh_fn,_game_data_,"shaders.xr");
	if (EFS.CheckLocking(0,sh_fn.c_str(),false,true)){
    	ELog.DlgMsg(mtInformation,"Shader Editor locked.");
    	return false;
    }
	// shader test locking
	AnsiString lc_fn;
    FS.update_path		(lc_fn,_game_data_,"shaders_xrlc.xr");
	if (EFS.CheckLocking(0,lc_fn.c_str(),false,true)){
    	ELog.DlgMsg(mtInformation,"Shader Editor locked.");
    	return false;
    }
	// material test locking
	AnsiString gm_fn;
    FS.update_path		(gm_fn,_game_data_,"gamemtl.xr");
	if (EFS.CheckLocking(0,gm_fn.c_str(),false,true)){
    	ELog.DlgMsg(mtInformation,"Shader Editor locked.");
    	return false;
    }
	//
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);

    // create tools
    RegisterTools		();

	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (!it->second->OnCreate()) return false;

   	// lock
    EFS.LockFile(0,sh_fn.c_str());
    EFS.LockFile(0,lc_fn.c_str());
    EFS.LockFile(0,gm_fn.c_str());
    return true;
}

void CShaderTools::OnDestroy()
{
	// destroy props
	TProperties::DestroyForm(m_ItemProps);
    TProperties::DestroyForm(m_PreviewProps);
	// unlock
    EFS.UnlockFile(_game_data_,"shaders.xr");
    EFS.UnlockFile(_game_data_,"shaders_xrlc.xr");
    EFS.UnlockFile(_game_data_,"gamemtl.xr");
	//
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	it->second->OnDestroy();

	UnregisterTools		();
}

void CShaderTools::Render()
{
	Current()->OnRender();
}

void CShaderTools::OnFrame()
{
	Current()->OnFrame();
}

void CShaderTools::ZoomObject(bool bOnlySel)
{
	Current()->ZoomObject(bOnlySel);
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

	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	it->second->OnDeviceCreate();
}

void CShaderTools::OnDeviceDestroy()
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	it->second->OnDeviceDestroy();
}

void CShaderTools::OnShowHint(AStringVec& ss)
{
	Current()->OnShowHint(ss);
}

void CShaderTools::ApplyChanges()
{
	Current()->ApplyChanges();
}

void CShaderTools::ShowProperties()
{
	m_ItemProps->ShowProperties();
}

void CShaderTools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
    s_fog				= psDeviceFlags.is(rsFog)?(1.0f - fFogness)* 0.85f * UI.ZFar():0.99f*UI.ZFar();
    e_fog				= psDeviceFlags.is(rsFog)?0.91f * UI.ZFar():UI.ZFar();
    fog_color 			= dwFogColor;
}

LPCSTR CShaderTools::CurrentToolsName()
{
	return Current()?Current()->ToolsName():"";
}

LPCSTR CShaderTools::GetInfo()
{
	return 0;
}

ISHTools* CShaderTools::FindTools(EToolsID id)
{
	ToolsPairIt it = m_Tools.find(id); R_ASSERT(it!=m_Tools.end());
    return it->second;
}

ISHTools* CShaderTools::FindTools(TElTabSheet* sheet)
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	if (it->second->Sheet()==sheet) return it->second;
    return 0;
}

void CShaderTools::Save()
{
    for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
        it->second->Save();
}

void CShaderTools::Reload()
{
	if (!Current()->IfModified()) return;
    if (ELog.DlgMsg(mtConfirmation,"Reload current items?")==mrYes)
        Current()->Reload();
}

#include "SHEngineTools.h"
#include "SHGameMtlTools.h"
#include "SHGameMtlPairTools.h"
#include "SHCompilerTools.h"
#include "SHSoundEnvTools.h"

void CShaderTools::RegisterTools()
{
	for (int k=aeFirstTool; k<aeMaxTools; k++){	
    	ISHTools* tools = 0;
		switch(k){
		case aeEngine:		tools = xr_new<CSHEngineTools>		(ISHInit( EToolsID(k),	fraLeftBar->tvEngine,	fraLeftBar->pmListCommand,	fraLeftBar->tsEngine,	m_ItemProps,	m_PreviewProps));   break;
    	case aeCompiler:	tools = xr_new<CSHCompilerTools>	(ISHInit( EToolsID(k),	fraLeftBar->tvCompiler,	fraLeftBar->pmListCommand,	fraLeftBar->tsCompiler, m_ItemProps,	m_PreviewProps));	break;
    	case aeMtl:			tools = xr_new<CSHGameMtlTools>		(ISHInit( EToolsID(k),	fraLeftBar->tvMtl,		fraLeftBar->pmListCommand,	fraLeftBar->tsMaterial,	m_ItemProps,	m_PreviewProps));	break;
    	case aeMtlPair:		tools = xr_new<CSHGameMtlPairTools>	(ISHInit( EToolsID(k),	fraLeftBar->tvMtlPair,	(TMxPopupMenu*)NULL,		fraLeftBar->tsMaterialPair,m_ItemProps,	m_PreviewProps));	break;
    	case aeSoundEnv:	tools = xr_new<CSHSoundEnvTools>	(ISHInit( EToolsID(k),	fraLeftBar->tvSoundEnv,	fraLeftBar->pmListCommand,	fraLeftBar->tsSoundEnv,	m_ItemProps,	m_PreviewProps));	break;
        }
        R_ASSERT(tools);
		m_Tools.insert(mk_pair(k,tools));
    }
}

void CShaderTools::UnregisterTools()
{
	for (ToolsPairIt it=m_Tools.begin(); it!=m_Tools.end(); it++)
    	xr_delete(it->second);
}

