//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHSoundEnvTools.h"
#include "ui_shadermain.h"
#include "folderlib.h"
#include "ChoseForm.h"
#include "leftbar.h"
#include "ItemList.h"
#include <eax.h>

//------------------------------------------------------------------------------
xr_token eax_environment[]		= {
	{"Alley",               EAX_ENVIRONMENT_ALLEY			},                          
	{"Arena",               EAX_ENVIRONMENT_ARENA			},                          
	{"Auditorium",          EAX_ENVIRONMENT_AUDITORIUM		},                               
	{"Bathroom",            EAX_ENVIRONMENT_BATHROOM		},                             
	{"Carpeted Hallway",    EAX_ENVIRONMENT_CARPETEDHALLWAY	},                                     
	{"Cave",                EAX_ENVIRONMENT_CAVE			},                         
	{"City",                EAX_ENVIRONMENT_CITY			},                         
	{"Concert Hall",        EAX_ENVIRONMENT_CONCERTHALL		},                                 
	{"Dizzy",               EAX_ENVIRONMENT_DIZZY			},                          
	{"Drugged",             EAX_ENVIRONMENT_DRUGGED			},                            
	{"Forest",              EAX_ENVIRONMENT_FOREST			},                           
	{"Generic",			   	EAX_ENVIRONMENT_GENERIC			},
	{"Hallway",             EAX_ENVIRONMENT_HALLWAY			},                            
	{"Hangar",              EAX_ENVIRONMENT_HANGAR			},                           
	{"Livingroom",          EAX_ENVIRONMENT_LIVINGROOM		},                               
	{"Mountains",           EAX_ENVIRONMENT_MOUNTAINS		},                              
	{"Padded Cell",		    EAX_ENVIRONMENT_PADDEDCELL		},
	{"Parkinglot",          EAX_ENVIRONMENT_PARKINGLOT		},                               
	{"Plain",               EAX_ENVIRONMENT_PLAIN			},                          
	{"Psychotic",           EAX_ENVIRONMENT_PSYCHOTIC		},
	{"Quarry",              EAX_ENVIRONMENT_QUARRY			},                           
	{"Room",                EAX_ENVIRONMENT_ROOM			},
	{"Sewer Pipe",          EAX_ENVIRONMENT_SEWERPIPE		},                               
	{"Stone Corridor",      EAX_ENVIRONMENT_STONECORRIDOR	},                                   
	{"Stone Room",          EAX_ENVIRONMENT_STONEROOM		},                               
	{"Under Water",         EAX_ENVIRONMENT_UNDERWATER		},                                
    {0,						0								}
};
//------------------------------------------------------------------------------
CSHSoundEnvTools::CSHSoundEnvTools(ISHInit& init):ISHTools(init)
{
    m_Env	 			= 0;
    m_PreviewSnd		= 0;
}

CSHSoundEnvTools::~CSHSoundEnvTools()
{
}
//---------------------------------------------------------------------------

void CSHSoundEnvTools::OnActivate()
{
	if (!psSoundFlags.is(ssHardware|ssEAX)){
    	Log("#!HARDWARE or FX flags are not set. Preview is disabled.");
    }else{
        m_PreviewSnd->Play	();
        PropItemVec items;
        m_PreviewSnd->FillProp	("Sound",items);
        Ext.m_PreviewProps->AssignItems(items);
        Ext.m_PreviewProps->ShowProperties();
    }
    // fill items
    FillItemList		();
    Ext.m_Items->SetOnModifiedEvent		(TOnModifiedEvent().bind(this,&CSHSoundEnvTools::Modified));
    Ext.m_Items->SetOnItemRenameEvent	(TOnItemRename().bind(this,&CSHSoundEnvTools::OnRenameItem));
    Ext.m_Items->SetOnItemRemoveEvent	(TOnItemRemove().bind(this,&CSHSoundEnvTools::OnRemoveItem));
    inherited::OnActivate		();
}
//---------------------------------------------------------------------------

void CSHSoundEnvTools::OnDeactivate()
{
	m_PreviewSnd->Stop();
	m_PreviewSnd->OnFrame();
    inherited::OnDeactivate		();
}
//---------------------------------------------------------------------------

void CSHSoundEnvTools::OnFrame()
{
	inherited::OnFrame();
	m_PreviewSnd->OnFrame();
}
//---------------------------------------------------------------------------

void CSHSoundEnvTools::OnRender()
{
	m_PreviewSnd->Render(1,false);
}
//---------------------------------------------------------------------------

bool CSHSoundEnvTools::OnCreate()
{
	m_PreviewSnd 					= xr_new<ESoundSource>((LPVOID)NULL, "Test");
    m_PreviewSnd->Select			(TRUE);
    Load							();
    return true;
}

void CSHSoundEnvTools::OnDestroy()
{
	m_Library.Unload	();
    m_bModified 		= FALSE;
    xr_delete			(m_PreviewSnd);
}
//---------------------------------------------------------------------------

