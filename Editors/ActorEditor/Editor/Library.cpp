//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Library.h"
#include "EditObject.h"
#include "ui_main.h"

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

void ELibrary::OnCreate()
{
	m_Current = "";
	Device.seqDevCreate.Add	(this,REG_PRIORITY_NORMAL);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);
    m_bReady = true;
}
//----------------------------------------------------

void ELibrary::OnDestroy()
{
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
    	xr_delete(O->second);
    }
	m_EditObjects.clear();
}
//----------------------------------------------------

void ELibrary::SetCurrentObject(LPCSTR nm)
{
	VERIFY(m_bReady);
	R_ASSERT(nm&&nm[0]);
    string256 name; strcpy(name,nm); strlwr(name);
    m_Current = name;
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
    string256 name; strcpy(name,nm); strlwr(name);
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

void ELibrary::EvictObjects()
{
	EditObjPairIt O = m_EditObjects.begin();
	EditObjPairIt E = m_EditObjects.end();
    for(; O!=E; O++)
    	O->second->EvictObject();
}
//----------------------------------------------------

CEditableObject* ELibrary::LoadEditObject(LPCSTR name, int age)
{
	VERIFY(m_bReady);
    CEditableObject* m_EditObject = xr_new<CEditableObject>(name);
    AnsiString fn=ChangeFileExt(name,".object");
    FS.update_path("$objects$",fn);
    if (FS.exist(fn.c_str())){
        if (m_EditObject->Load(fn.c_str())){
            m_EditObject->m_ObjVer.f_age = age;
            return m_EditObject;
        }
    }else{
		ELog.Msg(mtError,"Can't find object '%s'",name);
    }
    xr_delete(m_EditObject);
	return 0;
}
//---------------------------------------------------------------------------

CEditableObject* ELibrary::CreateEditObject(LPCSTR nm,int* age)
{
	VERIFY(m_bReady);
    R_ASSERT(nm&&nm[0]);
    AnsiString name;
    FS.update_path(name,"$objects$",nm);
    UI.ProgressInfo(name.c_str());
    CEditableObject* m_EditObject = 0;
    const CLocatorAPI::file* F = FS.exist(name.c_str());
    if (!F) return 0;
    if (age) *age 		= F->modif;
	EditObjPairIt it 	= m_EditObjects.find(name);
    if (it!=m_EditObjects.end())	m_EditObject = it->second;
    else if (m_EditObject=LoadEditObject(name.c_str(),F->modif))
		m_EditObjects[name]	= m_EditObject;
	if (m_EditObject) m_EditObject->m_RefCount++;
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
        	AnsiString nm;
		    FS.update_path(nm,"$objects$",O->second->GetName());
        	O->second->SaveObject(nm.c_str());
        }
}
//---------------------------------------------------------------------------

int ELibrary::GetObjects(FS_QueryMap& files)
{
    return FS.file_list(files,"$objects$",FS_ListFiles|FS_ClampExt,".object");
}
//---------------------------------------------------------------------------

