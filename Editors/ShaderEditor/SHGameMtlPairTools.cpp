//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHGameMtlPairTools.h"
#include "UI_Tools.h"
#include "ui_main.h"
#include "folderlib.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CSHGameMtlPairTools::CSHGameMtlPairTools(EToolsID id, TElTree* tv, TMxPopupMenu* mn, TElTabSheet* sheet, TProperties* props):ISHTools(id,tv,mn,sheet,props)
{
    m_MtlPair 			= 0;
    m_GameMtlTools		= 0;
}

CSHGameMtlPairTools::~CSHGameMtlPairTools()
{
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Modified()
{
	m_bModified=TRUE;
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::OnFrame()
{
}
//---------------------------------------------------------------------------

bool CSHGameMtlPairTools::OnCreate()
{
	m_GameMtlTools		= Tools.FindTools(aeMtl); R_ASSERT(m_GameMtlTools);
    Load();
    return true;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::OnDestroy()
{
    m_bModified = FALSE;
}
//---------------------------------------------------------------------------

bool CSHGameMtlPairTools::IfModified()
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
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Reload()
{
	ViewClearItemList();
    ResetCurrentItem();
    Load();
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::FillItemList()
{
    tvView->IsUpdating = true;
	ViewClearItemList();
    for (GameMtlPairIt p_it=GMLib.FirstMaterialPair(); p_it!=GMLib.LastMaterialPair(); p_it++)
        ViewAddItem(GMLib.MtlPairToName((*p_it)->GetMtl0(),(*p_it)->GetMtl1()));
	m_MtlPair=0;
    tvView->IsUpdating = false;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Load()
{
	AnsiString fn;
    FS.update_path		(fn,_game_data_,"gamemtl.xr");

    m_bLockUpdate		= TRUE;

    if (FS.exist(fn.c_str())){
    	GMLib.Load(fn.c_str());

        FillItemList	();

        ResetCurrentItem();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

    m_bLockUpdate		= FALSE;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Save()
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
//---------------------------------------------------------------------------

LPCSTR CSHGameMtlPairTools::GenerateItemName(LPSTR name, LPCSTR pref, LPCSTR source)
{
	return 0;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::UpdateProperties()
{
	PropItemVec items;
    if (m_MtlPair)	m_MtlPair->FillProp(items);
    m_Props->AssignItems		(items,true);
    m_Props->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::ApplyChanges()
{
}
//---------------------------------------------------------------------------

LPCSTR CSHGameMtlPairTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
//.
/*    SGameMtlPair* S = GMLib.AppendMaterialPair(m0,m1,parent);
    fraLeftBar->AddItem(GMLib.MtlPairToName(S->GetMtl0(),S->GetMtl1()));
    return S;              
*/
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::SetCurrentItem(LPCSTR parent_name)
{
    if (m_bLockUpdate) return;
	SGameMtlPair* parent=GMLib.GetMaterialPair(parent_name);
//.
/*
    // load material
	if (m_MtlPair!=S){
        m_MtlPair = S;
        UpdateProperties();
    }
	fraLeftBar->SetCurrentItem(GMLib.MtlPairToName(S->GetMtl0(),S->GetMtl1()));
*/
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::RemoveItem(LPCSTR name)
{
	R_ASSERT(name && name[0]);
    GMLib.RemoveMaterialPair(name);
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::ResetCurrentItem()
{
	m_MtlPair	= 0;
}
//---------------------------------------------------------------------------