void CSHSoundEnvTools::ApplyChanges(bool bForced)
{
	UseEnvironment		();
}

void CSHSoundEnvTools::Reload()
{
    ResetCurrentItem	();
    Load				();
    FillItemList		();
}

void CSHSoundEnvTools::FillItemList()
{
	// store folders
	RStrVec folders;
	Ext.m_Items->GetFolders(folders);
    // fill items
	ListItemsVec items;
    SoundEnvironment_LIB::SE_VEC& lst = m_Library.Library();
    for (SoundEnvironment_LIB::SE_IT it=lst.begin(); it!=lst.end(); it++)
        LHelper().CreateItem(items,*(*it)->name,0);
    // fill folders
    for (RStringVecIt s_it=folders.begin(); s_it!=folders.end(); s_it++)
        LHelper().CreateItem(items,**s_it,0);
    // assign items
	Ext.m_Items->AssignItems(items,false,true);
}

void CSHSoundEnvTools::Load()
{
	xr_string fn;
    FS.update_path		(fn,_game_data_,SNDENV_FILENAME);

    m_bLockUpdate		= TRUE;
    
    if (FS.exist(fn.c_str())){
		m_Library.Unload();
    	m_Library.Load	(fn.c_str());
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	m_bLockUpdate		= FALSE;
}

void CSHSoundEnvTools::Save()
{
    ApplyChanges();
	m_bLockUpdate				= TRUE;

    // save
	xr_string fn;
    FS.update_path				(fn,_game_data_,SNDENV_FILENAME);  
    // backup file
    EFS.BackupFile				(_game_data_,SNDENV_FILENAME);

    // save new file
    EFS.UnlockFile				(0,fn.c_str(),false);
    m_Library.Save				(fn.c_str());
    EFS.LockFile				(0,fn.c_str(),false);
	m_bLockUpdate				= FALSE;

    m_bModified					= FALSE;
}

CSoundRender_Environment* CSHSoundEnvTools::FindItem(LPCSTR name)
{
	if (name && name[0]){
    	return m_Library.Get(name);
    }else return 0;
}

LPCSTR CSHSoundEnvTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
	CSoundRender_Environment* parent= FindItem(parent_name);
    AnsiString pref		= parent_name?AnsiString(parent_name):AnsiString(folder_name)+"env";
    m_LastSelection		= FHelper.GenerateName(pref.c_str(),2,TFindObjectByName().bind(this,&CSHSoundEnvTools::ItemExist),false);
    CSoundRender_Environment* S 	= m_Library.Append(parent);
    if (!parent)		S->set_default();
    S->name				= m_LastSelection.c_str();
    ExecCommand			(COMMAND_UPDATE_LIST);
    ExecCommand			(COMMAND_UPDATE_PROPERTIES);
	Modified			();
    return *S->name;
}

void CSHSoundEnvTools::OnRenameItem(LPCSTR old_full_name, LPCSTR new_full_name, EItemType type)
{
	if (type==TYPE_OBJECT){
        ApplyChanges	();
        CSoundRender_Environment* S = m_Library.Get(old_full_name); R_ASSERT(S);
        S->name			= new_full_name;
        ExecCommand		(COMMAND_UPDATE_PROPERTIES);
        ExecCommand		(COMMAND_UPDATE_LIST);
    }
}

void CSHSoundEnvTools::OnRemoveItem(LPCSTR name, EItemType type, bool& res)
{
	if (type==TYPE_OBJECT){
        R_ASSERT		(name && name[0]);
        m_Library.Remove(name);
    }
    res = true;
}

void CSHSoundEnvTools::SetCurrentItem(LPCSTR name, bool bView)
{
    if (m_bLockUpdate) return;
	CSoundRender_Environment* S = FindItem(name);
	if (m_Env!=S){
        m_Env 			= S;
        if (m_Env) 		m_EnvSrc = *m_Env;
	    ExecCommand(COMMAND_UPDATE_PROPERTIES);
		if (bView) ViewSetCurrentItem(name);
    }
	UseEnvironment	();
}

void CSHSoundEnvTools::ResetCurrentItem()
{
	m_Env=0;
	UseEnvironment	();
}

void __fastcall CSHSoundEnvTools::OnRevResetClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_Env->set_identity();	break;
    case 1: OnEnvChange(sender);    break;
	}
    Ext.m_ItemProps->RefreshForm();
    Modified();
}

void __fastcall CSHSoundEnvTools::OnEnvSizeChange(PropValue* sender)
{
    CSoundRender_Environment 	test_env=*m_Env;
    test_env.EnvironmentSize	= m_EnvSrc.EnvironmentSize;
    test_env.DecayTime			= m_EnvSrc.DecayTime;
    test_env.Reflections	 	= m_EnvSrc.Reflections;
    test_env.ReflectionsDelay	= m_EnvSrc.ReflectionsDelay;
    test_env.Reverb				= m_EnvSrc.Reverb;
    test_env.ReverbDelay 		= m_EnvSrc.ReverbDelay;
    CSound_environment* E		= m_Env;
	Sound->set_environment_size	(&test_env,&E);
    ExecCommand					(COMMAND_UPDATE_PROPERTIES);
}

