//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHSoundEnvTools.h"
#include "ui_main.h"
#include "folderlib.h"
#include "ChoseForm.h"
#include "leftbar.h"
#include <eax.h>

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
    fraLeftBar->InplaceEdit->Tree = View();
	m_PreviewSnd->Play();
	PropItemVec items;
    m_PreviewSnd->FillProp	("Sound",items);
    Ext.m_PreviewProps->AssignItems(items);
    Ext.m_PreviewProps->ShowProperties();
}
//---------------------------------------------------------------------------

void CSHSoundEnvTools::OnDeactivate()
{
	m_PreviewSnd->Stop();
	m_PreviewSnd->OnFrame();
	Ext.m_PreviewProps->ClearProperties();
}
//---------------------------------------------------------------------------

void CSHSoundEnvTools::OnFrame()
{
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
	ViewClearItemList();
    ResetCurrentItem();
    Load();
}

void CSHSoundEnvTools::FillItemList()
{
    View()->IsUpdating 	= true;
	ViewClearItemList();
    SoundEnvironment_LIB::SE_VEC& lst = m_Library.Library();
    for (SoundEnvironment_LIB::SE_IT it=lst.begin(); it!=lst.end(); it++)
        ViewAddItem(*(*it)->name);
    View()->IsUpdating 	= false;
}

void CSHSoundEnvTools::Load()
{
	AnsiString fn;
    FS.update_path(fn,_game_data_,SNDENV_FILENAME);

    m_bLockUpdate		= TRUE;
    
    if (FS.exist(fn.c_str())){
    	m_Library.Load(fn.c_str());
        FillItemList			();
        ResetCurrentItem		();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	m_bLockUpdate		= FALSE;
}

void CSHSoundEnvTools::Save()
{
    ApplyChanges();
    AnsiString name;
    FHelper.MakeFullName		(View()->Selected,0,name);
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
	SetCurrentItem				(name.c_str());

    m_bModified					= FALSE;
}

CSoundRender_Environment* CSHSoundEnvTools::FindItem(LPCSTR name)
{
	if (name && name[0]){
    	return m_Library.Get(name);
    }else return 0;
}

LPCSTR CSHSoundEnvTools::GenerateItemName(LPSTR name, LPCSTR pref, LPCSTR source)
{
    int cnt = 0;
    if (source) strcpy(name,source); else sprintf(name,"%ssnd_env_%02d",pref,cnt++);
	while (FindItem(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%ssnd_env_%02d",pref,cnt++);
	return name;
}

LPCSTR CSHSoundEnvTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
	CSoundRender_Environment* parent= FindItem(parent_name);
    string64 new_name;
    GenerateItemName	(new_name,folder_name,parent_name);
    CSoundRender_Environment* S 	= m_Library.Append(parent);
    S->set_default		();
    S->name				= new_name;
	ViewAddItem			(*S->name);
	SetCurrentItem		(*S->name);
	Modified			();
    return *S->name;
}

void CSHSoundEnvTools::RenameItem(LPCSTR old_full_name, LPCSTR ren_part, int level)
{
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    RenameItem(old_full_name, new_full_name);
}

void CSHSoundEnvTools::RenameItem(LPCSTR old_full_name, LPCSTR new_full_name)
{
    ApplyChanges	();
	CSoundRender_Environment* S = m_Library.Get(old_full_name); R_ASSERT(S);
    S->name			= new_full_name;
}

void CSHSoundEnvTools::RemoveItem(LPCSTR name)
{
	R_ASSERT		(name && name[0]);
    m_Library.Remove(name);
}

void CSHSoundEnvTools::SetCurrentItem(LPCSTR name)
{
    if (m_bLockUpdate) return;
	CSoundRender_Environment* S = FindItem(name);
	if (m_Env!=S){
        m_Env = S;
	    UI->Command(COMMAND_UPDATE_PROPERTIES);
    }
	ViewSetCurrentItem(name);
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
	PropItemVec items;
	if (m_Env){
        // fill environment
		CSoundRender_Environment& S	= *m_Env;
        ButtonValue* B			= 0;
        PHelper.CreateRName_TI	(items, "Environment\\Name",								&S.name,  				FHelper.FindObject(View(),*S.name));
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


