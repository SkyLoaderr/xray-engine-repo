//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHEngineTools.h"
#include "Blender.h"
#include "ShaderTools.h"
#include "ui_main.h"
#include "LeftBar.h"
#include "PropertiesShader.h"
#include "xr_trims.h"
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
	UI->Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CSHCompilerTools::OnCreate(){
    Load();
}

void CSHCompilerTools::OnDestroy(){
    ClearData();

    // hide properties window
	TfrmShaderProperties::HideProperties();
    m_bModified = FALSE;
}

void CSHCompilerTools::ClearData(){
    // free constants, matrices, blenders
/*	// Shader
	for (ShaderPairIt b=m_Shaders.begin(); b!=m_Shaders.end(); b++)
	{
		free		(b->first);
		delete		b->second;
	}
	m_Shaders.clear	();

	// Matrices
	for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++){
		free		(m->first);
		delete		m->second;
	}
	m_Matrices.clear	();

	// Constants
	for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++){
		free		(c->first);
		delete		c->second;
	}
	m_Constants.clear	();
*/
}

bool CSHCompilerTools::IfModified(){
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The shaders has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI->Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CSHCompilerTools::ApplyChanges(){
    if (m_LibShader){
		*m_LibShader = m_EditShader;
		Modified();
    }
}

void CSHCompilerTools::Reload(){
	fraLeftBar->ClearCShaderList();
    ResetCurrentShader();
    ClearData();
    Load();
}

void CSHCompilerTools::Load(){
	AnsiString fn = "shaders_xrlc.xr";
    FS.m_GameRoot.Update(fn);

    m_bUpdateCurrent	= false;

    if (FS.Exist(fn.c_str())){
    	m_Library.Load(fn.c_str());

        Shader_xrLCVec lst = m_Library.Library();
        for (Shader_xrLCIt it=lst.begin(); it!=lst.end(); it++)
			fraLeftBar->AddCShader(it->Name,true);

        ResetCurrentShader		();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	m_bUpdateCurrent			= true;
}

void CSHCompilerTools::Save(){
    ApplyChanges();
    LPCSTR name					= m_LibShader?m_LibShader->Name:0;
	ResetCurrentShader			();
	m_bUpdateCurrent			= false;

    // save
	AnsiString fn = "shaders_xrlc.xr";
    FS.m_GameRoot.Update(fn);

    // copy exist file
    FS.MarkFile(fn);

    // save new file
    m_Library.Save			(fn.c_str());
	m_bUpdateCurrent		= true;
	SetCurrentShader		(name);

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
    if (source) strcpy(name,source); else sprintf(name,"%s\shader_%02d",fld,cnt++);
	while (FindShader(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%s\shader_%02d",fld,cnt++);
	return name;
}

Shader_xrLC* CSHCompilerTools::AppendShader(LPCSTR folder_name, Shader_xrLC* parent){
	// append blender
    Shader_xrLC* S = m_Library.Append(parent);
    char new_name[128]; new_name[0]=0;
    if (folder_name) strcpy(new_name,folder_name);
    GenerateShaderName(new_name,parent?parent->Name:0);
    strcpy(S->Name,new_name);
	fraLeftBar->AddCShader(S->Name,false);
    return S;
}

Shader_xrLC* CSHCompilerTools::CloneShader(LPCSTR name){
	Shader_xrLC* S = m_Library.Get(name); R_ASSERT(S);
	return AppendShader(0,S);
}

void CSHCompilerTools::RenameShader(LPCSTR old_full_name, LPCSTR ren_part, int level){
	Shader_xrLC* S = m_Library.Get(old_full_name); R_ASSERT(S);
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    _ReplaceItem(old_full_name,level,ren_part,S->Name,'\\');
	if (S==m_LibShader){
    	m_EditShader = *S;
    	TfrmShaderProperties::InitProperties();
    }
}

void CSHCompilerTools::RemoveShader(LPCSTR name){
	R_ASSERT(name && name[0]);
    m_Library.Remove(name);
}

void CSHCompilerTools::SetCurrentShader(Shader_xrLC* S){
    if (!m_bUpdateCurrent) return;

    // save changes
    if (m_LibShader)
    	*m_LibShader = m_EditShader;
    // load shader
	if (m_LibShader!=S){
        m_LibShader = S;
        if (m_LibShader) m_EditShader = *m_LibShader;
		TfrmShaderProperties::InitProperties();
    }
}

void CSHCompilerTools::ResetCurrentShader(){
	m_LibShader=0;
}

void CSHCompilerTools::SetCurrentShader(LPCSTR name){
	SetCurrentShader(FindShader(name));
}

