//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHGameMtlTools.h"
#include "PropertiesListHelper.h"
#include "ui_main.h"
#include "folderlib.h"
#include "UI_Tools.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CSHGameMtlTools::CSHGameMtlTools(ISHInit& init):ISHTools(init)
{
    m_Mtl 				= 0;
    m_GameMtlPairTools	= 0;
}

CSHGameMtlTools::~CSHGameMtlTools()
{
}
//---------------------------------------------------------------------------

void CSHGameMtlTools::OnFrame()
{
}
//---------------------------------------------------------------------------

bool CSHGameMtlTools::OnCreate()
{
	m_GameMtlPairTools	= Tools.FindTools(aeMtlPair); R_ASSERT(m_GameMtlPairTools);
    Load();
    return true;
}

void CSHGameMtlTools::OnDestroy()
{
    m_bModified = FALSE;
}

void CSHGameMtlTools::Reload()
{
	// mtl
	ViewClearItemList();
    ResetCurrentItem();
    // mtl pair
	m_GameMtlPairTools->ViewClearItemList();
    m_GameMtlPairTools->ResetCurrentItem();
    // load
    Load();
    // mtl pair
	m_GameMtlPairTools->FillItemList();
}

void CSHGameMtlTools::FillItemList()
{
    View()->IsUpdating = true;
	ViewClearItemList();
    for (GameMtlIt m_it=GMLib.FirstMaterial(); m_it!=GMLib.LastMaterial(); m_it++)
        ViewAddItem((*m_it)->name);
    View()->IsUpdating = false;
}

void CSHGameMtlTools::Load()
{
	AnsiString fn;
    FS.update_path		(fn,_game_data_,"gamemtl.xr");

    m_bLockUpdate		= TRUE;

    if (FS.exist(fn.c_str())){
    	GMLib.Unload	();
    	GMLib.Load		(fn.c_str());
        FillItemList	();
        ResetCurrentItem();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	m_bLockUpdate		= FALSE;
}

void CSHGameMtlTools::Save()
{
    AnsiString name;
    FHelper.MakeFullName(View()->Selected,0,name);
	ResetCurrentItem	();
    m_bLockUpdate		= TRUE;

    // save
	AnsiString fn;
    FS.update_path		(fn,_game_data_,"gamemtl.xr");
    EFS.UnlockFile		(0,fn.c_str(),false);
    // backup file
    EFS.BackupFile		(_game_data_,"gamemtl.xr");
    // save new file
    GMLib.Save			(fn.c_str());
    EFS.LockFile		(0,fn.c_str(),false);
	m_bLockUpdate		= FALSE;
	SetCurrentItem		(name.c_str());

    m_bModified	= FALSE;
}

SGameMtl* CSHGameMtlTools::FindItem(LPCSTR name)
{
	if (name && name[0]){
    	return GMLib.GetMaterial(name);
    }else return 0;
}

LPCSTR CSHGameMtlTools::GenerateItemName(LPSTR name, LPCSTR pref, LPCSTR source)
{
    int cnt = 0;
    if (source) strcpy(name,source); else sprintf(name,"%sgm_%02d",pref,cnt++);
	while (FindItem(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%sgm_%02d",pref,cnt++);
	return name;
}

LPCSTR CSHGameMtlTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
	SGameMtl* parent 	= FindItem(parent_name);
    string64 new_name;
    GenerateItemName	(new_name,folder_name,parent_name);
    SGameMtl* S 		= GMLib.AppendMaterial(parent);
    strcpy				(S->name,new_name);
	ViewAddItem			(S->name);
	SetCurrentItem		(S->name);
	Modified			();
    return S->name;
}

void CSHGameMtlTools::RenameItem(LPCSTR old_full_name, LPCSTR ren_part, int level)
{
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    RenameItem(old_full_name, new_full_name);
}

void CSHGameMtlTools::RenameItem(LPCSTR old_full_name, LPCSTR new_full_name)
{
	SGameMtl* S = FindItem(old_full_name); R_ASSERT(S);
    strcpy(S->name,new_full_name);
	if (S==m_Mtl)	UpdateProperties();

    // нужно переинициализировать лист пар
	m_GameMtlPairTools->FillItemList();
}

void CSHGameMtlTools::RemoveItem(LPCSTR name)
{
	R_ASSERT(name && name[0]);
    GMLib.RemoveMaterial(name);
    // нужно переинициализировать лист пар
	m_GameMtlPairTools->FillItemList();
}

void CSHGameMtlTools::SetCurrentItem(LPCSTR name)
{
    if (m_bLockUpdate) return;

	SGameMtl* S = FindItem(name);
    // load material
	if (m_Mtl!=S){
        m_Mtl = S;
        UpdateProperties();
    }
	ViewSetCurrentItem(name);
}

void CSHGameMtlTools::ResetCurrentItem()
{
	m_Mtl=0;
}
//---------------------------------------------------------------------------

void __fastcall CSHGameMtlTools::OnMaterialNameChange(PropValue* sender)
{
    // нужно переинициализировать лист пар
	m_GameMtlPairTools->FillItemList();
}

void CSHGameMtlTools::UpdateProperties()
{
	PropItemVec items;
    if (m_Mtl){ 
    	m_Mtl->FillProp			(items);
        PropItem* I 			= PHelper.FindItem(items,"Name",PROP_TEXT);			R_ASSERT(I);
		PropValue* P			= I->GetFrontValue();                               R_ASSERT(P);
        P->OnChangeEvent		= OnMaterialNameChange;
    	I->OnAfterEditEvent 	= PHelper.NameAfterEdit_TI;
        I->OnBeforeEditEvent	= PHelper.NameBeforeEdit;
	    I->OnDrawTextEvent		= PHelper.NameDraw;
    	I->tag					= (int)FHelper.FindObject(View(),m_Mtl->name); 		R_ASSERT(I->tag);
    }
    Ext.m_ItemProps->AssignItems		(items,true);
    Ext.m_ItemProps->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------

void CSHGameMtlTools::ApplyChanges(bool bForced)
{
}
//---------------------------------------------------------------------------


