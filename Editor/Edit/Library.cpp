//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Library.h"
#include "EditObject.h"
#include "SceneClassList.h"

//----------------------------------------------------
/*
bool CLibObject::ImportFrom(const char* name){
    _DELETE(m_EditObject);
    m_EditObject = new CEditableObject(this);
    if (FS.Exist(name, true))
        if (m_EditObject->Load(name)){
        	m_SrcName = ExtractFileName(AnsiString(name));
            m_EditObject->m_ObjVer.f_age = FS.GetFileAge(AnsiString(name));
            return true;
        }
    _DELETE(m_EditObject);
    return false;
}
*/
//----------------------------------------------------
ELibrary* Lib;
//----------------------------------------------------
static bool sort_pred(AnsiString& A, AnsiString& B)
{	return A<B; }
//----------------------------------------------------

ELibrary::ELibrary(){
	m_Current = 0;
	Device.seqDevCreate.Add	(this,REG_PRIORITY_NORMAL);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);
}
//----------------------------------------------------

ELibrary::~ELibrary(){
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	Clear();
}
//----------------------------------------------------

void ELibrary::Init(){
	m_Current = 0;
    int count = strlen(FS.m_Objects.m_Path);
    AStringVec& lst = FS.GetFiles(FS.m_Objects.m_Path);
    for (AStringIt it=lst.begin(); it!=lst.end(); it++){
	    AnsiString ext = ExtractFileExt(*it).LowerCase();
        if (ext==".object") m_Objects.push_back(it->Delete(1,count));
    }
	std::sort(m_Objects.begin(),m_Objects.end(),sort_pred);
}
//----------------------------------------------------

void ELibrary::Clear(){
	m_Current = "";

    // remove all instance CEditableObject
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++) delete O->second;
	m_EditObjects.clear();

    m_Objects.clear();
	ELog.Msg( mtInformation, "Lib: cleared" );
}
//----------------------------------------------------

void ELibrary::SetCurrentObject(LPCSTR T){
	AStringIt P = lower_bound(m_Objects.begin(),m_Objects.end(),AnsiString(T),sort_pred);
    if (P!=m_Objects.end()) m_Current = *P;
}
int ELibrary::ObjectCount(){
	return m_Objects.size();
}
//----------------------------------------------------

void ELibrary::UnloadMeshes(){
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++) delete O->second;
	m_EditObjects.clear();
}
//----------------------------------------------------
void ELibrary::ReloadLibrary(){
    Clear();
    Init();
}
//----------------------------------------------------
void ELibrary::RefreshLibrary(){
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
//    for(; O!=E; O++)
//    	delete O->second;
}
//----------------------------------------------------

void ELibrary::OnDeviceCreate(){
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++)
    	O->second->OnDeviceCreate();
}
//---------------------------------------------------------------------------

void ELibrary::OnDeviceDestroy(){
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++)
    	O->second->OnDeviceDestroy();
}
//---------------------------------------------------------------------------

CEditableObject* ELibrary::LoadEditObject(LPCSTR name){
    CEditableObject* m_EditObject = new CEditableObject(name);
    AnsiString fn=ChangeFileExt(name,".object");
    FS.m_Objects.Update(fn);
    if (FS.Exist(fn.c_str(), true))
        if (m_EditObject->Load(fn.c_str())){
            m_EditObject->m_ObjVer.f_age = FS.GetFileAge(fn);
            return m_EditObject;
        }
    _DELETE(m_EditObject);
	return m_EditObject;
}
//---------------------------------------------------------------------------

CEditableObject* ELibrary::GetEditObject(LPCSTR name)
{
    CEditableObject* m_EditObject = 0;
	AStringIt P = lower_bound(m_Objects.begin(),m_Objects.end(),AnsiString(name),sort_pred);
    if (P==m_Objects.end()) return 0;
	EditObjPairIt it = m_EditObjects.find(AnsiString(name));
    if ((it==m_EditObjects.end())&&(m_EditObject=LoadEditObject(name)))
		m_EditObjects[name] = m_EditObject;
	return m_EditObject;
}
//---------------------------------------------------------------------------

void ELibrary::UnloadObject(LPCSTR N)
{
	R_ASSERT(N&&N[0]);
	EditObjPairIt it = m_EditObjects.find(AnsiString(N));
    if (it!=m_EditObjects.end()){
    	delete it->second;
		m_EditObjects.erase(it);
    }
}
//---------------------------------------------------------------------------

void ELibrary::Save()
{
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++)
    	if (O->second->IsModified()){
        	AnsiString nm=O->second->GetName();
            FS.m_Objects.Update(nm);
        	O->second->SaveObject(nm.c_str());
        }
}
//---------------------------------------------------------------------------