void __fastcall CSHSoundEnvTools::OnEnvChange(PropValue* sender)
{
    CSound_environment* E		= m_Env;
    Sound->set_environment		(m_Env->Environment,&E);
    m_EnvSrc					= *m_Env;
    ExecCommand					(COMMAND_UPDATE_PROPERTIES);
}

void CSHSoundEnvTools::RealUpdateList()
{
	FillItemList			();
}
//------------------------------------------------------------------------------

void CSHSoundEnvTools::RealUpdateProperties()
{
	PropItemVec items;
	if (m_Env){
        // fill environment
		CSoundRender_Environment& S	= *m_Env;
        ButtonValue* B			= 0;
        PHelper().CreateName	(items, "Name",									&S.name,  				m_CurrentItem);
        B=PHelper().CreateButton(items, "Environment\\Set",	"Identity,Reset", 	ButtonValue::flFirstOnly);
        B->OnBtnClickEvent.bind	(this,&CSHSoundEnvTools::OnRevResetClick);
        PropValue* V=0;
        V=PHelper().CreateToken32(items,"Environment\\Preset",					&S.Environment	       ,eax_environment);
        V->OnChangeEvent.bind	(this,&CSHSoundEnvTools::OnEnvChange);
        V=PHelper().CreateFloat	(items, "Environment\\Size",					&S.EnvironmentSize     ,EAXLISTENER_MINENVIRONMENTSIZE, 	EAXLISTENER_MAXENVIRONMENTSIZE			,0.01f,	3);
        V->OnChangeEvent.bind	(this,&CSHSoundEnvTools::OnEnvSizeChange);
        PHelper().CreateFloat	(items, "Environment\\Diffusion",				&S.EnvironmentDiffusion,EAXLISTENER_MINENVIRONMENTDIFFUSION,EAXLISTENER_MAXENVIRONMENTDIFFUSION		,0.01f,	3);
        PHelper().CreateFloat	(items, "Room\\Room",							&S.Room                ,(float)EAXLISTENER_MINROOM, 	  	(float)EAXLISTENER_MAXROOM				,1.f,	0);
        PHelper().CreateFloat	(items, "Room\\RoomHF",							&S.RoomHF              ,(float)EAXLISTENER_MINROOMHF, 	  	(float)EAXLISTENER_MAXROOMHF			,1.f,	0);
        PHelper().CreateFloat	(items, "Distance Effects\\RoomRolloffFactor",	&S.RoomRolloffFactor   ,EAXLISTENER_MINROOMROLLOFFFACTOR, 	EAXLISTENER_MAXROOMROLLOFFFACTOR		,0.01f,	3);
        PHelper().CreateFloat	(items, "Distance Effects\\AirAbsorptionHF",  	&S.AirAbsorptionHF     ,EAXLISTENER_MINAIRABSORPTIONHF, 	EAXLISTENER_MAXAIRABSORPTIONHF			,0.01f,	3);
        PHelper().CreateFloat	(items, "Reflections\\Reflections",				&S.Reflections         ,(float)EAXLISTENER_MINREFLECTIONS,	(float)EAXLISTENER_MAXREFLECTIONS		,1.f,	0);
        PHelper().CreateFloat	(items, "Reflections\\ReflectionsDelay",		&S.ReflectionsDelay    ,EAXLISTENER_MINREFLECTIONSDELAY, 	EAXLISTENER_MAXREFLECTIONSDELAY			,0.01f,	3);
        PHelper().CreateFloat	(items, "Reverb\\Reverb",						&S.Reverb              ,(float)EAXLISTENER_MINREVERB, 	  	(float)EAXLISTENER_MAXREVERB			,1.f,	0);
        PHelper().CreateFloat	(items, "Reverb\\ReverbDelay",					&S.ReverbDelay         ,EAXLISTENER_MINREVERBDELAY, 		EAXLISTENER_MAXREVERBDELAY				,0.01f,	3);
        PHelper().CreateFloat	(items, "Decay\\DecayTime",						&S.DecayTime           ,EAXLISTENER_MINDECAYTIME, 			EAXLISTENER_MAXDECAYTIME				,0.01f,	3);
        PHelper().CreateFloat	(items, "Decay\\DecayHFRatio",					&S.DecayHFRatio        ,EAXLISTENER_MINDECAYHFRATIO, 		EAXLISTENER_MAXDECAYHFRATIO				,0.01f,	3);
    }
    Ext.m_ItemProps->AssignItems		(items);
    Ext.m_ItemProps->SetModifiedEvent	(TOnModifiedEvent().bind(this,&CSHSoundEnvTools::Modified));
}
//---------------------------------------------------------------------------


