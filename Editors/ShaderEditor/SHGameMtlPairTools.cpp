//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHGameMtlPairTools.h"
#include "UI_ShaderTools.h"
#include "folderlib.h"
#include "ChoseForm.h"
#include "UI_Main.h"
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
	ViewClearItemList();
    ResetCurrentItem();
    // mtl pair
	m_GameMtlTools->ViewClearItemList();
    m_GameMtlTools->ResetCurrentItem();
    // load
    Load();
    // mtl pair
	m_GameMtlTools->FillItemList();
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::FillItemList()
{
    View()->IsUpdating = true;
	ViewClearItemList();
    for (GameMtlIt m0_it=GMLib.FirstMaterial(); m0_it!=GMLib.LastMaterial(); m0_it++){
        SGameMtl* M0 		= *m0_it;
	    for (GameMtlIt m1_it=GMLib.FirstMaterial(); m1_it!=GMLib.LastMaterial(); m1_it++){
            SGameMtl* M1 	= *m1_it;
            GameMtlPairIt p_it = GMLib.GetMaterialPairIt(M0->GetID(),M1->GetID());
            if (p_it!=GMLib.LastMaterialPair())
				ViewAddItem(GMLib.MtlPairToName(M0->GetID(),M1->GetID()));
        }
    }
//	for (GameMtlPairIt p_it=GMLib.FirstMaterialPair(); p_it!=GMLib.LastMaterialPair(); p_it++)
//		ViewAddItem(GMLib.MtlPairToName((*p_it)->GetMtl0(),(*p_it)->GetMtl1()));
	m_MtlPair=0;
    View()->IsUpdating = false;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Load()
{
    m_bLockUpdate		= TRUE;

    GMLib.Unload		();
    GMLib.Load			();
    FillItemList		();
    ResetCurrentItem	();

    m_bLockUpdate		= FALSE;
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::Save()
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
//---------------------------------------------------------------------------

LPCSTR CSHGameMtlPairTools::GenerateItemName(LPSTR name, LPCSTR pref, LPCSTR source)
{
	return 0;
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
}
//---------------------------------------------------------------------------

LPCSTR CSHGameMtlPairTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
	if (GMLib.UpdateMtlPairs()) 
    	Modified();
    FillItemList();     
/*
    LPCSTR M0=0,M1=0;
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smGameMaterial,M0,1)){
	    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smGameMaterial,M1,1)){
        	int mtl0			= GMLib.GetMaterialID	(M0);
        	int mtl1			= GMLib.GetMaterialID	(M1);
	        SGameMtlPair* parent= GMLib.GetMaterialPair(parent_name);
			SGameMtlPair* S 	= GMLib.AppendMaterialPair(mtl0,mtl1,parent);
	        AnsiString nm		= GMLib.MtlPairToName(S->GetMtl0(),S->GetMtl1());
		    ViewAddItem			(nm.c_str());
            SetCurrentItem		(nm.c_str());
            Modified			();
	        return nm.c_str();
        }
    }
*/
    return 0;              
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::SetCurrentItem(LPCSTR name)
{
    if (m_bLockUpdate) return;
	SGameMtlPair* S=GMLib.GetMaterialPair(name);
    // set material
	if (m_MtlPair!=S){
        m_MtlPair = S;
	    UI->Command(COMMAND_UPDATE_PROPERTIES);
    }
	ViewSetCurrentItem(name);
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::RemoveItem(LPCSTR name)
{
//	R_ASSERT(name && name[0]);
//	GMLib.RemoveMaterialPair(name);
}
//---------------------------------------------------------------------------

void CSHGameMtlPairTools::ResetCurrentItem()
{
	m_MtlPair	= 0;
}
//---------------------------------------------------------------------------

