//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHGameMtlTools.h"
#include "PropertiesListHelper.h"
#include "ui_main.h"
#include "folderlib.h"
#include "UI_ShaderTools.h"
#include "ChoseForm.h"
#include "leftbar.h"
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

void CSHGameMtlTools::OnActivate()
{
    fraLeftBar->InplaceEdit->Tree = View();
}

void CSHGameMtlTools::OnFrame()
{
}
//---------------------------------------------------------------------------

bool CSHGameMtlTools::OnCreate()
{
	m_GameMtlPairTools	= STools->FindTools(aeMtlPair); R_ASSERT(m_GameMtlPairTools);
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
        ViewAddItem(*(*m_it)->m_Name);
    View()->IsUpdating = false;
}

void CSHGameMtlTools::Load()
{
    m_bLockUpdate		= TRUE;

    GMLib.Unload	();
    GMLib.Load		();
    FillItemList	();
    ResetCurrentItem();

	m_bLockUpdate		= FALSE;
}

void CSHGameMtlTools::Save()
{
    AnsiString name;
    FHelper.MakeFullName(View()->Selected,0,name);
	ResetCurrentItem	();
    m_bLockUpdate		= TRUE;

    // save
    EFS.UnlockFile		(_game_data_,GAMEMTL_FILENAME,false);
    EFS.BackupFile		(_game_data_,GAMEMTL_FILENAME);
    GMLib.Save			();
    EFS.LockFile		(_game_data_,GAMEMTL_FILENAME,false);
    
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

void __fastcall CSHGameMtlTools::FillChooseMtlType(ChooseItemVec& items)
{
    items.push_back(SChooseItem("Dynamic material",""));
    items.push_back(SChooseItem("Static material",""));
}

LPCSTR CSHGameMtlTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
    LPCSTR M=0;
    if (!TfrmChoseItem::SelectItem(smCustom,M,1,0,FillChooseMtlType)||!M) return 0;
	SGameMtl* parent 	= FindItem(parent_name);
    string64 new_name;
    GenerateItemName	(new_name,folder_name,parent_name);
    SGameMtl* S 		= GMLib.AppendMaterial(parent);
    S->m_Name			= new_name;
    S->Flags.set		(SGameMtl::flDynamic,0==strcmp(M,"Dynamic material"));
	ViewAddItem			(*S->m_Name);
	SetCurrentItem		(*S->m_Name);
	Modified			();
    return *S->m_Name;
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
    S->m_Name		= new_full_name;
	if (S==m_Mtl)
	    UI->Command(COMMAND_UPDATE_PROPERTIES);

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
	    UI->Command(COMMAND_UPDATE_PROPERTIES);
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

void CSHGameMtlTools::RealUpdateProperties()
{
	PropItemVec items;
    if (m_Mtl)
    	m_Mtl->FillProp	(items,FHelper.FindObject(View(),*m_Mtl->m_Name));
    Ext.m_ItemProps->AssignItems		(items);
    Ext.m_ItemProps->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------

void CSHGameMtlTools::ApplyChanges(bool bForced)
{
}
//---------------------------------------------------------------------------


