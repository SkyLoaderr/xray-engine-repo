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
ELibrary::ELibrary()
{
    m_bReady  = false;
}
//----------------------------------------------------

ELibrary::~ELibrary()
{
}
//----------------------------------------------------

void ELibrary::OnCreate()
{
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

void ELibrary::RefreshLibrary()
{
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

CEditableObject* ELibrary::LoadEditObject(LPCSTR name)
{
	VERIFY(m_bReady);
    CEditableObject* m_EditObject = xr_new<CEditableObject>(name);
    AnsiString fn=ChangeFileExt(name,".object");
    FS.update_path(_objects_,fn);
    const CLocatorAPI::file* F = FS.exist(fn.c_str());
    if (F){
        if (m_EditObject->Load(fn.c_str())){
        	if (m_EditObject->m_Version!=(int)F->modif){
	            m_EditObject->m_Version = F->modif;
//                m_EditObject->Modified();
            }
            return m_EditObject;
        }
    }else{
		ELog.Msg(mtError,"Can't find file '%s'",fn);
    }
    xr_delete(m_EditObject);
	return 0;
}
//---------------------------------------------------------------------------

CEditableObject* ELibrary::CreateEditObject(LPCSTR nm)
{
	VERIFY(m_bReady);
    R_ASSERT(nm&&nm[0]);
    UI.ProgressInfo		(nm);
    AnsiString name		= AnsiString(nm).LowerCase();
    // file exist - find in already loaded
    CEditableObject* m_EditObject = 0;
	EditObjPairIt it 	= m_EditObjects.find(name);
    if (it!=m_EditObjects.end())	m_EditObject = it->second;
    else if (0!=(m_EditObject=LoadEditObject(name.c_str())))
		m_EditObjects[name]	= m_EditObject;
    if (m_EditObject)	m_EditObject->m_RefCount++;
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
        	AnsiString nm = ChangeFileExt(FS.update_path(nm,_objects_,O->second->GetName()),".object");
        	O->second->SaveObject(nm.c_str());
        }
}
//---------------------------------------------------------------------------

int ELibrary::GetObjects(FS_QueryMap& files)
{
    return FS.file_list(files,_objects_,FS_ListFiles|FS_ClampExt,".object");
}
//---------------------------------------------------------------------------

BOOL ELibrary::RemoveObject(LPCSTR _fname)
{
	AnsiString src_name;
	AnsiString fname		= ChangeFileExt(_fname,".object");
    FS.update_path			(src_name,_objects_,fname.c_str());
	if (FS.exist(src_name.c_str())){
        AnsiString thm_name = ChangeFileExt(fname,".thm");
    	// source
        EFS.BackupFile		(_objects_,fname.c_str());
        FS.file_delete		(src_name.c_str());
        EFS.WriteAccessLog	(src_name.c_str(),"Remove");
        // thumbnail
        EFS.BackupFile		(_objects_,thm_name.c_str());
        FS.file_delete		(_objects_,thm_name.c_str());

        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------

void ELibrary::RenameObject(LPCSTR fn0, LPCSTR fn1)
{
	AnsiString nm0,nm1;
    // rename base file
    FS.update_path(nm0,_objects_,fn0);	nm0+=".object";
    FS.update_path(nm1,_objects_,fn1);	nm1+=".object";
	FS.file_rename(nm0.c_str(),nm1.c_str(),false);
    // rename thm
    FS.update_path(nm0,_objects_,fn0);	nm0+=".thm";
    FS.update_path(nm1,_objects_,fn1);	nm1+=".thm";
	FS.file_rename(nm0.c_str(),nm1.c_str(),false);
}
//---------------------------------------------------------------------------

