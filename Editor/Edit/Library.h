//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef _INCDEF_Library_H_
#define _INCDEF_Library_H_

#include "SceneObject.h"
//----------------------------------------------------
class ETexture;
class SceneObject;
class CEditObject;
struct FSChunkDef;

class CLibObject{
	friend class 	ELibrary;
	friend class 	ELibrary;
    friend class 	TfrmEditLibrary;

	bool			m_bNeedSave;
    bool			m_bLoadingError;
protected:
    CEditObject*	m_EditObject;
	void			LoadObject				();
protected:
	AnsiString		m_FolderName;
    AnsiString		m_RefName;
	AnsiString		m_FileName;
public:
					CLibObject				(){
                    	m_bLoadingError	= false;
                        m_EditObject	= 0;
                        m_bNeedSave		= false;
                    }
	virtual 		~CLibObject				();

    const char*		GetRefName				()	{return m_RefName.c_str();}
    const char*		GetFolderName			()	{return m_FolderName.c_str();}
    void			SetRefName				(AnsiString& s)	{m_RefName=s;}
	void			SetFolderName			(AnsiString& s)	{m_FolderName=s;}
	void			SetFileName				(AnsiString& s)	{m_FileName=s;}
	const char*		GetFileName				() {return m_FileName.c_str();}

	IC bool			IsLoaded				()	{return !!m_EditObject;}
	IC bool			IsValid					()	{return m_EditObject;}
    IC void			Refresh					()	{m_bLoadingError=false;}
    IC CEditObject*	GetReference			()	{if (!m_bLoadingError&&!IsLoaded()) LoadObject(); return m_EditObject;}

    bool			ImportFrom				(const char* name);
    void			SaveObject				();

  	void			Load					(CStream&);
	void 			Save					(CFS_Base&);

	void			UnloadObject			(){m_bLoadingError=false;_DELETE(m_EditObject);}
};

DEFINE_VECTOR(CLibObject*, LibObjVect, LibObjIt);

//----------------------------------------------------
class ELibrary {
	friend class TfrmChoseObject;
public:
	bool m_Valid;
    LibObjVect			m_Objects;
	CLibObject*			m_Current;
protected:
    bool 				Load				();
    void 				Save				();
public:
	IC bool 			valid				(){return m_Valid;}

	void 				AddObject			(CLibObject* obj);
	void 				RemoveObject		(CLibObject* obj);

	int 				ObjectCount        	();
	CLibObject*			SearchObject		(const char *name);
	CLibObject*			FindObjectByName	(const char *name, const CLibObject* pass_object);
public:
	void 				Init				();
	void 				Clear				();

	IC LibObjIt			FirstObj			(){return m_Objects.begin();}
	IC LibObjIt			LastObj				(){return m_Objects.end();}
    
    void 				SaveLibrary			(){Save();}
    void 				ReloadLibrary		();
    void 				RefreshLibrary		();
    void 				UnloadMeshes		();
    void				ResetAnimation		();

	void 				GenerateObjectName	(char* buffer, const char* start_name, const CLibObject* pass_object);
    
    void				SetCurrentObject	(LPCSTR T);
    CLibObject*			GetCurrentObject	(){return m_Current;}

    bool 				Validate			();
    void				Clean				();

						ELibrary			();
	virtual 			~ELibrary			();
};

extern ELibrary* Lib;
#define SKYDOME_FOLDER 		"$SkyDome"
#define DETAILOBJECT_FOLDER	"$Detail Objects"
//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

