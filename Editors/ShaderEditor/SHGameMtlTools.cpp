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
CSHGameMtlTools::CSHGameMtlTools(EToolsID id, TElTree* tv, TMxPopupMenu* mn, TElTabSheet* sheet, TProperties* props):ISHTools(id,tv,mn,sheet,props)
{
    m_Mtl 				= 0;
    m_GameMtlPairTools	= 0;
}

CSHGameMtlTools::~CSHGameMtlTools()
{
}
//---------------------------------------------------------------------------

void CSHGameMtlTools::Modified()
{
	m_bModified=TRUE;
	UI.Command(COMMAND_UPDATE_CAPTION);
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

bool CSHGameMtlTools::IfModified()
{
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The materials has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CSHGameMtlTools::Reload()
{
	ViewClearItemList();
    ResetCurrentItem();
    Load();
}

void CSHGameMtlTools::FillItemList()
{
    tvView->IsUpdating = true;
	ViewClearItemList();
    for (GameMtlIt m_it=GMLib.FirstMaterial(); m_it!=GMLib.LastMaterial(); m_it++)
        ViewAddItem((*m_it)->name);
    tvView->IsUpdating = false;
}

void CSHGameMtlTools::Load()
{
	AnsiString fn;
    FS.update_path		(fn,_game_data_,"gamemtl.xr");

    m_bLockUpdate		= TRUE;

    if (FS.exist(fn.c_str())){
    	GMLib.Load(fn.c_str());
        FillItemList		();
        ResetCurrentItem	();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	m_bLockUpdate		= FALSE;
}

void CSHGameMtlTools::Save()
{
    AnsiString name;
    FHelper.MakeFullName(tvView->Selected,0,name);
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
    	P->SetEvents			(FHelper.NameAfterEdit,FHelper.NameBeforeEdit,OnMaterialNameChange);
	    I->SetEvents			(FHelper.NameDraw);
    	I->tag					= (int)FHelper.FindObject(tvView,m_Mtl->name); 		R_ASSERT(I->tag);
    }
    m_Props->AssignItems		(items,true);
    m_Props->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------

void CSHGameMtlTools::ApplyChanges()
{
}
//---------------------------------------------------------------------------


