//----------------------------------------------------
// file: Library.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Library.h"
#include "EditObject.h"
#include "SceneClassList.h"
#include "FileSystem.h"
#include "Library.h"

//----------------------------------------------------
#define LOBJ_CHUNK_NAMES		0x1201
#define LOBJ_CHUNK_VERSION	  	0x1202
#define LOBJ_CHUNK_FOLDER		0x1203
#define LOBJ_CHUNK_SRCNAME		0x1204
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

void CLibObject::LoadObject(){
	if (!m_EditObject){
		m_bLoadingError = true;
    	m_EditObject = new CEditableObject(this);
        AnsiString fn=m_FileName;
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
    CEditableObject* obj = GetReference();
    // check need to transform to world
    Fvector& P = obj->t_vPosition;
    Fvector& S = obj->t_vScale;
    Fvector& R = obj->t_vRotate;
    if ((P.x!=0)||(P.y!=0)||(P.z!=0)|| (S.x!=1)||(S.y!=1)||(S.z!=1)|| (R.x!=0)||(R.y!=0)||(R.z!=0)){
    	Fmatrix mRotate, mScale, mTranslate, mTransform;
        // update transform matrix
        mRotate.setHPB			(R.y, R.x, R.z);

        mScale.scale			(S);
        mTranslate.translate	(P);
        mTransform.mul			(mTranslate,mRotate);
        mTransform.mul			(mScale);
        obj->TranslateToWorld	(mTransform);
		P.set					(0.f,0.f,0.f);
    	S.set   				(1.f,1.f,1.f);
	    R.set  					(0.f,0.f,0.f);
    }
    // mark as delete
    AnsiString fn;
    fn=m_FileName+AnsiString(".object");
    FS.m_Objects.Update(fn);
	FS.MarkFile(fn);

    // update new file name
    m_FileName = m_Name;
    // save object
    fn=m_FileName+AnsiString(".object");
    FS.m_Objects.Update(fn);

    obj->m_ObjVer.f_age = DateTimeToFileDate(Now());
    obj->SaveObject(fn.c_str());
    FS.SetFileAge(fn,obj->m_ObjVer.f_age);
    m_bNeedSave = false;
}

void CLibObject::Load(CStream& F){
    char buf[MAX_OBJ_NAME];

    R_ASSERT(F.FindChunk(LOBJ_CHUNK_NAMES));
    F.RstringZ	(buf); m_Name=buf;
    m_FileName = m_Name;

    R_ASSERT(F.FindChunk(LOBJ_CHUNK_FOLDER));
    F.RstringZ	(buf); m_FolderName=buf;

    if(F.FindChunk(LOBJ_CHUNK_SRCNAME)){
	    F.RstringZ(buf); m_SrcName=buf;
    }
}

void CLibObject::Save(CFS_Base& F){
	if (m_bNeedSave) SaveObject();

    F.open_chunk(LOBJ_CHUNK_NAMES);
    F.WstringZ	(m_Name.c_str());
    F.close_chunk();

    F.open_chunk(LOBJ_CHUNK_FOLDER);
    F.WstringZ	(m_FolderName.c_str());
    F.close_chunk();

    F.open_chunk(LOBJ_CHUNK_SRCNAME);
    F.WstringZ	(m_SrcName.c_str());
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
        AnsiString fn = (*l_obj)->m_Name+AnsiString(".object");
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
    FS.m_Meshes.Update(_FileName);

    DWORD version = 0;

	VERIFY( _FileName );
	if (FS.Exist(_FileName,true)){
	    CCompressedStream F(_FileName,"LIBRARY");

		ELog.Msg( mtInformation, "Object Library: loading %s...", _FileName );
	    // Version
		R_ASSERT(F.ReadChunk(CHUNK_VERSION, &version));

	    if (version!=CURRENT_LIBRARY_VERSION){
    	    ELog.DlgMsg( mtError, "Object Library: unsupported file version. Can't load library.");
	        return false;
    	}

        // Objects (������� ��� ������������� ������ � ����)
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

		ELog.Msg( mtInformation, "ELibrary: %d objects loaded", ObjectCount() );
		return true;
    }
	return false;
}
//----------------------------------------------------

void ELibrary::Save(){
  	char _FileName[MAX_PATH]="library";
    FS.m_Meshes.Update(_FileName);

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
    if (Load())	ELog.Msg( mtInformation, "Lib: initialized" );
    else		ELog.DlgMsg( mtError, "Can't initialized object library." );
}

void ELibrary::Clear(){
	m_Valid = false;

	m_Current = 0;

    for(LibObjIt it=FirstObj(); it!=LastObj(); it++) _DELETE( *it );

    m_Objects.clear();
	ELog.Msg( mtInformation, "Lib: cleared" );
}

ELibrary::ELibrary(){
	m_Valid = false;
	m_Current = 0;
	Device.seqDevCreate.Add	(this,REG_PRIORITY_NORMAL);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);
}

ELibrary::~ELibrary(){
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	m_Objects.clear();
	VERIFY( m_Valid == false );
}

//----------------------------------------------------
CLibObject *ELibrary::SearchObject(const char *name){
	if (!name) return 0;
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
        if((0==strcmp((*it)->m_Name.c_str(),name))) return *it;
	return 0;
}

CEditableObject* ELibrary::SearchEditObject(const char *name){
	if (!name) return 0;
    CLibObject* LO = SearchObject(name);
    if (!LO){
    	ELog.Msg( mtError, "Can't find '%s' in library.", name );
        return 0;
    }
    CEditableObject* EO = LO->GetReference();
    if (!EO) ELog.Msg( mtError, "Object '%s' can't load.", name );
    return EO;
}

CLibObject* ELibrary::FindObjectByName(const char *name, const CLibObject* pass_item){
	if (!name) return 0;
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
		if((0==stricmp((*it)->m_Name.c_str(),name))&&(pass_item!=(*it))) return *it;
	return 0;
}

bool ELibrary::Validate(){
// find duplicate name
	for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
        if (FindObjectByName((*it)->m_Name.c_str(), *it)){
            AnsiString s;
            s.sprintf("Duplicated Item name '%s'!",(*it)->m_Name.c_str());
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
	// �������� �� ���� ������ � ���������: meshes, pictures, pictures/thumbnail, import, temp � �������� ��� ���������
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

void ELibrary::OnDeviceCreate(){
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
    	if ((*it)->IsLoaded()) (*it)->GetReference()->OnDeviceCreate();
}

void ELibrary::OnDeviceDestroy(){
    for(LibObjIt it=FirstObj(); it!=LastObj(); it++)
    	if ((*it)->IsLoaded()) (*it)->GetReference()->OnDeviceDestroy();
}

