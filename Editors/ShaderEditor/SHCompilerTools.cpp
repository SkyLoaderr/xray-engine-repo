//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHCompilerTools.h"
#include "Blender.h"
#include "UI_Tools.h"
#include "ui_main.h"
#include "LeftBar.h"
#include "xr_trims.h"
#include "folderlib.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CSHCompilerTools::CSHCompilerTools(){
    m_bModified 		= FALSE;
    m_LibShader 		= 0;
    m_bUpdateCurrent	= false;
}

CSHCompilerTools::~CSHCompilerTools(){
}
//---------------------------------------------------------------------------

void CSHCompilerTools::Modified(){
	m_bModified=TRUE;
	UI.Command(COMMAND_UPDATE_CAPTION);
    ApplyChanges();
}
//---------------------------------------------------------------------------

void CSHCompilerTools::Update()
{
}
//---------------------------------------------------------------------------

void CSHCompilerTools::OnCreate(){
    Load();
}

void CSHCompilerTools::OnDestroy(){
    m_bModified = FALSE;
}

bool CSHCompilerTools::IfModified(){
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The shaders has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CSHCompilerTools::ApplyChanges(){
    if (m_LibShader) *m_LibShader = m_EditShader;
}

void CSHCompilerTools::Reload(){
	fraLeftBar->ClearCShaderList();
    ResetCurrentShader();
    Load();
}

void CSHCompilerTools::Load(){
	AnsiString fn = "shaders_xrlc.xr";
    Engine.FS.m_GameRoot.Update(fn);

    m_bUpdateCurrent	= false;
    fraLeftBar->tvCompiler->IsUpdating = true;

    if (Engine.FS.Exist(fn.c_str())){
    	m_Library.Load(fn.c_str());

        Shader_xrLCVec lst = m_Library.Library();
        for (Shader_xrLCIt it=lst.begin(); it!=lst.end(); it++)
			fraLeftBar->AddCShader(it->Name);

        ResetCurrentShader		();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

    fraLeftBar->tvCompiler->IsUpdating = false;
	m_bUpdateCurrent			= true;
}

void CSHCompilerTools::Save(){
    ApplyChanges();
    AnsiString name;
    FHelper.MakeFullName		(fraLeftBar->tvCompiler->Selected,0,name);
	ResetCurrentShader			();
	m_bUpdateCurrent			= false;

    // save
	AnsiString fn = "shaders_xrlc.xr";
    // backup file
    Engine.FS.BackupFile		(&Engine.FS.m_GameRoot,fn);

    Engine.FS.m_GameRoot.Update	(fn);

    // save new file
    Engine.FS.UnlockFile		(0,fn.c_str(),false);
    m_Library.Save				(fn.c_str());
    Engine.FS.LockFile			(0,fn.c_str(),false);
	m_bUpdateCurrent			= true;
	SetCurrentShader			(name.c_str());

    m_bModified	= FALSE;
}

Shader_xrLC* CSHCompilerTools::FindShader(LPCSTR name){
	if (name && name[0]){
    	return m_Library.Get(name);
    }else return 0;
}

LPCSTR CSHCompilerTools::GenerateShaderName(LPSTR name, LPCSTR source){
    int cnt = 0;
	char fld[128]; strcpy(fld,name);
    if (source) strcpy(name,source); else sprintf(name,"%sshader_%02d",fld,cnt++);
	while (FindShader(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%sshader_%02d",fld,cnt++);
	return name;
}

Shader_xrLC* CSHCompilerTools::AppendShader(LPCSTR folder_name, Shader_xrLC* parent){
	// append blender
    char old_name[128]; if (parent) strcpy(old_name,parent->Name);
    Shader_xrLC* S = m_Library.Append(parent);
    char new_name[128]; new_name[0]=0;
    if (folder_name) strcpy(new_name,folder_name);
    GenerateShaderName(new_name,parent?old_name:0);
    strcpy(S->Name,new_name);
	fraLeftBar->AddCShader(S->Name);
    return S;              
}

Shader_xrLC* CSHCompilerTools::CloneShader(LPCSTR name){
	Shader_xrLC* S = m_Library.Get(name); R_ASSERT(S);
	return AppendShader(0,S);
}

void CSHCompilerTools::RenameShader(LPCSTR old_full_name, LPCSTR ren_part, int level){
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    RenameShader(old_full_name, new_full_name);
}

void CSHCompilerTools::RenameShader(LPCSTR old_full_name, LPCSTR new_full_name){
    ApplyChanges();
	Shader_xrLC* S = m_Library.Get(old_full_name); R_ASSERT(S);
    strcpy(S->Name,new_full_name);
	if (S==m_LibShader){
    	m_EditShader = *S;
        UpdateProperties();
    }
}

void CSHCompilerTools::RemoveShader(LPCSTR name){
	R_ASSERT(name && name[0]);
    m_Library.Remove(name);
}

void CSHCompilerTools::SetCurrentShader(Shader_xrLC* S){
	if (Tools.ActiveEditor()!=aeCompiler) return;
    if (!m_bUpdateCurrent) return;

    // save changes
    if (m_LibShader)
    	*m_LibShader = m_EditShader;
    // load shader
	if (m_LibShader!=S){
        m_LibShader = S;
        if (m_LibShader) m_EditShader = *m_LibShader;
        UpdateProperties();
    }
	if (S) fraLeftBar->SetCurrentCShader(S->Name);
}

void CSHCompilerTools::ResetCurrentShader(){
	m_LibShader=0;
}

void CSHCompilerTools::SetCurrentShader(LPCSTR name){
	Shader_xrLC* S=FindShader(name);
	SetCurrentShader(S);
	if (!S) fraLeftBar->SetCurrentCShader(name);
}

