//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Library.h"
#include "EditObject.h"
#include "SceneClassList.h"
#include "Scene.h"
#include "FileSystem.h"

//----------------------------------------------------
#define LOBJ_CHUNK_NAMES		0x1201
#define LOBJ_CHUNK_VERSION	  	0x1202
#define LOBJ_CHUNK_FOLDER		0x1203
#define LOBJ_CHUNK_FILENAME		0x1204
//----------------------------------------------------
#define CURRENT_LIBRARY_VERSION 0x0011
#define CHUNK_VERSION       	0x9d01
//----------------------------------------------------
#define CHUNK_LIB_OBJECT_LIST  	0x9d02
//----------------------------------------------------
CLibObject::~CLibObject(){
	_DELETE(m_EditObject);
}

bool CLibObject::ImportFrom(const char* name){
    _DELETE(m_EditObject);
    m_EditObject = new CEditObject(true);
    if (FS.Exist(name, true))
        if (m_EditObject->Load(name)){
        	m_FileName = ExtractFileName(AnsiString(name));
            m_EditObject->m_ObjVer.f_age = FS.GetFileAge(AnsiString(name));
            return true;
        }
    _DELETE(m_EditObject);
    return false;
}

void CLibObject::LoadObject(){
	if (!m_EditObject){
		m_bLoadingError = true;
    	m_EditObject = new CEditObject(true);
        AnsiString fn=m_RefName;
        fn += ".object";
		FS.m_Objects.Update(fn);
	    if (FS.Exist(fn.c_str(), true))
	        if (m_EditObject->Load(fn.c_str())){
            	m_EditObject->m_ObjVer.f_age = FS.GetFileAge(fn);
                m_bLoadingError = false;
            	return;
            }
        _DELETE(m_EditObject);
    }
}

void CLibObject::SaveObject(){
    CEditObject* obj = GetReference();
    // check need to transform to world
    const Fvector& P = obj->Position();
    const Fvector& S = obj->GetScale();
    const Fvector& R = obj->GetRotate();
    if ((P.x!=0)||(P.y!=0)||(P.z!=0)|| (S.x!=1)||(S.y!=1)||(S.z!=1)|| (R.x!=0)||(R.y!=0)||(R.z!=0))
        obj->TranslateToWorld();

    // save object        
    AnsiString fn;
    fn=m_RefName+AnsiString(".object");
    FS.m_Objects.Update(fn);

    obj->m_ObjVer.f_age = DateTimeToFileDate(Now());
    obj->SaveObject(fn.c_str());
    FS.SetFileAge(fn,obj->m_ObjVer.f_age);
    m_bNeedSave = false;
}

void CLibObject::Load(CStream& F){
    char buf[MAX_OBJ_NAME];

    R_ASSERT(F.FindChunk(LOBJ_CHUNK_NAMES));
    F.RstringZ	(buf); m_RefName=buf;

    R_ASSERT(F.FindChunk(LOBJ_CHUNK_FOLDER));
    F.RstringZ	(buf); m_FolderName=buf;

    if(F.FindChunk(LOBJ_CHUNK_FILENAME)){
	    F.RstringZ(buf); m_FileName=buf;
    }
}

void CLibObject::Save(CFS_Base& F){
	if (m_bNeedSave) SaveObject();

    F.open_chunk(LOBJ_CHUNK_NAMES);
    F.WstringZ	(m_RefName.c_str());
    F.close_chunk();

    F.open_chunk(LOBJ_CHUNK_FOLDER);
    F.WstringZ	(m_FolderName.c_str());
    F.close_chunk();

    F.open_chunk(LOBJ_CHUNK_FILENAME);
    F.WstringZ	(m_FileName.c_str());
    F.close_chunk();
}

