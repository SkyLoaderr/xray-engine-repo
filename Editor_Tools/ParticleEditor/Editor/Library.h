//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef _INCDEF_Library_H_
#define _INCDEF_Library_H_

#include "CustomObject.h"
#include "pure.h"
//----------------------------------------------------
class CEditableObject;

DEFINE_MAP_PRED(AnsiString,CEditableObject*,EditObjMap,EditObjPairIt,astr_pred);
//----------------------------------------------------
class ELibrary:	public pureDeviceCreate, public pureDeviceDestroy
{
	bool				m_bReady;
	friend class TfrmChoseObject;
	EditObjMap			m_EditObjects;
    AStringVec			m_Objects;
    AnsiString			m_Current;

    CEditableObject*	LoadEditObject		(LPCSTR name);
public:
						ELibrary			();
	virtual 			~ELibrary			();

	void 				OnCreate			();
	void 				OnDestroy			();
	void 				Save				();

    void 				ReloadLibrary		();
    void 				RefreshLibrary		();
    void 				UnloadMeshes		();
//    void				ResetAnimation		();

//	void 				GenerateObjectName	(char* buffer, const char* start_name, const CLibObject* pass_object);
    void 				UnloadObject		(LPCSTR N);

    void				SetCurrentObject	(LPCSTR T);
    LPCSTR				GetCurrentObject	(){return m_Current.c_str();}
    CEditableObject*	GetEditObject		(LPCSTR name);

	int 				ObjectCount        	();
	AStringVec&			Objects	        	(){return m_Objects;}

	virtual		void	OnDeviceCreate		();
	virtual		void	OnDeviceDestroy		();
};

extern ELibrary Lib;
#define SKYDOME_FOLDER 		"$SkyDome"
#define DETAILOBJECT_FOLDER	"$Detail Objects"
//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

