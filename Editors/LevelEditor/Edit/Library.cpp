//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Library.h"
#include "EditObject.h"
#include "SceneClassList.h"
#include "UI_Main.h"

//----------------------------------------------------
ELibrary Lib;
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
    Engine.FS.GetFileList(Engine.FS.m_Objects.m_Path,m_Objects,true,true,false,"*.object");
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
    for(; O!=E; O++){
    	if (0!=O->second->m_RefCount){
        	ELog.DlgMsg(mtError,"Object '%s' still referenced.",O->first);
	    	R_ASSERT(0==O->second->m_RefCount);
        }
    	delete O->second;
    }
	m_EditObjects.clear();

    m_Objects.clear();
}
//----------------------------------------------------

void ELibrary::SetCurrentObject(LPCSTR nm){
	VERIFY(m_bReady);
	R_ASSERT(nm&&nm[0]);
    sh_name name; strcpy(name,nm); strlwr(name);
    FilePairIt it = m_Objects.find(name);
    if (it!=m_Objects.end()) m_Current = name;
}
int ELibrary::ObjectCount(){
	VERIFY(m_bReady);
	return m_Objects.size();
}
//----------------------------------------------------

void ELibrary::RefreshLibrary(){
	VERIFY(m_bReady);
    OnDestroy();
    OnCreate();
}
//----------------------------------------------------
void ELibrary::ReloadObject(LPCSTR nm)
{
	VERIFY(m_bReady);
	R_ASSERT(nm&&nm[0]);
    sh_name name; strcpy(name,nm); strlwr(name);
	EditObjPairIt it = m_EditObjects.find(name);
    if (it!=m_EditObjects.end())
    	it->second->Reload();
}
//---------------------------------------------------------------------------
void ELibrary::ReloadObjects(){
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

CEditableObject* ELibrary::LoadEditObject(LPCSTR name, int age){
	VERIFY(m_bReady);
    CEditableObject* m_EditObject = new CEditableObject(name);
    AnsiString fn=ChangeFileExt(name,".object");
    Engine.FS.m_Objects.Update(fn);
    if (Engine.FS.Exist(fn.c_str(), true))
        if (m_EditObject->Load(fn.c_str())){
            m_EditObject->m_ObjVer.f_age = age;
            return m_EditObject;
        }
    _DELETE(m_EditObject);
	return m_EditObject;
}
//---------------------------------------------------------------------------

CEditableObject* ELibrary::CreateEditObject(LPCSTR nm,int* age)
{
	VERIFY(m_bReady);
    R_ASSERT(nm&&nm[0]);
    string1024 name; strcpy(name,nm); strlwr(name);
    CEditableObject* m_EditObject = 0;
    FilePairIt p_it = m_Objects.find(name);
    if (p_it==m_Objects.end()) return 0;
    UI.ProgressStart(2,"Object loading...");
    UI.ProgressInc();
    if (age) *age = p_it->second;
	EditObjPairIt it = m_EditObjects.find(name);
    if (it!=m_EditObjects.end())	m_EditObject = it->second;
    else if (m_EditObject=LoadEditObject(name,p_it->second))
		m_EditObjects[name] = m_EditObject;
	if (m_EditObject) m_EditObject->m_RefCount++;
    UI.ProgressEnd();
	return m_EditObject;
}
//---------------------------------------------------------------------------

void ELibrary::RemoveEditObject(CEditableObject*& object)
{
	if (object){
	    object->m_RefCount--;
    	R_ASSERT(object->m_RefCount>=0);
        object=0;
//S если нужно то удалить физически
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
            Engine.FS.m_Objects.Update(nm);
        	O->second->SaveObject(nm.c_str());
        }
    m_Objects.clear();
    Engine.FS.GetFileList(Engine.FS.m_Objects.m_Path,m_Objects,true,true,false,"*.object");
}
//---------------------------------------------------------------------------

