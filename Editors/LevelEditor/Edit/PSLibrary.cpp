//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSLibrary.h"
#include "xrParticlesLib.h"
#include "ChoseForm.h"

CPSLibrary PSLib;
//----------------------------------------------------
void CPSLibrary::OnCreate(){
	AnsiString fn;
    fn = PSLIB_FILENAME;
    Engine.FS.m_GameRoot.Update(fn);
	if (Engine.FS.Exist(fn.c_str(),true)){
    	if (!Load(fn.c_str())) ELog.DlgMsg(mtInformation,"PS Library: Unsupported version.");
    }
}

void CPSLibrary::OnDestroy(){
    m_PSs.clear();
}

PS::SDef* CPSLibrary::FindPS(const char* name){
	return psLibrary_FindUnsorted(name,m_PSs);
}

void CPSLibrary::RenamePS(PS::SDef* src, LPCSTR new_name){
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

PS::SDef* CPSLibrary::AppendPS(PS::SDef* src){
	if (src)
    	m_PSs.push_back(*src);
	else{
    	m_PSs.push_back(PS::SDef());
	    m_PSs.back().InitDefault();
    }
    return &m_PSs.back();
}

void CPSLibrary::RemovePS(const char* nm){
    PS::SDef* sh = FindPS(nm);
    if (sh) m_PSs.erase(sh);
}

char* CPSLibrary::GenerateName(char* name, const char* source){
    int cnt = 0;
	char fld[128]; strcpy(fld,name);
    if (source) strcpy(name,source); else sprintf(name,"%s\ps_%02d",fld,cnt++);
	while (FindPS(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%s\ps_%02d",fld,cnt++);
	return name;
}
//----------------------------------------------------
bool CPSLibrary::Load(const char* nm){
	return psLibrary_Load(nm,m_PSs);
}
//----------------------------------------------------
void CPSLibrary::Save(){
	AnsiString fn;
    fn = PSLIB_FILENAME;
    Engine.FS.m_GameRoot.Update(fn);
    Save(fn.c_str());
}
//----------------------------------------------------
void CPSLibrary::Save(const char* nm){
	psLibrary_Save(nm,m_PSs);
}
//----------------------------------------------------
int CPSLibrary::Merge(const char* nm){
	int cnt = 0;
/*    CPSLibrary L0;
    CPSLibrary L1;
    L0.OnCreate();
    L1.Load(nm);
    for (PSIt l0_it=L0.FirstPS(); l0_it!=L0.LastPS(); l0_it++){
        PS::SDef* l1 = L1.FindPS(l0_it->m_Name);
        if (l1&&(memcmp(l0_it,l1,sizeof(PS::SDef))!=0)){
        	char pref[255], name[255];
            strcpy(pref,l1->m_Name);
            strcat(pref,"_merge");
            AppendPS((const char*)GenerateName(name,pref),l1);
            cnt++;
        }
    }
    for (PSIt l1_it=L1.FirstPS(); l1_it!=L1.LastPS(); l1_it++)
        if (!L0.FindPS(l1_it->m_Name)){
        	AddPS(l1_it->m_Name,l1_it);
            cnt++;
        }
*/
	return cnt;
}
//----------------------------------------------------
void CPSLibrary::Reload(){
	OnDestroy();
    OnCreate();
	ELog.Msg( mtInformation, "PS Library was succesfully reloaded." );
}
//----------------------------------------------------
void CPSLibrary::Backup(){
	AnsiString fn = PSLIB_FILENAME;
    Engine.FS.BackupFile(fn);
}
//----------------------------------------------------
//void CPSLibrary::RestoreBackup(){
//	AnsiString fn = PSLIB_FILENAME;
//	if (FS.RestoreBackup(fn)==1) Load(fn.c_str());
//}
//----------------------------------------------------
PS::SDef* CPSLibrary::ChoosePS(bool bSetCurrent){
	LPCSTR T = TfrmChoseItem::SelectPS(0,m_CurrentPS.c_str());
    if (T){
    	if (bSetCurrent) m_CurrentPS = T;
        return FindPS(T);
    }else return 0;
}
//----------------------------------------------------

