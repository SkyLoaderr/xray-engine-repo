//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef _INCDEF_Library_H_
#define _INCDEF_Library_H_

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
    FileMap				m_Objects;
    AnsiString			m_Current;

    CEditableObject*	LoadEditObject		(LPCSTR name, int age);
public:
						ELibrary			();
	virtual 			~ELibrary			();

	void 				OnCreate			();
	void 				OnDestroy			();
	void 				Save				();

    void 				ReloadObjects		();
    void 				RefreshLibrary		();
    void 				ReloadObject		(LPCSTR name);
//    void				ResetAnimation		();

//	void 				GenerateObjectName	(char* buffer, const char* start_name, const CLibObject* pass_object);

    void				SetCurrentObject	(LPCSTR T);
    LPCSTR				GetCurrentObject	(){return m_Current.IsEmpty()?0:m_Current.c_str();}
    CEditableObject*	CreateEditObject	(LPCSTR name,int* age=0);
    void				RemoveEditObject	(CEditableObject*& object);

	int 				ObjectCount        	();
	FileMap&			Objects	        	(){return m_Objects;}

    void				EvictObjects		();

	virtual		void	OnDeviceCreate		();
	virtual		void	OnDeviceDestroy		();
};

extern ELibrary Lib;
//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

