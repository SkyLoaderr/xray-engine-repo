//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHCompilerTools.h"
#include "ui_main.h"
#include "folderlib.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CSHCompilerTools::CSHCompilerTools(ISHInit& init):ISHTools(init)
{
	m_Shader		= 0;
}

CSHCompilerTools::~CSHCompilerTools(){
}
//---------------------------------------------------------------------------

void CSHCompilerTools::OnFrame()
{
}
//---------------------------------------------------------------------------

bool CSHCompilerTools::OnCreate()
{
    Load();
    return true;
}

void CSHCompilerTools::OnDestroy()
{
    m_bModified 		= FALSE;
}

void CSHCompilerTools::ApplyChanges(bool bForced)
{
}

void CSHCompilerTools::FillItemList()
{
    View()->IsUpdating 		= true;
	ViewClearItemList();
    Shader_xrLCVec& lst = m_Library.Library();
    for (Shader_xrLCIt it=lst.begin(); it!=lst.end(); it++)
        ViewAddItem(it->Name);
    View()->IsUpdating 		= false;
}

void CSHCompilerTools::Reload()
{
	ViewClearItemList	();
    ResetCurrentItem	();
    Load				();
}

void CSHCompilerTools::Load()
{
	AnsiString fn;
    FS.update_path(fn,_game_data_,"shaders_xrlc.xr");

    m_bLockUpdate			= TRUE;

    if (FS.exist(fn.c_str())){
    	m_Library.Load(fn.c_str());

        FillItemList			();

        ResetCurrentItem		();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	m_bLockUpdate			= FALSE;
}

void CSHCompilerTools::Save()
{
    ApplyChanges			();
    AnsiString name;
    FHelper.MakeFullName	(View()->Selected,0,name);
	ResetCurrentItem		();
	m_bLockUpdate			= TRUE;

    // save
	AnsiString fn;
    FS.update_path			(fn,_game_data_,"shaders_xrlc.xr");

    EFS.UnlockFile			(0,fn.c_str(),false);
    EFS.BackupFile			(_game_data_,"shaders_xrlc.xr");
    m_Library.Save			(fn.c_str());
    EFS.LockFile			(0,fn.c_str(),false);
	m_bLockUpdate			= FALSE;
	SetCurrentItem			(name.c_str());

    m_bModified	= FALSE;
}

Shader_xrLC* CSHCompilerTools::FindItem(LPCSTR name)
{
	if (name && name[0]){
    	return m_Library.Get(name);
    }else return 0;
}

LPCSTR CSHCompilerTools::GenerateItemName(LPSTR name, LPCSTR pref, LPCSTR source)
{
    int cnt = 0;
    if (source) strcpy(name,source); else sprintf(name,"%sshader_%02d",pref,cnt++);
	while (FindItem(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%sshader_%02d",pref,cnt++);
	return name;
}

LPCSTR CSHCompilerTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
	Shader_xrLC* parent 	= FindItem(parent_name);
    string64 new_name;
    GenerateItemName		(new_name,folder_name,parent_name);
    Shader_xrLC* S 			= m_Library.Append(parent);
    strcpy					(S->Name,new_name);
	ViewAddItem				(S->Name);
	SetCurrentItem			(S->Name);
	Modified				();
    return S->Name;
}

void CSHCompilerTools::RenameItem(LPCSTR old_full_name, LPCSTR ren_part, int level)
{
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    RenameItem(old_full_name, new_full_name);
}

void CSHCompilerTools::RenameItem(LPCSTR old_full_name, LPCSTR new_full_name)
{
    ApplyChanges();
	Shader_xrLC* S = FindItem(old_full_name); R_ASSERT(S);
    strcpy(S->Name,new_full_name);
	if (S==m_Shader){
    	*m_Shader = *S;
        UpdateProperties();
    }
}

void CSHCompilerTools::RemoveItem(LPCSTR name)
{
	R_ASSERT(name && name[0]);
    m_Library.Remove(name);
}

void CSHCompilerTools::SetCurrentItem(LPCSTR name)
{
    if (m_bLockUpdate) return;

	Shader_xrLC* S = FindItem(name);
    // load shader
	if (m_Shader!=S){
        m_Shader = S;
        UpdateProperties();
    }
	ViewSetCurrentItem(name);
}

void CSHCompilerTools::ResetCurrentItem()
{
	m_Shader=0;
}


