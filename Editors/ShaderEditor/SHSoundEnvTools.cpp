//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHSoundEnvTools.h"
#include "ui_main.h"
#include "folderlib.h"
#include "ChoseForm.h"
#include "leftbar.h"

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
    Ext.m_PreviewProps->AssignItems(items,true);
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
        ViewAddItem((*it)->name);
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
    S->set_default		(true,true,true);
    strcpy				(S->name,new_name);
	ViewAddItem			(S->name);
	SetCurrentItem		(S->name);
	Modified			();
    return S->name;
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
    strcpy			(S->name,new_full_name);
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

void __fastcall CSHSoundEnvTools::OnERevResetClick(PropValue* sender, bool& bModif)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_Env->set_default(false,false,true);	break;
    case 1: m_Env->set_identity(false,false,true);	break;
	}
    Ext.m_ItemProps->RefreshForm();
    Modified();
}
void __fastcall CSHSoundEnvTools::OnWRevResetClick(PropValue* sender, bool& bModif)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_Env->set_default(true,false,false);	break;
    case 1: m_Env->set_identity(true,false,false);	break;
	}
    Ext.m_ItemProps->RefreshForm();
    Modified();
}
void __fastcall CSHSoundEnvTools::OnEchoResetClick(PropValue* sender, bool& bModif)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_Env->set_default(false,true,false);	break;
    case 1: m_Env->set_identity(false,true,false);	break;
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
        ButtonValue* B=0;
        PHelper.CreateName_TI	(items, "Environment\\Name",								S.name,  		sizeof(S.name), FHelper.FindObject(View(),S.name));

        if (psSoundFlags.is(ssHardware)){
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\Room",				&S.L_Room, 				DSFX_I3DL2REVERB_ROOM_MIN,				DSFX_I3DL2REVERB_ROOM_MAX, 				1.f,	0);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\RoomHF",				&S.L_RoomHF, 			DSFX_I3DL2REVERB_ROOMHF_MIN,			DSFX_I3DL2REVERB_ROOMHF_MAX, 			1.f,	0);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\RoomRolloffFactor",	&S.L_RoomRolloffFactor,	DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN,	DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MAX,	0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\DecayTime",			&S.L_DecayTime, 		DSFX_I3DL2REVERB_DECAYTIME_MIN,			DSFX_I3DL2REVERB_DECAYTIME_MAX, 		0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\DecayHFRatio",		&S.L_DecayHFRatio, 		DSFX_I3DL2REVERB_DECAYHFRATIO_MIN,		DSFX_I3DL2REVERB_DECAYHFRATIO_MAX, 		0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\Reflections",			&S.L_Reflections, 		DSFX_I3DL2REVERB_REFLECTIONS_MIN,		DSFX_I3DL2REVERB_REFLECTIONS_MAX, 		1.f,	0);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\ReflectionsDelay",	&S.L_ReflectionsDelay, 	DSFX_I3DL2REVERB_REFLECTIONSDELAY_MIN,	DSFX_I3DL2REVERB_REFLECTIONSDELAY_MAX,	0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\Reverb",				&S.L_Reverb, 			DSFX_I3DL2REVERB_REVERB_MIN,			DSFX_I3DL2REVERB_REVERB_MAX, 			1.f,	0);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\ReverbDelay",			&S.L_ReverbDelay, 		DSFX_I3DL2REVERB_REVERBDELAY_MIN,		DSFX_I3DL2REVERB_REVERBDELAY_MAX, 		0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\Diffusion",			&S.L_Diffusion, 		DSFX_I3DL2REVERB_DIFFUSION_MIN,			DSFX_I3DL2REVERB_DIFFUSION_MAX, 		0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\Density",				&S.L_Density, 			DSFX_I3DL2REVERB_DENSITY_MIN,			DSFX_I3DL2REVERB_DENSITY_MAX, 			0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Env Reverb\\HFReference",			&S.L_HFReference, 		DSFX_I3DL2REVERB_HFREFERENCE_MIN,		DSFX_I3DL2REVERB_HFREFERENCE_MAX, 		0.01f,	3);
            B=PHelper.CreateButton	(items, "Environment\\Env Reverb\\Set",					"Default,Identity",ButtonValue::flFirstOnly);
            B->OnBtnClickEvent 		= OnERevResetClick;
        }

        if (!psSoundFlags.is(ssHardware)){
            PHelper.CreateFloat		(items, "Environment\\Waves Reverb\\In Gain",  	        &S.R_InGain,           	-96,	0, 		0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Waves Reverb\\Mix",	   	        &S.R_Mix,	           	-96,	0, 		0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Waves Reverb\\Time",	   	        &S.R_Time,	           	0.01f,	3000, 	0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Waves Reverb\\HF Ratio",	        &S.R_HFRatio,          	0.001f,	0.999f, 0.01f,	3);
            B=PHelper.CreateButton	(items, "Environment\\Waves Reverb\\Set",		        "Default,Identity",ButtonValue::flFirstOnly);
            B->OnBtnClickEvent 		= OnWRevResetClick;

            PHelper.CreateFloat		(items, "Environment\\Echo\\Wet Dry",			        &S.E_WetDry,	        0,		100,	0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Echo\\Feedback",			        &S.E_FeedBack,	        0,		100,	0.01f,	3);
            PHelper.CreateFloat		(items, "Environment\\Echo\\Delay",				        &S.E_Delay,		        1,		2000,	0.01f,	3);
            B=PHelper.CreateButton	(items, "Environment\\Echo\\Set",				        "Default,Identity",ButtonValue::flFirstOnly);
            B->OnBtnClickEvent 		= OnEchoResetClick;
        }
    }
    Ext.m_ItemProps->AssignItems		(items,true);
    Ext.m_ItemProps->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------


