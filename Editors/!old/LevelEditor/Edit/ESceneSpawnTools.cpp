#include "stdafx.h"
#pragma hdrstop

#include "ESceneSpawnTools.h"
#include "ui_leveltools.h"
#include "ESceneSpawnControls.h"
#include "FrameSpawn.h"
#include "PropertiesListHelper.h"
#include "Scene.h"
#include "ChoseForm.h"

static HMODULE hXRSE_FACTORY = 0;
static LPCSTR xrse_factory_library	= "xrSE_Factory.dll";
static LPCSTR create_entity_func 	= "_create_entity@4";
static LPCSTR destroy_entity_func 	= "_destroy_entity@4";
Tcreate_entity 	create_entity;
Tdestroy_entity destroy_entity;

void __stdcall  FillSpawnItems	(ChooseItemVec& lst, void* param)
{
	LPCSTR gcs					= (LPCSTR)param;
    ObjectList objects;
    Scene->GetQueryObjects		(objects,OBJCLASS_SPAWNPOINT,-1,-1,-1);
    
    std::string itm;
    int cnt 					= _GetItemCount(gcs);
    for (int k=0; k<cnt; k++){
        _GetItem				(gcs,k,itm);
        for (ObjectIt it=objects.begin(); it!=objects.end(); it++)
            if ((*it)->OnChooseQuery(itm.c_str()))	lst.push_back(SChooseItem((*it)->Name,""));
    }
}

ESceneSpawnTools::ESceneSpawnTools	():ESceneCustomOTools(OBJCLASS_SPAWNPOINT)
{
	m_Flags.zero();
    TfrmChoseItem::AppendEvents	(smSpawnItem,		"Select Spawn Item",		FillSpawnItems,		0,0,0);

    hXRSE_FACTORY	= LoadLibrary(xrse_factory_library);									VERIFY3(hXRSE_FACTORY,"Can't load library:",xrse_factory_library);
    create_entity 	= (Tcreate_entity)	GetProcAddress(hXRSE_FACTORY,create_entity_func);  	VERIFY3(create_entity,"Can't find func:",create_entity_func);
    destroy_entity 	= (Tdestroy_entity)	GetProcAddress(hXRSE_FACTORY,destroy_entity_func);	VERIFY3(destroy_entity,"Can't find func:",destroy_entity_func);
}

ESceneSpawnTools::~ESceneSpawnTools()
{
	FreeLibrary		(hXRSE_FACTORY);
    m_Icons.clear	();
}

void ESceneSpawnTools::CreateControls()
{
	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlSpawnAdd>(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraSpawn>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneSpawnTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneSpawnTools::FillProp(LPCSTR pref, PropItemVec& items)
{            
//.	PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Show Spawn Type"),	&m_Flags,		flShowSpawnType);
//.	PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Trace Visibility"),	&m_Flags,		flPickSpawnType);
	inherited::FillProp	(pref, items);
}
//------------------------------------------------------------------------------

ref_shader ESceneSpawnTools::CreateIcon(ref_str name)
{
    ref_shader S;
    if (pSettings->line_exist(name,"$ed_icon")){
	    LPCSTR tex_name = pSettings->r_string(name,"$ed_icon");
    	S.create("editor\\spawn_icon",tex_name);
        m_Icons[name] = S;
    }else{
        S = 0;
    }
    return S;
}

ref_shader ESceneSpawnTools::GetIcon(ref_str name)
{
	ShaderPairIt it = m_Icons.find(name);
	if (it==m_Icons.end())	return CreateIcon(name);
	else					return it->second;
}
//----------------------------------------------------

