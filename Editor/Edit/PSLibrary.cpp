//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "FileSystem.h"
#include "xrParticlesLib.h"
#include "ChoseForm.h"

CPSLibrary* PSLib;
//----------------------------------------------------
PS::SDef* CPSLibrary::FindPS(const char* name){
	return psLibrary_Find(name,m_PSs);
}

PS::SDef* CPSLibrary::AddPS(const char* name, PS::SDef* src){
	if (src)
    	m_PSs.push_back(*src);
	else{ 	
    	m_PSs.push_back(PS::SDef());
	    m_PSs.back().InitDefault();
    }
    m_PSs.back().SetName(name);
    Sort();
    return FindPS(name);
}

void CPSLibrary::DeletePS(const char* nm){
    PS::SDef* sh = FindPS(nm);
    if (sh) m_PSs.erase(sh);
}

void CPSLibrary::Sort(){
	psLibrary_Sort(m_PSs);
}

void CPSLibrary::Init(){
	AnsiString fn;
    fn = PSLIB_FILENAME;
    FS.m_GameRoot.Update(fn);
	if (FS.Exist(fn.c_str(),true)){
    	if (!Load(fn.c_str())) Log->DlgMsg(mtInformation,"PS Library: Unsupported version.");
    }
}

void CPSLibrary::Clear(){
    m_PSs.clear();
}

char* CPSLibrary::GenerateName(char* buffer, const char* pref){
    int m_LastAvail = 0;
	do{
    	if (!pref) 	sprintf(buffer, "ps_%04d", m_LastAvail);
        else		sprintf(buffer, "%s_%d", pref, m_LastAvail);
		m_LastAvail++;
	} while( FindPS( buffer ) );
    return buffer;
}
//----------------------------------------------------
bool CPSLibrary::Load(const char* nm){
	return psLibrary_Load(nm,m_PSs);
}
//----------------------------------------------------
void CPSLibrary::Save(){
	AnsiString fn;
    fn = PSLIB_FILENAME;
    FS.m_GameRoot.Update(fn);
	psLibrary_Sort(m_PSs);
    Save(fn.c_str());
}
//----------------------------------------------------
void CPSLibrary::Save(const char* nm){
	psLibrary_Save(nm,m_PSs);
}
//----------------------------------------------------
int CPSLibrary::Merge(const char* nm){
	int cnt = 0;
    CPSLibrary L0;
    CPSLibrary L1;
    L0.Init();
    L1.Load(nm);
    for (PSIt l0_it=L0.FirstPS(); l0_it!=L0.LastPS(); l0_it++){
        PS::SDef* l1 = L1.FindPS(l0_it->m_Name);
        if (l1&&(memcmp(l0_it,l1,sizeof(PS::SDef))!=0)){
        	char pref[255], name[255];
            strcpy(pref,l1->m_Name);
            strcat(pref,"_merge");
            AddPS((const char*)GenerateName(name,pref),l1);
            cnt++;
        }
    }
    for (PSIt l1_it=L1.FirstPS(); l1_it!=L1.LastPS(); l1_it++)
        if (!L0.FindPS(l1_it->m_Name)){ 
        	AddPS(l1_it->m_Name,l1_it);
            cnt++;
        }
	return cnt;
}
//----------------------------------------------------
void CPSLibrary::Reload(){
	Clear();
    Init();
	Log->Msg( mtInformation, "PS Library was succesfully reloaded." );
}
//----------------------------------------------------
void CPSLibrary::Backup(){
	AnsiString fn = PSLIB_FILENAME;
    FS.BackupFile(fn);
}
//----------------------------------------------------
void CPSLibrary::RestoreBackup(){
	AnsiString fn = PSLIB_FILENAME;
	if (FS.RestoreBackup(fn)==1) Load(fn.c_str());
}
//----------------------------------------------------
PS::SDef* CPSLibrary::ChoosePS(bool bSetCurrent){
	LPCSTR T = TfrmChoseItem::SelectPS();
    if (T){
    	if (bSetCurrent) m_CurrentPS = T;
        return FindPS(T);
    }else return 0;
}
//----------------------------------------------------

