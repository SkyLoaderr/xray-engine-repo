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
ELibrary Lib;
//----------------------------------------------------
static bool sort_pred(AnsiString& A, AnsiString& B)
{	return A<B; }
//----------------------------------------------------

ELibrary::ELibrary(){
	m_Current = "";
    m_bReady  = false;
}
//----------------------------------------------------

ELibrary::~ELibrary(){
}
//----------------------------------------------------

void ELibrary::OnCreate(){
	m_Current = "";
    int count = strlen(FS.m_Objects.m_Path);
    AStringVec& lst = FS.GetFiles(FS.m_Objects.m_Path);
    for (AStringIt it=lst.begin(); it!=lst.end(); it++){
	    AnsiString ext = ExtractFileExt(*it).LowerCase();
        if (ext==".object"){
        	AnsiString name = it->Delete(1,count);
            name = name.Delete(name.Length()-6,7);
        	m_Objects.push_back(name);
        }
    }
	std::sort(m_Objects.begin(),m_Objects.end(),sort_pred);
	Device.seqDevCreate.Add	(this,REG_PRIORITY_NORMAL);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);
    m_bReady = true;
}
//----------------------------------------------------

void ELibrary::OnDestroy(){
	VERIFY(m_bReady);
    m_bReady = false;
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
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
	VERIFY(m_bReady);
    bool b = binary_search(m_Objects.begin(),m_Objects.end(),AnsiString(T),sort_pred);
    if (b) m_Current = T;
}
int ELibrary::ObjectCount(){
	VERIFY(m_bReady);
	return m_Objects.size();
}
//----------------------------------------------------

void ELibrary::UnloadMeshes(){
	VERIFY(m_bReady);
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++) delete O->second;
	m_EditObjects.clear();
}
//----------------------------------------------------
void ELibrary::ReloadLibrary(){
	VERIFY(m_bReady);
    OnDestroy();
    OnCreate();
}
//----------------------------------------------------
void ELibrary::RefreshLibrary(){
	VERIFY(m_bReady);
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++)
    	O->second->Reload();
}
//----------------------------------------------------

void ELibrary::OnDeviceCreate(){
	VERIFY(m_bReady);
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++)
    	O->second->OnDeviceCreate();
}
//---------------------------------------------------------------------------

void ELibrary::OnDeviceDestroy(){
	VERIFY(m_bReady);
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++)
    	O->second->OnDeviceDestroy();
}
//---------------------------------------------------------------------------

CEditableObject* ELibrary::LoadEditObject(LPCSTR name){
	VERIFY(m_bReady);
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
	VERIFY(m_bReady);
    R_ASSERT(name&&name[0]);
    CEditableObject* m_EditObject = 0;
	AStringIt P = lower_bound(m_Objects.begin(),m_Objects.end(),AnsiString(name),sort_pred);
    if (P==m_Objects.end()) return 0;
	EditObjPairIt it = m_EditObjects.find(AnsiString(name));
    if (it!=m_EditObjects.end())	m_EditObject = it->second;
    else if (m_EditObject=LoadEditObject(name))
		m_EditObjects[name] = m_EditObject;
	return m_EditObject;
}
//---------------------------------------------------------------------------

void ELibrary::UnloadObject(LPCSTR N)
{
	VERIFY(m_bReady);
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
	VERIFY(m_bReady);
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

