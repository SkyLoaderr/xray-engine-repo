//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Library.h"
#include "EditObject.h"
#include "ui_main.h"
#include "EditorPref.h"

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
		if ((object->m_RefCount==0)&&!frmEditPrefs->cbLeaveEmptyRef->Checked)
			if (!object->IsModified()) UnloadEditObject(object->GetName());
        object=0;
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

BOOL ELibrary::RemoveObject(LPCSTR _fname, EItemType type)   
{
	if (TYPE_FOLDER==type){
    	FS.dir_delete			(_objects_,_fname);
		return TRUE;
    }else if (TYPE_OBJECT==type){
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

	        UnloadEditObject	(_fname);
            
            return TRUE;
        }
    }else THROW;
    return FALSE;
}
//---------------------------------------------------------------------------

void ELibrary::RenameObject(LPCSTR nm0, LPCSTR nm1, EItemType type)
{
	if (TYPE_FOLDER==type){
    	FS.dir_delete			(_objects_,nm0);
    }else if (TYPE_OBJECT==type){
        AnsiString fn0,fn1;
        // rename base file
        FS.update_path(fn0,_objects_,nm0);	fn0+=".object";
        FS.update_path(fn1,_objects_,nm1);	fn1+=".object";
        FS.file_rename(fn0.c_str(),fn1.c_str(),false);
        // rename thm
        FS.update_path(fn0,_objects_,nm0);	fn0+=".thm";
        FS.update_path(fn1,_objects_,nm1);	fn1+=".thm";
        FS.file_rename(fn0.c_str(),fn1.c_str(),false);

        // rename in cache
        EditObjPairIt it 	= m_EditObjects.find(nm0);
	    if (it!=m_EditObjects.end()){
            m_EditObjects[nm1]	= it->second;
            m_EditObjects.erase	(it);
        }
	}
}
//---------------------------------------------------------------------------

void ELibrary::UnloadEditObject(LPCSTR full_name)
{
    EditObjPairIt it 	= m_EditObjects.find(full_name);
    if (it!=m_EditObjects.end()){
    	if (0!=it->second->m_RefCount){
        	ELog.DlgMsg(mtError,"Object '%s' still referenced.",it->first);
            THROW;
        }
    	m_EditObjects.erase(it);
    	xr_delete		(it->second);
    }
}
//---------------------------------------------------------------------------

