//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef _INCDEF_Library_H_
#define _INCDEF_Library_H_

#include "CustomObject.h"
#include "pure.h"
//----------------------------------------------------
class ETexture;
class CCustomObject;
class CEditableObject;
struct FSChunkDef;

class CLibObject{
	friend class 	ELibrary;
	friend class 	ELibrary;
    friend class 	TfrmEditLibrary;

	bool			m_bNeedSave;
    bool			m_bLoadingError;
protected:
    CEditableObject*m_EditObject;
	void			LoadObject				();
protected:
    AnsiString		m_Name;
    AnsiString		m_FileName;
	AnsiString		m_SrcName;
public:
					CLibObject				(){
                    	m_bLoadingError	= false;
                        m_EditObject	= 0;
                        m_bNeedSave		= false;
                    }
	virtual 		~CLibObject				();

    LPCSTR			GetName					()	{return m_Name.c_str();}
  	LPCSTR			GetSrcName				()	{return m_SrcName.c_str();}
    void			SetName					(const AnsiString& s)	{m_Name=s;}

	IC bool			IsLoaded				()	{return !!m_EditObject;}
	IC bool			IsValid					()	{return m_EditObject;}
    IC void			Refresh					()	{m_bLoadingError=false;}
    IC CEditableObject*	GetReference		()	{if (!m_bLoadingError&&!IsLoaded()) LoadObject(); return m_EditObject;}

    void			Modified				(bool bFlag=true){m_bNeedSave=bFlag;}

    bool			ImportFrom				(LPCSTR name);
    void			SaveObject				();

  	void			Load					(CStream&);
	void 			Save					(CFS_Base&);

	void			UnloadObject			(){m_bLoadingError=false;_DELETE(m_EditObject);}
};

DEFINE_VECTOR(CLibObject*, LibObjVect, LibObjIt);

//----------------------------------------------------
class ELibrary:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
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
	void 				RemoveObject		(LPCSTR name);

	int 				ObjectCount        	();
	CLibObject*			SearchObject		(const char *name);
	CEditableObject*	SearchEditObject	(const char *name);
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

	virtual		void	OnDeviceCreate		();
	virtual		void	OnDeviceDestroy		();
};

extern ELibrary* Lib;
#define SKYDOME_FOLDER 		"$SkyDome"
#define DETAILOBJECT_FOLDER	"$Detail Objects"
//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