//----------------------------------------------------
ELibrary* Lib;
//----------------------------------------------------
void ELibrary::SetCurrentObject(LPCSTR T){
	m_Current = SearchObject(T);
}
int ELibrary::ObjectCount(){
	return m_Objects.size();
}
//----------------------------------------------------
void ELibrary::AddObject(CLibObject* obj){
	VERIFY( obj );
	VERIFY( m_Valid );
    m_Objects.push_back(obj);
}
void ELibrary::RemoveObject(CLibObject* obj){
	VERIFY( obj );
	VERIFY( m_Valid );
    LibObjIt l_obj = find(m_Objects.begin(),m_Objects.end(),obj);
    if (l_obj!=m_Objects.end()){
        AnsiString fn;
        fn = (*l_obj)->GetFileName();
        FS.m_Meshes.Update(fn);
        FS.MarkFile(fn);
     	m_Objects.erase(l_obj);
    }
}
//----------------------------------------------------
void ELibrary::UnloadMeshes(){
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
        _DELETE((*it)->m_EditObject);
}
//----------------------------------------------------
void ELibrary::ReloadLibrary(){
    VERIFY(!Scene->ObjCount());
    Clear();
    Init();
}
//----------------------------------------------------
void ELibrary::RefreshLibrary(){
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
		(*it)->Refresh();
}
//----------------------------------------------------
bool ELibrary::Load(){
  	char _FileName[MAX_PATH]="library";
    FS.m_Config.Update(_FileName);

    DWORD version = 0;

	VERIFY( _FileName );
	if (FS.Exist(_FileName,true)){
	    CCompressedStream F(_FileName,"LIBRARY");

		Log->Msg( mtInformation, "Object Library: loading %s...", _FileName );
	    // Version
		R_ASSERT(F.ReadChunk(CHUNK_VERSION, &version));

	    if (version!=CURRENT_LIBRARY_VERSION){
    	    Log->DlgMsg( mtError, "Object Library: unsupported file version. Can't load library.");
	        return false;
    	}

        // Objects (оставил для совместимости запись в чанк)
        CStream* OBJ = F.OpenChunk(0);
        if(OBJ){
            CStream* O   = OBJ->OpenChunk(0);
            for (int count=1; O; count++) {
                CLibObject* curobj	= new CLibObject();
                curobj->Load		(*O);
                AddObject			(curobj);
                O->Close			();
                O = OBJ->OpenChunk	(count);
            }
            OBJ->Close();
        }

		Log->Msg( mtInformation, "ELibrary: %d objects loaded", ObjectCount() );
		return true;
    }
	return false;
}
//----------------------------------------------------

void ELibrary::Save(){
  	char _FileName[MAX_PATH]="library";
    FS.m_Config.Update(_FileName);

    CFS_Memory F;

    F.open_chunk	(CHUNK_VERSION);
    F.Wdword		(CURRENT_LIBRARY_VERSION);
    F.close_chunk	();

	F.open_chunk	(0); 
    int count = 0;
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++){
        F.open_chunk(count); count++;
        (*it)->Save(F);
        F.close_chunk();
    }
    F.close_chunk	();

    F.SaveTo		(_FileName,"LIBRARY");
}

void ELibrary::Init(){
	m_Current = 0;
	m_Valid = true;
    if (Load())	Log->Msg( mtInformation, "Lib: initialized" );
    else		Log->DlgMsg( mtError, "Can't initialized object library." );
}

void ELibrary::Clear(){
	m_Valid = false;

	m_Current = 0;

    for(LibObjIt it=FirstObj(); it!=LastObj(); it++) _DELETE( *it );

    m_Objects.clear();
	Log->Msg( mtInformation, "Lib: cleared" );
}

ELibrary::ELibrary(){
	m_Valid = false;
	m_Current = 0;
}

ELibrary::~ELibrary(){
	m_Objects.clear();
	VERIFY( m_Valid == false );
}

//----------------------------------------------------
CLibObject *ELibrary::SearchObject(const char *name){
	if (!name) return 0;
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
        if((0==strcmp((*it)->m_RefName.c_str(),name))) return *it;
	return 0;
}

CLibObject* ELibrary::FindObjectByName(const char *name, const CLibObject* pass_item){
	if (!name) return 0;
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
		if((0==stricmp((*it)->m_RefName.c_str(),name))&&(pass_item!=(*it))) return *it;
	return 0;
}

bool ELibrary::Validate(){
// find duplicate name
	for(LibObjIt it=FirstObj(); it!=LastObj(); it++) 
        if (FindObjectByName((*it)->m_RefName.c_str(), *it)){
            AnsiString s;
            s.sprintf("Duplicated Item name '%s'!",(*it)->m_RefName.c_str());
            MessageDlg(s, mtError, TMsgDlgButtons() << mbOK, 0);
            return false;
        }
    return true;
}
//---------------------------------------------------------------------------

void ELibrary::ResetAnimation(){
	for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
        if ((*it)->IsLoaded()) (*it)->GetReference()->ResetAnimation();
}

//---------------------------------------------------------------------------
void ELibrary::GenerateObjectName(char* buffer, const char* start_name, const CLibObject* pass_object){
    strcpy(buffer,start_name);
    int idx = 0;
    while(FindObjectByName(buffer,pass_object)){
        sprintf( buffer, "%s_%d", start_name, idx );
        idx++;
    }
}

void ELibrary::Clean(){
	// пройтись по всем файлам в каталогах: meshes, pictures, pictures/thumbnail, import, temp и пометить как удаленные
    TSearchRec sr;
	int iAttributes = faAnyFile;
    AnsiString nm,fn;
    char name[MAX_PATH];
    // meshes
    nm = "*.object";
    FS.m_Objects.Update(nm);
	if (FindFirst(nm, iAttributes, sr) == 0){
	    do{
			if ((sr.Attr & iAttributes) == sr.Attr){
				_splitpath( sr.Name.c_str(), 0, 0, name, 0 );
                if (!SearchObject(name)){ 
                	fn=sr.Name; FS.m_Objects.Update(fn);
                	FS.MarkFile(fn);
                }
            }
	    } while (FindNext(sr) == 0);
	    FindClose(sr);
	}
}

