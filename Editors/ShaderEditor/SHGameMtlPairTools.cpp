//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHGameMtlPairTools.h"
#include "UI_ShaderTools.h"
#include "folderlib.h"
#include "ChoseForm.h"
#include "UI_Main.h"
#include "ItemList.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CSHGameMtlPairTools::CSHGameMtlPairTools(ISHInit& init):ISHTools(init)
{
    m_MtlPair 			= 0;
    m_GameMtlTools		= 0;
}

CSHGameMtlPairTools::~CSHGameMtlPairTools()
{
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::OnFrame()
{
	inherited::OnFrame();
}
//---------------------------------------------------------------------------

bool CSHGameMtlPairTools::OnCreate()
{
	m_GameMtlTools		= STools->FindTools(aeMtl); R_ASSERT(m_GameMtlTools);
    Load();
    return true;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::OnDestroy()
{
    m_bModified = FALSE;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Reload()
{
	// mtl
    ResetCurrentItem();
    // mtl pair
    m_GameMtlTools->ResetCurrentItem();
    // load
    Load();
    // mtl pair
	m_GameMtlTools->FillItemList();
    FillItemList		();
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::FillItemList()
{
	ListItemsVec items;
    for (GameMtlIt m0_it=GMLib.FirstMaterial(); m0_it!=GMLib.LastMaterial(); m0_it++){
        SGameMtl* M0 		= *m0_it;
	    for (GameMtlIt m1_it=GMLib.FirstMaterial(); m1_it!=GMLib.LastMaterial(); m1_it++){
            SGameMtl* M1 	= *m1_it;
            GameMtlPairIt p_it = GMLib.GetMaterialPairIt(M0->GetID(),M1->GetID());
            if (p_it!=GMLib.LastMaterialPair())
                LHelper.CreateItem(items,GMLib.MtlPairToName(M0->GetID(),M1->GetID()),0);
        }
    }
	Ext.m_Items->AssignItems(items,false,true);
	m_MtlPair=0;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Load()
{
    m_bLockUpdate		= TRUE;

    GMLib.Unload		();
    GMLib.Load			();
    ResetCurrentItem	();

    m_bLockUpdate		= FALSE;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Save()
{
    m_bLockUpdate		= TRUE;

    // save
    EFS.UnlockFile		(_game_data_,GAMEMTL_FILENAME,false);
    EFS.BackupFile		(_game_data_,GAMEMTL_FILENAME);
    GMLib.Save			();
    EFS.LockFile		(_game_data_,GAMEMTL_FILENAME,false);
    
    m_bLockUpdate		= FALSE;

    m_bModified	= FALSE;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::RealUpdateProperties()
{
	PropItemVec items;
    if (m_MtlPair)	m_MtlPair->FillProp(items);
    Ext.m_ItemProps->AssignItems		(items);
    Ext.m_ItemProps->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::ApplyChanges(bool bForced)
{
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::OnActivate()
{
    FillItemList();
    Ext.m_Items->OnModifiedEvent= Modified;
    inherited::OnActivate		();
    m_StoreFlags				= Ext.m_Items->m_Flags.flags;
    Ext.m_Items->m_Flags.set	(TItemList::ilFolderStore);
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::OnDeactivate()
{
    inherited::OnDeactivate		();
    Ext.m_Items->m_Flags.set	(m_StoreFlags);
}

void CSHGameMtlPairTools::SetCurrentItem(LPCSTR name, bool bView)
{
    if (m_bLockUpdate) return;
	SGameMtlPair* S=GMLib.GetMaterialPair(name);
    // set material
	if (m_MtlPair!=S){
        m_MtlPair = S;
	    UI->Command(COMMAND_UPDATE_PROPERTIES);
	 	if (bView) ViewSetCurrentItem(name);
   }
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::ResetCurrentItem()
{
	m_MtlPair	= 0;
}
//---------------------------------------------------------------------------

