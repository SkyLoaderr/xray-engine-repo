//----------------------------------------------------
// file: Shader.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Shader.h"
#include "ui_main.h"
#include "xrShaderLib.h"
#include "Library.h"

CShaderLibrary* SHLib;
//----------------------------------------------------
//----------------------------------------------------

SH_ShaderDef* CShaderLibrary::AddShader(AnsiString& name, SH_ShaderDef* src){
	if (src)m_Shaders.push_back(*src);
	else{ 	
    	m_Shaders.push_back(SH_ShaderDef());
	    m_Shaders.back().Init();
    }
    m_Shaders.back().SetName(name.c_str());
	shLibrary_Sort(m_Shaders);
    return shLibrary_Find(name.c_str(), m_Shaders);
}

void CShaderLibrary::DeleteShader(AnsiString& name){
    SH_ShaderDef* sh = shLibrary_Find(name.c_str(), m_Shaders);
    if (sh)
    	m_Shaders.erase(sh);
}

void CShaderLibrary::Sort(){
	shLibrary_Sort(m_Shaders);
}

void CShaderLibrary::Clear(){
//	FS.deletefile(SHLIB_BACKUPNAME);
    m_Shaders.clear();
}

bool CShaderLibrary::SearchName(char *name){
    for(ShaderIt it=m_Shaders.begin(); it!=m_Shaders.end(); it++)
		if( 0==stricmp(it->cName,name) ) return true;
	return false;
}

void CShaderLibrary::GenerateName(AnsiString& buffer, AnsiString* pref){
    int m_LastAvail = 0;
	do{
    	if (!pref) 	buffer.sprintf( "shader_%04d", m_LastAvail );
        else		buffer.sprintf( "%s_%d", pref->c_str(), m_LastAvail );
		m_LastAvail++;
	} while( SearchName( buffer.c_str() ) );
}
//----------------------------------------------------
bool CShaderLibrary::LoadExternal(const char* nm){
	Clear();
    shLibrary_Load(nm, m_Shaders);
	return true;
}
//----------------------------------------------------
int CShaderLibrary::Merge(const char* nm){
	int cnt = 0;
    CShaderLibrary L0;
    CShaderLibrary L1;
    L0.Init();
    L1.LoadExternal(nm);
    for (ShaderIt l0_it=L0.FirstShader(); l0_it!=L0.LastShader(); l0_it++){
        SH_ShaderDef* l1 = L1.FindShader(AnsiString(l0_it->cName));
        if (l1&&(memcmp(l0_it,l1,sizeof(SH_ShaderDef))!=0)){
            AnsiString pref = AnsiString(l1->cName)+AnsiString("_merge");
            AnsiString name;
            GenerateName(name,&pref);
            AddShader(name,l1);
            cnt++;
        }
    }
    for (ShaderIt l1_it=L1.FirstShader(); l1_it!=L1.LastShader(); l1_it++)
        if (!L0.FindShader(AnsiString(l1_it->cName))){
        	AddShader(AnsiString(l1_it->cName),l1_it);
            cnt++;
        }
	return cnt;
}
//----------------------------------------------------
bool CShaderLibrary::Load(const char* nm){
  	AnsiString fn;
    fn = nm;
    FS.m_GameRoot.Update(fn);
    if (FS.Exist(fn.c_str(),true)){
    	shLibrary_Load(fn.c_str(), m_Shaders);
        return true;
    }else return false;
}
//----------------------------------------------------
void CShaderLibrary::Reload(const char* nm){
	Clear();
    Load(nm);
	Log->Msg( mtInformation, "Shader Library was succesfully reloaded." );
    UI->Device.ReloadShaders();
}
//----------------------------------------------------

void CShaderLibrary::SaveLibrary(const char* nm){
  	AnsiString fn=nm;
    FS.m_GameRoot.Update(fn);
    shLibrary_Save(fn.c_str(), m_Shaders);
    UI->Device.ReloadShaders();
}
//----------------------------------------------------

void CShaderLibrary::Backup(){
  	AnsiString fn=SHLIB_FILENAME;
    FS.m_GameRoot.Update(fn);
    FS.BackupFile(fn);
//	UI->Device.ReloadShaders();
//	SaveLibrary(SHLIB_BACKUPNAME);
}

void CShaderLibrary::RestoreBackup(){
  	AnsiString fn=SHLIB_FILENAME;
    FS.m_GameRoot.Update(fn);
	if (FS.RestoreBackup(fn)==1) Reload(SHLIB_FILENAME);
//	SaveLibrary(SHLIB_FILENAME);
}

