//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHGameMaterialTools.h"
#include "Blender.h"
#include "UI_Tools.h"
#include "ui_main.h"
#include "LeftBar.h"
#include "xr_trims.h"
#include "folderlib.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CSHGameMaterialTools::CSHGameMaterialTools()
{
    m_bModified 		= FALSE;
    m_Mtl 				= 0;
    m_MtlPair 			= 0;
    m_bUpdateCurrent	= false;
}

CSHGameMaterialTools::~CSHGameMaterialTools()
{
}
//---------------------------------------------------------------------------

void CSHGameMaterialTools::Modified()
{
	m_bModified=TRUE;
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CSHGameMaterialTools::OnFrame()
{
}
//---------------------------------------------------------------------------

void CSHGameMaterialTools::OnCreate()
{
    Load();
}

void CSHGameMaterialTools::OnDestroy()
{
    m_bModified = FALSE;
}

bool CSHGameMaterialTools::IfModified()
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

void CSHGameMaterialTools::Reload()
{
	fraLeftBar->ClearItemList();
    fraLeftBar->ClearItemList(); //. pair list
    ResetCurrentMaterial();
    Load();
}

void CSHGameMaterialTools::FillMtlsView()
{
	fraLeftBar->ClearItemList();
    for (GameMtlIt m_it=GMLib.FirstMaterial(); m_it!=GMLib.LastMaterial(); m_it++)
        fraLeftBar->AddItem((*m_it)->name);
}
void CSHGameMaterialTools::FillMtlPairsView()
{
	fraLeftBar->ClearItemList();
    for (GameMtlPairIt p_it=GMLib.FirstMaterialPair(); p_it!=GMLib.LastMaterialPair(); p_it++)
        fraLeftBar->AddItem(GMLib.MtlPairToName((*p_it)->GetMtl0(),(*p_it)->GetMtl1()));
	m_MtlPair=0;
}

void CSHGameMaterialTools::Load()
{
	AnsiString fn;
    FS.update_path		(fn,_game_data_,"gamemtl.xr");

    m_bUpdateCurrent	= false;
    fraLeftBar->tvMaterial->IsUpdating = true;

    if (FS.exist(fn.c_str())){
    	GMLib.Load(fn.c_str());

        FillMtlsView();
        FillMtlPairsView();

        ResetCurrentMaterial	();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

    fraLeftBar->tvMaterial->IsUpdating = false;
	m_bUpdateCurrent			= true;
}

void CSHGameMaterialTools::Save()
{
    AnsiString name;
    FHelper.MakeFullName		(fraLeftBar->tvMaterial->Selected,0,name);
	ResetCurrentMaterial		();
	m_bUpdateCurrent			= false;

    // save
	AnsiString fn;
    FS.update_path				(fn,_game_data_,"gamemtl.xr");
    // backup file
    EFS.BackupFile				(0,fn);
    // save new file
    EFS.UnlockFile				(0,fn.c_str(),false);
    GMLib.Save					(fn.c_str());
    EFS.LockFile				(0,fn.c_str(),false);
	m_bUpdateCurrent			= true;
	SetCurrentMaterial			(name.c_str());

    m_bModified	= FALSE;
}

SGameMtl* CSHGameMaterialTools::FindMaterial(LPCSTR name)
{
	if (name && name[0]){
    	return GMLib.GetMaterial(name);
    }else return 0;
}

LPCSTR CSHGameMaterialTools::GenerateMaterialName(LPSTR name, LPCSTR source)
{
    int cnt = 0;
	char fld[128]; strcpy(fld,name);
    if (source) strcpy(name,source); else sprintf(name,"%sgm_%02d",fld,cnt++);
	while (FindMaterial(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%sgm_%02d",fld,cnt++);
	return name;
}

SGameMtl* CSHGameMaterialTools::AppendMaterial(LPCSTR folder_name, SGameMtl* parent)
{
	// append material
    char old_name[128]; if (parent) strcpy(old_name,parent->name);
    SGameMtl* S = GMLib.AppendMaterial(parent);
    string128 new_name; new_name[0]=0;
    if (folder_name) strcpy(new_name,folder_name);
    GenerateMaterialName(new_name,parent?old_name:0);
    strcpy(S->name,new_name);
	fraLeftBar->AddItem(S->name);
    return S;              
}

SGameMtl* CSHGameMaterialTools::CloneMaterial(LPCSTR name){
	SGameMtl* S = GMLib.GetMaterial(name); R_ASSERT(S);
	return AppendMaterial(0,S);
}

void CSHGameMaterialTools::RenameMaterial(LPCSTR old_full_name, LPCSTR ren_part, int level)
{
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    RenameMaterial(old_full_name, new_full_name);
}

void CSHGameMaterialTools::RenameMaterial(LPCSTR old_full_name, LPCSTR new_full_name)
{
	SGameMtl* S = GMLib.GetMaterial(old_full_name); R_ASSERT(S);
    strcpy(S->name,new_full_name);
	if (S==m_Mtl)	UpdateProperties();

    // нужно переинициализировать лист пар
    OnMaterialNameChange(0);
}

void CSHGameMaterialTools::RemoveMaterial(LPCSTR name)
{
	R_ASSERT(name && name[0]);
    GMLib.RemoveMaterial(name);
    // нужно переинициализировать лист пар
	FillMtlPairsView	();
}

void CSHGameMaterialTools::SetCurrentMaterial(SGameMtl* S)
{
	if (Tools.ActiveEditor()!=aeMaterial) return;
    if (!m_bUpdateCurrent) return;

    // load material
	if (m_Mtl!=S){
        m_Mtl = S;
        UpdateProperties();
    }
	if (S) fraLeftBar->SetCurrentItem(S->name);
}

void CSHGameMaterialTools::ResetCurrentMaterial()
{
	m_Mtl=0;
	m_MtlPair=0;
}

void CSHGameMaterialTools::SetCurrentMaterial(LPCSTR name)
{
	SGameMtl* S=FindMaterial(name);
	SetCurrentMaterial(S);
	if (!S) fraLeftBar->SetCurrentItem(name);
}
//---------------------------------------------------------------------------

void __fastcall CSHGameMaterialTools::OnMaterialNameChange(PropValue* sender)
{
    // нужно переинициализировать лист пар
	FillMtlPairsView	();
}

void CSHGameMaterialTools::UpdateProperties()
{
    TProperties* P		= Tools.m_Props;
	PropItemVec items;
	switch (Tools.ActiveEditor()){
    case aeMaterial: 	if (m_Mtl) m_Mtl->FillProp	(items);		break;
    case aeMaterialPair:if (m_MtlPair)	m_MtlPair->FillProp(items);	break;
    }
    P->AssignItems		(items,true);
    P->SetModifiedEvent	(Modified);
}
//---------------------------------------------------------------------------

void CSHGameMaterialTools::ApplyChanges()
{
}
//---------------------------------------------------------------------------

SGameMtlPair* CSHGameMaterialTools::AppendMaterialPair(int m0, int m1, SGameMtlPair* parent)
{
    SGameMtlPair* S = GMLib.AppendMaterialPair(m0,m1,parent);
    fraLeftBar->AddItem(GMLib.MtlPairToName(S->GetMtl0(),S->GetMtl1()));
    return S;              
}
//---------------------------------------------------------------------------

void CSHGameMaterialTools::SetCurrentMaterialPair(SGameMtlPair* S)
{
	if (Tools.ActiveEditor()!=aeMaterialPair) return;
    if (!m_bUpdateCurrent) return;

    // load material
	if (m_MtlPair!=S){
        m_MtlPair = S;
        UpdateProperties();
    }
	if (S) fraLeftBar->SetCurrentItem(GMLib.MtlPairToName(S->GetMtl0(),S->GetMtl1()));
}
//---------------------------------------------------------------------------

void CSHGameMaterialTools::SetCurrentMaterialPair(LPCSTR name)
{
	SGameMtlPair* S=GMLib.GetMaterialPair(name);
	SetCurrentMaterialPair(S);
	if (!S) fraLeftBar->SetCurrentItem(name);
}
//---------------------------------------------------------------------------

void CSHGameMaterialTools::RemoveMaterialPair(LPCSTR name)
{
	R_ASSERT(name && name[0]);
    GMLib.RemoveMaterialPair(name);
}
//---------------------------------------------------------------------------

