//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHSoundEnvTools.h"
#include "ui_main.h"
#include "folderlib.h"
#include "ChoseForm.h"
#include "leftbar.h"
#include <eax.h>
#include "ItemList.h"

//------------------------------------------------------------------------------

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
	if (!psSoundFlags.is(ssHardware|ssFX)){
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
    Ext.m_Items->OnModifiedEvent= Modified;
    Ext.m_Items->OnItemRename	= OnRenameItem;
    Ext.m_Items->OnItemRemove	= OnRemoveItem;
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
	m_PreviewSnd 		= xr_new<ESoundSource>((LPVOID)NULL, "Test");
    m_PreviewSnd->Select(TRUE);
    Load();
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
    ResetCurrentItem();
    Load();
    FillItemList		();
}

void CSHSoundEnvTools::FillItemList()
{
	// store folders
	AStringVec folders;
	Ext.m_Items->GetFolders(folders);
    // fill items
	ListItemsVec items;
    SoundEnvironment_LIB::SE_VEC& lst = m_Library.Library();
    for (SoundEnvironment_LIB::SE_IT it=lst.begin(); it!=lst.end(); it++)
        LHelper.CreateItem(items,*(*it)->name,0);
    // fill folders
    for (AStringIt s_it=folders.begin(); s_it!=folders.end(); s_it++)
        LHelper.CreateItem(items,s_it->c_str(),0);
    // assign items
	Ext.m_Items->AssignItems(items,false,true);
}

void CSHSoundEnvTools::Load()
{
	AnsiString fn;
    FS.update_path(fn,_game_data_,SNDENV_FILENAME);

    m_bLockUpdate		= TRUE;
    
    if (FS.exist(fn.c_str())){
    	m_Library.Load(fn.c_str());
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	m_bLockUpdate		= FALSE;
}

void CSHSoundEnvTools::Save()
{
    ApplyChanges();
	ResetCurrentItem			();
	m_bLockUpdate				= TRUE;

    // save
	AnsiString fn;
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
    m_LastSelection		= FHelper.GenerateName(pref.c_str(),2,ItemExist,false);
    CSoundRender_Environment* S 	= m_Library.Append(parent);
    if (!parent)		S->set_default();
    S->name				= m_LastSelection.c_str();
    UI->Command			(COMMAND_UPDATE_PROPERTIES);
	Modified			();
    return *S->name;
}

void CSHSoundEnvTools::OnRenameItem(LPCSTR old_full_name, LPCSTR new_full_name, EItemType type)
{
	if (type==TYPE_OBJECT){
        ApplyChanges	();
        CSoundRender_Environment* S = m_Library.Get(old_full_name); R_ASSERT(S);
        S->name			= new_full_name;
    }
}

BOOL CSHSoundEnvTools::OnRemoveItem(LPCSTR name, EItemType type)
{
	if (type==TYPE_OBJECT){
        R_ASSERT		(name && name[0]);
        m_Library.Remove(name);
    }
    return TRUE;
}

void CSHSoundEnvTools::SetCurrentItem(LPCSTR name, bool bView)
{
    if (m_bLockUpdate) return;
	CSoundRender_Environment* S = FindItem(name);
	if (m_Env!=S){
        m_Env = S;
	    UI->Command(COMMAND_UPDATE_PROPERTIES);
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
    case 0: m_Env->set_default();	break;
    case 1: m_Env->set_identity();	break;
	}
    Ext.m_ItemProps->RefreshForm();
    Modified();
}

void CSHSoundEnvTools::RealUpdateProperties()
{
	FillItemList				();
	PropItemVec items;
	if (m_Env){
        // fill environment
		CSoundRender_Environment& S	= *m_Env;
        ButtonValue* B			= 0;
        PHelper.CreateRName		(items, "Environment\\Name",								&S.name,  				m_CurrentItem);
        B=PHelper.CreateButton	(items, "Environment\\Set",	"Default,Identity",				ButtonValue::flFirstOnly);
        B->OnBtnClickEvent 		= OnRevResetClick;

        PHelper.CreateFloat		(items, "Environment\\Environment\\EnvironmentSize",		&S.EnvironmentSize     ,EAXLISTENER_MINENVIRONMENTSIZE, 	EAXLISTENER_MAXENVIRONMENTSIZE			,0.01f,	3);
        PHelper.CreateFloat		(items, "Environment\\Environment\\EnvironmentDiffusion",	&S.EnvironmentDiffusion,EAXLISTENER_MINENVIRONMENTDIFFUSION,EAXLISTENER_MAXENVIRONMENTDIFFUSION		,0.01f,	3);
        PHelper.CreateFloat		(items, "Environment\\Room\\Room",							&S.Room                ,(float)EAXLISTENER_MINROOM, 	  	(float)EAXLISTENER_MAXROOM				,1.f,	0);
        PHelper.CreateFloat		(items, "Environment\\Room\\RoomHF",						&S.RoomHF              ,(float)EAXLISTENER_MINROOMHF, 	  	(float)EAXLISTENER_MAXROOMHF			,1.f,	0);
        PHelper.CreateFloat		(items, "Environment\\Distance Effects\\RoomRolloffFactor",	&S.RoomRolloffFactor   ,EAXLISTENER_MINROOMROLLOFFFACTOR, 	EAXLISTENER_MAXROOMROLLOFFFACTOR		,0.01f,	3);
        PHelper.CreateFloat		(items, "Environment\\Distance Effects\\AirAbsorptionHF",  	&S.AirAbsorptionHF     ,EAXLISTENER_MINAIRABSORPTIONHF, 	EAXLISTENER_MAXAIRABSORPTIONHF			,0.01f,	3);
        PHelper.CreateFloat		(items, "Environment\\Reflections\\Reflections",			&S.Reflections         ,(float)EAXLISTENER_MINREFLECTIONS,	(float)EAXLISTENER_MAXREFLECTIONS		,1.f,	0);
        PHelper.CreateFloat		(items, "Environment\\Reflections\\ReflectionsDelay",		&S.ReflectionsDelay    ,EAXLISTENER_MINREFLECTIONSDELAY, 	EAXLISTENER_MAXREFLECTIONSDELAY			,0.01f,	3);
        PHelper.CreateFloat		(items, "Environment\\Reverb\\Reverb",						&S.Reverb              ,(float)EAXLISTENER_MINREVERB, 	  	(float)EAXLISTENER_MAXREVERB			,1.f,	0);
        PHelper.CreateFloat		(items, "Environment\\Reverb\\ReverbDelay",					&S.ReverbDelay         ,EAXLISTENER_MINREVERBDELAY, 		EAXLISTENER_MAXREVERBDELAY				,0.01f,	3);
        PHelper.CreateFloat		(items, "Environment\\Decay\\DecayTime",					&S.DecayTime           ,EAXLISTENER_MINDECAYTIME, 			EAXLISTENER_MAXDECAYTIME				,0.01f,	3);
        PHelper.CreateFloat		(items, "Environment\\Decay\\DecayHFRatio",					&S.DecayHFRatio        ,EAXLISTENER_MINDECAYHFRATIO, 		EAXLISTENER_MAXDECAYHFRATIO				,0.01f,	3);
    }
    Ext.m_ItemProps->AssignItems		(items);
    Ext.m_ItemProps->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------


